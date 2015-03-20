/*
 * Chord.cpp
 *
 *  Created on: Mar 17, 2015
 *      Author: jonno
 */

#include <iostream>
#include <cstdio>
#include <sha.h>
#include <memory>
#include <thread>
#include "Chord.h"
#include "Node.h"
#include "utils/csapp.h"
#include <sstream>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <ifaddrs.h>

using namespace std;

shared_ptr<Chord> Chord::myInstance = nullptr;

void Chord::Listen() {
	assert(myKey != 0);

	myListenFD = Open_listenfd(myListenPort);
	if (myListenFD < 0) {
		exit(-1);
	}

	int optval = 1;
	setsockopt(myListenFD, SOL_SOCKET, SO_REUSEADDR, (const void *) &optval, sizeof(int));

	cout << "Chord listening at " << myIPAddress << ":" << myListenPort << endl;

	socklen_t client_len;
	struct sockaddr_in client_addr;

	int newConnection;

	vector<thread> threads;

	while (true) {
		client_len = sizeof(client_addr);
		newConnection = Accept(myListenFD, (sockaddr*) &client_addr, &client_len);

		cout << "New Connection! " << inet_ntoa(client_addr.sin_addr) << ":" << ntohs(client_addr.sin_port) << endl;

		threads.push_back(
				thread(&Chord::handleRequest, this, newConnection, client_addr)
				);
	}

}

Chord::Chord() {
	myListenPort = 0;
	myListenFD = 0;
	myIPAddress = Chord::getLocalIPAddress();
}

Chord::~Chord() {
	Close(myListenFD);
}

void Chord::JoinRing(std::string entry_ip, int entry_port) {
	// do something with this information
}

void Chord::handleRequest(int socket_fd, sockaddr_in sockaddr) {
	cout << "Processing connection." << endl;

	RIOBuffered connection(socket_fd);

	char msg[256];
	sprintf(msg, "My ID is %x\n", myKey);
	connection.writeLine(&(Chord::WELCOME_MESSAGE));

	RIO::writep(socket_fd, (void*) msg, strlen(msg));

	string message = connection.readLine();
	stringstream parse(message);

	string command;
	parse >> command;

	if (command.compare("Node") == 0) {
		// Node identification message: Node Key IP:Port
		string message;
		getline(parse, message);

		shared_ptr<Node> node = Node::createFromInfo(message);
		if (node == nullptr) {
			string msg("You are using an invalid key.\n");
			RIO::writeString(socket_fd, &msg);
		}
		else {
			node->processCommunication(&connection);
		}
	}
	else if (command.find("Query") == 0) {
		//Node n(socket_fd, "", "");
	}
	else {
		RIO::writeString(socket_fd, &(Chord::ERROR_GOODBYE_MESSAGE));
		cout << "Unknown client connected." << endl;
	}

	shutdown(socket_fd, 0);
	Close(socket_fd);
}

chord_key Chord::hashKey(std::string value) {
	byte output[CryptoPP::SHA1::DIGESTSIZE];
	CryptoPP::SHA1().CalculateDigest(output, (byte*) value.c_str(), value.length());

	chord_key resultKey = 0;
	// 20 byte SHA key
	// turn it into a 4 byte key
	for (unsigned int i = 0; i < sizeof(output); i += 4) {
		chord_key current;
		memcpy(&current, (void*) (output + i), 4);
		resultKey = resultKey ^ current;
	}

	return resultKey;
}



std::shared_ptr<Chord> Chord::getInstance() {
	if (myInstance == nullptr) {
		myInstance = shared_ptr<Chord>(new Chord());
	}
	return myInstance;
}

void Chord::init(int port) {
	myListenPort = port;
	myKey = Chord::hashKey(myIPAddress + ":" + to_string(myListenPort));
	NodeInfo = shared_ptr<Node>(new Node(myIPAddress, myListenPort));
}

std::string Chord::getLocalIPAddress() {
	struct ifaddrs *ifaddr, *ifa;
	int n, s;
	int family;
	char host[NI_MAXHOST];

	if (getifaddrs(&ifaddr) == -1) {
		perror("getifaddrs");
		exit(EXIT_FAILURE);
	}

	for (ifa = ifaddr, n = 0; ifa != NULL; ifa = ifa->ifa_next, n++) {
		if (ifa->ifa_addr == nullptr) {
			continue;
		}

		family = ifa->ifa_addr->sa_family;
		if (family == AF_INET) {
			if (strcmp(ifa->ifa_name, "lo") != 0) {
				s = getnameinfo(ifa->ifa_addr,
						(family == AF_INET) ? sizeof(struct sockaddr_in) : sizeof(struct sockaddr_in6),
								host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
				if (s != 0) {
					printf("getnameinfo() failed: %s\n", gai_strerror(s));
					exit(EXIT_FAILURE);
				}
			}
		}

	}
	freeifaddrs(ifaddr);
	return string(host);
}

std::string Chord::toString() {
	stringstream result;
	result << hex << myKey << " " << myIPAddress << ":" << dec << myListenPort << endl;
	return result.str();
}

std::shared_ptr<Node> Chord::findSuccessor(chord_key key) {
	if (Successors.size() == 0) {
		// we have no successors - first node in circle?
		return NodeInfo;
	}
	else {
		shared_ptr<Node> n;
		n = findPredecessor(key);
		return n->getSuccessor();
	}

}

std::shared_ptr<Node> Chord::findPredecessor(chord_key key) {
	shared_ptr<Node> n = NodeInfo;
	// TODO: iterate over the finger table instead of the successor
	for (int i = Successors.size() - 1; i >= 0; --i) {
		// key in (current, mySuccessors[i])
		if (myKey < key && key <= Successors[i].getKey()) {
			n = shared_ptr<Node>(&(Successors[i]));
			return n;
		}
	}
	return n;
}

void Chord::parseIPPort(std::string message, std::string* ip, int* port) {
	*port = std::stoi(message.substr(message.find(":") + 1));
	*ip = message.substr(0, message.find(":"));
}

std::tuple<int, int> Chord::getRange() {
	int lowerKey = myKey;
	if (Predecessors.size() > 0) {
		lowerKey = Predecessors[0].getKey();
	}
	return tuple<int, int>(lowerKey, myKey);
}
