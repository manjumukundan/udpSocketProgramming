/* Create a UDP Server that manages the Bool Loan Management System */

#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include"Database.h"
#include<stdbool.h>

#define LIBRARY "library.db"
#define INVENTORY "Inventory"
#define USERS "Users"

void isValidUser(ServerMessage* msg);
int validateISBN(char isbn[]);

int main (int argc, char* argv[])
{
	int sock; // socket descriptor
	struct sockaddr_in *udpServerAddr; // Local address
	struct sockaddr_in *udpClientAddr; // Client address
	unsigned short serverPort; // server port number
	int clientAddrLen;

	// check for correct no. of parameters
	if (argc != 2)	
	{
		perror("\nINVALID NO. OF ARGUMENTS \n");
		exit(1);
	}
	
	serverPort = atoi(argv[1]);

	/* Create socket for sending and receiving datagram*/
	if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
	{
		perror("\nSOCKET CREATION FAILED !!!!!!!!!! \n");
		exit(1);
	}
	
	udpServerAddr = (struct sockaddr_in *)malloc(sizeof(struct sockaddr_in *));	
	/* fill the server address structure with zeros*/
	memset(udpServerAddr, 0, sizeof(*udpServerAddr));
	/*Creat the server address structure*/
	udpServerAddr->sin_family = AF_INET;
	udpServerAddr->sin_addr.s_addr = htonl(INADDR_ANY); // any incoming interface
	udpServerAddr->sin_port = htons(serverPort); // Local Port
	
	/* bind to the local address */
	if (bind(sock, (struct sockaddr *)udpServerAddr, sizeof(*udpServerAddr) ) < 0)
	{
		perror("\nSOCKET BIND FAILURE !!!!!!!!!! \n");
		exit(1);
	}
	
	printf("\n************ Book Loan Management System Server Running *************** \n");
	
	/*Create or open database*/
	initLibraryDatabase();

	udpClientAddr = (struct sockaddr_in *)malloc(sizeof(struct sockaddr_in *));
	memset(udpClientAddr, 0, sizeof(*udpClientAddr));
	
	ClientMessage * clientMsg = malloc(sizeof(ClientMessage));	
	memset(clientMsg, 0, sizeof(*clientMsg));
	
	/* Block until receive a message from clients */

	int validISBN = 0;
	bool validUser = 0;

	while (1)
	{
		printf("\nWaiting for Clients to connect ....................... \n");

		clientAddrLen = sizeof(struct sockaddr_in *);
		if (recvfrom(sock, clientMsg, sizeof(*clientMsg), 0, (struct sockaddr *)udpClientAddr, 
			&clientAddrLen) < 0)	
		{
			perror("RECEIVE MESSAGE FROM CLIENT FAILED !!!!!!!!! \n");
			exit(1);
		}

		ServerMessage * serverMsg = NULL;
		serverMsg = malloc(sizeof(ServerMessage));
		memset(serverMsg, 0, sizeof(*serverMsg));
		serverMsg->requestID = clientMsg->requestID;
		serverMsg->userID = clientMsg->userID;

		// check database if valid user id.
		isValidUser(serverMsg);
		if (serverMsg->responseType == Okay)
			validUser = 1;
		else
			validUser = 0;

		if (validUser)
		{
			int requestType = -1;
			switch (clientMsg->requestType)
			{
				case Login:
					if (serverMsg->loginStatus == 1)
					{
						validUser = 0;
						serverMsg->responseType = InvalidLogin;
					}
					else
					{
						validUser = 1;
						serverMsg->responseType = Okay;
						serverMsg->loginStatus = 1;
						updateUsersLoginStatus(serverMsg);
					}
					break;

				case Query:
					requestType = 0;
					validISBN = validateISBN(clientMsg->isbn);
					if (validISBN)
					{
						serverMsg->responseType = NoInventory;
						//check if isbn has an inventory, if yes populate msg.
						retrieveBook(serverMsg, clientMsg->isbn);
					}
					else
					{
						serverMsg->responseType = ISBNError;
					}
					break;

				case Borrow:
					requestType = 1;
					validISBN = validateISBN(clientMsg->isbn);
					if (validISBN)
					{
						serverMsg->responseType = NoInventory;
						//check if isbn has an inventory, if yes populate msg.
						retrieveBook(serverMsg, clientMsg->isbn);
						updateBook(serverMsg, clientMsg->isbn, requestType);
					}
					else
					{
						serverMsg->responseType = ISBNError;
					}
					break;

				case Return:
					requestType = 2;
					validISBN = validateISBN(clientMsg->isbn);
					if (validISBN)
					{
						serverMsg->responseType = NoInventory;
						//check if isbn has an inventory, if yes populate msg.
						retrieveBook(serverMsg, clientMsg->isbn);
						updateBook(serverMsg, clientMsg->isbn, requestType);
					}
					else
					{
						serverMsg->responseType = ISBNError;
					}
					break;

				case Logout:
						if (serverMsg->loginStatus == 1)
						{
							serverMsg->loginStatus = 0;
							updateUsersLoginStatus(serverMsg);
						}
					break;

				default:
					break;

			}

		}
		else
		{
			serverMsg->responseType = InvalidLogin;
		}

		if (sendto(sock, serverMsg, sizeof(*serverMsg), 0, (struct sockaddr *)udpClientAddr, 
				sizeof(*udpClientAddr)) < 0)
		{
			perror("SEND DATA FAILED !!!!!!!! \n");
		}
	}	
}

void isValidUser(ServerMessage* msg)
{
	msg->responseType = InvalidLogin;
	retrieveUsers(msg);
}

int validateISBN(char isbn[])
{
	long checksum = 0;
	int i, mul = 1, sum = 0, m10, j;

	int len = strlen(isbn);
	if(len != 13)
		return -1;

	for(i=0; i<len-1; i++)
	{
		char c = isbn[i];
		int val = atoi(&c);
		sum += val * mul;
	    if(mul==3)
	    	mul=1;
	    else
	    	mul = 3;
	}
	m10 = sum%10;

	char checkDigit = isbn[len - 1];
	checksum = atoi(&checkDigit);

	int sub = 10 - m10;
	if (m10 == 0)
		sub = 0;

	if(sub == checksum)
		return 1;
	else
		return 0;
}
