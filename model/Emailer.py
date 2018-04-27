import smtplib
from email.mime.text import MIMEText

class Emailer(object):
    '''
    Convenience class for sending emails for system alerts
    '''
    subject_burst = "Pipe burst detected alert"
    subject_flow = "Flow settings exceeded"
    subject_add = "New device added"
    subject_remove = "Device removed"

    recipients = []

    def __init__(self, recipients=None):
        '''
        Constructor

        Args:
            recipients (list): list of email addresses that alerts are sent to

        Returns:
            Emailer object
        '''
        emailer = smtplib.SMTP('smtp.gmail.com', 587)
        emailer.ehlo("TrickleTerminators")
        self.recipients = recipients

    def add_recipient(self, recipient):
        '''
        Adds a new email address to the email list

        Args:
            recipient (str): email address to be added to the mail list

        Returns:
            None
        '''
        pass

    def remove_recipient(self, recipient):
        '''
        Removes an email address from the mail list

        Args:
            recipient (str): email address to be removed from the mail list

        Returns:
            None
        '''
        pass

    def send_message(self, template="burst"):
        '''
        Sends a message based on a specified template

        Args:
            template (str): specifies the type of alert to send output

        Returns:
            None
        '''
        if template == "burst":
            subject = subject_burst
        pass
