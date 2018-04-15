
'''
Created on Feb 21, 2018

@author: Anthony Bell
'''
from sqlalchemy.ext.declarative.api import declarative_base
from sqlalchemy import Column, Integer, Text, DateTime
import datetime

Base = declarative_base()
class Device(Base):
    """docstring for."""
    stored_id = 1
    __tablename__ = 'device'
    id = Column(Integer, primary_key=True)
    device = Column(Text)
    max_flow = Column(Integer)
    status = Column(Text)

    def __init__(self, name, flow=1000):
        self.id = Device.stored_id
        Device.stored_id += 1
        self.name = name
        self.flow = flow
        self.status = "on"
