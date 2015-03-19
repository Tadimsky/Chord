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
	rio_t connection;

	RIO::readinitb(&connection, socket_fd);
	char msg[256];
	sprintf(msg, "My ID is %x\n", myKey);
	RIO::writep(socket_fd, (void*) Chord::WELCOME_MESSAGE.c_str(), Chord::WELCOME_MESSAGE.length());
	RIO::writep(socket_fd, (void*) msg, strlen(msg));

	char read_msg[RIO_BUFSIZE];
	RIO::readlineb(&connection, (void*) read_msg, RIO_BUFSIZE);
	string message(read_msg);
	stringstream parse(message);

	string command;
	parse >> command;

	if (command.compare("Node") == 0) {
		// Node identification message: Node Key IP:Port

		chord_key key;
		string ip;
		int port;
		parse >> hex >> key >> ip;

		// TODO: remove && false
		if (key != Chord::hashKey(ip) && false) {
			// invalid key
			string msg("You are using an invalid key.\n");
			RIO::writeString(socket_fd, &msg);
		}
		else {
			port = std::stoi(ip.substr(ip.find(":") + 1));
			ip = ip.substr(0, ip.find(":"));

			Node n(socket_fd, ip, port);
			n.processCommunication();
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

	cout << read_msg << endl;
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
