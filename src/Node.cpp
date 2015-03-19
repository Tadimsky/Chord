/*
 * Node.cpp
 *
 *  Created on: Mar 16, 2015
 *      Author: jonno
 */

#include "Node.h"
#include "Chord.h"
#include <iomanip>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "utils/csapp.h"

using namespace std;

//struct sockaddr_in address;
//	socklen_t length;
//
//	getpeername(file_descriptor, (sockaddr*)&address, &length);

Node::Node(std::string ip_addr, int port) : Node(0, ip_addr, port) {
}

Node::Node(int file_descriptor, std::string ip_addr, int port) {
	myFD = file_descriptor;
	myIPAddress = ip_addr;
	myPort = port;
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

void Node::processCommunication() {
	stringstream stream;
	stream << "Hello " << std::hex << myKey << "\n";
	string message(stream.str());
	RIO::writep(myFD, (void*)message.c_str(), message.length());
	while (true) {

	}
}
