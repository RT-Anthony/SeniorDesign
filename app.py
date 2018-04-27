'''
Created on Feb 21, 2018

@author: Anthony Bell
'''
from flask import Flask, request, render_template, redirect, flash, send_from_directory, make_response, url_for
from flask_bootstrap import Bootstrap
from flask_login import LoginManager, login_user, current_user
from flask_login.utils import login_required, logout_user
import os
import sys
import datetime
import socket

from model.Database import Database

app = Flask(__name__)    # Construct an instance of Flask class
bootstrap = Bootstrap(app)
app.secret_key = "iegAe43JUlx9eNQD"
app.config['SECRET_KEY'] = "iegAe43JUlx9eNQD"
app.config['DEBUG'] = True

@app.route('/')
def index():
    '''
    UI Dashboard/Homepage
    '''
    return render_template('index.html')

@app.route('/settings', methods=['GET', 'POST'])
def settings():
    '''
    Interface for managing max flow rates for devices
    '''
    devices = db.get_devices()
    if request.method == 'POST':
        for device in devices:
            db.update_device(device.device, flow=request.form[device.device])
    return render_template('settings.html', devices=devices)

@app.route('/valves', methods=['GET'])
def valves():
    '''
    Interface for user valve management
    '''
    devices = db.get_devices()
    return render_template('valves.html', devices=devices)

@app.route('/valves/<device>/<status>')
def valves_update(device, status):
    _device = db.get_device(device)
    if status == "on":
        mySocket.connect((_device.ip, 42425))
        mySocket.send("open_valve")
        mySocket.close()
    else:
        mySocket.connect((_device.ip, 42425))
        mySocket.send("close_valve")
        mySocket.close()
    db.update_device(device, status=status)
    return redirect(url_for("valves"))

@app.route('/update/<device>/<flow>')
def update(device, flow):
    '''
    Interface for recieving flow data from devices
    '''
    _device = db.get_device(device)
    if not _device:
        db.add_device(device)
    hourly_flow = db.add_minute_data(device, flow)
    if hourly_flow >= db.get_device(device).max_flow:
        db.update_device(device, status="off")
        mySocket.connect((_device.ip, 42425))
        mySocket.send("close_valve")
        mySocket.close()
        return("Shutoff")
    return("Pass")

@app.route('/add_device/<name>')
def add_device(name):
    '''
    Interface for adding a device to be tracked
    '''
    ip = str(request.remote_addr)
    db.add_device(str(name), ip)
    db.add_notification(name, "Device has been added")
    return('PLACEHOLDER')

@app.route('/notifications')
def notifications():
    notifications = db.get_notifications()
    return render_template('notifications.html', notifications=notifications)

@app.route('/notify/<device>/<message>')
def notify(device, message):
    '''
    Interface for recieving notifications from devices
    '''
    db.add_notification(device, message)
    #TODO UPDAT THE RETURN FOR THIS PART
    return('PLACEHOLDER')

@app.teardown_appcontext
def shutdown_session(exception=None):
    '''
    Closes Database on shutdown for data integrity
    '''
    db.close()

if __name__ == '__main__':  # Script executed directly?
    #define port number via cmd-line arguments
    #checks for -p as first argument, and presence of a number as the second
    #argument
    if len(sys.argv) > 2:
        if sys.argv[1] == '-p':
            try:
                srvport = int(sys.argv[2])
            except:
                srvport = 80
    else:
        srvport = 80

    db = Database()
    mySocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    app.run(host='0.0.0.0', port=srvport)  # Launch built-in web server and run this Flask webapp
