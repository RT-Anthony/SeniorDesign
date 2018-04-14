'''
Created on Feb 21, 2018

@author: Anthony Bell
'''

class Notification(object):
    '''
    The Notification class is a convenience class for handling
    notifications sent by individual devices.
    '''
    stored_id = 1
    __tablename__ = 'alert_data'
    id = Column(Integer, primary_key=True)
    device = Column(Text)
    timestamp = Column(DateTime)
    message = Column(Text)

    def __init__(self, params):
        '''
        Constructor
        '''
