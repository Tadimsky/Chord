/*
 * Node.cpp
 *
 *  Created on: Mar 16, 2015
 *      Author: jonno
 */

#include "Node.h"
#include "Chord.h"
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "utils/csapp.h"

using namespace std;

Node::Node(std::string ip_addr, int port) {
	myFD = 0;
	myIPAddress = ip_addr;
	myPort = port;
	myKey = Chord::hashKey(myIPAddress + ":" + to_string(myPort));
}

Node::Node(int file_descriptor) {
	myFD = file_descriptor;
	struct sockaddr_in address;
	socklen_t length;

	getpeername(file_descriptor, (sockaddr*)&address, &length);
	myIPAddress = string(inet_ntoa(address.sin_addr));
	myPort = ntohs(address.sin_port);
	myKey = Chord::hashKey(myIPAddress + ":" + to_string(myPort));
}

Node::~Node() {
	// TODO Auto-generated destructor stub
}

bool Node::Connect() {
	myFD = Open_clientfd(myIPAddress.c_str(), myPort);
	if (myFD > 0) {
		return true;
	}
	return false;
}

unsigned int Node::getKey() {
	return myKey;
}
