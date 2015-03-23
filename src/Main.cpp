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
#include <signal.h>
#include <thread>
#include "Chord.h"
#include "query/Query.h"

using namespace std;

void handleSignal(int signal) {
	cout << "Received signal: " << signal << endl;
	// get out before we quit
	Chord::getInstance()->LeaveRing();
	exit(EXIT_SUCCESS);
}

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
	cerr << endl;
	cerr << "Alternatively, you can run " << procname << " query ip port" << endl;
	cerr << "\t This will allow you to interact with the Chord network" << endl;
	exit(1);
}

int main(int argc, const char* argv[]) {
	string port;
	string entry_ip, entry_port;

	int listen_port;

	if (argc < 2) {
		showUsageMessage(argv[0]);
	}

	// port to listen on
	if (argc > 1) {
		port = argv[1];
	}

	if (port.find("query") == 0) {
		// this is a call to query;
		if (argc == 4) {
			entry_ip = argv[2];
			entry_port = argv[3];

			Query::Query q(entry_ip, entry_port);
			q.Start();
		}
		else {
			showUsageMessage(argv[0]);
		}

		return 0;
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

	shared_ptr<Chord> chord;
	chord = Chord::getInstance();
	if (chord != nullptr) {
		chord->init(listen_port);
	}

	if (entry_ip.empty()) {

	}

	thread listenThread(&Chord::Listen, chord);

	if (!entry_ip.empty() && !entry_port.empty()) {
		int entry_port_i = atoi(entry_port.c_str());
		chord->JoinRing(entry_ip, entry_port_i);
	}

	// setup signal handler
	struct sigaction signal;
	signal.sa_handler = handleSignal;
	sigemptyset(&(signal.sa_mask));
	signal.sa_flags = SA_RESTART;

	sigaction(SIGINT, &signal, NULL);
	sigaction(SIGTERM, &signal, NULL);

	// process commands on this node.
	while (true) {
		cout << "Welcome to Chord." << endl;

		string command;
		cin >> command;

		if (command.compare("LEAVE") == 0){
			chord->LeaveRing();
		}
		else {
			cout << "You can type LEAVE in order to leave the network." << endl;
		}

	}
	return 0;
}




