'''
Created on Feb 21, 2018

@author: Anthony Bell
'''
from sqlalchemy.ext.declarative.api import declarative_base
from sqlalchemy import Column, Integer, Text, DateTime
import datetime

Base = declarative_base()
class HourData(Base):
    """HourData:
    Class for tracking the hourly flow data for devices"""
    stored_id = 1
    __tablename__ = 'hour_flow_data'
    id = Column(Integer, primary_key=True)
    device = Column(Text)
    hour = Column(DateTime)
    flow = Column(Integer)

    def __init__(self, device, flow):
        """
        Constructor

        Args:
            device (str): name of the device
            flow (int): hourly flow for the specified device

        Returns:
            HourData object
        """
        self.id = HourData.stored_id
        HourData.stored_id += 1
        self.device = device
        self.flow = flow
        self.hour = datetime.datetime.now()
