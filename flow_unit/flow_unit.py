import threading
import time
import RPi.GPIO as GPIO
import socket
import bluetooth
import urllib.request

class flow_unit(object):
    """This object handles the intialization of the flow unit, as
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

    controller = "http://localhost/"
    device = "test1"
    def __init__(self):
    """Main initialization module for flow_unit object. See Docstring for object.

    Returns:
        None

    """
        GPIO.setmode(GPIO.BCM)
        self.count = 0 #variable to count the number of ticks from flow sensor
        self.flow_port = 9 #port flow sensor is on
        self.open_port = 5 #update port numbers
        self.close_port = 26 #update port numbers
        listenport = 42425
        GPIO.setup(open_port, GPIO.OUT) #enable output port
        GPIO.setup(close_port, GPIO.OUT) #enable output port
        #enable flow thread
        flowthread = threading.Thread(target = self.init_flow)
        flowthread.start()
        #enable valve control listener (commands issued from main controller)
        listenthread = threading.Thread(target = self.init_listener)
        listenthread.start()
        #enable bluetooth thread, to be completed. currently disabled for
        #testing
        #bt_thread = threading.Thread(target = self.init_bluetooth)
        #bt_thread.start()

    def flow_callback(self,channel):
    """This module serves as the interrupt callback function for a detected
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
    """Module that is utilized to initialize the flow sensor. This function is
    called during object instanciation, and becomes the module that the thread
    object runs.

    Returns:
        None. Module designed to perform threaded task for the flow sensor.

    """

        GPIO.setup(self.flow_port, GPIO.IN, pull_up_down=GPIO.PUD_UP)
        GPIO.add_event_detect(self.flow_port,GPIO.RISING,callback=self.flow_callback)
        currtime = time.time()
        lasttime = currtime
        while True:
            currtime = time.time()
            if ((currtime - lasttime) >= 10):
                last_time = time.time()
                if self.count == 0:
                    #send 0 to controller
                    self.count = 0
                else:
                    freq = self.count/10
                    self.count = 0
                    flow = (freq+3)/8.1 #flow = lpm flow rate
                    #return lpm to controller
                    print("Average flow rate over 10 seconds = ", flow)
                    #TODO implement after adjustment to minute
                    if "Shutoff" in urllib.request.urlopen(controller + device + "/" + flow).read():
                        close_flow()

    def close_flow(self,timedelay):
    """Module to close the flow valve.

    Args:
        timedelay (float): numerical value to wait while the valve is closed.

    Returns:
        None.

    """
        GPIO.output(self.close_port,1)
        time.sleep(timedelay)
        GPIO.output(self.close_port,0)

    def open_flow(self,timedelay):
    """Module to open the flow valve.

    Args:
        timedelay (float): numerical value to wait while the valve is opened.

    Returns:
        None.

    """
        GPIO.output(self.open_port,1)
        time.sleep(timedelay)
        GPIO.output(self.open_port,0)

    def init_listener(self):
    """Module that performs tasks related to listening for commands from main
    control unit. This module is used as the threaded moduled handed to the
    thread object. This is handled upon instanciation of the flow_unit object.

    Returns:
        None, this code is executed within a thread object.

    """
        sock = socket.socket(socket=AF_INET,socket.SOCK_STREAM)
        sock.bind(("0.0.0.0",self.listenport))
        sock.listen(5)
        While True:
            (clientsock,address) = sock.accept()
            sock.sendto("Connection accepted",(clientsock,address))
            data = sock.recv(1024)
            if data:
                if data == "open_valve":
                    self.open_flow()
                    sock.sendto("valve openeded")
                elif data == "close_valve":
                    self.close_valve()
                    sock.sendto("valve closed")
                else:
                    #insert error message
                    continue


    def init_bluetooth(self):
        """To be filled in"""
        #do bluetooth stuff
        x = 1+1
