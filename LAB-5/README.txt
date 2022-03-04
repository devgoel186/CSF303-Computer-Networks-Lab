* Name - DEV GOEL
* BITS Id - 2019A7PS0236G
* Lab Number - LAB-5

Following are the steps for running client-server communication:

1. 	Firstly compile the `server.c` and the `client.c` files using the below given command:
	`gcc server.c -o server`
	`gcc client.c -o client`

	Alternatively, you can also run the Makefile provided in the zip folder by running the following command in the terminal:
	`make compile`
	You must have the make utility installed in your system for this to work.

2. 	Open two terminals side-by-side in the folder where you have these files, and enter:
	`./server` in the first terminal,
	`./client` in the second terminal.

3. 	The client will establish connection with the server on port number 4444. You can proceed with the actual communication.

4. 	To exit the conversation, enter '.' from the client side to initiate termination process. You will be greeted with a 'Thank you' message and the program will terminate.