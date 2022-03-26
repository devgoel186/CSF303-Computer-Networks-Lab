* Name - DEV GOEL
* Campus Id - 31120190236
* Lab Number - LAB-Midsem

Following are the steps for running client-server communication:

1. 	Firstly compile the `server.c` and the `client.c` files using the below given command:
	`gcc server.c -o server`
	`gcc client.c -o client`

2. 	Open two terminals side-by-side in the folder where you have these files, and follow the steps below:
	In one terminal, do the following:
	-> `cd server`
	-> `./server <port_number>`, replacing the <port_number> with a port number of your choice to 
  start the server on that port.

	In the other terminal, do the following:
	-> `cd client`
	-> `./client <server_address> <port_number>`

3. 	The client will establish connection with the server on port number. You can proceed with the actual communication.