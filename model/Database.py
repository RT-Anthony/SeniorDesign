'''
Created on Feb 21, 2018

@author: Anthony Bell
'''
import os
import datetime
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
        self.session = sessionmaker(expire_on_commit=False, autoflush=False)
        self.session.configure(bind=self.engine)
        Base.metadata.create_all(self.engine)
        self.s = self.session()

    def set_next_id(self):
        '''
        Sets the stored_id values so that database collisions do not occur, call on startup

        Args:
            None

        Returns:
            None
        '''
        try:
            DailyData.stored_id = self.s.query(func.max(DailyData.id)) + 1
            HourData.stored_id = self.s.query(func.max(HourData.id)) + 1
            MinuteData.stored_id = self.s.query(func.max(MinuteData.id)) + 1
            Device.stored_id = self.s.query(func.max(Device.id)) + 1
            Notification.stored_id = self.s.query(func.max(Notification.id)) + 1
        except:
            pass

    def update_hourly_data(self, device):
        past_hour = datetime.datetime.now() - datetime.timedelta(hours=1)
        minute_entries = self.s.query(MinuteData).filter(MinuteData.minute > past_hour)
        hourly_flow = 0
        for entry in minute_entries:
            hourly_flow += entry.flow
        self.s.add(HourData(device, hourly_flow))
        self.s.commit()

    def get_hour_data(self):
        '''
        Gets all the hourly data from the database

        Args:
            None

        Returns:
            list of HourData objects
        '''
        self.s.query(HourData)


    def update_daily_date(self, device):
        past_day = datetime.datetime.now() - datetime.timedelta(days=1)
        minute_entries = s.query(MinuteData).filter(MinuteData.minute > past_day)
        daily_flow = 0
        for entry in minute_entries:
            daily_flow += entry.flow
        self.s.add(DailyData(device, daily_flow))
        self.s.commit()

    def get_daily_data(self):
        '''
        Gets all the daily data from the database

        Args:
            None

        Returns:
            list of DailyData objects
        '''
        self.s.query(HourData)

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

    def get_minute_data(self):
        '''
        Gets all the minute by minute data from the database

        Args:
            None

        Returns:
            list of MinuteData objects
        '''
        self.s.query(MinuteData)

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
        self.s.commit()

    def add_device(self, name, ip="0.0.0.0"):
        '''
        Adds a device to the database

        Args:
            name (str): the name of the device being added
            ip (str): the ip address of the device being added

        Returns:
            None
        '''
        new_device = Device(name=name,ip=ip)
        self.s.add(new_device)
        self.s.commit()

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
