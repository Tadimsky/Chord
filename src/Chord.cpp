//============================================================================
// Name        : Chord.cpp
// Author      : Jonno Schmidt
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <cstdio>
#include "../lib/cryptopp/sha.h"
using namespace std;

int main() {
	byte output[CryptoPP::SHA1::DIGESTSIZE];
	string value = "test";
	CryptoPP::SHA1().CalculateDigest(output, (byte*)value.c_str(), value.length());
	cout << "!!!Hello World!!!" << endl; // prints !!!Hello World!!!
	fprintf(stdout, "Hash: %s\n", output);
	return 0;
}
