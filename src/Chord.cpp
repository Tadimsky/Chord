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

using namespace std;

void Chord::Listen() {
	myListenFD = Open_listenfd(myListenPort);
	if (myListenFD < 0) {
		exit(-1);
	}

	int optval = 1;
	setsockopt(myListenFD, SOL_SOCKET, SO_REUSEADDR, (const void *) &optval, sizeof(int));

	cout << "Chord listening on Port " << myListenPort << endl;

	socklen_t client_len;
	struct sockaddr_in client_addr;

	int newConnection;

	thread clientThread;

	while (true) {
		client_len = sizeof(client_addr);
		newConnection = Accept(myListenFD, (sockaddr*)&client_addr, &client_len);

		cout << "Connection! " << inet_ntoa(client_addr.sin_addr) << ":" << ntohs(client_addr.sin_port) << endl;

		clientThread = thread(&Chord::handleRequest, this, 1, client_addr);
	}

}

Chord::Chord(int port) {
	myListenPort = port;
	myListenFD = 0;
}

Chord::~Chord() {
	Close(myListenFD);
}


void Chord::JoinRing(std::string entry_ip, int entry_port) {
	// do something with this information
}

void Chord::handleRequest(int socket_fd, sockaddr_in sockaddr) {
	cout << "Processing connection." << endl;
}

chord_key Chord::hashKey(std::string value) {
	byte output[CryptoPP::SHA1::DIGESTSIZE];
	CryptoPP::SHA1().CalculateDigest(output, (byte*) value.c_str(), value.length());

	chord_key resultKey = 0 - 1;
	// 20 byte SHA key
	// turn it into a 4 byte key
	for (unsigned int i = 0; i < sizeof(output); i += 4) {
		chord_key current;
		memcpy(&current, (void*)(output + i), 4);
		resultKey = resultKey ^ current;
	}

	return resultKey;
}
