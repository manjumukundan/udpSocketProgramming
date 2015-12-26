#include<stdio.h>
#include<stdlib.h>
#include<sqlite3.h>
#include"Database.h"

int initLibraryDatabase()
{
	int res = 0;
	res = createInventoryTable();
	res = createUsersTable();
	return res;
}

sqlite3 * openDatabase()
{
	sqlite3 *db;

	int res = sqlite3_open("library.db", &db);

	if (res!= SQLITE_OK)
	{

		fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);

		return NULL;
	}
	return db;
}

int createInventoryTable()
{
	sqlite3 * db = openDatabase();
	if (NULL != db)
	{
		char *sql_stmt = "SELECT * FROM Inventory";
		char *err_msg = 0;

		int res = sqlite3_exec(db, sql_stmt, 0, 0, &err_msg);

		if (res != SQLITE_OK )
		{

			fprintf(stderr, "Failed to select data\n");
			fprintf(stderr, "SQL error: %s\n", err_msg);

			printf("\n Creating table Inventory.........\n");
			sql_stmt = "CREATE TABLE IF NOT EXISTS Inventory(Id INTEGER PRIMARY KEY AUTOINCREMENT, ISBN_13 INTEGER NOT NULL, Authors TEXT NOT NULL, "
										"Title TEXT NOT NULL, Edition INTEGER, Year_Published INTEGER, Publisher TEXT, Copies_in_Inventory INTEGER,"
										" Copies_Available_for_Loan INTEGER);"
						"INSERT INTO Inventory (ISBN_13, Authors, Title, Edition, Year_Published, Publisher, Copies_in_Inventory, Copies_Available_for_Loan)"
							" VALUES(9780132126953, 'Andrew Tanenbaum, David Wetherall', 'Computer Networks', 5, 2011, 'Prentice-Hall', 5, 2);"
						"INSERT INTO Inventory (ISBN_13, Authors, Title, Edition, Year_Published, Publisher, Copies_in_Inventory, Copies_Available_for_Loan)"
							" VALUES(9780123745408, 'Michael Donahoo, Kenneth Calvert', 'TCP/IP Sockets in C', 2, 2009, 'Morgan Kaufman', 3, 0);"
						"INSERT INTO Inventory (ISBN_13, Authors, Title, Edition, Year_Published, Publisher, Copies_in_Inventory, Copies_Available_for_Loan)"
							" VALUES(9780133354690, 'William Stallings', 'Cryptography and Network Security', 6, 2014, 'Prentice-Hall', 3, 3);"
						"INSERT INTO Inventory (ISBN_13, Authors, Title, Edition, Year_Published, Publisher, Copies_in_Inventory, Copies_Available_for_Loan)"
							" VALUES(9780072467505, 'Yale Patt, Sanjay Patel', 'Introduction to Computing Systems from bits & gates to C & beyond',"
							"2, 2004, 'Prentice-Hall', 1, 0);";

			res = sqlite3_exec(db, sql_stmt, 0, 0, &err_msg);
			if (res != SQLITE_OK)
			{
				fprintf(stderr, "Failed to create table Inventory\n");
				fprintf(stderr, "SQL error: %s\n", err_msg);
			}
			else
			{
				printf("Inventory table successfully created !!!!!!!!!!!");
			}

			sqlite3_free(err_msg);
			sqlite3_close(db);

			return 1;
		}
	}

	sqlite3_close(db);
	return 0;

}


int createUsersTable()
{
	sqlite3 * db = openDatabase();
	if (NULL != db)
	{
		char *sql_stmt = "SELECT * FROM Users";
		char *err_msg = 0;

		int res = sqlite3_exec(db, sql_stmt, 0, 0, &err_msg);

		if (res != SQLITE_OK )
		{

			fprintf(stderr, "Failed to select data\n");
			fprintf(stderr, "SQL error: %s\n", err_msg);

			printf("\n Creating table Users.........\n");
			sql_stmt = "CREATE TABLE IF NOT EXISTS Users(Id INTEGER PRIMARY KEY AUTOINCREMENT, User_ID INTEGER, Login_Status INTEGER);"
										"INSERT INTO Users (User_ID, Login_Status) VALUES(10, 0);"
										"INSERT INTO Users (User_ID, Login_Status) VALUES(20, 0);"
										"INSERT INTO Users (User_ID, Login_Status) VALUES(30, 0);";
			res = sqlite3_exec(db, sql_stmt, 0, 0, &err_msg);
			if (res != SQLITE_OK)
			{
				fprintf(stderr, "Failed to create table Users\n");
				fprintf(stderr, "SQL error: %s\n", err_msg);
			}
			else
			{
				printf("Users table successfully created !!!!!!!!!!!");
			}

			sqlite3_free(err_msg);
			sqlite3_close(db);

			return 1;
		}
	}

	sqlite3_close(db);
	return 0;

}

void retrieveUsers(ServerMessage* msg)
{
	sqlite3 * db = openDatabase();
	if (NULL != db)
	{
		char* final = NULL;
		char *sql_stmt = "SELECT * FROM Users where User_ID = ";
		char userid[10] = "\0";

		sprintf(userid, "%d", msg->userID);
		char * colon = ";";

		final = (char*)malloc(strlen(sql_stmt) + strlen(userid) + strlen(colon) + 1 + 1 + 1 );
		memset(final, 0, sizeof(final));

		strcat(final, sql_stmt);
		strcat(final, userid);
		strcat(final, colon);

		char *err_msg = 0;

		int res = sqlite3_exec(db, final, usersCallback, msg, &err_msg);

		if (res != SQLITE_OK )
		{

			msg->responseType = 10;
			fprintf(stderr, "Failed to select data\n");
			fprintf(stderr, "SQL error: %s\n", err_msg);

			sqlite3_free(err_msg);
		}
	}
	sqlite3_close(db);
}

int usersCallback(void *data, int argc, char **argv, char **colName)
{
	if (argc > 0)
	{
		ServerMessage* msg = (ServerMessage*)data;
		msg->responseType = Okay;
		msg->loginStatus = atoi(argv[2]);
	}
    return 0;
}

int updateUsersLoginStatus(ServerMessage * smsg)
{
	sqlite3 *db = openDatabase();
	if (NULL != db)
	{
		char *errMsg = 0;
		int res;
		const char* data = "Updating Table";

		char* final = NULL;
		char * sql_stmt = "UPDATE Users set Login_Status = ";
		char stat[2]  = "\0";
		sprintf(stat, "%d", smsg->loginStatus);
		char * where = " where User_ID = ";
		char userid[10] = "\0";
		sprintf(userid, "%d", smsg->userID);
		char * colon = ";";

		final = (char*)malloc(strlen(sql_stmt) + 1 + strlen(stat) + 1 + strlen(where) + 1 + strlen(userid) + 1 + strlen(colon) + 1);
		memset(final, 0, sizeof(final));

		strcat(final, sql_stmt);
		strcat(final, stat);
		strcat(final, where);
		strcat(final, userid);
		strcat(final, colon);

		/* Execute SQL statement */
		res = sqlite3_exec(db, final, 0, smsg, &errMsg);
		if( res != SQLITE_OK )
		{
			smsg->responseType = 10;
			fprintf(stderr, "\nSQL error: %s\n", errMsg);
			sqlite3_free(errMsg);
			sqlite3_close(db);
			return 1;
		}
		else
		{
			smsg->responseType = Okay;
		}

	}
	sqlite3_close(db);

	return 0;
}

void retrieveBook(ServerMessage* smsg, char * clientISBN)
{
	sqlite3 * db = openDatabase();
	if (NULL != db)
	{
		char* final = NULL;
		char *sql_stmt = "SELECT * FROM Inventory where ISBN_13 = ";
		char * isbn = clientISBN;
		char * colon = ";";

		final = (char*)malloc(strlen(sql_stmt) + strlen(isbn) + strlen(colon) + 1 + 1 + 1 );
		memset(final, 0, sizeof(final));

		strcat(final, sql_stmt);
		strcat(final, isbn);
		strcat(final, colon);

		char *err_msg = 0;

		int res = sqlite3_exec(db, final, callback, smsg, &err_msg);

		if (res != SQLITE_OK )
		{

			smsg->responseType = 10;
			fprintf(stderr, "Failed to select data\n");
			fprintf(stderr, "SQL error: %s\n", err_msg);

			sqlite3_free(err_msg);
		}
	}
	sqlite3_close(db);
}


int callback(void *data, int argc, char **argv, char **colName)
{
    ServerMessage* msg = (ServerMessage*)data;

	msg->responseType = Okay;

	memcpy(msg->isbn, argv[1], strlen(argv[1]));

	memcpy(msg->authors, argv[2], strlen(argv[2]));

	memcpy(msg->title, argv[3], strlen(argv[3]));

	msg->edition = atoi(argv[4]);

	msg->year = atoi(argv[5]);

	memcpy(msg->publisher, argv[6], strlen(argv[6]));

	msg->inventory = atoi(argv[7]);

	msg->available = atoi(argv[8]);

    printf("\n");
    return 0;
}

int updateBook(ServerMessage* smsg, char* isbn, int reqType)
{

	if (reqType == 1)
	{
		if (smsg->available == 0)
		{
			smsg->responseType = AllGone;
			return 0;
		}
	}

	if (reqType == 2)
	{
		if (smsg->available == smsg->inventory)
		{
			smsg->responseType = NoInventory;
			return 0;
		}
	}

	sqlite3 *db = openDatabase();
	if (NULL != db)
	{
		char *errMsg = 0;
		int res;
		const char* data = "Updating Table";

		char* final = NULL;
		char * sql_stmt = "UPDATE Inventory set Copies_Available_for_Loan = ";
		char * avail;
		if (reqType == 1)
		{
			avail = "Copies_Available_for_Loan - 1 ";
		}
		else if (reqType == 2)
		{
			avail = "Copies_Available_for_Loan + 1 ";
		}
		char * where = " where ISBN_13 = ";
		char * isbnVal = isbn;
		char * colon = ";";

		final = (char*)malloc(strlen(sql_stmt) + 1 + strlen(avail) + 1 + strlen(where) + 1 + strlen(isbnVal) + 1 + strlen(colon) + 1);
		memset(final, 0, sizeof(final));

		strcat(final, sql_stmt);
		strcat(final, avail);
		strcat(final, where);
		strcat(final, isbnVal);
		strcat(final, colon);

		/* Execute SQL statement */
		res = sqlite3_exec(db, final, 0, smsg, &errMsg);
		if( res != SQLITE_OK )
		{
			smsg->responseType = 10;
			fprintf(stderr, "\nSQL error: %s\n", errMsg);
			sqlite3_free(errMsg);
			sqlite3_close(db);
			return 1;
		}
		else
		{
			smsg->responseType = Okay;
		}

	}
	sqlite3_close(db);

	return 0;
}
