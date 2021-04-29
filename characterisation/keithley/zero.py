#! /usr/bin/env python

import keithley as ky
import sys
import numpy as np
import matplotlib.pyplot as plt
import datetime
import time

ky.echo = False
ky.log = True

### check input arguments
if len(sys.argv) < 2:
    print('--- ERROR: need at least one argument')
    exit()

### output file run tag
outfiletagname = ''
for i in range(1, len(sys.argv)):
    outfiletagname += sys.argv[i]
    if (i < len(sys.argv) - 1):
        outfiletagname += '_'

### start
starttime = time.time()
print('--- Keithley zero program started: %s' % outfiletagname)

### add time tag
timenow = datetime.datetime.now()
timetag = timenow.strftime('%Y%m%d%H%M%S')
outfiletagname += '.' + timetag

### define plots
plt.ion()
fig, ax = plt.subplots(1, 1, figsize=(5, 5))
ax.set_title('zero volt offset')
ax.set_xlabel('time (s)')
ax.set_ylabel('I (A)')
ax.set_ylim([-2.e-9, 2.e-9])
fig.tight_layout()

### connect and initialize
ky.connect()
ky.initialize()
ky.read_config()

########################
### ZERO VOLT OFFSET ###
########################

### configure source and trigger model
ky.source_config(Vscan = [0.])
ky.trigger_config(Twait = 0., Tstep = 0., Tmeas = 30., Nmeas = 'INF')

### start trigger model and wait
ky.trigger_init()
ky.trigger_wait()

### read measurements
measurements = ky.read_measurements()

### plot measurements
ky.plot_measurements(measurements, 'REL', 'READ', ax, invertX = False, invertY = True);

### write measurements
ky.write_measurements(measurements, outfiletagname + '.zero.csv')

### write commands
ky.write_commands(outfiletagname + '.zero.scpi')

### close connection
ky.close()

### done
endtime = time.time()
print('--- Keithley zero program completed: %f seconds' % (endtime - starttime))

### wait for plot
print('--- plot is ready and saved: waiting for 5 seconds')
plt.savefig(outfiletagname + '.zero.png')
time.sleep(3)
