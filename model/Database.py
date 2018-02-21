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
        '''
        