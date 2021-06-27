# Makefile for TCP/IP test
# Save the file with name "Makefile"

$(CC) = gcc

client:	client.o CRC16bit.o
				$(CC) client.o CRC16bit.o -o client
             
client.o:	client.c 
				$(CC)  -c client.c 

CRC16bit.o:		CRC16bit.c CRC16bit.h
						$(CC) -c CRC16bit.c
					
clean:
			rm *.o client          
