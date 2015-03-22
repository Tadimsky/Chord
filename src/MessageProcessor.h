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
public:
	MessageProcessor(Node* node);
	void handleMessage(std::string message);
	virtual ~MessageProcessor();
};

#endif /* MESSAGEPROCESSOR_H_ */
