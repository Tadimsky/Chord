/*
 * MessageProcessor.cpp
 *
 *  Created on: Mar 22, 2015
 *      Author: jonno
 */

#include "MessageProcessor.h"
#include <string>
#include <sstream>
#include <iostream>

using namespace std;

MessageProcessor::MessageProcessor(Node* node) {
	// store a raw pointer because we will not outlive Node
	// famous last words

	myNode = node;
	// more fast
	chord = Chord::getInstance();
}

void MessageProcessor::processFindSuccessor(stringstream& str) {
	unsigned int key;
	str >> hex >> key;
	// find the closest predecessor
	// return successor of that
	auto node = chord->findSuccessor(key);
	string msg = node->toString();
	myNode->send(&msg);
}

void MessageProcessor::processSearchSuccessor(stringstream& str) {
	unsigned int key;
	str >> hex >> key;
	// get best option we know of
	auto node = chord->findSuccessor(key);
	while (true) {
		// check this entry for validity
		// pred is less than key
		auto pred = node->getPredecessor();
		cout << "Key Range (" << hex << pred->getKey() << ", " << hex << node->getKey() << "]" << endl;
		// check if key between predecessor and provided successor
		if (Chord::inRange(pred->getKey(), node->getKey(), key)) {
			// if so, then this is the successor!
			string msg = node->toString();
			myNode->send(&msg);
			break;
		} else {
			cout << "We have " << hex << node->getKey() << ". We are looking for " << hex << key << endl;
			// get the next best attempt
			node = node->FindSuccessor(key);
		}
	}
}

bool MessageProcessor::handleMessage(std::string& message) {
	stringstream str(message);
	string command;
	str >> command;

	// SET Queries - all specific to the instance of Chord
	if (command.compare("GET") == 0) {
		// get next part of get request
		str >> command;

		if (command.compare("SUCCESSOR") == 0) {
			processGetNode(str, chord->Successors);
		} else if (command.compare("PREDECESSOR") == 0) {
			processGetNode(str, chord->Predecessors);
		} else if (command.compare("SUCCESSORS") == 0) {
			processGetNodes(chord->Successors);
		} else if (command.compare("PREDECESSORS") == 0) {
			processGetNodes(chord->Predecessors);
		} else if (command.compare("INFO") == 0) {
			string s = chord->toString();
			myNode->send(&s);
		} else if (command.compare("RANGE") == 0) {
			auto range = chord->getRange();
			stringstream msg_s;
			msg_s << hex << get<0>(range) << " " << hex << get<1>(range) << endl;
			string msg(msg_s.str());
			myNode->send(&msg);
		}

	}
	else if (command.compare("INSERT") == 0) {
		str >> command;
		if (command.compare("SUCCESSOR") == 0 || command.compare("PREDECESSOR") == 0) {
			processInsertNode(str, command);
		}
	}
	else if (command.compare("REPLACE") == 0) {
		str >> command;
		if (command.compare("SUCCESSOR") == 0 || command.compare("PREDECESSOR") == 0) {
			processReplaceNode(str, command);
		}
	}
	// Find queries
	else if (command.compare("FIND") == 0) {
		str >> command;
		if (command.compare("SUCCESSOR") == 0) {
			processFindSuccessor(str);
		}
	}
	// Find queries
	else if (command.compare("SEARCH") == 0) {
		str >> command;
		if (command.compare("SUCCESSOR") == 0 || command.compare("KEY") == 0) {
			processSearchSuccessor(str);
		}
	} else if (command.compare(Messages::EXIT_MSG) == 0) {
		// quit and close
		string goodbye = "Goodbye Friend!\n";
		myNode->send(&goodbye);
		return false;
	} else if (command.compare("LEAVE") == 0) {
		chord->LeaveRing();
	} else {
		cout << "Unknown Request: " << message;
	}
	return true;
}

void MessageProcessor::processGetNode(std::stringstream& stream, std::vector<std::shared_ptr<Node> >& items) {
	size_t index = -1;
	stream >> index;
	string response;

	if (items.size() > index - 1) {
		response = items[index - 1]->toString();
	} else {
		response = Messages::NOT_FOUND;
	}
	myNode->send(&response);
}

void MessageProcessor::processGetNodes(std::vector<std::shared_ptr<Node> >& items) {
	stringstream output;

	for (size_t i = 0; i < items.size(); ++i) {
		output << i + 1 << ": " << items[i]->toString();
	}

	string msg = output.str();
	myNode->send(&msg);
}

void MessageProcessor::processInsertNode(std::stringstream& stream, std::string& item) {
	int index;
	stream >> index;

	auto node = Node::createFromInfo(stream);
	if (node->getKey() == myNode->getKey()) {
		// it is this node trying to make us point to it
		node = shared_ptr<Node>(myNode);
	}
	if (item.compare("SUCCESSOR") == 0) {
		// we don't want to reset up a link to the node.
		// they just told us to set it up.
		chord->insertSuccessor(index, node, false);
	} else {
		chord->insertPredecessor(index, node, false);
	}
}

void MessageProcessor::processReplaceNode(std::stringstream& stream, std::string& item) {
	int index;
	stream >> index;
	auto node = Node::createFromInfo(stream);
	if (node->getKey() == myNode->getKey()) {
		// it is this node trying to make us point to it
		node = shared_ptr<Node>(myNode);
	}

	string twoWay;
	stream >> twoWay;

	bool bi = (twoWay.compare("TWOWAY") == 0);

	if (item.compare("SUCCESSOR") == 0) {
		// we don't want to reset up a link to the node.
		// they just told us to set it up.
		chord->replaceSuccessor(index, node, bi);
	} else {
		chord->replaceSuccessor(index, node, bi);
	}
}



MessageProcessor::~MessageProcessor() {
	// TODO Auto-generated destructor stub
}

