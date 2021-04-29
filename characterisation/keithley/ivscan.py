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

### hardcoded settings (valid for SensL)
Vbd = 24.5
Vlim = 35.0

### output file run tag
outfiletagname = ''
for i in range(1, len(sys.argv)):
    outfiletagname += sys.argv[i]
    if (i < len(sys.argv) - 1):
        outfiletagname += '_'

### start
starttime = time.time()
print('--- Keithley \'ivscan\' program started: %s' % outfiletagname)

### add time tag
timenow = datetime.datetime.now()
timetag = timenow.strftime('%Y%m%d%H%M%S')
outfiletagname += '.' + timetag

### define plots
plt.ion()
fig, ax = plt.subplots(1, 1, figsize=(5, 5))
ax.set_title('IV scan')
ax.set_xlabel('Vbias (V)')
ax.set_ylabel('I (A)')
ax.set_yscale('log')
fig.tight_layout()

### connect and initialize
ky.connect()
ky.initialize()
ky.read_config()

#########################
### REVERSE BIAS SCAN ###
#########################

### [Vbd - 5, Vbd - 1] in 200 mV steps
Vmin  = Vbd - 5.
Vmax  = Vbd - 1.
Vstep = 0.200
Vscan = np.arange(Vmin, Vmax, Vstep)
### [Vbd - 1, Vbd + 2] in 50 mV steps
Vmin  = Vbd - 1.
Vmax  = Vbd + 2.
Vstep = 0.050
Vscan = np.append(Vscan, np.arange(Vmin, Vmax, Vstep))
### [Vbd + 2, Vlim] in 200 mV steps
Vmin  = Vbd + 2.
Vmax  = Vlim
Vstep = 0.200
Vscan = np.append(Vscan, np.arange(Vmin, Vmax, Vstep))

### configure source and trigger model
ky.source_config(Vscan = -Vscan)
ky.trigger_config(Twait = 30., Tstep = 1., Tmeas = 0., Nmeas = 25)

### start trigger model and wait
ky.trigger_init()
ky.trigger_wait()

### read measurements
measurements = ky.read_measurements()

### plot measurements
ky.plot_measurements(measurements, 'SOUR', 'READ', ax, invertX = True, invertY = True);

### write measurements
ky.write_measurements(measurements, outfiletagname + '.ivscan.csv')

### write commands
ky.write_commands(outfiletagname + '.ivscan.scpi')

### close connection
ky.close()

### done
endtime = time.time()
print('--- Keithley \'ivscan\' program completed: %f seconds' % (endtime - starttime))

### wait for plot
print('--- plot is ready and saved: waiting for 5 seconds')
plt.savefig(outfiletagname + '.ivscan.png')
time.sleep(5)
