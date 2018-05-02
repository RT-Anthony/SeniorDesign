import threading
import multiprocessing
import time
import os
import RPi.GPIO as GPIO
import socket
import http.client


class flow_unit(object):
    """
    This object handles the intialization of the flow unit, as
    well as the threads for managing the device. It seeds three thread
    objects that perform the following tasks:

    1- flow sensor thread: handles 10 second averages of the flow sensor
    to calculate flow rate of water throught the sensorself.

    2- listener thread: listens for commands from the main controller on
    TCP port 42425. Sends command acknoledgment responses, as well as
    connection accepted message.

    3- bluetooth thread: connects to nearest bluetooth device, listens
    for keepalives, and listens for a water detected event.

    Example:

        $ <var> = flow_unit()

    Attributes:
        count - This variable is utilized to count the number of square waves
                received over the flow sensor port. This is utilized to calculate
                the frequency over a given time interval, which determines
                average flow rate.
        flow_port - int that represents the BCM port number of the flow sensor
                    GPIO pin.
        open_port - int that represents the BCM port number for the GPIO output
                    that serves as the "open valve" line which controls the
                    relay to open the valve.
        close_port - int that represents the BCM port number for the GPIO output
                     that serves as the "close valve" line which controls the
                     relay to close the valve.
        listenport - hardcoded port number to listen for commands from the main
                     controller's commands on.
        flowthread - Thread object that handles the flow sensor calculations and
                     handling the counter resets based upon time.
        listenthread - Thread object that handles the TCP server which listens
                       for commands from the main control unit.
        bt_thread - Thread object that handles BlueTooth connection and listening
                    for the burst detector unit. Updates the main unit with event
                    detections as they come in, and attempts to shut down the
                    flow of water.

    Todo:
        -Currently the Bluetooth thread needs to be implemented.
        -Add sections to each thread init function to properly handle updating
        the main control unit.
        -Test intercommunication between the flow unit and the main controller.
    """

    def __init__(self, dev_name, srvip, port = 80):
        """Main initialization module for flow_unit object. See Docstring for object.
        """

        GPIO.setmode(GPIO.BCM)
        self.count = 0 #variable to count the number of ticks from flow sensor
        self.flow_port = 9 #port flow sensor is on
        self.open_port = 5 #update port numbers
        self.close_port = 26 #update port numbers
        self.listenport = 42425
        self.serverport = port
        self.serverip = srvip
        self.devicename = dev_name
        self.valve_closed = False
        GPIO.setup(self.open_port, GPIO.OUT) #enable output port
        GPIO.setup(self.close_port, GPIO.OUT) #enable output port
        #enable flow thread
        flowthread = threading.Thread(target = self.init_flow)
        flowthread.start()
        #enable valve control srvlistener (commands issued from main controller)
        listenthread = threading.Thread(target = self.init_listener)
        listenthread.start()
        #enable bluetooth thread, to be completed. currently disabled for
        #testing
        bt_thread = threading.Thread(target = self.init_bluetooth)
        bt_thread.start()

    def flow_callback(self,channel):
        """
        This module serves as the interrupt callback function for a detected
        event on the flow sensor input. This function counts the number of
        square waves received in order to calculate the frequency over 10 secs
        in the flow sensor thread.

        Args:
            channel (int): flow unit BCM port number. Generally should be handed
            'self.flow_port' for ease of use.

        Returns:
            None, but does modify parent object's count variable.

        """
        self.count = self.count + 1

    def init_flow(self):
        """
        Module that is utilized to initialize the flow sensor. This function is
        called during object instanciation, and becomes the module that the thread
        object runs.

        Returns:
            None. Module designed to perform threaded task for the flow sensor.

        """

        GPIO.setup(self.flow_port, GPIO.IN, pull_up_down=GPIO.PUD_UP)
        GPIO.add_event_detect(self.flow_port,GPIO.RISING,callback=self.flow_callback)
        currtime = time.time()
        lasttime = currtime
        self.add_device_controller()
        while True:
            currtime = time.time()
            if ((currtime - lasttime) >= 10):
                lasttime = time.time()
                if self.count == 0:
                    #send 0 to controller
                    self.count = 0
                    self.flow_update_controller(0)
                    flow = 0
                else:
                    freq = self.count/10
                    self.count = 0
                    flow = (freq+3)/8.1 #flow = lpm flow rate
                    flow = flow/6.0
                    #return lpm to controller
                    self.flow_update_controller(flow)
                    flow = 0
                    print("Average flow rate over 10 seconds = ", flow)
                    #TODO implement after adjustment to minute

    def add_device_controller(self):
        """This function is used to register a device with the controller using
        http://<controller>/add_device/<devicename>"""
        base_addy = self.serverip + ":" + str(self.serverport)
        device_url = "/add_device/" + self.devicename
        update_conn = http.client.HTTPConnection(base_addy)
        update_conn.request('GET',device_url)
        update_conn.close()

    def flow_update_controller(self,average_flow):
        """This function is used to update the main control unit with the average
        flow over 60 seconds"""
        base_addy = self.serverip + ":" + str(self.serverport)
        device_url = "/update/" + self.devicename + "/" + str(average_flow)
        update_conn = http.client.HTTPConnection(base_addy)
        update_conn.request('GET',device_url)
        update_conn.close()

    def close_flow(self,timedelay=5):
        """
        Module to close the flow valve.

        Args:
            timedelay (float): numerical value to wait while the valve is closed.

        Returns:
            None.

        """
        GPIO.output(self.close_port,1)
        time.sleep(timedelay)
        GPIO.output(self.close_port,0)
        self.valve_closed = True

    def open_flow(self,timedelay=5):
        """
        Module to open the flow valve.

        Args:
            timedelay (float): numerical value to wait while the valve is opened.

        Returns:
            None.

        """
        GPIO.output(self.open_port,1)
        time.sleep(timedelay)
        GPIO.output(self.open_port,0)
        self.valve_closed = False

    def socket_handler(self, connection, address):
        try:
            while True:
                data = connection.recv(1024)
                if data:
                    if data.decode() == "open_valve":
                        self.open_flow()
                    elif data.decode() == "close_valve":
                        self.close_flow()
        except:
            pass
        finally:
            connection.close()


    def init_listener(self):
        """
        Module that performs tasks related to listening for commands from main
        control unit. This module is used as the threaded moduled handed to the
        thread object. This is handled upon instanciation of the flow_unit object.

        Returns:
            None, this code is executed within a thread object.

        """
        delay = 5 #set value for delay to be 5 seconds
        sock = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
        sock.bind(("0.0.0.0",self.listenport))
        sock.listen(5)
        while True:
            #clientsock,address = sock.accept()
            clientsock,address = sock.accept()
            process = multiprocessing.Process(target=self.socket_handler, args=(clientsock,address))
            process.daemon = True
            process.start()


    def init_bluetooth(self):
        """
        To be filled in
        """
        while True:
            ble_devices = os.popen('timeout -s INT 2s hcitool lescan').read()
            if "TT_BURST" in ble_devices:
                self.close_flow()
                base_addy = self.serverip + ":" + str(self.serverport)
                device_url = "/valves/" + self.devicename + "/off")
                update_conn = http.client.HTTPConnection(base_addy)
                update_conn.request('GET',device_url)
                update_conn.close()
                print("BURST DETECTED!!!!!!")
            else:
                ble_devices = None
        #do bluetooth stuff

if __name__ == '__main__':
    unit = flow_unit('test_sink','192.168.1.129')
    while True:
        continue
