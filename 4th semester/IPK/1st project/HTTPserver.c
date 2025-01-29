#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>

#define MAX_SIZE 1024

void outputOkay(int socket)
{
	char header[MAX_SIZE] = "HTTP/1.1 200 OK \r\nContent-Type: text/plain; \r\n\r\n";
	write(socket, header, strlen(header));
}

void outputBadReq(int socket)
{
	//TODO jestli ma vypsat hlasku 400 bad request
	char header[MAX_SIZE] = "HTTP/1.1 400 BAD REQUEST \r\n\r\n";
	write(socket, header, strlen(header));
}

void getHostName(int socket)
{
	char buffer[MAX_SIZE];
	//Open file and get host name
	FILE *fp = fopen("/proc/sys/kernel/hostname", "r");
	//Check for error in fopen
	if(fp == NULL)
	{
		perror("Hostname file is not readable");
		exit(1);
	}
	fgets(buffer, MAX_SIZE, fp);
	//Add 200 OK header
	outputOkay(socket);
	//Write response
	write(socket, buffer, strlen(buffer));
	fclose(fp);
}

void getCpuName(int socket)
{
	char buffer[MAX_SIZE];
	char outputBuffer[MAX_SIZE];

	int i = 0;

	//Get procesor info
	FILE *fp = fopen("/proc/cpuinfo", "r");
	if(fp == NULL)
	{
		perror("Cpuinfo file is not readable");
		exit(1);
	}

	//Get line with model name
	while(strncmp(buffer, "model name", 10))
	{
		fgets(buffer, MAX_SIZE, fp);
	}

	//Write okay header to socket
	outputOkay(socket);

	//Skip to :
	while(buffer[i] != ':')
	{
		i++;
	}
	//Add 2 to i, to skip ':' and space between
	i = i+2;

	int x = 0;

	//Copy name of cpu into outputBuffer
	while(buffer[i] != '\n')
	{
		outputBuffer[x] = buffer[i];
		x++;
		i++;
	}

	//add whitespaces for write
	outputBuffer[x] = '\n';
	outputBuffer[x+1] = '\0';

	write(socket, outputBuffer, strlen(outputBuffer));
	fclose(fp);
}

void getCpuLoad(int socket)
{
	char buffer[MAX_SIZE];
	char *holder[11];

	//Get cpu token
	FILE *fp = fopen("/proc/stat", "r");
	if(fp == NULL)
	{
		perror("Cpustats file is not readable");
		exit(1);
	}

	//Write okay header to socket
	outputOkay(socket);

	//Init
	long double user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nic;  

	user= nice= system= idle= iowait= irq= softirq= steal= guest= guest_nic = 0;

	//Get stats
	fgets(buffer, 1024, fp);

	sscanf(buffer, "cpu %Lf %Lf %Lf %Lf %Lf %Lf %Lf %Lf %Lf %Lf", 
			&user, &nice, &system, &idle, &iowait, &irq, &softirq, &steal, &guest, &guest_nic);


	long double prevIdle, idleCount, prevNonIdle, nonIdle, prevTotal, total, totald, idled;

	prevIdle = idle + iowait;
	prevNonIdle = user + nice + system + irq + softirq + steal;
	prevTotal = prevIdle + prevNonIdle;

	long double cpuPercentage;

	fclose(fp);

	sleep(1);

	memset(buffer, 0, MAX_SIZE);
	
	//Second open of the file to get new values
	FILE *fpp = fopen("/proc/stat", "r");
	if(fpp == NULL)
	{
		perror("Cpustats file is not readable");
		exit(1);
	}

	//Get stats
	fgets(buffer, 1024, fpp);

	sscanf(buffer, "cpu %Lf %Lf %Lf %Lf %Lf %Lf %Lf %Lf %Lf %Lf", 
			&user, &nice, &system, &idle, &iowait, &irq, &softirq, &steal, &guest, &guest_nic);

	idleCount = idle + iowait; 
	nonIdle = user + nice + system + irq + softirq + steal;
	total = idleCount + nonIdle;

	totald = total - prevTotal;
	idled = idleCount - prevIdle;

	cpuPercentage = (totald - idled)/totald;
	cpuPercentage = cpuPercentage*100;

	char outputBuffer[MAX_SIZE];

	fclose(fpp);

	sprintf(outputBuffer, "%.2Lf\n", cpuPercentage);

	write(socket, outputBuffer, strlen(outputBuffer));

}

void handle(int socket)
{

	char readBuffer[MAX_SIZE];

	read(socket, readBuffer, sizeof(readBuffer));

	if(!strncmp(readBuffer, "GET /hostname", 13))
	{
		getHostName(socket);
	}
	else if(!strncmp(readBuffer, "GET /cpu-name", 13))
	{
		getCpuName(socket);
	}
	else if(!strncmp(readBuffer, "GET /load", 9))
	{
		getCpuLoad(socket);
	}
	else
	{
		outputBadReq(socket);
	}
	
}

int main(int argc, char const *argv[])
{ 

	if(argc != 2)
	{
		fprintf(stderr, "Error: program must be run with an argument \n ./hinfosvc PORTNUMBER \n");
		exit(1);
	}

	//TODO PORT RANGE
	//Get port from argument
	int portArg = atoi(argv[1]);

	char buffer[MAX_SIZE];

	//Socket initialization
	int serverSocket, clientSocket;
	if((serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
	{
		perror("Socket ERROR");
		exit(1);
	}

	//Address setup
	struct sockaddr_in serverAddress, clientAddress;
	socklen_t clientAddressLen = sizeof(clientAddress);
	socklen_t serverAddressLen = sizeof(serverAddress);	
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(portArg);
	serverAddress.sin_addr.s_addr = INADDR_ANY;

	//Bind socketu
	if(bind(serverSocket, (struct sockaddr *) &serverAddress, serverAddressLen) < 0)
	{
		perror("Bind ERROR");
		exit(1);
	}

	//Listen
	if(listen(serverSocket, 0) < 0)
	{
		perror("Listen ERROR");
		exit(1);
	}

	while (1)
	{
		clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddress, &clientAddressLen);
		handle(clientSocket);
		close(clientSocket);
	}

	return 0;
}
