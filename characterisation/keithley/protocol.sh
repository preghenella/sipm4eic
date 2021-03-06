#! /usr/bin/env bash

declare -A valid_boards
for I in SENSL BCOM HAMA1 HAMA2 FBK; do
    valid_boards[$I]=1
done

declare -A valid_temperatures
for I in 293 283 273 263 253 243; do
    valid_temperatures[$I]=1
done

declare -A valid_channels
for I in A B C D E F G H; do
    for J in 1 2 3 4; do
	valid_channels[$I$J]=1
    done
done

banner() {
    echo "====================================================="
    echo
    echo " SiPM Keithley protocol"
    echo " make sure that your input respects the convention"
    echo
    echo " [board] [serial] [temperature] [channel]"
    echo
    echo " express temperature in Kelvin (forget decimals)"
    echo " following this as an example to start the protocol"
    echo
    echo " ./protocol.sh SENSL 2 293 A1"
    echo
    echo " valid board names are"
    echo
    echo " SENSL"
    echo " BCOM"
    echo " HAMA1"
    echo " HAMA2"
    echo " FBK"
    echo
    echo " valid temperatures are"
    echo
    
    echo " valid channel names are"
    echo
    echo " A1 A2 A3 A4"
    echo " B1 B2 B3 B4"
    echo " ..........."
    echo " H1 H2 H3 H4"
    echo
    echo "====================================================="
}

banner

### check input arguments
if [ $# -lt 4 ]; then
    echo
    echo " insufficient arguments"
    echo
    echo " usage: ./protocol.sh [board] [serial] [temperature] [channel]"
    echo
    exit 1
fi

board=$1
serial=$2
temperature=$3
channel=$4

### check valid board name
if [[ ! ${valid_boards[$board]} ]]; then
    echo
    echo " invalid board name: $board"
    echo
    echo " usage: ./protocol.sh [board] [serial] [temperature] [channel]"
    echo
    exit 1
fi

### check valid temperature
if [[ ! ${valid_temperatures[$temperature]} ]]; then
    echo
    echo " invalid temperature: $temperature"
    echo
    echo " usage: ./protocol.sh [board] [serial] [temperature] [channel]"
    echo
    exit 1
fi

### check valid channel name
if [[ ! ${valid_channels[$channel]} ]]; then
    echo
    echo " invalid channel name: $channel"
    echo
    echo " usage: ./protocol.sh [board] [serial] [temperature] [channel]"
    echo
    exit 1
fi

### start protocol

echo "====================================================="
echo
echo " starting protocol"
echo
echo "      board  = ${board}"
echo "      serial = #${serial}"
echo " temperature = ${temperature}K"
echo "     channel = ${channel}"
echo
echo "====================================================="

arg="${board} sn${serial} ${temperature}K ${channel}"
tag="${board}_sn${serial}_${temperature}K_${channel}"
mkdir -p $tag

echo "====================================================="
echo

./zero.py --board $board --serial $serial --temperature $temperature --channel $channel
mv $tag.zero.* $tag/.


echo
echo "====================================================="
echo

./ivscan.py --board $board --serial $serial --temperature $temperature --channel $channel
mv $tag.ivscan.* $tag/.

echo
echo "====================================================="
echo

./fwdscan.py --board $board --serial $serial --temperature $temperature --channel $channel
mv $tag.fwdscan.* $tag/.

echo
echo "====================================================="
echo

if [ -d "/data/sipm4eic" ]; then
    echo
    echo " copying everything to /data/sipm4eic"
    mkdir -p /data/sipm4eic/DISORDER
    cp -rp $tag /data/sipm4eic/DISORDER/.
fi

### end protocol

echo
echo "====================================================="
echo
echo " ending protocol"
echo
echo " press ctrl-c to interrupt"
echo
echo "====================================================="

### beep and sleep

while true; do
    echo -e '\a'
    sleep 1
done
