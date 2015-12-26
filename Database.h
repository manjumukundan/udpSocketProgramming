#include<stdio.h>
#include<stdlib.h>
#include<sqlite3.h>
#include<stdbool.h>

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

typedef struct
{
	unsigned int requestID; /* unique client identifier*/
	unsigned int userID;/*unique user id*/
	enum RequestType{Login, Logout, Query, Borrow, Return} requestType;
	char isbn[14];/*book's validated ISBN-13*/
} ClientMessage;

int initLibraryDatabase();

sqlite3 * openDatabase();

int callback(void *data, int argc, char **argv, char **colName);

int createInventoryTable();

int createUsersTable();

void retrieveUsers(ServerMessage* msg);

int usersCallback(void *data, int argc, char **argv, char **colName);

int updateUsersLoginStatus(ServerMessage * smsg);

void retrieveBook(ServerMessage* smsg, char * clientISBN);

int updateBook(ServerMessage* smsg, char* isbn, int reqType);


