* Name - DEV GOEL
* BITS Id - 2019A7PS0236G
* Lab Number - LAB-10

Following are the steps for running client-server communication:

1. 	Firstly compile the `server.c` and the `client.c` file using the below steps:
	-> `cd server && gcc server.c -o server -lpthread && cd ..`
	-> `cd client && gcc client.c -o client -lpthread && cd ..`

	Alternatively, you can also run the Makefile provided in the zip folder by running the following command in the root folder of the zip you extracted:
	-> `make`
	You must have the make utility installed in your system for this to work.

2. 	Open three terminals side-by-side in the folder where you have these files, and follow the steps below:
	In one terminal, do the following:
	-> `cd server`
	-> `./server <port_number>`, replacing the <port_number> with the port number for the server you want to start it at.
	
	-> `cd client`
	-> `./client <server_address> <port_number>`, replacing the <server_address> with the server address (127.0.0.1 in this case), 
	   and the <port_number> with the port number for the server.
	
	Repeat the client procedure for the starting another client.

3. 	A connection will be established between the two clients.