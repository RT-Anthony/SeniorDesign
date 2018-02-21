'''
Created on Feb 21, 2018

@author: Anthony Bell
'''

class Device(object):
    '''
    The Device class will keep track of all current device statistics
    for a single device. This includes whether the valve is on or off and
    usage statistics
    '''


    def __init__(self, params):
        '''
        Constructor
        '''
        