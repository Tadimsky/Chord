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
#include <iostream>
#include <sstream>
#include <assert.h>

using namespace std;

//	struct sockaddr_in address;
//	socklen_t length;
//	getpeername(file_descriptor, (sockaddr*)&address, &length);

Node::Node(std::string ip_addr, int port) : Node(0, ip_addr, port) {
}

Node::Node(int file_descriptor, std::string ip_addr, int port) {
	myFD = file_descriptor;
	if (myFD > 0) {
		myRIOBuffer = shared_ptr<RIOBuffered>(new RIOBuffered(myFD));
	}

	myIPAddress = ip_addr;
	myPort = port;
	myKey = Chord::hashKey(myIPAddress + ":" + to_string(myPort));
}

Node::~Node() {
}

bool Node::Connect() {
	myFD = Open_clientfd(myIPAddress.c_str(), myPort);
	if (myFD > 0) {
		myRIOBuffer = shared_ptr<RIOBuffered>(new RIOBuffered(myFD));

		// other node sends hello message
		myRIOBuffer->readLine();
		// node's identity
		string message = myRIOBuffer->readLine();

		message = "Node " + this->toString();
		this->send(&message);
		return true;
	}
	return false;
}

unsigned int Node::getKey() {
	return myKey;
}

std::string Node::readLine() {
	return myRIOBuffer->readLine();
}

size_t Node::send(const std::string* message) {
	assert(myFD > 0);
	return RIO::writeString(myFD, message);
}

void Node::processCommunication(RIOBuffered* rio) {
	myRIOBuffer = std::shared_ptr<RIOBuffered>(rio);
	myFD = myRIOBuffer->getFD();

	stringstream stream;
	stream << "Hello " << std::hex << myKey << "\n";
	string message(stream.str());
	this->send(&message);

	while (true) {
		// block until it reads a line.
		message = this->readLine();
		stringstream str(message);
		string command;
		str >> command;

		// SET Queries - all specific to the instance of Chord
		if (command.compare("GET") == 0) {
			// get next part of get request
			str >> command;

			if (command.compare("SUCCESSOR") == 0) {
				int index;
				str >> index;
				if (Chord::getInstance()->mySuccessors.size() > index - 1) {
					string response = Chord::getInstance()->mySuccessors[index - 1].toString();
					this->send(&response);
				}
				else {
					this->send(&(Node::NOT_FOUND));
				}
			}
			else if (command.compare("PREDECESSOR") == 0) {
					int index;
					str >> index;
			}
			else if (command.compare("INFO") == 0) {
				string s = Chord::getInstance()->toString();
				this->send(&s);
			}


		}
		// Get Queries - all specific to the instance of Chord
		else if (command.compare("SET") == 0) {

		}
		// Find queries
		else if (command.compare("FIND") == 0) {
			str >> command;
			if (command.compare("SUCCESSOR") == 0) {
				string key;
				str >> key;
				// find the closest predecessor
				// return successor of that
			}
		}
		// Find queries
		else if (command.compare("SEARCH") == 0) {
			str >> command;
			if (command.compare("SUCCESSOR") == 0) {
				string key;
				str >> key;
				// perform iterative lookup of successor for a key
				// use FIND SUCCESSOR on nodes to get the actual value
				// find predecessor to the id
				// return successor of that
			}
		}
		// Find queries
		else if (command.compare("REQUEST") == 0) {
			// Join
		}
		else {
			cout << "Unknown Request: " << message;
		}
	}
}

std::string Node::toString() {
	stringstream result;
	result << hex << myKey << " " << myIPAddress << ":" << dec << myPort << endl;
	return result.str();
}

std::shared_ptr<Node> Node::getSuccessor(int index) {
	if (!this->isConnected()) {
		this->Connect();
	}
	stringstream s;
	s << "GET SUCCESSOR " << index;
	string msg = s.str();
	myRIOBuffer->writeLine(&msg);
	msg = myRIOBuffer->readLine();
	return Node::createFromInfo(msg);
}

std::shared_ptr<Node> Node::getPredecessor(int index) {
}

bool Node::isConnected() {
	if (myFD == 0) {
		return false;
	}
}

std::shared_ptr<Node> Node::createFromInfo(std::string info) {
	// this is in the same format as toString
	unsigned int key;
	std::string ip;
	int port;

	stringstream result(info);
	result >> hex >> key >> ip;

	// validate key and ip:port
	// TODO: remove && false
	if (key != Chord::hashKey(ip) && false) {
		// invalid key
		cout << "Node " << hex << key << " had an invalid key." << endl;
		return nullptr;
	}

	Chord::parseIPPort(ip, &ip, &port);

	return shared_ptr<Node>(new Node(ip, port));
}
