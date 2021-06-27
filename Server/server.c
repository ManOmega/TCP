#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include "server.h"
#include "CRC16bit.h"

/* the port users will be connecting to */
#define MYPORT 8080 
/* how many pending connections queue will hold */
#define BACKLOG 10 

	
void sigchld_handler(s4 s)
{
	while(wait(NULL) > 0);
}

s4 main(void)
{
	/* listen on sock_fd, new connection on new_fd */
	s4 sockfd, new_fd; 
	/* my address information */
	struct sockaddr_in my_addr; 
	/* connector’s address information */
	struct sockaddr_in their_addr; 
	s4 sin_size;
	struct sigaction sa;
	s4 yes=1;
    s4 messagelength = 0;
	s4 bytecount = 0;
    u2 CRC = 0;
	u2 Raw_CRC = 0;
	
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
	
    struct packet pkt;
	
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) 
	{
		perror("socket");
		exit(1);
	}

	if (setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(s4)) == -1) 
	{
		perror("setsockopt");
		exit(1);
	}

    /* host byte order */
	my_addr.sin_family = AF_INET; 
	/* short, network byte order */
	my_addr.sin_port = htons(MYPORT); 
	/* automatically fill with my IP */
	my_addr.sin_addr.s_addr = INADDR_ANY; 
	/* zero the rest of the struct */
	bzero(&(my_addr.sin_zero), 8); 

	if (bind(sockfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) == -1) 
	{
		perror("bind");
		exit(1);
	}

	if (listen(sockfd, BACKLOG) == -1) 
	{
		perror("listen");
		exit(1);
	}

    /* reap all dead processes */
	sa.sa_handler = sigchld_handler; 
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;

	if (sigaction(SIGCHLD, &sa, NULL) == -1) 
	{
		perror("sigaction");
		exit(1);
	}

    /* main accept() loop */
	while(1) 
	{ 	
		sin_size = sizeof(struct sockaddr_in);
		if ((new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size)) == -1) 
		{
			perror("accept");
			continue;
		}
		printf("server: got connection from %s\n",
		inet_ntoa(their_addr.sin_addr));
		
		/* this is the child process */
		if (!fork()) 
		{ 	
			 /* child doesn’t need the listener */
			 close(sockfd); 
				
			bytecount = recv(new_fd, &pkt, sizeof(pkt), 0);
			printf("bytecount = %d\n",bytecount);
			
			CRC = crc16((u1 *)&pkt, (sizeof(pkt)-2));
			printf("CRC = %x\n", CRC);			
			printf("sizeof(pkt) = %ld\n", sizeof(pkt));	
	
 			Raw_CRC = pkt.CRC_HI;
			Raw_CRC = (Raw_CRC << 8) | pkt.CRC_LO;
			printf("Raw_CRC = %x\n", Raw_CRC);
			
			if(CRC != Raw_CRC)
			{
				perror("invalid CRC\n");		
				 /* indicate client, done testing*/
			    if (send(new_fd, "Invalid CRC\n", 14, 0) == -1)
			    {
				    perror("send");
			    }				
			}
			else
			{
				 /* indicate client, done testing*/
			    if (send(new_fd, "Test1 is done\n", 14, 0) == -1)
			    {
				    perror("send");
			    }				
			}	
			
		    printf("pkt.FCT = %x\n", pkt.FCT);
			printf("pkt.BANK = %x\n", pkt.BANK);
			printf("pkt.DEVICE = %x\n", pkt.DEVICE);
		    printf("pkt.ID = %x\n", pkt.ID);
			printf("pkt.CMD = %x\n", pkt.CMD);
			printf("pkt.LEN = %x\n", pkt.LEN);
		    printf("pkt.DATA = %x\n", pkt.DATA);
			printf("pkt.CRC_HI = %x\n", pkt.CRC_HI);
			printf("pkt.CRC_LO = %x\n", pkt.CRC_LO);			
						
			/* release connection */
			close(new_fd);
			
			exit(0);
		}
		
		/* parent doesn’t need this */
		close(new_fd); 
		
	}

	return 0;

}