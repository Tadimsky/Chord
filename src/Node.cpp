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
#include <tuple>

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
	if (myFD > 0) {
		// let them know we are done.
		// myRIOBuffer->writeLine(&(Node::EXIT_MSG));
		shutdown(myFD, 0);
		Close(myFD);
	}
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
		message = myRIOBuffer->readLine();

		return message.find("Hello") == 0;
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

void Node::processCommunication(std::shared_ptr<RIOBuffered> rio) {
	myRIOBuffer = rio;
	myFD = myRIOBuffer->getFD();

	auto chord = Chord::getInstance();

	stringstream stream;
	stream << "Hello " << std::hex << myKey << "\n";
	string message(stream.str());
	this->send(&message);

	while (true) {
		// block until it reads a line.
		message = this->readLine();
		if (message.length() == 0) {
			// we have been shutdown by the other side. Bye!
			return;
		}
		stringstream str(message);
		string command;
		str >> command;

		// SET Queries - all specific to the instance of Chord
		if (command.compare("GET") == 0) {
			// get next part of get request
			str >> command;

			if (command.compare("SUCCESSOR") == 0) {
				size_t index = -1;
				str >> index;
				string response;

				if (chord->Successors.size() > index - 1) {
					response = chord->Successors[index - 1]->toString();
				}
				else {
					if (index == 1) {
						// this happens when there are no successors because it's the only one.
						response = chord->NodeInfo->toString();
					}
					else {
						response = Node::NOT_FOUND;
					}
				}
				this->send(&response);
			}
			else if (command.compare("PREDECESSOR") == 0) {
					size_t index;
					str >> index;
					string response;

					if (chord->Predecessors.size() > index - 1) {
						response = chord->Predecessors[index - 1]->toString();
					}
					else {
						if (index == 1) {
							// this happens when there are no successors because it's the only one.
							response = chord->NodeInfo->toString();
						}
						else {
							response = Node::NOT_FOUND;
						}
					}
					this->send(&response);
			}
			else if (command.compare("INFO") == 0) {
				string s = chord->toString();
				this->send(&s);
			}
			else if (command.compare("RANGE") == 0) {
				auto range = chord->getRange();
				stringstream msg_s;
				msg_s << hex << get<0>(range) << " " << hex << get<1>(range) << endl;
				string msg(msg_s.str());
				this->send(&msg);
			}

		}
		// Get Queries - all specific to the instance of Chord
		else if (command.compare("SET") == 0) {
			str >> command;

			if (command.compare("SUCCESSOR") == 0 || command.compare("PREDECESSOR") == 0) {
				int index;
				str >> index;

				string info;
				// get the Node info
				getline(str, info);
				auto node = Node::createFromInfo(info);
				if (node->getKey() == myKey) {
					// it is this node trying to make us point to it
					node = shared_ptr<Node>(this);
				}
				if (command.compare("SUCCESSOR") == 0) {
					// we don't want to reset up a link to the node.
					// they just told us to set it up.
					chord->setSuccessor(index, node, false);
				}
				else {
					chord->setPredecessor(index, node, false);
				}
			}
		}
		// Find queries
		else if (command.compare("FIND") == 0) {
			str >> command;
			if (command.compare("SUCCESSOR") == 0) {
				unsigned int key;
				str >> hex >> key;
				// find the closest predecessor
				// return successor of that
				auto node = chord->findSuccessor(key);
				string msg = node->toString();
				myRIOBuffer->writeLine(&msg);
			}
		}
		// Find queries
		else if (command.compare("SEARCH") == 0) {
			str >> command;
			if (command.compare("SUCCESSOR") == 0) {
				unsigned int key;
				str >> hex >> key;
				// get best option we know of
				auto node = chord->findSuccessor(key);

				while (true) {
					// check this entry for validity
					// pred is less than key
					auto pred = node->getPredecessor();
					cout << "Key Range (" << hex << pred->getKey() << ", " << hex << node->getKey() << "]" << endl;

					// check if key between predecessor and provided successor
					if (Chord::inRange(pred->getKey(), node->getKey(), key)) {
						// if so, then this is the successor!
						string msg = node->toString();
						myRIOBuffer->writeLine(&msg);
						break;
					}
					else {
						cout << "We have " << hex << node->getKey() << ". We are looking for " << hex << key << endl;
						// get the next best attempt
						node = node->FindSuccessor(key);
					}
				}



//				if (node->getKey() == Chord::getInstance()->NodeInfo->getKey()) {
//					// we just return ourselves
//					string msg = node->toString();
//					myRIOBuffer->writeLine(&msg);
//				}

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
			if (command.compare(Node::EXIT_MSG) == 0) {
				// quit and close
				string goodbye = "Goodbye Friend!\n";
				myRIOBuffer->writeLine(&goodbye);
				return;
			}
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
	s << "GET SUCCESSOR " << index << endl;
	string msg = s.str();
	myRIOBuffer->writeLine(&msg);
	msg = myRIOBuffer->readLine();
	if (msg.compare(Node::NOT_FOUND) == 0) {
		return nullptr;
	}
	return Node::createFromInfo(msg);
}

std::shared_ptr<Node> Node::getPredecessor(int index) {
	if (!this->isConnected()) {
		this->Connect();
	}
	stringstream s;
	s << "GET PREDECESSOR " << index << endl;
	string msg = s.str();
	myRIOBuffer->writeLine(&msg);
	msg = myRIOBuffer->readLine();
	if (msg.compare(Node::NOT_FOUND) == 0) {
		return nullptr;
	}
	return Node::createFromInfo(msg);
}

bool Node::isConnected() {
	return (myFD > 0);
}

std::shared_ptr<Node> Node::SearchSuccessor(unsigned int key) {
	if (!this->isConnected()) {
			this->Connect();
	}
	stringstream s;
	s << "SEARCH SUCCESSOR " << hex << key << endl;
	string msg = s.str();
	myRIOBuffer->writeLine(&msg);
	msg = myRIOBuffer->readLine();
	if (msg.compare(Node::NOT_FOUND) == 0) {
		return nullptr;
	}
	return Node::createFromInfo(msg);
}

std::shared_ptr<Node> Node::FindSuccessor(unsigned int key) {
	if (!this->isConnected()) {
			this->Connect();
	}
	stringstream s;
	s << "FIND SUCCESSOR " << hex << key << endl;
	string msg = s.str();
	myRIOBuffer->writeLine(&msg);
	msg = myRIOBuffer->readLine();
	if (msg.compare(Node::NOT_FOUND) == 0) {
		return nullptr;
	}
	return Node::createFromInfo(msg);
}

void Node::setSuccessor(Node* node, int index) {
	if (!this->isConnected()) {
			this->Connect();
	}
	stringstream s;
	s << "SET SUCCESSOR " << index << " " << node->toString();
	string msg = s.str();
	myRIOBuffer->writeLine(&msg);
}

void Node::setPredecessor(Node* node, int index) {
	if (!this->isConnected()) {
		this->Connect();
	}
	stringstream s;
	s << "SET PREDECESSOR " << index << " " << node->toString();
	string msg = s.str();
	myRIOBuffer->writeLine(&msg);
}

std::tuple<unsigned int, unsigned int> Node::getRange() {
	if (!this->isConnected()) {
			this->Connect();
	}
	stringstream s;
	s << "GET RANGE" << endl;
	string msg = s.str();
	myRIOBuffer->writeLine(&msg);
	msg = myRIOBuffer->readLine();

	stringstream response(msg);

	unsigned int lower;
	unsigned int upper;

	response >> lower >> upper;
	return tuple<int, int>(lower, upper);
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
