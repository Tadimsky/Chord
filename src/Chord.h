/*
 * Chord.h
 *
 *  Created on: Mar 17, 2015
 *      Author: jonno
 */

#ifndef CHORD_H_
#define CHORD_H_
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "Node.h"


typedef unsigned char byte;
typedef unsigned int chord_key;

void showUsageMessage(std::string procname);

class Chord {
private:
	const int NUM_PREDECESSORS = 2;
	const int NUM_SUCCESSORS = 2;
	const std::string WELCOME_MESSAGE = "Welcome! Go Ahead.\n";

	int myListenFD;
	int myListenPort;

	std::vector<Node> myPredecessors;
	std::vector<Node> mySuccessors;

	void handleRequest(int socket_fd, sockaddr_in sockaddr);

public:
	Chord(int port);

	virtual ~Chord();

	void Listen();

	void JoinRing(std::string entry_ip, int entry_port);

	static chord_key hashKey(std::string value);
};

#endif /* CHORD_H_ */
