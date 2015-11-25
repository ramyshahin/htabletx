#ifndef _HASHTABLE_H
#define _HASHTABLE_H

#include <functional>
#include <cstring>
#include <exception>
#include <immintrin.h>
#include <iostream>

using namespace std;

size_t index = 0;

inline void TX(function<void(void)> F) {  
  index++;
  volatile auto xbegin_ret = _xbegin();
  if (xbegin_ret == _XBEGIN_STARTED) {
    F();    
	_xabort(0xff);
    //_xend();
  }
  else {
    cerr << "_xbegin() failed: " << xbegin_ret << " Index: " << index << endl;
  }
}

class TableFull : public std::exception
{
}; // class TableFull

template<
  typename TK,                    // key type
  typename TV,                    // value type
  typename H = std::hash<TK>,     // hashing functor			
  TK       NIL_KEY = TK()
  >
class HashTable
{
private:
  struct Entry {
    TK key;
    TV value;
    
    Entry()
    {
    }

    Entry(const TK& k, const TV& v): key(k), value(v)
    {
    } 
  }; // struct Entry

  Entry* table;
  size_t numEntries;
  H fhash;
  unsigned int mutex;

public:
  HashTable(size_t size):     
    table(new Entry[size]),
    numEntries(size),
	mutex(0)
  {
    memset(table, 0, sizeof(Entry) * size);
  }

  ~HashTable()
  {
    delete[] table;
  }

  bool lock() {
	  if (_xbegin() == _XBEGIN_STARTED)
	  {
		  if (mutex == 0) {
			  return true;
		  }
		  _xabort(0xff);
	  }
	  cout << "lock elision failed" << endl;
	  return false;
  }

  void unlock() {
	  if (mutex == 0) {
		  _xend();
	  }
	  else {
		  cout << "unlock: lock elision failed" << endl;
	  }
  }

  void insert(const TK& key, const TV& val)
  {
	  Entry e(key, val);
	  size_t index = fhash(key) % numEntries;
	  size_t probeCount = 0;

	  if (lock())
	  {
		  while (table[index].key != NIL_KEY && probeCount < numEntries) {
			  index = (index + 1) % numEntries;
			  probeCount++;
		  }

		  if (probeCount == numEntries) {
			  cerr << "Full\n";
			  throw TableFull();
		  }
		  table[index] = e;

		  unlock();
	  }
  }

  TV find(const TK& k) 
  {
    size_t index = fhash(k) % numEntries;
    size_t probeCount = 0;

    TV ret = 0;

	//lock();
    {
      while(table[index].key != NIL_KEY && probeCount < numEntries) {
        if (table[index].key == k) {
          ret = table[index].value;
        }
  
        index = (index+1) % numEntries;
        probeCount++;
      } //while 
    }
	//unlock();

    return ret;
  } // operator[]

}; // class HashTable 

#endif //_HASHTABLE_H
