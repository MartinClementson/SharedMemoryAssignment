#!/bin/python
__author__ = 'fll'

import threading
import subprocess
import time
import sys

tests = [
    {
        'testname': 'simple_1to1',
        'serverDelay': 5,
        'clientsDelays': [5],
        'numMsg': 10000,
        'memSize': 10,
        'msgSize': 1024,
    },
    {
        'testname': 'fastServer_1to1',
        'serverDelay': 1,
        'clientsDelays': [5],
        'numMsg': 10000,
        'memSize': 10,
        'msgSize': 1024,
    },
    {
        'testname': 'slowServer_1to1',
        'serverDelay': 5,
        'clientsDelays': [1],
        'numMsg': 10000,
        'memSize': 10,
        'msgSize': 1024,
    },
    {
        'testname': 'fastServer_1toN',
        'serverDelay': 1,
        'clientsDelays': [5,6,7,8],
        'numMsg': 10000,
        'memSize': 10,
        'msgSize': 1024,
    },
    {
        'testname': 'slowServer_1toN',
        'serverDelay': 10,
        'clientsDelays': [1,2,3,4],
        'numMsg': 10000,
        'memSize': 10,
        'msgSize': 1024,
    },
    {
         'testname': 'randomMsg',
        'serverDelay': 0,
        'clientsDelays': [0],
        'numMsg': 10000,
        'memSize': 10,
        'msgSize': 'random',
    }
]

# one thread per command
class ThreadTest(threading.Thread):
    def __init__(self, cmd):
        self.stdout = None
        self.stderr = None
        self.cmd = cmd
        threading.Thread.__init__(self)

    def run(self):
        p = subprocess.Popen( self.cmd.split(),
                         shell = False,
                         stdout = subprocess.PIPE,
                         stderr = subprocess.PIPE )
        self.stdout, self.stderr = p.communicate()


# change executable name if necessary
program = 'shared.exe'

if __name__=='__main__':
    programName = program
    testName = 'all'
    for arg in sys.argv[1:]:
        # specify ONE test only
        if 'test' in arg:
            testName = arg[5:]

    for test in tests:
        # can use: test1414.py test=testone, testtwo, testthree for example
        if test['testname'] in testName or testName == 'all':
            # repeat each test N times!
            for run in range(5):
                server = None
                clients = []
                print "====================================="
                print "Running test: " + test['testname']

                msgConf = "%s %s %s" % (test['memSize'],test['numMsg'],test['msgSize'])
                startTime = time.clock()
                # initiate all clients first
                for clientDelay in test['clientsDelays']:
                    commandLine = "%s consumer %s %s" % (programName, clientDelay, msgConf)
                    print "Running: " + commandLine
                    clients.append ( ThreadTest (commandLine) )
                    clients[-1].start()
                    time.sleep(0.01)

                time.sleep(0.1)
                commandLine = "%s producer %s %s" % (programName, test['serverDelay'], msgConf)
                server = ThreadTest(commandLine)
                server.start()

                server.join()
                # wait for all clients to finish
                for c in clients:
                    c.join()

                endTime = time.clock()
                elapsed = endTime - startTime
                print "Time elapsed: ", elapsed
                # read the output of the server (producer)
                result = server.stdout
                for c in clients:
                    # compare the output of each client with the server's
                    if (result != c.stdout):
                        print ""
                        print "Test failed:"
                        print "Server count: " + result
                        print "Client count: " + c.stdout
                        break
                else:
                    print "Test passed!"




