// PartitionFile.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
using namespace std;

int main(int argc, char* argv[])
{
	if (argc != 3) {
		cout << "Usage: " << argv[0] << " filename num-partitions\n";
		return -1;
	}

	ifstream in(argv[1]);
	size_t partCount = atoi(argv[2]);

	if (!in.good()) {
		cerr << "Can not open file " << argv[1] << endl;
		return -1;
	}

	vector<ofstream*> out(partCount);

	char partName[] = "000.part";
	for (size_t i = 0; i < partCount; i++) {
		partName[0] = '0' + (i / 100);
		partName[1] = '0' + (i % 100) / 10;
		partName[2] = '0' + i % 10;

		out[i] = new ofstream(partName);
		if (!out[i]->good()) {
			cerr << "Can not open file " << partName << endl;
			return -1;
		}
	}

	cout << "Partitioning " << argv[1] << " into " << partCount << " partitions.\n";

	size_t partitionIndex = 0;

	while (!in.eof()) {
		string s;
		getline(in, s);
		if (s.size() == 0) {
			continue;
		}

		(*out[partitionIndex]) << s << endl;
		
		partitionIndex++;

		if (partitionIndex == partCount) {
			partitionIndex = 0;
		}
	}

	for (size_t i = 0; i < partCount; i++) {
		delete out[i];
	}
    return 0;
}

