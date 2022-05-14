* Name - DEV GOEL
* BITS Id - 2019A7PS0236G
* Lab Number - LAB-Compre

Following are the steps for running client-server communication:

1. 	Firstly compile the `server.c` and the `client.c` files using the below steps:
	-> `gcc server.c -o server -lpthread`
	-> `gcc client.c -o client -lpthread`

2. 	Open two terminals side-by-side in the folder where you have these files, and follow the steps below:
	In one terminal, do the following:
	-> `./server <port_number>`, replacing the <port_number> with a port number of your choice to 
	start the server on that port.

	In the other terminal, do the following:
	-> `./client <username> <server_address> <port_number>`, replacing the <server_address> with '127.0.0.1', and the <port_number> with a 
	port number of your choice (same as the server to connect).

3. 	The client will establish connection with the server on port number.