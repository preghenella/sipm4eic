 #! /usr/bin python

import time
import socket
import sys
import numpy as np
import math

import matplotlib.pyplot as plt

# Create a TCP/IP socket
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# Connect the socket to the port where the server is listening
server_address = ('131.154.12.91', 5025)
#server_address = ('127.0.0.1', 8393)
#print >>sys.stderr, 'connecting to %s port %s' % server_address
sock.connect(server_address)

### output file tagname
outfiletagname = ''
for i in range(1, len(sys.argv)):
    outfiletagname += sys.argv[i]
    if (i < len(sys.argv) - 1):
        outfiletagname += '_'
print ("--- Keithley IV scan program started: tagename = %s" % outfiletagname)
        
### plots
plt.ion()
fig, ax = plt.subplots(1, 4, figsize = (16, 4))
axtitle = ["coarse scan", "wide scan", "fine scan", "forward scan"]
axtitlex = ["Vbias (V)", "Vbias (V)", "Vbias (V)", "Vforward (V)"]
axlog = [True, True, True, False]
for i in range(4):
    ax[i].set_title(axtitle[i])
    ax[i].set_xlabel(axtitlex[i])
    ax[i].set_ylabel('I (A)')
    if (axlog[i]):
        ax[i].set_yscale('log')
fig.tight_layout()

def initialize(filename):

    print("--- initializing from file: %s" % filename)
    sock.send('ABORT\n')
    sock.send('*RST\n')
    sock.send('*IDN?\n')
    data = sock.recv(256).strip()
    print (data)
    
    ### configure
    for line in open(filename):
        li = line.strip()
        if not li.startswith("#"):
#            print line.rstrip()
            sock.send(line)

def scan(Vmin, Vmax, Ilim, Vstep, Twait, Nave):
    print ("--- starting scan: Vbias = [%f, %f] V | Ilim = %f A | Vstep = %f V | Twait = %s s | Nave = %d" % (Vmin, Vmax, Ilim, Vstep, Twait, Nave, fPlot ))
    measurements = []

    ### move to Vmin, turn on and wait few seconds
    sock.send('SOUR:VOLT:ILIM {I}\n'.format(I = Ilim))
    sock.send('SOUR:VOLT:LEV {V}\n'.format(V = Vmin))
    sock.send('OUTPUT:STAT ON\n')
    time.sleep(3);

    ### Initialize Plot variables
    fPlotCurrent = []
    fPlotVoltage = []
    
    ### start the scan
    Vscan = np.arange(Vmin, Vmax, Vstep)
    Vscan = np.append(Vscan, Vmax)
    for Vbias in Vscan :
        ### set voltage and wait
        sock.send('SOUR:VOLT:LEV {V}\n'.format(V = Vbias))
        time.sleep(Twait)
        ### measure current averaging
        currents = []
        for i in range(0, Nave) :
            sock.send('MEAS:CURR?\n')
            current = float(sock.recv(256).strip())
            sock.send('SOUR:VOLT:ILIM:TRIP?\n')
            trip = sock.recv(256).strip()
            if (trip == '1' or math.fabs(current) > Ilim):
                sock.send('OUTPUT:STAT OFF\n')
                sock.send('SOUR:VOLT:LEV 0.\n')
                sock.send('SOUR:VOLT:ILIM 100.e-6\n')
                return measurements
            currents.append(current)
            
        meas = { "voltage" : float(Vbias) , "current" : np.mean(currents) , "stddev" : np.std(currents) }
        measurements.append(meas)
        
        fPlotVoltage.append(meas["voltage"])
        fPlotCurrent.append(meas["current"])
        fPlot.scatter(fPlotVoltage, fPlotCurrent, color="blue", marker="x")
        plt.draw()
        plt.pause(0.1)
    
    ### turn off and move to 0 V
    sock.send('OUTPUT:STAT OFF\n')
    sock.send('SOUR:VOLT:LEV 0.\n')
    sock.send('SOUR:VOLT:ILIM 100.e-6\n')

    return measurements

def estimate_Vbd(measurements, Vmax):

    ### compute derivatives and find first max in the slope
    points = []
    for i in range(len(measurements) - 1):
        center = 0.5 * (measurements[i]["voltage"] + measurements[i + 1]["voltage"])
        deltax = measurements[i + 1]["voltage"] - measurements[i]["voltage"]
        deltay = math.log(measurements[i + 1]["current"]) - math.log(measurements[i]["current"])
        slope = deltay / deltax
        points.append((center, slope))

    points.sort(key = lambda x : x[1], reverse = True)
    for point in points:
        if (point[0] < Vmax):
            return point[0]

    return 0.

def write_measurements(measurements, filename):

    fout = open(filename, "w")
    for meas in measurements:
        fout.write("%e %e\n" % (meas["voltage"], meas["current"]))
    fout.close()

def plot_measurements(measurements, ax, fact = 1.):

    x = []
    y = []
    for meas in measurements:
        x.append(fact * meas["voltage"])
        y.append(fact * meas["current"])
    ax.scatter(x, y)
    plt.draw()
    plt.pause(0.1)
    
try:

    initialize("initialize.keithley")
    start = time.time()

    ### coarse scan
    coarse_start = time.time()
    measurements = scan(Vmin = 10., Vmax = 100, Ilim = 250.e-6, Vstep = 1., Twait = 0.1, Nave = 1, fPlot = ax[0])
    write_measurements(measurements, outfiletagname + '.coarse.cvs')
    plot_measurements(measurements, ax[0])
    Vbd = estimate_Vbd(measurements, 30.)
    coarse_end = time.time()
    print("--- coarse scan took %f seconds: estimated Vbd = %f" % (coarse_end - coarse_start, Vbd))

    ### wide scan
    wide_start = time.time()
    measurements = scan(Vmin = Vbd - 5., Vmax = 100., Ilim = 250.e-6, Vstep = 0.2, Twait = 1., Nave = 3, fPlot = ax[1])
    write_measurements(measurements, outfiletagname + '.wide.cvs')
    plot_measurements(measurements, ax[1])
    Vbd = estimate_Vbd(measurements, 30.)
    wide_end = time.time();
    print("--- wide scan took %f seconds: estimated Vbd = %f" % (wide_end - wide_start, Vbd))

    ### fine scan
    fine_start = time.time()
    measurements = scan(Vmin = Vbd - 1., Vmax = Vbd + 2., Ilim = 250.e-6, Vstep = 0.05, Twait = 1., Nave = 10, fPlot = ax[2])
    write_measurements(measurements, outfiletagname + '.fine.cvs')
    plot_measurements(measurements, ax[2])
    Vbd = estimate_Vbd(measurements, 30.)
    fine_end = time.time();
    print("--- fine scan took %f seconds: estimated Vbd = %f" % (fine_end - fine_start, Vbd))

    ### forward scan
    forward_start = time.time()
    measurements = scan(Vmin = 0., Vmax = -3., Ilim = 25.e-3, Vstep = -0.1, Twait = 1., Nave = 3, fPlot = ax[3])
    write_measurements(measurements, outfiletagname + '.forward.cvs')
    plot_measurements(measurements, ax[3], -1.)
    forward_end = time.time();
    print("--- forward scan took %f seconds" % (forward_end - forward_start))
    
    ### completed
    end = time.time()
    print("--- whole process took %f seconds: so long" % (end - start))
    
finally:
#    print >>sys.stderr, 'closing socket'
    sock.close()
    plt.waitforbuttonpress()
    
