#include "stdafx.h"
#include "../hashtable.h"
#include "../timer.h"
#include <string>
#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

const size_t TABLE_SIZE = 1000000;

#if 0 
size_t fhash(const string* key)
{
  const size_t WORD_LENGTH = sizeof(int*);

  size_t index = ((size_t) key) / WORD_LENGTH;
  return index % TABLE_SIZE;
}
#endif

int main()
{
  ifstream in("test0.txt");
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

  Timer timer;

  unsigned long t = timer.time([&]() {
	  for (size_t i = 0; i < count; i++) {
		  table.insert(keys[i], vals[i]);
	  }
  });


  cout << "time: " << t << " us." << endl;

  size_t mismatchCount = 0;
  for(size_t i=0; i<count; i++) {
    int v = table.find(keys[i]);
    if (v != vals[i]) {
      cerr << "Mismatch at " << i << ". Actual: " << v << "\tExpected: " << vals[i] << endl;
	  mismatchCount++;
    }
  }

  cout << mismatchCount << "mismatches" << endl;
  cout << "Done." << endl;

  return 0;
} 
