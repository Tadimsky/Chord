/*
 * MessageProcessor.h
 *
 *  Created on: Mar 22, 2015
 *      Author: jonno
 */

#ifndef MESSAGEPROCESSOR_H_
#define MESSAGEPROCESSOR_H_

#include <string>
#include <memory>
#include <sstream>
#include "Chord.h"
#include "Node.h"

class MessageProcessor {
private:
	/**
	 * The Node that this Message Processor deals with
	 */
	Node* myNode;

	// faster than calling singleton.
	std::shared_ptr<Chord> chord;

private:

	void processGetNode(std::stringstream& stream, std::vector<std::shared_ptr<Node>>& items);
	void processInsertNode(std::stringstream& stream, std::string& item);
	void processReplaceNode(std::stringstream& stream, std::string& item);
	void processGetNodes(std::vector<std::shared_ptr<Node>>& items);
	void processFindSuccessor(std::stringstream& str);
	void processSearchSuccessor(std::stringstream& str);

public:
	MessageProcessor(Node* node);
	bool handleMessage(std::string& message);
	virtual ~MessageProcessor();
};

#endif /* MESSAGEPROCESSOR_H_ */
