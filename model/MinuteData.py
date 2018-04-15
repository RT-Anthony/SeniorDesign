'''
Created on Feb 21, 2018

@author: Anthony Bell
'''
from sqlalchemy.ext.declarative.api import declarative_base
from sqlalchemy import Column, Integer, Text, DateTime
import datetime

Base = declarative_base()
class MinuteData(Base):
    """docstring for."""
    stored_id = 1
    __tablename__ = 'minute_flow_data'
    id = Column(Integer, primary_key=True)
    device = Column(Text)
    day = Column(DateTime)
    flow = Column(Integer)
    def __init__(self, device, flow):
        self.id = MinuteData.stored_id
        MinuteData.stored_id += 1
        self.device = device
        self.flow = flow
        self.day = datetime.datetime.now()
