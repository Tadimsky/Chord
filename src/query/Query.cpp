/*
 * Query.cpp
 *
 *  Created on: Mar 22, 2015
 *      Author: jonno
 */

#include "Query.h"
#include "../Chord.h"
#include <iostream>
#include <iomanip>

using namespace std;

namespace Query {

Query::Query(std::string ip, std::string port) {
	int portN = stoi(port);
	myNode = unique_ptr<Node>(new Node(ip, portN));
	if (myNode->Connect()) {
		cout << "Connected to node " << ip <<", port " << portN << ", position " << hex << myNode->getKey() << endl;
	}
	else {
		cout << "Could not connect to Node." << endl;
		exit(-1);
	}
}

Query::~Query() {
	// TODO Auto-generated destructor stub
}

void Query::Start() {

	while (true) {
		string command;
		cout << setfill('-') << setw(50) << "-" << endl;
		cout << "Please enter your search key (or type 'quit' to leave):" << endl;
		cout << "Query: ";
		getline(cin, command);

		if (command.find("quit") == 0) {
			cout << "Goodbye!" << endl;
			return;
		}
		else {
			size_t hash = Chord::hashKey(command);
			cout << "Hash value: " << hex << hash << endl;
			auto dataHost = myNode->SearchSuccessor(hash);
		}
		cout << endl;
	}
}

} /* namespace Query */
