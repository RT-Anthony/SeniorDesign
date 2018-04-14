'''
Created on Feb 21, 2018

@author: Anthony Bell
'''
import os
from sqlalchemy.ext.declarative.api import declarative_base
from sqlalchemy import create_engine, text
from sqlalchemy.orm import sessionmaker

class Database(object):
    '''
    The Database class is a convenience class for
    interacting with the sqlite3 database.
    '''


    def __init__(self, params):
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

    def add_minute_data(self, flow=0):
        pass

    def update_hourly_data(self):
        pass

    def update_daily_date(self):
        pass

    def add_device(self, name):
        pass

    def remove_device(self, name):
        pass
