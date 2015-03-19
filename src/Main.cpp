/*
 * Main.cpp
 *
 *  Created on: Mar 17, 2015
 *      Author: jonno
 */
#include <iostream>
#include <cstdio>
#include <sha.h>
#include <memory>
#include "Chord.h"

using namespace std;

void showUsageMessage(string procname) {
	cerr << "To run Chord, you need to provide a port to listen on." << endl;
	cerr << "\tUsage: " << procname << " port" << endl;
	cerr << "\tExample: " << procname << " 8001" << endl;
	cerr
			<< "If you are connecting to an existing Chord network, use the following:"
			<< endl;
	cerr << "\t" << procname
			<< " port [entry point IP address] [entry point port]" << endl;
	cerr << "\tExample: " << procname << " 8001 128.2.205.42 8010" << endl;
	exit(1);
}

int main(int argc, const char* argv[]) {
	string port;
	string entry_ip, entry_port;

	shared_ptr<Chord> chord;

	int listen_port;

	if (argc < 2) {
		showUsageMessage(argv[0]);
	}

	// port to listen on
	if (argc > 1) {
		port = argv[1];
	}
	// connecting to an entry point
	if (argc > 2) {
		entry_ip = argv[2];
	}
	// entry point port
	if (argc > 3) {
		entry_port = argv[3];
	}

	if (!entry_ip.empty() && entry_port.empty()) {
		showUsageMessage(argv[0]);
	}

	listen_port = atoi(port.c_str());

	chord = Chord::getInstance();
	if (chord != nullptr) {
		chord->init(listen_port);
	}

	if (entry_ip.empty()) {

	}

	if (!entry_ip.empty() && !entry_port.empty()) {
		int entry_port_i = atoi(entry_port.c_str());
		chord->JoinRing(entry_ip, entry_port_i);
	}

	chord->Listen();

	return 0;
}




