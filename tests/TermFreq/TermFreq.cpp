// TermFreq.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <string>
#include <strstream>
#include <vector>
#include <omp.h>
#include <windows.h>

#define USE_PROBING
//#define USE_BUCKETS

#ifdef USE_BUCKETS
#include "..\..\ht_buckets.h"
const size_t TABLE_SIZE = 1000000;
#endif

#ifdef USE_PROBING
#include "..\..\ht_probing.h"
const size_t TABLE_SIZE = 10000000;
#endif

#include "..\..\timer.h"


using namespace std;

__forceinline bool validToken(string& s) {
	if (s.size() < 2) {
		return false;
	}

	for (auto& c : s) {
		if (c < 'A' || c > 'z' || (c > 'Z' && c < 'a')) {
			return false;
		}
		else {
			c = toupper(c);
		}
	}

	return true;
}

#if 0
class ParallelFileWrapper
{
private:
	istream& in;
	CRITICAL_SECTION critSect;

public:
	ParallelFileWrapper(istream& i) : in(i)
	{
		InitializeCriticalSection(&critSect);
	}

	~ParallelFileWrapper() {
		DeleteCriticalSection(&critSect);
	}

	bool BatchRead(vector<string>& batch) {
		size_t i = 0;

		EnterCriticalSection(&critSect);

		while (!in.eof() && i < batch.size()) {
			getline(in, batch[i]);
			if (batch[i].size() > 0) {
				i++;
			}
		}

		LeaveCriticalSection(&critSect);

		return (i > 0);
	}
}; // ParallelFile
#endif

__forceinline void inc(size_t& s) {
	s++;
}

int main(int argc, char* argv[])
{
	if (argc != 2) {
		cerr << "Usage: " << argv[0] << " thread-count\n";
		return -1;
	}

	int threadCount = atoi(argv[1]);

	HashTable<string, size_t> table(TABLE_SIZE);

	omp_set_dynamic(0);     // Explicitly disable dynamic teams

	Timer timer;

	const char splitters[] = "\t\r\n ,.;\'\"()@*-_!";

	const size_t PART_COUNT = 32;

	size_t count = 0;
	Microseconds t = timer.time([&]() {
#pragma omp parallel for num_threads(threadCount)
		for (int i = 0; i < PART_COUNT; i++) {
			char partName[] = "000.part";
			partName[1] = '0' + i / 10;
			partName[2] = '0' + i % 10;
			ifstream in(partName);
			if (!in.good()) {
				cerr << "Can not open file " << partName << endl;
			}

			while (!in.eof()) {
				string l;
				getline(in, l);

				char* c = strtok((char*)l.c_str(), splitters);
				while (c) {
					string token(c);

					if (validToken(token)) {
						table.insertOtherwiseUpdate(token, 1, inc);
					}
					c = strtok(NULL, splitters);
				}
			}
		} // for
	});

	ofstream out("termFreq.dump");
	table.dump(out);

	ofstream timeLog("time.log", ios::app);

	timeLog << threadCount << "\t" << t/1000000.0 << endl;

	return 0;
}

