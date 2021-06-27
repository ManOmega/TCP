#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include "client.h"
#include "CRC16bit.h"
#include "CRC16bit.h"
/* the port client will be connecting to */
#define PORT 8080


/* max number of bytes we can get at once */
#define MAXDATASIZE 100 

/* number of test case */
#define NUMBEROFTESTCASE 4 

/* number of test case */
#define LENGTHOFTESTNAME 10 

/*  Data set */
#define NUMBEROFDATASET 3 

s4 main(s4 argc, s1 *argv[])
{
	s4 sockfd, numbytes;
	s1 buf[MAXDATASIZE];
	struct hostent *he;
	/* connector’s address information */
	struct sockaddr_in their_addr; 
	u2 CRC;	
	s1 tests1;
	u1 testu1;
	s2 tests2;
	u2 testu2;
	s4 tests4;
	u4 testu4;
	l4 testl4;
	l4 testlu4;
	s8 tests8;
	u8 testu8;
	
	 struct packet
    { 
         u1 FCT;
         u1 BANK;
         u1 DEVICE;
         u1 ID;
         u1 CMD;
         u1 LEN;
		 u1 DATA;
		 u1 CRC_HI;
		 u1 CRC_LO;
    };

/* while(loop < 3)
{ */
	if (argc != 2) 
	{
		fprintf(stderr,"usage: client hostname\n");
		exit(1);
	}

    /* get the host info */
	if ((he=gethostbyname(argv[1])) == NULL) 
	{ 	
		perror("gethostbyname");
		exit(1);
	}

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) 
	{
		perror("socket");
		exit(1);
	}

    /* host byte order */
	their_addr.sin_family = AF_INET; 
	/* short, network byte order */
	their_addr.sin_port = htons(PORT); 
	their_addr.sin_addr = *((struct in_addr *)he->h_addr);
	/* zero the rest of the struct */
	bzero(&(their_addr.sin_zero), 8); 

	if (connect(sockfd, (struct sockaddr *)&their_addr, sizeof(struct sockaddr)) == -1) 
	{
		perror("connect");
		exit(1);
	}
	
	struct packet pkt;

    printf("Started Creating packet\n");
    pkt.FCT = 0x0A;
    pkt.BANK = 0xF2;
    pkt.DEVICE = 0x31;
    pkt.ID = 0xFE;
	
	printf("pkt.FCT = %x\n", pkt.FCT);
	printf("pkt.BANK = %x\n", pkt.BANK);
	printf("pkt.DEVICE = %x\n", pkt.DEVICE);
	printf("pkt.ID = %x\n", pkt.ID);
	printf("pkt.CMD = %x\n", pkt.CMD);
	printf("pkt.LEN = %x\n", pkt.LEN);
	printf("pkt.DATA = %x\n", pkt.DATA);
	printf("pkt.CRC_HI = %x\n", pkt.CRC_HI);
	printf("pkt.CRC_LO = %x\n", pkt.CRC_LO);	
			
	CRC = crc16((u1 *)&pkt, (sizeof(pkt)-2));
	printf("CRC = %x\n", CRC);
	
	pkt.CRC_LO = CRC;
	pkt.CRC_HI = CRC >> 8;
	
	/* Send notification of data amount */
	s4 pktlength = sizeof(pkt);
	printf("pktlength = %d\n", pktlength);
	
	if ((send(sockfd, &pkt, sizeof(pkt),0)) < 0)
	{	
		fprintf(stderr,"Error writing data to socket.\n");	
	}	
		
	if ((numbytes=recv(sockfd, buf, MAXDATASIZE - 1, 0)) == -1) 
	{
		perror("recv");
		exit(1);
	}

    printf("numbytes= %d\n", numbytes);
	buf[numbytes] = '\0';
	printf("Received: %s\n",buf);

	close(sockfd);

	return 0;

}