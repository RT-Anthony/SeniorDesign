'''
Created on Feb 21, 2018

@author: Anthony Bell
'''
import os
from sqlalchemy.ext.declarative.api import declarative_base
from sqlalchemy import create_engine, text
from sqlalchemy.orm import sessionmaker

from model.Device import Device
from model.MinuteData import MinuteData
from model.HourData import HourData
from model.DailyData import DailyData
from model.Notification import Notification

class Database(object):
    '''
    The Database class is a convenience class for
    interacting with the sqlite3 database.
    '''


    def __init__(self, params=None):
        '''
        Constructor
        Establishes session to sqlite database

        Args:
            None

        Returns:
            Database object
        '''
        Base = declarative_base()
        basedir = os.path.join(os.path.abspath(os.path.dirname(__file__)), '..')
        self.engine = create_engine('sqlite:///'+os.path.join(basedir, 'TrickleTerminators.db'))
        self.session = sessionmaker(expire_on_commit=False)
        self.session.configure(bind=self.engine)
        Base.metadata.create_all(self.engine)
        self.s = self.session()

    def add_minute_data(self, device, flow=0):
        '''
        Adds minute flow data to the database and updates hourly and daily data

        Args:
            device (str): device for which data is being logged
            flow (int): Flow data for the device (default = 0)

        Returns:
            None
        '''
        self.s.add(MinuteData(device, flow))
        self.update_hourly_data(device)
        self.update_daily_data(device)
        self.s.commit()

    def add_notification(self, device, message):
        '''
        Adds a notification to the database

        Args:
            device (str): device for which notification is being logged
            message (str): notification message

        Returns:
            None
        '''
        self.s.add(Notification(device, message))

    def update_hourly_data(self, device):
        pass

    def update_daily_date(self, device):
        pass

    def add_device(self, name):
        '''
        Adds a device to the database

        Args:
            name (str): the name of the device being added

        Returns:
            None
        '''
        self.s.add(Device(name))

    def remove_device(self, name):
        '''
        Removes a device from the database

        Args:
            name (str): the name of the device being removed

        Returns:
            None
        '''
        pass

    def close(self):
        '''
        Closes the database

        Args:
            None

        Returns:
            None
        '''
        self.s.close()
