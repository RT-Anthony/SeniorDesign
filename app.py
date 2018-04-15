'''
Created on Feb 21, 2018

@author: Anthony Bell
'''
from flask import Flask, request, render_template, redirect, flash, send_from_directory, make_response
from flask_bootstrap import Bootstrap
from flask_login import LoginManager, login_user, current_user
from flask_login.utils import login_required, logout_user
from email.mime.text import MIMEText
import smtplib
import os
import sys
import datetime

from model.Database import Database

app = Flask(__name__)    # Construct an instance of Flask class
bootstrap = Bootstrap(app)
app.secret_key = "iegAe43JUlx9eNQD"
app.config['SECRET_KEY'] = "iegAe43JUlx9eNQD"
app.config['DEBUG'] = True

@app.route('/')
def index():
    return render_template('index.html')

@app.route('/settings')
def settings():
    return render_template('settings.html')

@app.route('/valves')
def valves():
    return render_template('valves.html')

@app.route('/update/<device>/<flow>')
def update(device, flow):
    db.add_minute_data(device, flow)
    #TODO UPDATE THE RETURN FOR THIS PART
    return("PLACEHOLDER")

@app.route('/notify/<device>/<message>')
def notify(device, message):
    db.add_notification(device, message)
    #TODO UPDAT THE RETURN FOR THIS PART
    return('PLACEHOLDER')

@app.teardown_appcontext
def shutdown_session(exception=None):
    '''Closes Database on shutdown for data integrity'''
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

    app.run(host='0.0.0.0', port=srvport)  # Launch built-in web server and run this Flask webapp
