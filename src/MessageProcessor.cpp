/*
 * MessageProcessor.cpp
 *
 *  Created on: Mar 22, 2015
 *      Author: jonno
 */

#include "MessageProcessor.h"

MessageProcessor::MessageProcessor(Node* node) {
	// store a raw pointer because we will not outlive Node
	// famous last words

	myNode = node;
	// more fast
	chord = Chord::getInstance();
}

void MessageProcessor::handleMessage(std::string message) {
}

MessageProcessor::~MessageProcessor() {
	// TODO Auto-generated destructor stub
}

