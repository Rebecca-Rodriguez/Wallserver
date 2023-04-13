#include "Messages.h"

#include <iostream>
#include <vector>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>

std::vector<std::string> vector_queue;      // Using a std::vector as a queue
std::string command;                        // Used for accepting command from user

int main(int argc, char const* argv[]) {
	int queueSize = 20;                 // Default
	unsigned short port = 5514;         // Default

	int serverSocket, clientSocket;
	struct sockaddr_in addr;
	socklen_t addrLen = sizeof(addr);
	int opt = 1;

	const int BUFFER_SIZE = 256;       // 25*80
	char data[BUFFER_SIZE];

	bool loop = true;
	bool nestedLoop = true;

	// This means the queue size is entered
	if (argc == 2) {
		queueSize = std::stoi(argv[1]);
	}

	// This means the queue size and port are entered
	if (argc == 3) {
		queueSize = std::stoi(argv[1]);
		port = std::stoi(argv[2]);
	}

	// This means there are too many arguments
	if (argc >= 4) {
		std::cerr << "ERROR: Too many arguments" << std::endl;
		exit(EXIT_FAILURE);
	}

	// Create a socket file descriptor
	serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (serverSocket < 0) {
		std::cerr << "Error creating socket!" << std::endl;
		exit(EXIT_FAILURE);
	}

	// Forcefully attach socket to the port
	if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) < 0) {
		std::cerr << "Error with setsockopt!" << std::endl;
		exit(EXIT_FAILURE);
	}

	// Prepare the sockaddr_in structure
	addr.sin_family = AF_INET;                    // match the socket() call 
	addr.sin_addr.s_addr = htonl(INADDR_ANY);     // bind to any local address 
	addr.sin_port = htons(port);                  // specify port to listen on

	// Bind the socket to the specified port
	if (bind(serverSocket, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		std::cerr << "Error binding socket!" << std::endl;
		exit(EXIT_FAILURE);
	}

	// Listen for incoming connections
	if (listen(serverSocket, 5) < 0) {
		std::cerr << "Error listening!" << std::endl;
		exit(EXIT_FAILURE);
	}

	while (loop) {
		// Accept a client connection
		clientSocket = accept(serverSocket, (struct sockaddr*)&addr, &addrLen);
		if (clientSocket < 0) {
			std::cerr << "Error accepting connection!" << std::endl;
			exit(EXIT_FAILURE);
		}

		// Check to see if there is a new client
		if (clientSocket > 0 ) {
			nestedLoop = true;
		}

		while(nestedLoop) {
			send(clientSocket, WALL_HEADER, strlen(WALL_HEADER), 0);

			// If there are no message entries; else display the wall's contents
			if (vector_queue.empty()) {
				send(clientSocket, EMPTY_MESSAGE, strlen(EMPTY_MESSAGE), 0);
			}
			else {
				for (std::string message : vector_queue) {
					char* char_array = new char[message.length() + 1];
					strcpy(char_array, message.c_str());
					send(clientSocket, char_array, strlen(char_array), 0);
					delete[] char_array;
				}
			}
			send(clientSocket, "\n", 2, 0);

			send(clientSocket, COMMAND_PROMPT, strlen(COMMAND_PROMPT), 0);

			// Read command from user and check if worked
			int read1 = read(clientSocket, data, BUFFER_SIZE - 1);
			if (read1 < 0) {
				std::cerr << "Error reading command from socket!" << std::endl;
				exit(EXIT_FAILURE);
			}

			read1--;        // decrease size for conversion

			// Copy values from char to string
			for (int i = 0; i < read1; i++) {
				command += data[i];
			}

			memset(data, 0, sizeof(data));
			read1 = 0;

			if (command == "clear") {
				command = "";
				vector_queue.clear();
				send(clientSocket, CLEAR_MESSAGE, strlen(CLEAR_MESSAGE), 0);
			}
			else if (command == "post") {
				command = "";
				std::string name; // used for accepting name from user
				std::string message; // used for accepting message from user

				send(clientSocket, NAME_PROMPT, strlen(NAME_PROMPT), 0);

				// Read name from user and check if worked
				int read2 = read(clientSocket, data, sizeof(data));
				if (read2 < 0) {
					std::cerr << "Error reading name from socket!" << std::endl;
					exit(EXIT_FAILURE);
				}
				read2--;        // decrease size for conversion

				// Copy values from char to string
				for (int i = 0; i < read2; i++) {
					name += data[i];
				}

				memset(data, 0, sizeof(data));
				read2 = 0;

				int remaining = 78 - name.length();     // 78 because need ": "
				std::string temp = std::to_string(remaining);
				const char* num = temp.c_str();

				send(clientSocket, POST_PROMPT1, strlen(POST_PROMPT1), 0);
				send(clientSocket, num, strlen(num), 0);
				send(clientSocket, POST_PROMPT2, strlen(POST_PROMPT2), 0);

				// Read name from user and check if worked
				int read3 = read(clientSocket, data, sizeof(data));
				if (read3 < 0) {
					std::cerr << "Error reading message from socket!" << std::endl;
					exit(EXIT_FAILURE);
				}
				read3--;        // decrease size for conversion

				// Copy values from char to string
				for (int i = 0; i < read3; i++) {
					message += data[i];
				}

				memset(data, 0, sizeof(data));
				read2 = 0;

				if (message.size() > remaining) {
					send(clientSocket, ERROR_MESSAGE, strlen(ERROR_MESSAGE), 0);
				}
				else {
					if (vector_queue.size() >= queueSize) {
						// Remove the first element
						vector_queue.erase(vector_queue.begin());
					}
					vector_queue.push_back(name + ": " + message + "\n");
					send(clientSocket, SUCCESS_MESSAGE, strlen(SUCCESS_MESSAGE), 0);
				}
			}

			else if (command == "quit" || command == "kill"){
				nestedLoop = false;
			}
			else {
				command = "";
				nestedLoop = true;
			}
		}

		if (command == "quit") {
			// Displays a termination message and closes the client’s socket, but does not shut down the server or clear the wall.
			command = "";

			send(clientSocket, QUIT_MESSAGE, strlen(QUIT_MESSAGE), 0);

			// Close the connected socket
			close(clientSocket);
			clientSocket = 0;           // set the clientSocket to 0 to stop the inner loop
		}

		if (command == "kill") {
			loop = false;
		}
	}
	if (command == "kill") {
		// Causes the server to shut down (terminate), and close the socket, disconnecting the user.
		command = "";

		send(clientSocket, KILL_MESSAGE, strlen(KILL_MESSAGE), 0);

		// Close the connected socket
		close(clientSocket);

		// Close the listening socket
		shutdown(serverSocket, SHUT_RDWR);
	}

	return 0;
}