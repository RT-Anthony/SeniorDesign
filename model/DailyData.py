'''
Created on Feb 21, 2018

@author: Anthony Bell
'''
from sqlalchemy.ext.declarative.api import declarative_base
from sqlalchemy import Column, Integer, Text, DateTime
import datetime

Base = declarative_base()
class DailyData(Base):
    """DailyData:
    Class for tracking the daily flow data for devices
    This class serves primarily as an ORM mapping for SQLAlchemy."""
    stored_id = 1
    __tablename__ = 'daily_flow_data'
    id = Column(Integer, primary_key=True)
    device = Column(Text)
    day = Column(DateTime)
    flow = Column(Integer)

    def __init__(self, device, flow):
        """
        Constructor

        Args:
            device (str): name of the device
            flow (int): daily flow for the specified device

        Returns:
            DailyData object
        """
        self.id = DailyData.stored_id
        DailyData.stored_id += 1
        self.device = device
        self.flow = flow
        self.day = datetime.datetime.now()
