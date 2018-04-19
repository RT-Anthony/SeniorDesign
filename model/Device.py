
'''
Created on Feb 21, 2018

@author: Anthony Bell
'''
from sqlalchemy.ext.declarative.api import declarative_base
from sqlalchemy import Column, Integer, Text, DateTime
import datetime

Base = declarative_base()
class Device(Base):
    """Device:
    Class for tracking individual devices and device settings"""
    stored_id = 1
    __tablename__ = 'device'
    id = Column(Integer, primary_key=True)
    device = Column(Text)
    max_flow = Column(Integer)
    status = Column(Text)
    ip = Column(Text)


    def __init__(self, name, flow=1000, ip="0.0.0.0"):
        """
        Constructor

        Args:
            name (str): name of the device
            flow (int): maximum flow rate per hour for the device. Default is 1000 L

        Returns:
            Device object
        """
        self.id = Device.stored_id
        Device.stored_id += 1
        self.name = name
        self.flow = flow
        self.status = "on"
        self.ip = ip
