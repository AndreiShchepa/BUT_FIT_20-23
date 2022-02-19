Project: implementation of an HTTP server\
Author: xshcha00, Shchapaniak Andrei, [xshcha00@stud.fit.vutbr.cz](xshcha00@stud.fit.vutbr.cz)

## Description
The server provides various information about the system.
It listens to the specified port and returns the required information according to the URL.
The answer type is text/plain.
Communication with the server is possible using both a web browser and tools such as wget and curl.
It is possible to shut down the server using CTRL+C.

## Compilation
```bash
make
```

## Running
```bash
./hinfosvc <port>
```

## Examples
1. Obtaining a domain name
```bash
curl http://servername:12345/hostname
```
2. Obtaining information about the CPU
```bash
curl http://servername:12345/cpu-name
```
3. Obtaining actual stress of the CPU 
```bash
curl http://servername:12345/load
```
