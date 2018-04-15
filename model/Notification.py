'''
Created on Feb 21, 2018

@author: Anthony Bell
'''
from sqlalchemy.ext.declarative.api import declarative_base
from sqlalchemy import Column, Integer, Text, DateTime
import datetime

class Notification(object):
    '''
    The Notification class is a convenience class for handling
    notifications sent by individual devices.
    '''
    stored_id = 1
    __tablename__ = 'alert_data'
    id = Column(Integer, primary_key=True)
    device = Column(Text)
    timestamp = Column(DateTime)
    message = Column(Text)

    def __init__(self, device, message):
        '''
        Constructor
        '''
        self.id = Notification.stored_id + 1
        Notification.stored_id += 1
        self.device = device
        self.message = message
        self.timestamp = datetime.datetime.now()
