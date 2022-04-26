Project: implementation of a network analyzer in C++,\
Author: xshcha00, Shchapaniak Andrei, [xshcha00@stud.fit.vutbr.cz](xshcha00@stud.fit.vutbr.cz)

## Description
Sniffer is able to capture and filter packets
on a certain network interface. This project was implemented in C++ language, because it has more useful
functionality which makes programming more efficient. The program must be run with sudo privileges to allow
raw packet captures and network interface manipulation.

## Compilation
```bash
make
```

## Running
```bash
./ipk-sniffer [-i interface|--interface interface] {-p ­­port} {[--tcp|-t] [--udp|-u] [--arp] [--icmp] } {-n num}
```

## Examples
```bash
./ipk-sniffer -i eth0 -p 23 --tcp -n 2
./ipk-sniffer -i eth0 --udp
./ipk-sniffer -i eth0 -n 10      
./ipk-sniffer -i eth0 -p 22 --tcp --udp --icmp --arp
./ipk-sniffer -i eth0 -p 22
./ipk-sniffer -i eth0
```

## Submitted files
* sniffer.cpp
* disassembler.cpp
* disassembler.h
* Makefile
* packets.py
* README.md
* manual.pdf
