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
	const std::string NOT_FOUND = "Item Not Found\n";
	std::string myIPAddress;
	int myPort;

	int myFD;

	std::shared_ptr<RIOBuffered> myRIOBuffer;

	unsigned int myKey;

	char buffer[RIO_BUFSIZE];
	std::string readLine();
	size_t send(const std::string* message);

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

	void processCommunication(RIOBuffered* rio);

	std::shared_ptr<Node> getSuccessor(int index = 1);
	std::shared_ptr<Node> getPredecessor(int index = 1);

	std::string toString();

	bool isConnected();
};

#endif /* SRC_NODE_H_ */
