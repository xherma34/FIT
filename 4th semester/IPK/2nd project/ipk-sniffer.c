#include <stdio.h>
#include <stdlib.h>
#include <pcap.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <netinet/ip.h>
#include <netinet/if_ether.h>
#include <netinet/ether.h>
#include <netinet/ip6.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <time.h>
//// source: https://docs.huihoo.com/doxygen/linux/kernel/3.7/structicmphdr.html
#include <linux/icmp.h>
//// source: https://linux.die.net/man/3/inet_ntoa
// pro prevod uint8_t k tisknut
#include <arpa/inet.h>


#define ETHERTYPE_IPV4 0x86DD

/**
 * @brief Funkce vypise vsechna dostupna rozhrani a ukonci program
 */
void devicesOnly()
{
	char errbuf[PCAP_ERRBUF_SIZE];
	pcap_if_t *interfaces;
	if(pcap_findalldevs(&interfaces, errbuf) == PCAP_ERROR)
	{
		printf("nebyl nalezen zadny interface\n");
		exit(1);
	}
	pcap_if_t *interface;
	char printme[100];
	printf("Dostupná zařízení:\n--------------------\n");
	for(interface = interfaces; interface != NULL; interface = interface->next)
	{
		printf("%s \n", interface->name);
	}
	printf("--------------------\n");
	exit(0);
}


/**
 * @brief funkce kontroluje zda jsou ve stringu pouze cisla
 * @param value
 * @return 1 pokud se jedna o cislo, jinak nula
 */
int isnumber(char *value)
{
	int len = strlen(value);
	for(int i = 0; i < len; i++)
	{
		if(!isdigit(value[i]))
		{
			return 0;
		}
	}
	return 1;
}


/**
 * Funkce zpracovava vstupni argumenty a nastavuje podle nich hodnoty promennych
 * @param argv argumenty zadany pri spusteni
 * @param argc pocet argumentu
 * @param tcp boolean
 * @param udp boolean
 * @param arp boolean
 * @param icmp boolean
 * @param port int
 * @param interface jmeno rozhrani
 * @param num pocet nacitanych packetu
 */
void argparse(char* argv[], int argc, bool *tcp, bool *udp, bool *arp, bool *icmp, int *port, char *interface, int *num)
{
	//Ze zadani nebylo jasne jak se ma chovat program, kdyz uzivatel napise napr. 3 argumenty a mezi nimi bude samotne -i
	//problem jsem vyresil tak, ze se nebude vubec "vycmuchavat" paket, ale pouze se vypisou dostupna zarizeni
	if((argc == 1) || (argc == 2 && (strcmp(argv[1], "-i") == 0)) || argc == 2 && (strcmp(argv[1], "--interface") == 0))
	{
		devicesOnly();
	}

	for(int i = 1; i < argc; i++)
	{
		if(!strcmp(argv[i], "-i") || !(strcmp(argv[i], "--interface")))
		{
			if(argv[i+1] != NULL)
			{
				strcpy(interface, argv[i+1]);
				i++;
			}
			else
			{
				fprintf(stderr, "ERROR: nebyl zadan interface\n");
				exit(1);
			}
		}
		else if(!strcmp(argv[i], "-p"))
		{
			if(argv[i+1] != NULL && isnumber(argv[i+1]))
			{
				*port = atoi(argv[i+1]);
				i++;
			}
			else
			{
				fprintf(stderr, "ERROR: port musi byt cislo\nport musi mit argument\n");
				exit(1);
			}
		}
		else if(!strcmp(argv[i], "--tcp") || !strcmp(argv[i], "-t"))
		{
			*tcp = true;
		}
		else if(!strcmp(argv[i], "--udp") || !strcmp(argv[i], "-u"))
		{
			*udp = true;
		}
		else if(!strcmp(argv[i], "--arp"))
		{
			*arp = true;
		}
		else if(!strcmp(argv[i], "--icmp"))
		{
			*icmp = true;
		}
		else if(!strcmp(argv[i], "-n"))
		{
			if(argv[i+1] != NULL && isnumber(argv[i+1]))
			{
				*num = atoi(argv[i+1]);
				i++;
			}
			else
			{
				fprintf(stderr, "ERROR: number musi byt cislo\nnumber musi mit argument\n");
				exit(1);
			}
		}
		else
		{
			fprintf(stderr, "ERROR: neplatny argument\n");
			exit(1);
		}
	}
	//Call funkce make filter
}


/**
 * tiskne mac adresy
 * @param etherHeader
 */
void printmacaddresses(struct ether_header *etherHeader)
{
	//// source: https://linux.die.net/man/3/ether_ntoa
	struct ether_addr *etherSource = (struct ether_addr*)etherHeader->ether_shost;
	struct ether_addr *etherDest = (struct ether_addr*)etherHeader->ether_dhost;
	printf("src MAC: %s\n", ether_ntoa(etherSource));
	printf("dst MAC: %s\n", ether_ntoa(etherDest));
}


/**
 * tiskne ip adresy podle toho jestli je ipv4 nebo ipv6
 * @param etherHeader
 * @param type
 * @param packet
 */
void printipaddresses(struct ether_header *etherHeader, char *type, const u_char *packet)
{
	struct ip6_hdr *ip6Header = (struct ip6_hdr*)(packet + sizeof(struct ether_header*));
	struct iphdr *ip4Header = (struct iphdr*)(packet + sizeof(struct ether_header));
	if(!strcmp(type, "ipv4"))
	{
		//// source https://www.dreamincode.net/forums/topic/363245-display-ip-address-from-iphdr-using-inet-ntoa/
		printf("src IP: %s\n", inet_ntoa(*(struct in_addr*)&ip4Header->saddr));
		printf("dst IP: %s\n", inet_ntoa(*(struct in_addr*)&ip4Header->daddr));
	}
	else if(!strcmp(type, "ipv6"))
	{
		//// source https://man7.org/linux/man-pages/man3/inet_ntop.3.html
		char src[INET6_ADDRSTRLEN];
		char dst[INET6_ADDRSTRLEN];
		inet_ntop(AF_INET6, &ip6Header->ip6_src, src, INET6_ADDRSTRLEN);
		inet_ntop(AF_INET6, &ip6Header->ip6_dst, dst, INET6_ADDRSTRLEN);
		printf("src IP: %s\n", src);
		printf("dst IP: %s\n", dst);
	}
}

/**
 * funkce tiskne udp porty
 * @param etherHeader
 * @param type ipv6/ipv4
 * @param packet
 * @param ipLen delka ipheaderu
 */
void printudpports(struct ether_header *etherHeader, char *type, const u_char *packet, int ipLen)
{
	if(!strcmp(type, "ipv4"))
	{
		//nacteni udphlavicky (offset: ethernetova hlavicka + delka ipv4 hlavicky)
		struct udphdr *udpHeader = (struct udphdr*)(packet + sizeof(struct ether_header) + (ipLen));
		printf("src port: %d\n", ntohs(udpHeader->uh_sport));
		printf("dst port: %d\n", ntohs(udpHeader->uh_dport));
	}
	else if(!strcmp(type, "ipv6"))
	{
		//nacteni udphlavicky (offset: ethernetova hlavicka + 40 fixnich bytu ipv6 hlavicky)
		struct udphdr *udpHeader = (struct udphdr*)(packet + sizeof(struct ether_header) + (40));
		printf("src port: %d\n", ntohs(udpHeader->uh_sport));
		printf("dst port: %d\n", ntohs(udpHeader->uh_dport));
	}
}


/**
 * funkce tiskne tcp porty
 * @param etherHeader
 * @param type ipv6/ipv4
 * @param packet
 * @param ipLen delka ipheaderu
 */
void printtcpports(struct ether_header *etherHeader, char *type, const u_char *packet, int ipLen)
{
	if(!strcmp(type, "ipv4"))
	{
		//nacteni tcphlavicky (offset: ethernetova hlavicka + delka ipv4 hlavicky)
		struct tcphdr *tcpHeader = (struct tcphdr*)(packet + sizeof(struct ether_header) + (ipLen));
		printf("src port: %d\n", ntohs(tcpHeader->th_sport));
		printf("dst port: %d\n", ntohs(tcpHeader->th_dport));
	}
	else if(!strcmp(type, "ipv6"))
	{
		//nacteni tcphlavicky (offset: ethernetova hlavicka + 40 fixnich bytu ipv6 hlavicky)
		struct tcphdr *tcpHeader = (struct tcphdr*)(packet + sizeof(struct ether_header) + (40));
		printf("src port: %d\n", ntohs(tcpHeader->th_sport));
		printf("dst port: %d\n", ntohs(tcpHeader->th_dport));
	}
}

//// source: https://gist.github.com/kevinz/3076999
//// author: kevinz
//// date: 23.04.2022
void printpayload(const u_char *payload, int len)
{
	const u_char *buffer = (const u_char *)payload;

	// posun po hexa bytech
	for(int i = 0; i < len; i = i+16)
	{
		if(i != 0)
		{
			//pro prvni radek
			printf("\n");
		}
		printf("0x%.4x: ", i);
		for(int j = i; j < i+16; j++)
		{
			if(j >= len)
			{
				//dotisknuti radku mezerami
				for(int k = 0; k < i+16-j; k++)
				{
					printf("   ");
				}
				break;
			}
			printf("%.2x ", buffer[j]);
		}
		for(int j = i; j < i+16; j++)
		{
			if(j >= len)
			{
				break;
			}
			if(isprint(buffer[j]))
			{
				printf("%c", buffer[j]);
			}
			else
			{
				printf(".");
			}

		}
	}
	printf("\n");
}

/**
 * funkce tiskne cas(timestamp) ve spravnem formatu
 * @param packetHeader ktery obsahuje casovy udaj
 */
void printtime(const struct pcap_pkthdr *packetHeader)
{
	//// source: https://stackoverflow.com/questions/2408976/struct-timeval-to-printable-format
	//unixovy cas v sekundach
	struct tm *unixTime = localtime(&packetHeader->ts.tv_sec);
	int ms = packetHeader->ts.tv_usec/1000;
	char time[50];
	char timeOffset[7];
	strftime(time, 50, "%Y-%m-%dT%H:%M:%S", unixTime);
	char msArray[10];

	//prevedu milisekundy na array a provedu konkatenaci s casem
	sprintf(msArray, ".%d", ms);
	strcat(time, msArray);

	strftime(timeOffset, 7, "%z", unixTime);
	//potrebuji dostat do offsetu :
	//ted vypada timeOffset takto:
	// '+/-''0''0''0''0''_''\0'
	timeOffset[5] = timeOffset[4];
	timeOffset[4] = timeOffset[3];
	timeOffset[3] = ':';

	//provedu konkatenaci s offsetem -> ziskavam vysledny tvar
	strcat(time, timeOffset);
	printf("timestamp: %s\n", time);
}


/**
 * funkce pro zpracovavani packetu (volana pcap_loop)
 * @param handle session
 * @param packetHeader hlavicka packetu
 * @param packet samotny packet
 */
void callbackfunc(u_char *handle, const struct pcap_pkthdr *packetHeader, const u_char *packet)
{
	//Inicializace promennych
	char iptype[100];
	strcpy(iptype, "none");
	int payloadSize = packetHeader->len;

	//nacteni ethernetove hlavicky
	struct ether_header *etherHeader = (struct ether_header* ) packet;
	//tiskne cas
	printtime(packetHeader);

	//// source: https://stackoverflow.com/questions/66732931/get-ip-version-from-packet-data
	// IPV6
	if(etherHeader->ether_type == ntohs(ETHERTYPE_IPV6))
	{
		//tiskne mac adresy
		printmacaddresses(etherHeader);
		//tiskne frame delku
		printf("frame length: %d bytes\n", packetHeader->len);
		strcpy(iptype, "ipv6");

		//nacteni ipv6 hlavicky (offset: ethernetova hlavicka)
		struct ip6_hdr *ipHeader = (struct ip6_hdr*)(packet + sizeof(struct ether_header*));

		//Tiskne ip adresy
		printipaddresses(etherHeader, iptype, packet);

		//// source https://en.wikipedia.org/wiki/List_of_IP_protocol_numbers
		//Potrebuji 20. byte, ktery obsahuje informaci o protokolu (14 ether + 6. byte ipv6 headeru)
		u_char ipv6Header = packet[20];
		// --------TCP IPV6----------
		if(ipv6Header == 6)
		{
			printf("--TCP--\n");
			printtcpports(etherHeader, iptype, packet, 40);
			//// source: https://gist.github.com/kevinz/3076999

			printpayload(packet, payloadSize);
		}
		// ----------UDP IPV6----------
		else if(ipv6Header == 17)
		{
			printudpports(etherHeader, iptype, packet, 40);

			printpayload(packet, payloadSize);
		}
		// ----------ICMP IPV6-----------
		else if(ipv6Header == 58)
		{

			printpayload(packet, payloadSize);
		}
	}
	// ---------------------------IPV4--------------------
	else if(etherHeader->ether_type == ntohs(ETHERTYPE_IP))
	{
		//tiskne mac adresy
		printmacaddresses(etherHeader);
		//tiskne frame delku
		printf("frame length: %d bytes\n", packetHeader->len);
		strcpy(iptype, "ipv4");

		//nacteni ipv4 hlavicky (offset: ethernetova hlavicka)
		struct iphdr *ipHeader = (struct iphdr*)(packet + sizeof(struct ether_header));

		//ulozeni delky ip hlavicky
		//// source https://stackoverflow.com/questions/8925885/determining-size-of-ip-header-why-is-it-different-from-others
		//IHL = internet header length -> v 32bit wordech (musi se nasobit 4 pro prevod na byte)
		int ipLen = ipHeader->ihl*4;

		//vytiskne ip adresy
		printipaddresses(etherHeader, iptype, packet);
		// ------------TCP IPV4----------
		if(ipHeader->protocol == 6)
		{
			//vytiskne porty
			printtcpports(etherHeader, iptype, packet, ipLen);

			//nacteni payload (offset: ethernetova hlavicka + delka ip hlavicky + tcp hlavicka)
			//const u_char *payload = (const u_char*)(packet + sizeof(struct ether_header) + ipLen + sizeof(struct tcphdr));
			printpayload(packet, payloadSize);
		}
		// ------------UDP IPV4-----------------
		else if(ipHeader->protocol == 17)
		{
			//vytiskne porty
			printudpports(etherHeader, iptype, packet, ipLen);

//			//nacteni payload (offset: ethernetova hlavicka + delka ip hlavicky + udp hlavicka)
//			const u_char *payload = (const u_char*)(packet + sizeof(struct ether_header) + ipLen + sizeof(struct udphdr));
			printpayload(packet, payloadSize);
		}
		else if(ipHeader->protocol == 1)
		{
			//nacteni payload (offset: ethernetova hlavicka + delka ip hlavicky + icmp hlavicka)
			//const u_char *payload = (const u_char*)(packet + 14 + sizeof(struct iphdr) + sizeof(struct icmphdr));
			printpayload(packet, payloadSize);
		}
		else
		{
			return;
		}
	}
	else if(etherHeader->ether_type == ntohs(ETHERTYPE_ARP))
	{
		//tiskne mac adresy
		printmacaddresses(etherHeader);
		//tiskne frame delku
		printf("frame length: %d bytes\n", packetHeader->len);
	}
	else
	{
		return;
	}
}


void setfilter(bool *tcp, bool *udp, bool *arp, bool *icmp, int *port, char *filter)
{
	if(*tcp && *udp && *arp && *icmp && *port == 0)
	{
		strcpy(filter, "none");
		return;
	}

	if(((*tcp && *udp && *arp && *icmp)||(!*tcp && !*udp && !*arp && !*icmp)) && *port != 0)
	{
		strcpy(filter, "port ");
		char portArr[15];
		sprintf(portArr, "%d ", *port);
		strcat(filter, portArr);
		return;
	}

	if(*port != 0)
	{
		strcpy(filter, "port ");
		char portArr[30];
		sprintf(portArr, "%d ", *port);
		strcat(filter, portArr);
		strcpy(portArr, filter);

		//tcp je vzdy na prvni pozici, nepotrebuji else
		if(*tcp && !strcmp(filter, portArr))
		{
			strcat(filter, "tcp ");
		}

		if(*udp && !strcmp(filter, portArr))
		{
			strcat(filter, "udp ");
		}
		else if(*udp)
		{
			strcat(filter, "or udp ");
		}

		if(*icmp && !strcmp(filter, portArr))
		{
			strcat(filter, "icmp ");
		}
		else if(*icmp)
		{
			strcat(filter, "or icmp ");
		}

		if(*arp && !strcmp(filter, portArr))
		{
			strcat(filter, "arp ");
		}
		else if(*arp)
		{
			strcat(filter, "or arp ");
		}
	}
	else
	{
		strcpy(filter, "none");

		if(*tcp && !strcmp(filter, "none"))
		{
			strcpy(filter, "tcp ");
		}

		if(*udp && !strcmp(filter, "none"))
		{
			strcpy(filter, "udp ");
		}
		else if(*udp)
		{
			strcat(filter, "or udp ");
		}

		if(*icmp && !strcmp(filter, "none"))
		{
			strcpy(filter, "icmp ");
		}
		else if(*icmp)
		{
			strcat(filter, "or icmp ");
		}

		if(*arp && !strcmp(filter, "none"))
		{
			strcpy(filter, "arp ");
		}
		else if(*arp)
		{
			strcat(filter, "or arp ");
		}
	}
}

/**
 * --------------------------- MAIN ---------------------------
 */
int main(int argc, char* argv[])
{
	bool tcp = false, udp = false, arp = false, icmp = false;
	char interface[100];
	char filter[100];
	int num = 1, port = 0;
	strcpy(interface, "none");

	argparse(argv, argc, &tcp, &udp, &arp, &icmp, &port, interface, &num);
	setfilter(&tcp, &udp, &arp, &icmp, &port, filter);

	//TODO ODSTRANIT NA ODEVZDAVANI, JEN PRO TESTOVANI FILTERU
//	printf("--------------------------------------------------\n");
//	printf("interface name je: %s\n", interface);
//	printf("port number je: %d\n", port);
//	printf("packet number je: %d\n", num);
//	printf(tcp ? "tcp true\n" : "tcp false\n");
//	printf(udp ? "udp true\n" : "udp false\n");
//	printf(arp ? "arp true\n" : "arp false\n");
//	printf(icmp ? "icmp true\n" : "icmp false\n");
//	printf("FILTER MA FORMAT: %s\n", filter);
//	printf("--------------------------------------------------\n");

	// --OTEVRENI SESSION--
	char errbuf[PCAP_ERRBUF_SIZE];
	pcap_t *handle;
	handle = pcap_open_live(interface, BUFSIZ, 0, 1000, errbuf);
	if(handle == NULL)
	{
		fprintf(stderr, "ERROR: nelze otevrit device\n");
		fprintf(stderr, "%s\n", errbuf);
		exit(1);
	}

	//TODO FILTER
	if(strcmp(filter, "none"))
	{
		struct bpf_program fp;
		bpf_u_int32  net;
		if(pcap_compile(handle, &fp, filter, 0, net) == -1)
		{
			fprintf(stderr, "ERROR: chyba pri kompilaci filteru\n");
			fprintf(stderr, "%s", errbuf);
			exit(1);
		}

		if(pcap_setfilter(handle, &fp) == -1)
		{
			fprintf(stderr, "ERROR: chyba pri nastavovani filteru\n");
			fprintf(stderr, "%s", errbuf);
			exit(1);
		}
	}

	//loop pres packety
	pcap_loop(handle, num, callbackfunc, NULL);
	pcap_close(handle);
	return 0;
}


/**
 * Prace s knihovnou PCAP je inspirovana https://www.tcpdump.org/pcap.html
 * Autor: Tim Carstens
 *
 * sniffex.c
 *
 * Sniffer example of TCP/IP packet capture using libpcap.
 *
 * Version 0.1.1 (2005-07-05)
 * Copyright (c) 2005 The Tcpdump Group
 *
 * This software is intended to be used as a practical example and
 * demonstration of the libpcap library; available at:
 * http://www.tcpdump.org/
 *
 ****************************************************************************
 *
 * This software is a modification of Tim Carstens' "sniffer.c"
 * demonstration source code, released as follows:
 *
 * sniffer.c
 * Copyright (c) 2002 Tim Carstens
 * 2002-01-07
 * Demonstration of using libpcap
 * timcarst -at- yahoo -dot- com
 *
 * "sniffer.c" is distributed under these terms:
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 4. The name "Tim Carstens" may not be used to endorse or promote
 *    products derived from this software without prior written permission
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 * <end of "sniffer.c" terms>
 *
 * This software, "sniffex.c", is a derivative work of "sniffer.c" and is
 * covered by the following terms:
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Because this is a derivative work, you must comply with the "sniffer.c"
 *    terms reproduced above.
 * 2. Redistributions of source code must retain the Tcpdump Group copyright
 *    notice at the top of this source file, this list of conditions and the
 *    following disclaimer.
 * 3. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 4. The names "tcpdump" or "libpcap" may not be used to endorse or promote
 *    products derived from this software without prior written permission.
 *
 * THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.
 * BECAUSE THE PROGRAM IS LICENSED FREE OF CHARGE, THERE IS NO WARRANTY
 * FOR THE PROGRAM, TO THE EXTENT PERMITTED BY APPLICABLE LAW.  EXCEPT WHEN
 * OTHERWISE STATED IN WRITING THE COPYRIGHT HOLDERS AND/OR OTHER PARTIES
 * PROVIDE THE PROGRAM "AS IS" WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED
 * OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  THE ENTIRE RISK AS
 * TO THE QUALITY AND PERFORMANCE OF THE PROGRAM IS WITH YOU.  SHOULD THE
 * PROGRAM PROVE DEFECTIVE, YOU ASSUME THE COST OF ALL NECESSARY SERVICING,
 * REPAIR OR CORRECTION.
 *
 * IN NO EVENT UNLESS REQUIRED BY APPLICABLE LAW OR AGREED TO IN WRITING
 * WILL ANY COPYRIGHT HOLDER, OR ANY OTHER PARTY WHO MAY MODIFY AND/OR
 * REDISTRIBUTE THE PROGRAM AS PERMITTED ABOVE, BE LIABLE TO YOU FOR DAMAGES,
 * INCLUDING ANY GENERAL, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES ARISING
 * OUT OF THE USE OR INABILITY TO USE THE PROGRAM (INCLUDING BUT NOT LIMITED
 * TO LOSS OF DATA OR DATA BEING RENDERED INACCURATE OR LOSSES SUSTAINED BY
 * YOU OR THIRD PARTIES OR A FAILURE OF THE PROGRAM TO OPERATE WITH ANY OTHER
 * PROGRAMS), EVEN IF SUCH HOLDER OR OTHER PARTY HAS BEEN ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGES.
 * <end of "sniffex.c" terms>
 */