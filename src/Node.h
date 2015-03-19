/*
 * Node.h
 *
 *  Created on: Mar 16, 2015
 *      Author: jonno
 */

#ifndef SRC_NODE_H_
#define SRC_NODE_H_

#include <string>

class Node {
private:
	std::string myIPAddress;
	int myPort;

	int myFD;

	unsigned int myKey;

public:
	/**
	 * Create new Node with a given ip address and port.
	 */
	Node(std::string ip_addr, int port);

	/**
	 * Create new Node with an established connection file descriptor.
	 */
	Node(int file_descriptor);
	virtual ~Node();

	bool Connect();

	unsigned int getKey();

	void processCommunication();
};

#endif /* SRC_NODE_H_ */
