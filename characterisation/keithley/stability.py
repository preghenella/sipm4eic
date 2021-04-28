#! /usr/bin/env python

import keithley as ky
import sys
import datetime

### check input arguments
if len(sys.argv) < 2:
    print('--- ERROR: need at least one argument')
    exit()

### output file tagname
outfiletagname = ''
for i in range(1, len(sys.argv)):
    outfiletagname += sys.argv[i]
    if (i < len(sys.argv) - 1):
        outfiletagname += '_'
print('--- Keithley stability scan program started: tagname = %s' % outfiletagname)

### add time tag
timenow = datetime.datetime.now()
timetag = timenow.strftime('%Y%m%d%H%M%S')
outfiletagname += '.' + timetag

### connect and initialize
ky.connect()
ky.initialize()
ky.read_config()

######################
### STABILITY SCAN ###
######################

Vscan = [-27.]
ky.source_config(Vscan = Vscan)
ky.trigger_config(Twait = 0., Tstep = 0., Tmeas = 60., Nmeas = 'INF')

### continuous scan
while True:

    ### start trigger model and wait
    ky.trigger_init()
    ky.trigger_wait()

    ### read measurements and append to file
    measurements = ky.read_measurements()
    ky.write_measurements(measurements, outfiletagname + '.stability.csv', 'a+')

ky.close()
