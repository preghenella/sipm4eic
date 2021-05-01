import socket
import numpy as np
import os
import matplotlib.pyplot as plt
import argparse

sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server_address = ('131.154.12.91', 5025)
idn_tag = "KEITHLEY INSTRUMENTS,MODEL 2450,04474830,1.6.7c"

echo = False
log = True
commands = []

def parse_arguments():
    
    ### choices
    boards = ['SENSL', 'BCOM', 'FBK']
    temperatures = [293, 283, 273, 263, 253, 243]
    channels = [row + column
                for row in ['A', 'B', 'C', 'D', 'E', 'F', 'G', 'H']
                for column in ['1', '2', '3', '4']]
    
    ### arguments
    parser = argparse.ArgumentParser(description='Keithley ivscan program',
                                     formatter_class=argparse.ArgumentDefaultsHelpFormatter)

    parser.add_argument('--board',
                        type=str,
                        required=True,
                        choices=boards,
                        help='Board name')
    
    parser.add_argument('--serial',
                        type=int,
                        required=True,
                        help='Board serial number')
    
    parser.add_argument('--temperature',
                        type=int,
                        required=True,
                        choices=temperatures,
                        help='Temperature in Kelvin')
    
    parser.add_argument('--channel',
                        type=str,
                        required=True,
                        choices=channels,
                        help='Board channel')
    
    parser.add_argument('--notes',
                        type=str,
                        required=False,
                        default=None,
                        action='append',
                        nargs='+',
                        help='Additional notes')
    
    args = parser.parse_args()

    return args

def build_tagname(args):
    tagname = args.board
    tagname += '_sn' + str(args.serial)
    tagname += '_' + str(args.temperature) + 'K'
    tagname += '_' + args.channel
    if args.notes is not None:
        for notes in args.notes:
            for note in notes:
                tagname += '_' + note
    return tagname

def connect():
    sock.connect(server_address)

def close():
    sock.close()

def write_commands(filename):
    fout = open(filename, 'w')
    for cmd in commands:
        fout.write('%s \n' % (cmd))
    fout.close()
    print('--- commands written to file: {filename}'.format(filename = filename))
    
def send(command):
    if log:
        commands.append(command)
    if echo:
        print (command)
    command += '\n'
    sock.send(command.encode())

def recv():
    data = sock.recv(4096)
    while data[-1] != '\n':
        data += sock.recv(4096)
    return data.strip()
        
def initialize():
    send('ABORT')
    send('*RST')
    send('STAT:CLE')
    send('SYST:CLE')
    send('*IDN?')
    data = recv()
    while data != idn_tag:
        data = recv()
    print('--- instrument initialised: %s ' % data)

def read_config(filename = 'config.keithley'):
    print("--- read configuration from file: %s" % filename)
    for line in open(filename):
        li = line.strip()
        if len(li) == 0 or li.startswith("#"):
            continue
        send(line.strip())

def output_on():
    send('OUTP:STAT ON')

def output_off():
    send('OUTP:STAT OFF')
            
def set_voltage(V):
    send('SOUR:VOLT:LEV {V}'.format(V = V))

def measure(n):
    send('TRAC:CLEAR')
    send('SENS:COUN {n}'.format(n = n))
    send('TRAC:TRIG')
    send('TRAC:DATA? 1, {n}, \"defbuffer1\", READ'.format(n = n))

def write_measurements(measurements, filename, ioflags = 'w'):
    fout = open(filename, ioflags)
    if os.stat(filename).st_size == 0:
        fout.write('# timestamp , source voltage , measured current , status , source status \n')
    for meas in measurements:
        fout.write('%f %e %e %s %s \n' % (meas['SEC'] + meas['FRAC'], meas['SOUR'], meas['READ'], meas['STAT'], meas['SOURSTAT']))
    fout.close()
    print('--- measurements written to file: {filename}'.format(filename = filename))
    
def read_measurements():
    send('TRAC:ACT?')
    n = int(recv())
    print('--- reading measurements: there are %d points in the buffer' % n)
    if n == 0:
        return []
    send('TRAC:DATA? 1, {n}, \"defbuffer1\", SEC, FRAC, REL, SOUR, READ, STAT, SOURSTAT'.format(n = n))
    data = recv().split(',')
    return [{ 'SEC'      : int(data[i]),
              'FRAC'     : float(data[i+1]),
              'REL'      : float(data[i+2]),
              'SOUR'     : float(data[i+3]),
              'READ'     : float(data[i+4]),
              'STAT'     : data[i+5],
              'SOURSTAT' : data[i+6]}
            for i in range(0, len(data), 7)]

def plot_measurements(measurements, xkey, ykey, ax, invertX = False, invertY = False):

    xval = []
    yval = []
    for meas in measurements:
        xval.append(meas[xkey])
        yval.append(meas[ykey])
    if invertX:
        xval = [-x for x in xval]
    if invertY:
        yval = [-x for x in yval]
    ax.scatter(xval, yval)
    plt.grid()
    plt.draw()
    plt.pause(0.1)

def measure_config(name = 'MEASURE_CONFIG'):
    print('--- configuring the measure list')

    send('SENS:CURR:AVER:COUNT 10')
    send('SENS:CURR:AVER:TCON REP')
    send('SENS:CURR:AVER ON')
    send('SENS:CONF:LIST:STOR \"{name}\"'.format(name = name))

    send('SENS:CURR:AVER:COUNT 10')
    send('SENS:CURR:AVER:TCON REP')
    send('SENS:CURR:AVER OFF')
    send('SENS:CONF:LIST:STOR \"{name}\"'.format(name = name))
    
def source_config(Vscan, Ilim = 25.e-6, name = 'SOURCE_CONFIG', verbose = False):
    
#    send('SOUR:CONF:LIST:DEL \"SOURCE_CONFIG\"')
    send('SOUR:CONF:LIST:CRE \"{name}\"'.format(name = name))

#    send('SOUR:VOLT:ILIM {Ilim}'.format(Ilim = Ilim))
    for V in Vscan:
#        send('SOUR:VOLT:RANG {V}'.format(V = V))
        send('SOUR:VOLT:LEV {V}'.format(V = V))
        send('SOUR:CONF:LIST:STOR \"{name}\"'.format(name = name))

    if (verbose):
        query_source_config(name)
        
def query_source_config(name):
    send('SOUR:CONF:LIST:SIZE? \"{name}\"'.format(name = name))
    npoints = int(recv())
    print('--- source list configuration list')
    for point in range(1, npoints + 1):
        send('SOUR:CONF:LIST:QUER? \"{name}\", {point}'.format(name = name, point = point))
        print('--- source configuration for point #{point}'.format(point = point))
        for conf in recv().split(','):
            print(conf)
                
def trigger_init():
    print('--- starting the trigger model')
    send('INIT')

def trigger_wait():
    print('--- waiting for the trigger model to terminate')
    send('*WAI')
                
def trigger_config(Twait = 60., Tstep = 1., Tmeas = 1., Nmeas = 'INF', name = 'SOURCE_CONFIG', verbose = False):
    print('--- configuring the trigger model: Twait = {Twait}, Tstep = {Tstep}, Tmeas = {Tmeas}, Nmeas = {Nmeas}'.format(Twait = Twait, Tstep = Tstep, Tmeas = Tmeas, Nmeas = Nmeas))
    
    ### retrive size of source configuration list
    send('SOUR:CONF:LIST:SIZE? \"{name}\"'.format(name = name))
    npoints = int(recv())

    ### configure the trigger model
    send('TRIG:LOAD \"EMPTY\"')
    send('TRIG:BLOC:BUFF:CLEAR 1, \"defbuffer1\"')
    send('TRIG:BLOC:CONF:REC 2, \"{name}\", 1'.format(name = name))
    send('TRIG:BLOC:SOUR:STAT 3, ON')
    send('TRIG:BLOC:DEL:CONS 4, {Twait}'.format(Twait = Twait))
    send('TRIG:BLOC:BRAN:ALW 5, 8')
    send('TRIG:BLOC:CONF:NEXT 6, \"{name}\"'.format(name = name))
    send('TRIG:BLOC:DEL:CONS 7, {Tstep}'.format(Tstep = Tstep))
    send('TRIG:BLOC:BRAN:EVEN 8, SLIM, 15')
    send('TRIG:BLOC:MEAS 9, \"defbuffer1\", {Nmeas}'.format(Nmeas = Nmeas))
    send('TRIG:BLOC:DEL:CONS 10, {Tmeas}'.format(Tmeas = Tmeas))
    send('TRIG:BLOC:MEAS 11, \"defbuffer1\", 0')
    send('TRIG:BLOC:BRAN:COUN 12, {npoints}, 6'.format(npoints = npoints))
    send('TRIG:BLOC:SOUR:STAT 13, OFF')
    send('TRIG:BLOC:BRAN:ALW 14, 0')
    send('TRIG:BLOC:SOUR:STAT 15, OFF')

    ### print the setting of the trigger model
    if (verbose):
        query_trigger_config()

    return


    send('SENS:CONF:LIST:CRE \"MEASURE_CONFIG\"')

    send('SENS:CURR:AVER:COUNT 10')
    send('SENS:CURR:AVER:TCON REP')
    send('SENS:CURR:AVER ON')
    send('SENS:CONF:LIST:STOR \"MEASURE_CONFIG\"')

    send('SENS:CURR:AVER:COUNT 10')
    send('SENS:CURR:AVER:TCON REP')
    send('SENS:CURR:AVER OFF')
    send('SENS:CONF:LIST:STOR \"MEASURE_CONFIG\"')


    ### configure the trigger model
    ##### initialisation blocks
    send('TRIG:LOAD \"EMPTY\"')
    send('TRIG:BLOC:BUFF:CLEAR 1, \"defbuffer1\"')
    send('TRIG:BLOC:CONF:REC 2, \"SOURCE_CONFIG\", 1')
    send('TRIG:BLOC:CONF:REC 3, \"MEASURE_CONFIG\", 1')
    send('TRIG:BLOC:SOUR:STAT 4, ON')
    send('TRIG:BLOC:BRAN:COUN:RES 5, 8')
    send('TRIG:BLOC:MEAS 6, \"defbuffer1\"')
    send('TRIG:BLOC:BRAN:DELT 7, 1.e9, 9, 6')
    send('TRIG:BLOC:BRAN:COUN 8, 1, 5')
    send('TRIG:BLOC:CONF:REC 9, \"MEASURE_CONFIG\", 2')
    send('TRIG:BLOC:BRAN:ALW 10, 12')
    ### measurement blocks
    send('TRIG:BLOC:CONF:NEXT 11, \"{name}\"'.format(name = name))
    send('TRIG:BLOC:BRAN:EVEN 12, SLIM, 19')
    send('TRIG:BLOC:MEAS 13, \"defbuffer1\", INF')
    send('TRIG:BLOC:DEL:CONS 14, {Tmeas}'.format(Tmeas = Tmeas))
    send('TRIG:BLOC:MEAS 15, \"defbuffer1\", 0')
    send('TRIG:BLOC:BRAN:COUN 16, {npoints}, 11'.format(npoints = npoints))
    send('TRIG:BLOC:SOUR:STAT 17, OFF')
    send('TRIG:BLOC:BRAN:ALW 18, 0')
    send('TRIG:BLOC:SOUR:STAT 19, OFF')

    ### print the setting of the trigger model
    print('--- configuration list of the trigger model')
    send('TRIG:BLOC:LIST?')
    print(recv())

def query_trigger_config():
    print('--- configuration list of the trigger model')
    send('TRIG:BLOC:LIST?')
    print(recv())
    
