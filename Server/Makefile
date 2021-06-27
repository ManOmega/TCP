# Makefile for TCP/IP test
# Save the file with name "Makefile"

$(CC) = gcc

server:		server.o CRC16bit.o
				$(CC) server.o CRC16bit.o -o server
             
server.o:		server.c 
					$(CC)  -c server.c
					
CRC16bit.o:		CRC16bit.c CRC16bit.h
						$(CC) -c CRC16bit.c 
						                                   
clean:
				rm *.o server          
