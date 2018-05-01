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

    email_from = "notifications@TrickleTerminators.com"

    def __init__(self, recipients=[]):
        '''
        Constructor

        Args:
            recipients (list): list of email addresses that alerts are sent to

        Returns:
            Emailer object
        '''
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

    def send_message(self, template="burst", message=""):
        '''
        Sends a message based on a specified template

        Args:
            template (str): specifies the type of alert to send output

        Returns:
            None
        '''
        emailer = smtplib.SMTP('smtp.gmail.com', 587)
        emailer.ehlo("TrickleTerminators")
        emailer.starttls()
        #need to add authentication stuff
        if template == "burst":
            subject = self.subject_burst
        elif template == "flow":
            subject = self.subject_flow
        elif template == "add":
            subject = self.subject_add
        elif template == "remove":
            subject = self.subject_remove
        else:
            subject = "Alert posted"
        msg = MIMEText(message)
        msg['Subject'] = subject
        msg['From'] = self.email_from
        #try:
        for recipient in self.recipients:
            msg['To'] = recipient
            emailer.send_message(msg)
            print("Email sent to: " + recipient)
        #except:
            #pass
        emailer.close()
