# Wallserver

Wallserver is a simple message board server application that allows clients to connect and post messages on a wall. The server is implemented in C++ using sockets and can handle multiple client connections concurrently. Clients can connect to the server using a TCP/IP socket and interact with the wall by posting messages, clearing the wall, and quitting the application.

## Features

* Multiple clients can connect to the server concurrently and post messages on the wall.
* Messages posted by clients are displayed on the wall with a timestamp.
* Clients can clear the wall, removing all the messages from the wall.
* Clients can quit the application, disconnecting from the server gracefully.

## Prerequisites

* C++ compiler that supports C++17 standard (e.g., GCC, Clang)
* POSIX-compliant operating system (e.g., Linux, macOS)

## Usage
### Compilation

Use the provided Makefile to compile the wallserver application. Simply run the following command in the terminal:
```
make
```

This will compile the `wallserver.cpp` file using the C++17 standard and generate an executable named `wallserver`.

### Running the Server

After compilation, you can run the `wallserver` application using the following command:

```
./wallserver [queue_size] [port]
```

where ``[queue_size]`` is an optional argument that specifies the maximum number of messages that the wall can hold (default is 20), and ``[port]`` is an optional argument that specifies the port number that the server should listen on (default is 5514).

### Connecting to the Server

Clients can connect to the server using a TCP/IP socket. The server listens for incoming connections on the specified port (default is 5514). Clients can connect to the server using tools like `telnet` or by implementing a custom client using a socket library.

### Commands

Once connected to the server, clients can interact with the wall by sending commands to the server. The following commands are supported:

* `post`: Allows clients to post a message on the wall. The server prompts the client to enter their name and the message to be posted. The maximum length of the message is specified during compilation (default is 256 characters).
* `clear`: Clears all the messages from the wall.
* `quit`: Quits the application and disconnects from the server gracefully.
