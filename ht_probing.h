#ifndef _HT_PROBING_H
#define _HT_PROBING_H

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
  typename TX_POLICY = TxRTM<5>
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
  
  TK       NIL_KEY;
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

	  exclusive([&]()
	  {
		  while (table[index].key != NIL_KEY && probeCount < numEntries) {
			  index = (index + 1) % numEntries;
			  probeCount++;
		  }

		  //if (probeCount == numEntries) {
		//	  cerr << "Full\n";
			//  throw TableFull();
		  //}
		  table[index] = e;
	  });
  }

  TV find(const TK& k) 
  {
    size_t index = fhash(k) % numEntries;
    size_t probeCount = 0;

    TV ret = 0;

	shared([&]()
    {
      while(table[index].key != NIL_KEY && probeCount < numEntries) {
        if (table[index].key == k) {
          ret = table[index].value;
		  break;
        }
  
        index = (index+1) % numEntries;
        probeCount++;
      } //while 
	});
	
    return ret;
  } // operator[]

  void insertOtherwiseUpdate(
	  const TK& key,
	  const TV& val,
	  const function<void(TV&)>& fUpdate
	  )
  {
	  size_t index = fhash(key) % numEntries;

	  exclusive([&]()
	  {
		  size_t probeCount = 0;
		  bool found = false;
		  while (table[index].key != NIL_KEY && probeCount < numEntries) {
			  if (table[index].key == key) {
				  fUpdate(table[index].value);
				  found = true;
				  break;
			  }

			  
			  index = (index + 1) % numEntries;
			  probeCount++;
		  } //while 

		  if (!found) {
			  Entry e(key, val);
			  table[index] = e;
		  }
	  });
  }

  void dump(ostream& out) {
	  for (size_t i = 0; i < numEntries; i++) {
		  if (table[i].key != NIL_KEY) {
			  out << table[i].key << "\t" << table[i].value << endl;
		  }
	  }
  }

#ifdef TX_DIAGNOSTICS
  void dumpDiagnostics(ostream& out) const
  {
	  //double pctRetry = (((double)GetRetryCount()) / GetTxCount()) * 100.0;
	  //out << GetTxCount() << "\t" << GetRetryCount() << "\t" << pctRetry << "\t" << GetLockCount() << "\t";
	  dumpCommitHistogram(out);
  }
#endif //TX_DIAGNOSTICS
}; // class HashTable 

#endif //_HT_PROBING_H
