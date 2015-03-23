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



	const size_t NUM_PRED_SUCC = 2;
	const std::string WELCOME_MESSAGE = "Welcome! Who are you?\n";
	const std::string ERROR_GOODBYE_MESSAGE = "I don't know who you are. Please leave.\n";

	int myListenFD;
	int myListenPort;
	std::string myIPAddress;

	chord_key myKey;



	void handleRequest(int socket_fd, sockaddr_in sockaddr);

	Chord();

	void insertNode(size_t index, std::vector<std::shared_ptr<Node>>& items, std::shared_ptr<Node> node);
	void replaceNode(size_t index, std::vector<std::shared_ptr<Node>>& items, std::shared_ptr<Node> node);

public:
	std::shared_ptr<Node> NodeInfo;
	std::vector<std::shared_ptr<Node>> Predecessors;
	std::vector<std::shared_ptr<Node>> Successors;

	static std::shared_ptr<Chord> getInstance();

	void init(int port);

	virtual ~Chord();

	void Listen();

	void JoinRing(std::string entry_ip, int entry_port);
	void LeaveRing();

	static chord_key hashKey(std::string value);
	static void parseIPPort(std::string message, std::string* ip, int* port);

	static std::string getLocalIPAddress();

	std::string toString();

	std::shared_ptr<Node> findSuccessor(chord_key key);
	std::shared_ptr<Node> findPredecessor(chord_key key);

	void insertSuccessor(size_t index, std::shared_ptr<Node> node, bool setupOther = true);
	void insertPredecessor(size_t index, std::shared_ptr<Node> node, bool setupOther = true);

	void replaceSuccessor(size_t index, std::shared_ptr<Node> node, bool setupOther = true);
	void replacePredecessor(size_t index, std::shared_ptr<Node> node, bool setupOther = true);

	/**
	 * Remove a successor that is not responding to us anymore. We don't want to talk to him anymore.
	 * We only clean up ourselves, not anybody else who may talk to it. They will clean it up themselves later on.
	 */
	void spliceSuccessor(size_t index);
	void splicePredecessor(size_t index);

	std::tuple<int, int> getRange();

	static bool inRange(chord_key lower, chord_key upper, chord_key key, bool inclusiveEnd = true);

};

#endif /* CHORD_H_ */
