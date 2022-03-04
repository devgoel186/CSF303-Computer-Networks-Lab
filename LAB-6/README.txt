* Name - DEV GOEL
* BITS Id - 2019A7PS0236G
* Lab Number - LAB-6

Following are the steps for running client-server communication:

1. 	Firstly compile the `server.c` and the `client.c` files using the below steps:
	-> `cd server && gcc server.c -o server && cd ..`
	-> `cd client && gcc client.c -o client && cd ..`

	Alternatively, you can also run the Makefile provided in the zip folder by running the following command in the root folder of the zip you extracted:
	-> `make compile`
	You must have the make utility installed in your system for this to work.

2. 	Open two terminals side-by-side in the folder where you have these files, and follow the steps below:
	In one terminal, do the following:
	-> `cd server`
	-> `./server <port_number>`, replacing the <port_number> with a port number of your choice to 
  start the server on that port.

	In the other terminal, do the following:
	-> `cd client`
	-> `./client <server_address> <port_number> <filename>`, replacing the <server_address> with '127.0.0.1', the <port_number> with a port number of your choice (same as the
		server to connect), and the <filename> with the file you want.


3. 	The client will establish connection with the server on port number. The server will send back the first 10 bytes from the file you requested.

4.  You can open the newly created file in the client folder. This will now contain the first 10 bytes sent by the server.