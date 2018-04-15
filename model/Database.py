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
        '''
        Base = declarative_base()
        basedir = os.path.join(os.path.abspath(os.path.dirname(__file__)), '..')
        self.engine = create_engine('sqlite:///'+os.path.join(basedir, 'TrickleTerminators.db'))
        self.session = sessionmaker(expire_on_commit=False)
        self.session.configure(bind=self.engine)
        Base.metadata.create_all(self.engine)
        self.s = self.session()

    def add_minute_data(self, device, flow=0):
        self.s.add(MinuteData(device, flow))
        self.update_hourly_data(device)
        self.update_daily_data(device)
        self.s.commit()

    def add_notification(self, device, message):
        self.s.add(Notification(device, message))

    def update_hourly_data(self):
        pass

    def update_daily_date(self):
        pass

    def add_device(self, name):
        pass

    def remove_device(self, name):
        pass

    def close(self):
        self.s.close()
