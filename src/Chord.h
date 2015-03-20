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
#include <vector>
#include <memory>
#include <tuple>
#include "Node.h"


typedef unsigned char byte;
typedef unsigned int chord_key;

void showUsageMessage(std::string procname);

class Chord {
private:
	static std::shared_ptr<Chord> myInstance;



	const int NUM_PREDECESSORS = 2;
	const int NUM_SUCCESSORS = 2;
	const std::string WELCOME_MESSAGE = "Welcome! Who are you?\n";
	const std::string ERROR_GOODBYE_MESSAGE = "I don't know who you are. Please leave.\n";

	int myListenFD;
	int myListenPort;
	std::string myIPAddress;

	chord_key myKey;



	void handleRequest(int socket_fd, sockaddr_in sockaddr);

	Chord();

public:
	std::shared_ptr<Node> NodeInfo;
	std::vector<std::shared_ptr<Node>> Predecessors;
	std::vector<std::shared_ptr<Node>> Successors;

	static std::shared_ptr<Chord> getInstance();

	void init(int port);

	virtual ~Chord();

	void Listen();

	void JoinRing(std::string entry_ip, int entry_port);

	static chord_key hashKey(std::string value);
	static void parseIPPort(std::string message, std::string* ip, int* port);

	static std::string getLocalIPAddress();

	std::string toString();

	std::shared_ptr<Node> findSuccessor(chord_key key);
	std::shared_ptr<Node> findPredecessor(chord_key key);

	std::tuple<int, int> getRange();

};

#endif /* CHORD_H_ */
