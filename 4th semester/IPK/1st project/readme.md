# IPK 2022 - 1. Projekt
#### Autor: Pavel Heřmann
#### Login: xherma34

## Project info

The task was to create a C server that communicates using the http protocol. I implemented the initialization of the server mostly according to the presentation of the IPK course. The server responds to the queries: hostname, cpu-name and load. 
The program is compiled via the make command, then an executable file hinfosvc is created, to run it is necessary to specify a port number, where port is an integer.
##### Compile
`$ make`

##### Run
`$ ./hinfosvc PORT`

