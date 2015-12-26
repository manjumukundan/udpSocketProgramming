/* Create a UDP Client that requests for a book from a UDP Server that maintains the book loan/library system using the book ISBN */

#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<sys/types.h>
#include<stdbool.h>


typedef struct
{
	unsigned int requestID; /* unique client identifier*/
	unsigned int userID;/*unique user id*/
	enum RequestType{Login, Logout, Query, Borrow, Return} requestType;
	char isbn[14];/*book's validated ISBN-13*/
} ClientMessage;

typedef struct
{
	unsigned int requestID; /* unique client id */
	unsigned int userID; /* unique user identifier*/
	bool loginStatus;
	enum ResponseType{Okay, ISBNError, AllGone, NoInventory, InvalidLogin}responseType;
	char isbn[14]; /* book ISBN-13*/
	char authors[100]; /* book author*/
	char title[100]; /* book title*/
	unsigned int edition; /* book publication year*/
	unsigned int year; /* book publication year*/
	char publisher[100]; /* book authors*/
	unsigned int inventory; /* inventory count*/
	unsigned int available; /* number of available books*/
} ServerMessage;

int sendMessageToServer(int sock, ClientMessage * msg);
int receiveDataFromServer(int sock, ServerMessage* serverMsg);

struct sockaddr_in *udpServerAddr; // server address details - ip and port number

int main(int argc, char *argv[])
{
	int sock; // socket descriptor

	char *serverIP; // server IP address
	unsigned short serverPort; // server port number
	int userId = 0;

	if (argc < 2 || argc > 3)
	{
		perror("INVALID NO. OF ARGUMENTS");
		exit(1);
	}

	serverIP = argv[1];
	serverPort = atoi(argv[2]);
	srand(time(NULL)); // set up the seed for the random generator

	/* allocate pointer for udp server addr structure*/
	udpServerAddr = (struct sockaddr_in *)malloc(sizeof(struct sockaddr_in *));
	/*  fill the structure with zeros */
	memset(udpServerAddr, 0, sizeof(*udpServerAddr));
	/* construct the server address structure. */
	udpServerAddr->sin_family = AF_INET;
	udpServerAddr->sin_addr.s_addr = inet_addr(serverIP);
	udpServerAddr->sin_port = htons(serverPort);
	
	/* Create a UDP socket to connect*/
	if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP )) < 0)
	{
		perror("\nSOCKET CREATION FAILED !!!!!!!!!!!!!\n");
		exit(1);
	}		
	
	printf("\nConnected to Book Loan Management System Server\n");

	/* ******************* LOGIN OPERATION START ******************* */

	/* Allocate for the client message to be sent*/
	ClientMessage *clientMsg = (ClientMessage*)malloc(sizeof(ClientMessage));
	memset(clientMsg, 0, sizeof(*clientMsg));

 	// Create the structure for receiving the server response
	ServerMessage* serverMsg = (ServerMessage *)malloc(sizeof(ServerMessage));
	memset(serverMsg, 0, sizeof(*serverMsg));

	do
	{
		printf("Enter a Valid User id to login to Server\n");
		printf("User ID: ");
		scanf("%d", &userId);
		clientMsg->userID = userId;
		clientMsg->requestID = rand() % 100 + 1;
		clientMsg->requestType = Login;

		/* Send message to login to server*/
		sendMessageToServer(sock, clientMsg);

		/*wait for  receive response from server*/
		receiveDataFromServer(sock, serverMsg);

		if (serverMsg->responseType == InvalidLogin)
		{
			printf("\n INVALID LOGIN !!!!!!!!!\n");
		}
	}
	while (serverMsg->responseType != Okay);

	printf("\n Logged in Successfully \n");
	printf("\n Welcome to book Loan Management System \n");

	/* ******************* LOGIN OPERATION END ******************* */



	/* ******************* LOAN OPERATIONS START ******************* */
	int option;
	char isbn[14] = "\0";

	while(1)
	{
		printf("\n======================================================================\n");
		printf("\nPlease select following options to continue: \n");
		printf("1) Query\n");
		printf("2) Check-out\n");
		printf("3) Check-in\n");
		printf("4) Logout\n");
		do
		{

			printf("\nEnter a valid a option: ");
			scanf("%d", &option);
		}
		while (option < 1 && option > 4);

		printf("\n======================================================================\n");

		switch (option)
		{
			case 1:// Query
				printf("\nPlease enter the ISBN number of the book to QUERY: ");
				scanf("%s", isbn);

				//memset(clientMsg, 0, sizeof(*clientMsg));
				clientMsg->requestID = rand() % 100 + 1;
				clientMsg->requestType = Query;

				memcpy(clientMsg->isbn, isbn, strlen(isbn));

				/*send query message to server*/
				sendMessageToServer(sock, clientMsg);
				/*receive response from server*/
				receiveDataFromServer(sock, serverMsg);

				if (serverMsg->requestID == clientMsg->requestID
						&& serverMsg->responseType == Okay)
				{
					printf("\nPLEASE FIND THE DETAILS OF REQUESTED BOOK: \n");
					printf("ISBN : %s\n", serverMsg->isbn);
					printf("Title : %s\n", serverMsg->title);
					printf("Author(s) : %s\n", serverMsg->authors);
					printf("Edition : %d\n", serverMsg->edition);
					printf("Year Published : %d\n", serverMsg->year);
					printf("Publisher : %s\n", serverMsg->publisher);
					printf("Available : %d\n", serverMsg->available);
					printf("Inventory Count : %d\n", serverMsg->inventory);

				}
				else if (serverMsg->requestID == clientMsg->requestID
							&& serverMsg->responseType == ISBNError)
				{
					printf("\nISBN ERROR: INVALID ISBN !!!!!!!!!\n");
				}

				else if (serverMsg->requestID == clientMsg->requestID
						&& serverMsg->responseType == NoInventory)
				{
					printf("\nSORRY, NO INVENTORY FOR REQUESTED BOOK\n");
				}
				else
				{
					printf("\nINVALID RESPONSE FROM SERVER !!!\n");
				}
				break;

			case 2: // Borrow
				printf("\nPlease enter the ISBN number of the book to CHECKOUT: ");
				scanf("%s", isbn);

				clientMsg->requestID = rand() % 100 + 1;
				clientMsg->requestType = Borrow;
				memcpy(clientMsg->isbn, isbn, strlen(isbn));

				/*send query message to server*/
				sendMessageToServer(sock, clientMsg);
				/*receive response from server*/
				receiveDataFromServer(sock, serverMsg);
				if (serverMsg->requestID == clientMsg->requestID
						&& serverMsg->responseType == Okay)
				{
					printf("\nREQUESTED BOOK IS CHECKED OUT. THANK YOU\n");
				}
				else if (serverMsg->requestID == clientMsg->requestID
							&& serverMsg->responseType == ISBNError)
				{
					printf("\nISBN ERROR: INVALID ISBN !!!!!!!!!\n");
				}
				else if (serverMsg->requestID == clientMsg->requestID
						&& serverMsg->responseType == AllGone)
				{
					printf("\nSORRY, REQUESTED BOOK IS NOT AVAILABLE FOR CHECKOUT.\n");
				}

				else if (serverMsg->requestID == clientMsg->requestID
						&& serverMsg->responseType == NoInventory)
				{
					printf("\nSORRY, NO INVENTORY FOR REQUESTED BOOK\n");
				}
				else
				{
					printf("\nINVALID RESPONSE FROM SERVER !!!\n");
				}
				break;

			case 3:// Return
				printf("\nPlease enter the ISBN number of the book to CHECKIN: ");
				scanf("%s", isbn);

				clientMsg->requestID = rand() % 100 + 1;
				clientMsg->requestType = Return;
				memcpy(clientMsg->isbn, isbn, strlen(isbn));

				/*send query message to server*/
				sendMessageToServer(sock, clientMsg);
				/*receive response from server*/
				receiveDataFromServer(sock, serverMsg);

				if (serverMsg->requestID == clientMsg->requestID
						&& serverMsg->responseType == Okay)
				{
					printf("\nREQUESTED BOOK IS CHECKED IN. THANK YOU\n", isbn);
				}
				else if (serverMsg->requestID == clientMsg->requestID
						&& serverMsg->responseType == ISBNError)
				{
					printf("\nISBN ERROR: INVALID ISBN\n");
				}
				else if (serverMsg->requestID == clientMsg->requestID
										&& serverMsg->responseType == NoInventory)
				{
					printf("\nINVENTORY IS FULL.\n");
				}
				else
				{
					printf("\nINVALID RESPONSE FROM SERVER !!!\n");
				}
				break;

			case 4: // Logout
				clientMsg->requestID = rand() % 100 + 1;
				clientMsg->requestType = Logout;
				/*send query message to server*/
				sendMessageToServer(sock, clientMsg);
				/*receive response from server*/
				receiveDataFromServer(sock, serverMsg);

				if (serverMsg->requestID == clientMsg->requestID
						&& serverMsg->responseType == Okay)
				{
					printf("\nLOGGING OUT. THANK YOU\n");
					exit(1);
				}
				else
				{
					printf("\nINVALID RESPONSE FROM SERVER !!!\n");
				}
				break;

			default:
				break;
		}
	}
	return 0;
}
	 

int sendMessageToServer(int sock, ClientMessage * msg)
{
	int sendLen = -1;

	// send message to server
	sendLen = sendto(sock, msg, sizeof(*msg), 0, (struct sockaddr *)udpServerAddr,
		sizeof(*udpServerAddr));
	
	if (sendLen < 0)
	{
		perror(" SENDING DATA FAILED !!!!!!!!!!!!!");
		exit(1);
	}

	return sendLen;
}

int receiveDataFromServer(int sock, ServerMessage* serverMsg)
{
	int responseSize = -1;
	int serverAddrLen = -1;
	struct sockaddr_in *responseAddr; // response source address

    /*allocate for the response address structure */
	responseAddr = (struct sockaddr_in *)malloc(sizeof(struct sockaddr_in *));
	memset(responseAddr, 0, sizeof(*responseAddr));

	/* Loop recvfrom till a valid data received */
	while (1)
	{
		serverAddrLen = sizeof(struct sockadr_in *);
		responseSize = recvfrom(sock, serverMsg, sizeof(*serverMsg), 0, (struct sockaddr *)responseAddr,
					&serverAddrLen);
		if (responseSize > 0)
		{
			break;
		}
		else
		{
			perror("RECEIVING RESPONSE MESSAGE FAILED !!!!!!!!!");
			exit(1);
		}
	}

	// check whether the response is from valid source
	if (responseAddr->sin_addr.s_addr != udpServerAddr->sin_addr.s_addr)
	{
		perror("RESPONSE MESSAGE IS FROM AN UNKNOWN SOURCE !!!!!!!!!");
		exit(1);
	}

	return responseSize;
}
