// AvgPerfResults.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <vector>

using namespace std;
int main(int argc, char* argv[])
{
	if (argc != 3) {
		cerr << "Usage: " << argv[0] << " filename result-count\n";
		return -1;
	}

	size_t resultCount = atoi(argv[2]);

	vector<string> expNames;
	vector<double>* results = new vector<double>[resultCount];
	vector<size_t> counts;

	ifstream in(argv[1]);
	if (!in.good()) {
		cerr << "Can not open file " << argv[1] << endl;
		return -1;
	}

	double* result = new double[resultCount];

	while (!in.eof())
	{
		char name[80];
		

		in >> name;

		if (name[0] == '\0') {
			break;
		}

		for (size_t i = 0; i < resultCount; i++) {
			in >> result[i];
		}

		string str(name);

		auto it = find(expNames.begin(), expNames.end(), str);
		if (it == expNames.end()) {
			expNames.push_back(str);

			for (size_t i = 0; i < resultCount; i++) {
				results[i].push_back(result[i]);
			}
			
			counts.push_back(1);
		}
		else {
			size_t index = it - expNames.begin();

			for (size_t i = 0; i < resultCount; i++) {
				results[i][index] += result[i];
			}
			
			counts[index]++;
		}
	}

	delete[] result;

	string outputFileName(string(argv[1]) + ".avg");
	ofstream out(outputFileName);
	if (!out.good()) {
		cerr << "Can not open file " << outputFileName.c_str() << endl;
		return -1;
	}

	for (size_t i = 0; i < expNames.size(); i++) {
		out << expNames[i].c_str();
		
		for (size_t j = 0; j < resultCount; j++) {
			out << "\t" << (results[j][i] / counts[i]);
		}

		out << endl;
	}

	delete[] results;
    return 0;
}

