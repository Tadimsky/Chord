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
#include "MessageProcessor.h"
#include <tuple>

using namespace std;

//	struct sockaddr_in address;
//	socklen_t length;
//	getpeername(file_descriptor, (sockaddr*)&address, &length);

Node::Node(std::string ip_addr, int port) :
		Node(0, ip_addr, port) {
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

	MessageProcessor processor(this);

	while (true) {
		// block until it reads a line.
		message = this->readLine();
		cout << message;

		if (message.length() == 0) {
			// we have been shutdown by the other side. Bye!
			return;
		}

		if (!processor.handleMessage(message)) {
			// we need to exit
			return;
		}
	}
}

std::string Node::toString(bool endLine) {
	stringstream result;
	result << hex << myKey << " " << myIPAddress << ":" << dec << myPort;
	if (endLine) result << endl;
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
	if (msg.compare(Messages::NOT_FOUND) == 0) {
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
	if (msg.compare(Messages::NOT_FOUND) == 0) {
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
	if (msg.compare(Messages::NOT_FOUND) == 0) {
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
	if (msg.compare(Messages::NOT_FOUND) == 0) {
		return nullptr;
	}
	return Node::createFromInfo(msg);
}

void Node::insertSuccessor(Node* node, int index) {
	if (!this->isConnected()) {
		this->Connect();
	}
	stringstream s;
	s << "INSERT SUCCESSOR " << index << " " << node->toString();
	string msg = s.str();
	myRIOBuffer->writeLine(&msg);
}

void Node::insertPredecessor(Node* node, int index) {
	if (!this->isConnected()) {
		this->Connect();
	}
	stringstream s;
	s << "INSERT PREDECESSOR " << index << " " << node->toString();
	string msg = s.str();
	myRIOBuffer->writeLine(&msg);
}

void Node::replaceSuccessor(Node* node, int index, bool twoWay) {
	if (!this->isConnected()) {
		this->Connect();
	}
	stringstream s;
	s << "REPLACE SUCCESSOR " << index << " " << node->toString(!twoWay);
	if (twoWay) {
		s << " TWOWAY" << endl;
	}
	string msg = s.str();
	myRIOBuffer->writeLine(&msg);
}

void Node::replacePredecessor(Node* node, int index, bool twoWay) {
	if (!this->isConnected()) {
		this->Connect();
	}
	stringstream s;
	s << "REPLACE PREDECESSOR " << index << " " << node->toString(!twoWay);
	if (twoWay) {
		s << " TWOWAY" << endl;
	}
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
	stringstream data(info);
	return createFromInfo(data);
}

std::shared_ptr<Node> Node::createFromInfo(std::stringstream& info) {
	// this is in the same format as toString
	unsigned int key;
	std::string ip;
	int port;

	info >> hex >> key >> ip;

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

bool Node::checkConnection() {
	if (!isConnected()) {
		Connect();
	}
	size_t bytes = this->send(&(Messages::PING));
	if (bytes < 0) {
		return false;
	}

	string message = this->readLine();
	return message.find("PONG") == 0;
}
