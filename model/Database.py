'''
Created on Feb 21, 2018

@author: Anthony Bell
'''
import os
import datetime
import socket
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
    This class acts primarily as a wrapper for SQLAlchemy with the addition
    of some application specific helper functions.
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

    #Getter functions
    def get_devices(self):
        '''
        Returns all devices in the database

        Args:
            None

        Returns:
            None
        '''
        return self.s.query(Device)

    def get_hour_data(self):
        '''
        Gets all the hourly data from the database

        Args:
            None

        Returns:
            list of HourData objects
        '''
        return self.s.query(HourData)

    def get_current_hour(self, device):
        '''
        Gets the flow data for the current hour for the specified device

        Args:
            Device (str): name of the device to be queried

        Returns:
            HourData table entry for the current flow data of the device
        '''
        query = self.s.query(HourData).filter(HourData.device == device).order_by(desc(HourData.timestamp)).limit(1).all()
        return query

    def get_current_day(self, device):
        '''
        Gets the flow data for the current day for the specified device

        Args:
            Device (str): name of the device to be queried

        Returns:
            DailyData table entry for the current flow data of the device
        '''
        query = self.s.query(DailyData).filter(DailyData.device == device).order_by(desc(DailyData.timestamp)).limit(1).all()
        return query

    def get_daily_data(self):
        '''
        Gets all the daily data from the database

        Args:
            None

        Returns:
            list of DailyData objects
        '''
        return self.s.query(HourData)

    def get_minute_data(self):
        '''
        Gets all the minute by minute data from the database

        Args:
            None

        Returns:
            list of MinuteData objects
        '''
        return self.s.query(MinuteData)

    def get_device(self, device):
        '''
        Gets the device specified by the given name

        Args:
            device (str): the name of the device that is being requested

        Returns:
            Device object
        '''
        _device = self.s.query(Device).filter(Device.device == device).first()
        return _device

    def get_notifications(self):
        return self.s.query(Notification)


    #Setter functions
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


    #Helper functions
    def update_hourly_data(self, device):
        '''
        Updates the database with the current flow for the past hour for a given device

        Args:
            Device (str): name of the device to be updated

        Returns:
            hourly_flow (int): total flow for the previous hour
        '''
        past_hour = datetime.datetime.now() - datetime.timedelta(hours=1)
        minute_entries = self.s.query(MinuteData).filter(MinuteData.minute > past_hour)
        hourly_flow = 0
        for entry in minute_entries:
            hourly_flow += entry.flow
        self.s.add(HourData(device, hourly_flow))
        _device = self.s.query(Device).filter(Device.device == device).first()
        if _device.max_flow <= hourly_flow:
            pass #Shut off valve and update db
        self.s.commit()
        return hourly_flow

    def update_daily_data(self, device):
        '''
        Updates the database with the current flow for the past 24 hours for a given device

        Args:
            Device (str): name of the device to be updated

        Returns:
            None
        '''
        past_day = datetime.datetime.now() - datetime.timedelta(days=1)
        minute_entries = self.s.query(MinuteData).filter(MinuteData.minute > past_day)
        daily_flow = 0
        for entry in minute_entries:
            daily_flow += entry.flow
        self.s.add(DailyData(device, daily_flow))
        self.s.commit()


    def add_minute_data(self, device, flow=0):
        '''
        Adds minute flow data to the database and updates hourly and daily data

        Args:
            device (str): device for which data is being logged
            flow (int): Flow data for the device (default = 0)

        Returns:
            hourly_flow (int): flow for the last hour
        '''
        self.s.add(MinuteData(device, flow))
        hourly_flow = self.update_hourly_data(device)
        self.update_daily_data(device)
        self.s.commit()
        return hourly_flow

    def add_notification(self, device, message):
        '''
        Adds a notification to the database

        Args:
            device (str): device for which notification is being logged
            message (str): notification message

        Returns:
            None
        '''
        if message == "burst":
            message = "There has been a burst at " + device
        if message == "flow":
            message = device + " has exceeded the maximum flow allowed"
        if message == "remove":
            message = device + " has been removed"
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
        query = self.s.query(Device).fliter(Device.device == name)
        query.delete()
        s.commit()

    def update_device(self, device, flow=None, status=None):
        '''
        Updates the flow settings for a specified device

        Args:
            device (str): the name of the device being updated
            flow (int): the maximum flow value in L/Hr
            status (str): the status of the device ("on" or "off")

        Returns:
            None
        '''
        _device = self.s.query(Device).filter(Device.device == device).first()
        if flow is not None:
            _device.max_flow = flow
            self.s.commit()
        if status is not None:
            _device.status = status
            self.s.commit()

    def close(self):
        '''
        Closes the database

        Args:
            None

        Returns:
            None
        '''
        self.s.close()
