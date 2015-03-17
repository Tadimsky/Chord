/*
 * Chord.cpp
 *
 *  Created on: Mar 17, 2015
 *      Author: jonno
 */

#include <iostream>
#include <cstdio>
#include <sha.h>
#include "utils/csapp.h"
#include "Chord.h"

using namespace std;

int main(int argc, const char* argv[]) {
	string port;
	string entry_ip;
	string entry_port;

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

	if (entry_ip.empty()) {
		// this is the start of a new circle
	}

	if (!entry_ip.empty() && !entry_port.empty()) {
		// this is connecting to a node
	}



	byte output[CryptoPP::SHA1::DIGESTSIZE];
	string value = "test";
	CryptoPP::SHA1().CalculateDigest(output, (byte*) value.c_str(),
			value.length());
	cout << "!!!Hello World!!!" << endl; // prints !!!Hello World!!!
	fprintf(stdout, "Hash: %s\n", output);
	return 0;
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
	exit(1);
}

Chord::Chord() {
	// TODO Auto-generated constructor stub

}

Chord::~Chord() {
	// TODO Auto-generated destructor stub
}

