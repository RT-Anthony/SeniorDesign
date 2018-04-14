from sqlalchemy.ext.declarative.api import declarative_base
from sqlalchemy import Column, Integer, Text, DateTime
import datetime

Base = declarative_base()
class DailyData(Base):
    """docstring for."""
    stored_id = 1
    __tablename__ = 'daily_flow_data'
    id = Column(Integer, primary_key=True)
    device = Column(Text)
    day = Column(DateTime)
    flow = Column(Integer)

    def __init__(self, arg):
        pass
