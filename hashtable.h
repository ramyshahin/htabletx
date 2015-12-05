#ifndef _HASHTABLE_H
#define _HASHTABLE_H

#include <functional>
#include <cstring>
#include <exception>
#include <iostream>
#include "TxPolicy.h"

using namespace std;

size_t index = 0;

class TableFull : public std::exception
{
}; // class TableFull

template<
  typename TK,                    // key type
  typename TV,                    // value type
  typename H = std::hash<TK>,     // hashing functor			
  TK       NIL_KEY = TK(),
  typename TX_POLICY = TxNon //TxRTM<2>
  >
class HashTable : protected TX_POLICY
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
  
public:
  HashTable(size_t size):     
    table(new Entry[size]),
    numEntries(size)
  {
    memset(table, 0, sizeof(Entry) * size);
  }

  ~HashTable()
  {
    delete[] table;
  }

  void insert(const TK& key, const TV& val)
  {
	  Entry e(key, val);
	  size_t index = fhash(key) % numEntries;
	  size_t probeCount = 0;

	  if (TxBegin())
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
		  TxEnd();
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
		  break;
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
