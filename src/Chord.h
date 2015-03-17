/*
 * Chord.h
 *
 *  Created on: Mar 17, 2015
 *      Author: jonno
 */

#ifndef CHORD_H_
#define CHORD_H_

typedef unsigned char byte;

void showUsageMessage(std::string procname);

class Chord {
private:
	int myListenFD;
	int myListenPort;

public:
	Chord(int port);
	virtual ~Chord();
	void test();
};

#endif /* CHORD_H_ */
