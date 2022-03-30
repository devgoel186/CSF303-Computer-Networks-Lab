* Name - DEV GOEL
* BITS Id - 2019A7PS0236G
* Lab Number - LAB-8

Following are the steps for running client-server communication:

1. 	Firstly compile the `client.c` file using the below steps:
	-> `gcc client.c -o client`

	Alternatively, you can also run the Makefile provided in the zip folder by running the following command in the root folder of the zip you extracted:
	-> `make compile`
	You must have the make utility installed in your system for this to work.

2. 	Open two terminals side-by-side in the folder where you have these files, and follow the steps below:
	In one terminal, do the following:
	-> `./client <url>`, replacing the <url> with the URL for the file which you want to download.

3. 	The client will establish connection with the server and download the file over TCP.

4. This downloaded file will be placed in your local folder.