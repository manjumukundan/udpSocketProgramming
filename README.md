# udpSocketProgramming
A Library database manager using UDP Socket Programming in C. Use sqlite database.
==============================================================
This project has a client process (UDPClient.c) that requests for certain library operations like login, query, checkin and checkout of books, logout.
The server process (UDPServer.c) manages the client requests, checks for valid login and valid ISBN.
Server maintains a database (sqlite3) for storage of books and their availability through Database.c

Amalgamation file for sqlite3 is used in the project.

Compilation and Execution
==========================

UDPServer.c
===========
Compilation : gcc -std=c99 -o udpServer UDPServer.c Database.c sqlite3.c -lpthread -ldl
Execution UDPServer.c : ./udpServer 23000


UDPClient.c
===========

Compilation: gcc -std=c99 -o udpClient UDPClient.c
Execution: ./udpClient 127.0.0.1 23000
