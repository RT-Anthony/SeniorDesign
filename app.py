from flask import Flask, request, render_template, redirect, flash, send_from_directory, make_response
from flask_bootstrap import Bootstrap
from flask_login import LoginManager, login_user, current_user
from flask_login.utils import login_required, logout_user
from email.mime.text import MIMEText
import smtplib
import os
import sys
import datetime

app = Flask(__name__)    # Construct an instance of Flask class
bootstrap = Bootstrap(app)
app.secret_key = "iegAe43JUlx9eNQD"
app.config['SECRET_KEY'] = "iegAe43JUlx9eNQD"
app.config['DEBUG'] = True

@app.route('/')
def index():
    return render_template('index.html')

if __name__ == '__main__':  # Script executed directly?
    app.run(host='0.0.0.0', port=80)  # Launch built-in web server and run this Flask webapp
    