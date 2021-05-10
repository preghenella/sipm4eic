#! /usr/bin/env python

import argparse
parser = argparse.ArgumentParser()
parser.add_argument('--input', type=str, required=True)
parser.add_argument('--output', type=str, required=True)
args = parser.parse_args()
                                 
boards = ['SENSL', 'BCOM', 'FBK']
temperatures = [293, 283, 273, 263, 253, 243]
channels = [row + column
            for row in ['A', 'B', 'C', 'D', 'E', 'F', 'G', 'H']
            for column in ['1', '2', '3', '4']]

def read_data(filename):
    points = []
    point = {}
    with open(filename) as file:
        for line in file:
            li = line.strip()
            if len(li) == 0 or li.startswith("#"):
                continue
            data = li.split()
            if data[0] in boards:
                print('board header detected: %s' % li)
                point['board'] = data[0]
                point['serial'] = int(data[1])
                point['temp'] = float(data[2])
                point['vbias'] = float(data[3])
                point['vover'] = float(data[4])
                point['time'] = float(data[5])
                continue
            elif data[0] in channels:
                print('data from channel: %s' % li)
                point['channel'] = data[0]
                point['counts'] = int(data[1])
                points.append(point.copy())
                continue
            else:
                print('invalid line: %s' % li)
    return points

def write_data(points, filename):
    with open(filename, 'w') as file:
        file.write('board/C:serial/I:temp/I:vbias/F:vover/F:time/F:channel/C:counts/I\n')
        for point in points:
            file.write('%s %d %d %f %f %f %s %d\n' %
                       (point['board'], point['serial'], point['temp'], point['vbias'], point['vover'], point['time'], point['channel'], point['counts']))
    
points = read_data(args.input)
write_data(points, args.output)

