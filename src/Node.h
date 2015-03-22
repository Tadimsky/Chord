/*
 * Node.h
 *
 *  Created on: Mar 16, 2015
 *      Author: jonno
 */

#ifndef SRC_NODE_H_
#define SRC_NODE_H_

#include <string>
#include <memory>
#include "utils/csapp.h"


class Node {
private:
	std::string myIPAddress;
	int myPort;

	int myFD;

	std::shared_ptr<RIOBuffered> myRIOBuffer;

	unsigned int myKey;

	char buffer[RIO_BUFSIZE];
	std::string readLine();


public:
	/**
	 * Create new Node with a given ip address and port.
	 */
	Node(std::string ip_addr, int port);

	/**
	 * Create new Node with an established connection file descriptor.
	 */
	Node(int file_descriptor, std::string ip_addr, int port);
	virtual ~Node();

	bool Connect();

	unsigned int getKey();

	void processCommunication(std::shared_ptr<RIOBuffered> rio);

	std::shared_ptr<Node> getSuccessor(int index = 1);
	std::shared_ptr<Node> getPredecessor(int index = 1);

	void insertSuccessor(Node* node, int index = 1);
	void insertPredecessor(Node* node, int index = 1);

	void replaceSuccessor(Node* node, int index = 1, bool twoWay = false);
	void replacePredecessor(Node* node, int index = 1, bool twoWay = false);

	std::shared_ptr<Node> SearchSuccessor(unsigned int key);
	std::shared_ptr<Node> FindSuccessor(unsigned int key);

	std::tuple<unsigned int, unsigned int> getRange();

	std::string toString(bool endLine = true);

	bool isConnected();

	static std::shared_ptr<Node> createFromInfo(std::string info);

	size_t send(const std::string* message);
};

namespace Messages {
	const std::string NOT_FOUND = "Item Not Found\n";
	const std::string EXIT_MSG = "EXIT";
};

#endif /* SRC_NODE_H_ */
