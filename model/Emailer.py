import smtplib
from email.mime.text import MIMEText

class Emailer(object):
    '''
    Convenience class for sending emails for system alerts
    '''
    subject_burst = ""
    subject_flow = ""
    subject_add = ""
    subject_remove = ""

    def __init__(self, recipients):
        emailer = smtplib.SMTP('smtp.gmail.com', 587)
        emailer.ehlo("TrickleTerminators")

    def add_recipient(self, recipient):
        pass

    def remove_recipient(self, recipient):
        pass

    def send_message(self, template="burst"):
        pass
