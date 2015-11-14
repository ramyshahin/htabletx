#include "../hashtable.h"
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

  vector<int> keys(count);
  vector<int> vals(count);

  for(size_t i=0; i<count; i++) {
    int key, val;
    in >> key >> val;
    keys.push_back(key);
    vals.push_back(val);
  }

  HashTable<int, int> table(TABLE_SIZE);

  timespec start, end;

  if (clock_gettime(CLOCK_REALTIME, &start)) {
    cerr << "gettime failed\n";
  }

  for(size_t i=0; i<count; i++) {
    table.insert(keys[i], vals[i]);
  }

  if (clock_gettime(CLOCK_REALTIME, &end)) {
    cerr << "gettime failed\n";
  }

  cout << "Start time: " << start.tv_sec << ", " << start.tv_nsec << endl;
  cout << "End   time: " << end.tv_sec   << ", " << end.tv_nsec << endl;
  cout << "time delta: " << end.tv_nsec - start.tv_nsec << " ns." << endl;

  for(size_t i=0; i<count; i++) {
    int v = table.find(keys[i]);
    if (v != vals[i]) {
      cerr << "Mismatch at " << keys[i] << ". Actual: " << v << "\tExpected: " << vals[i] << endl;
    }
  }

  cout << "Done." << endl;

  return 0;
} 
