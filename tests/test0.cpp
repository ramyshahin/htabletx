#include "stdafx.h"

#define TX_DIAGNOSTICS
#define HT_PROBING
//#define HT_BUCKETS

#ifdef HT_PROBING
#include "../ht_probing.h"
#endif //HT_PROBING

#ifdef HT_BUCKETS
#include "../ht_buckets.h"
#endif //HT_BUCKETS

#include "../timer.h"
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <omp.h>

using namespace std;

#ifdef HT_PROBING
const size_t TABLE_SIZE = 2000000;
#endif

#ifdef HT_BUCKETS
const size_t TABLE_SIZE = 1000000;
#endif

int main(int argc, char* argv[])
{
  if (argc != 2)
  {
	  cerr << "Usage: " << argv[0] << " thread-count" << endl;
	  return -1;
  }

  int threadCount = atoi(argv[1]);

  ifstream in("test1M.txt");
  if (!in.is_open()) {
	  cerr << "Couldn't open input file" << endl;
	  return -1;
  }

  size_t count;
  in >> count;

  vector<int> keys; keys.reserve(count);
  vector<int> vals; vals.reserve(count);

  for(size_t i=0; i<count; i++) {
    int key, val;
    in >> key >> val;
    keys.push_back(key);
    vals.push_back(val);
  }

  HashTable<int, int> table(TABLE_SIZE);

  omp_set_dynamic(0);     // Explicitly disable dynamic teams
  
  Timer timer;

  Microseconds t = timer.time([&]() {
      #pragma omp parallel for num_threads(threadCount)
	  for (int i = 0; i < count; i++) {
		  table.insert(keys[i], vals[i]);
	  }
  });


  cout << threadCount << "\t" << t << "\t";
  
#ifdef TX_DIAGNOSTICS
  table.dumpDiagnostics(cout);
#endif // TX_DIAGNOSTICS

  cout << endl;

  size_t mismatchCount = 0;
  for(size_t i=0; i<count; i++) {
    int v = table.find(keys[i]);
    if (v != vals[i]) {
      cerr << "Mismatch at " << keys[i] << ". Actual: " << v << "\tExpected: " << vals[i] << endl;
	  mismatchCount++;
    }
  }

  //cout << mismatchCount << "mismatches" << endl;
  //cout << "Done." << endl;

  return 0;
} 
