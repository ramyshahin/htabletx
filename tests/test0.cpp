#include "stdafx.h"
#include "../hashtable.h"
#include "../timer.h"
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <omp.h>

using namespace std;

const size_t TABLE_SIZE = 2000000;

int main()
{
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

  unsigned long t = timer.time([&]() {
      #pragma omp parallel for num_threads(32)
	  for (int i = 0; i < count; i++) {
		  table.insert(keys[i], vals[i]);
	  }
  });


  cout << "time: " << t << " us." << endl;

  size_t mismatchCount = 0;
  for(size_t i=0; i<count; i++) {
    int v = table.find(keys[i]);
    if (v != vals[i]) {
      cerr << "Mismatch at " << keys[i] << ". Actual: " << v << "\tExpected: " << vals[i] << endl;
	  mismatchCount++;
    }
  }

  cout << mismatchCount << "mismatches" << endl;
  cout << "Done." << endl;

  return 0;
} 
