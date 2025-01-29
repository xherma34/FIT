### IPK - 2. Projekt: Sniffer Paketů
### Name: Pavel Heřmann
### Login: xherma34
### How to start the program:
``$make`` compiles the program and creates a binary file <br>
``$./ipk-sniffer [-i [interface]| --interface [interface]] {-p port} {[--tcp|-t][--udp|-u][--arp][--icmp]}{-n num}``
#### Run arguments:
- Run without arguments: lists the available interfaces on which packets can be sniffed
- **-i | --interface**: specifies the interface on which the sniffer will catch packets if the only argument is
, it lists all available interfaces
- **-p port**: where port is the number of port
- **--tcp | -t**: shows tcp packets
- **--udp | -u**: shows udp packets
- **--arp**: shows arp packets
- **--icmp**: shows icmp packets
- **-n num**: where num is a number of packets to be sniffed 
<br>
<br>
arguments for the packet protocol specification can be combined (--tcp|-t, --udp|-u, --arp, --icmp) <br>
arguments can be in any order <br>

#### Files:
**manual.pdf**: documentation of the project <br>
**ipk-sniffer.c**: source code <br>
**Makefile**: makefile for compilation <br>