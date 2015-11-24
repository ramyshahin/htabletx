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
  size_t count;
  in >> count;

  count = 5;
  vector<int> keys(count);
  vector<int> vals(count);

  for(size_t i=0; i<count; i++) {
    int key, val;
    in >> key >> val;
    keys.push_back(key);
    vals.push_back(val);
  }

  HashTable<int, int> table(TABLE_SIZE);

  Timer timer;

  Microseconds t = timer.time([&]() {
	  for (size_t i = 0; i < count; i++) {
		  table.insert(keys[i], vals[i]);
	  }
  });


  cout << "time: " << t << " us." << endl;

  for(size_t i=0; i<count; i++) {
    int v = table.find(keys[i]);
    if (v != vals[i]) {
      cerr << "Mismatch at " << keys[i] << ". Actual: " << v << "\tExpected: " << vals[i] << endl;
    }
  }

  cout << "Done." << endl;

  return 0;
} 
