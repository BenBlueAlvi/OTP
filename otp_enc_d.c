#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

void error(const char *msg) { perror(msg); exit(1); } // Error function used for reporting issues

void encode(int sock){
    char buffer[20480];
	char out[20480];
    int nRecieved = recv(sock, buffer, sizeof(buffer), 0);
    char* key = strchr(buffer, ':');
	if (!key || (strlen(buffer)-strlen(key) > strlen(key)-1)){
		int charsR = send(sock, "Invalid Data", 12, 0); // Send success back
		if (charsR < 0) error("ERROR writing to socket");
		
		close(sock);
		error("ERROR: Invalid data");
	}
	//remove the space, buffer is now plaintext and key is key
	int len = strlen(buffer)-strlen(key);
	int i;
	for (i=0; i < len; i++){
		
		if (buffer[i] == ' '){
			buffer[i] = '[';
		}
		if (key[i+1] == ' '){
			key[i+1] = '[';
		} 
		out[i] = ((buffer[i] - 'A') + (key[i+1]- 'A')) % 27 + 'A';
		
		
		//if we go over, it's a space
		if (out[i] == '['){
			out[i] = ' ';
		}
	
	}
	out[i] = '\0';
	int charsRead = send(sock, out, strlen(out), 0); // Send success back
	if (charsRead < 0) error("ERROR writing to socket");
	close(sock);
	exit(0);


}

void communicate(int* sockFD, struct sockaddr* addr, socklen_t* addrlen){
	int running = 1;
	int childs = 0;
	while (running){
		//catch childs
		int result = 0;
		if (childs > 0) { waitpid(0, &result, WNOHANG); }
		int sock = accept(*sockFD, addr, addrlen);
		

		int pid = fork();
		switch(pid){
			case -1:
				printf("Problem Forking\n");
				break;
			case 0:
			
				encode(sock);
				break;
			default:
				childs++;
	
				//got to clean up child
				break;
		}

	}
	
}

int main(int argc, char *argv[])
{
	int listenSocketFD, establishedConnectionFD, portNumber, charsRead;
	socklen_t sizeOfClientInfo;
	char buffer[256];
	struct sockaddr_in serverAddress, clientAddress;

	if (argc < 2) { fprintf(stderr,"USAGE: %s port\n", argv[0]); exit(1); } // Check usage & args

	// Set up the address struct for this process (the server)
	memset((char *)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
	portNumber = atoi(argv[1]); // Get the port number, convert to an integer from a string
	serverAddress.sin_family = AF_INET; // Create a network-capable socket
	serverAddress.sin_port = htons(portNumber); // Store the port number
	serverAddress.sin_addr.s_addr = INADDR_ANY; // Any address is allowed for connection to this process

	// Set up the socket
	listenSocketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
	if (listenSocketFD < 0) error("ERROR opening socket");

	// Enable the socket to begin listening
	if (bind(listenSocketFD, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) // Connect socket to port
		error("ERROR on binding");
	
	listen(listenSocketFD, 5); // Flip the socket on - it can now receive up to 5 connections
	
    //commune!
	socklen_t addrSize = sizeof(serverAddress);
    communicate(&listenSocketFD, (struct sockaddr *)&serverAddress, &addrSize);


	// Send a Success message back to the client
	
	close(establishedConnectionFD); // Close the existing socket which is connected to the client
	close(listenSocketFD); // Close the listening socket
	return 0; 
}
