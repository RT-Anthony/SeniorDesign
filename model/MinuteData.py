'''
Created on Feb 21, 2018

@author: Anthony Bell
'''
from sqlalchemy.ext.declarative.api import declarative_base
from sqlalchemy import Column, Integer, Text, DateTime
import datetime

Base = declarative_base()
class MinuteData(Base):
    """MinuteData:
    Class for tracking the minute by minute flow data for devices"""
    stored_id = 1
    __tablename__ = 'minute_flow_data'
    id = Column(Integer, primary_key=True)
    device = Column(Text)
    minute = Column(DateTime)
    flow = Column(Integer)

    def __init__(self, device, flow):
        """
        Constructor

        Args:
            device (str): name of the device
            flow (int): minute by minute flow for the specified device

        Returns:
            MinuteData object
        """
        self.id = MinuteData.stored_id
        MinuteData.stored_id += 1
        self.device = device
        self.flow = flow
        self.minute = datetime.datetime.now()
