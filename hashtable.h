#ifndef _HASHTABLE_H
#define _HASHTABLE_H

#include <functional>
#include <cstring>
#include <exception>
#include <immintrin.h>

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

public:
  HashTable(size_t size):     
    table(new Entry[size]),
    numEntries(size)
  {
    memset(table, sizeof(Entry) * size, 0);
  }

  ~HashTable()
  {
    delete[] table;
  }

  void insert(const TK& key, const TV& val)
  {
    Entry e(key, val);
    size_t index = fhash(key);
    size_t probeCount = 0;

    while(table[index].key != NIL_KEY && probeCount < numEntries) {
      index = (index + 1) % numEntries;
      probeCount++;
    }

    if (probeCount == numEntries) {
      throw TableFull();
    }

    table[index] = e;
  }

  TV find(const TK& k) 
  {
    size_t index = fhash(k);
    size_t probeCount = 0;

    while(table[index].key != NIL_KEY && probeCount < numEntries) {
      if (table[index].key == k) {
        return table[index].value;
      }
  
      index = (index+1) % numEntries;
      probeCount++;
    } //while 

    return 0;
  } // operator[]

}; // class HashTable 

#endif //_HASHTABLE_H
