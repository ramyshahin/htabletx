#ifndef _HT_BUCKETS_H
#define _HT_BUCKETS_H

#include <functional>
#include <cstring>
#include <exception>
#include <iostream>
#include <list>
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

	bool operator==(const TK& k) const
	{
		return key == k;
	}
  }; // struct Entry

  std::list<Entry>* table;
  size_t numBuckets;
  H fhash;
  
  static const TK NIL_KEY;

public:
  HashTable(size_t buckets):     
    table(new std::list<Entry>[buckets]),
    numBuckets(buckets)
  {
  }

  ~HashTable()
  {
    delete[] table;
  }

  void insert(const TK& key, const TV& val)
  {
	  Entry e(key, val);
	  size_t index = fhash(key) % numBuckets;
	  
	  exclusive([&]()
	  {
		  table[index].push_back(e);
	  });
  }

  TV find(const TK& k) 
  {
    size_t index = fhash(k) % numBuckets;
    size_t probeCount = 0;

    TV ret = 0;

	shared([&]()
    {
		const list<Entry>& l = table[index];
		auto ie = std::find(l.begin(), l.end(), k);
		ret = ie->value;
	});
	
    return ret;
  } // operator[]

  void insertOtherwiseUpdate(
	  const TK& key,
	  const TV& val,
	  const function<void(TV&)>& fUpdate
	  )
  {
	  size_t index = fhash(key) % numBuckets;

	  exclusive([&]()
	  {
		  auto& bucket = table[index];
		  auto& it = std::find(bucket.begin(), bucket.end(), key);
		  if (it == bucket.end()) {
			  Entry e(key, val);
			  bucket.push_back(e);
		  }
		  else {
			  fUpdate(it->value);
		  }
	  });
  }

  void dump(ostream& out) {
	  for (size_t i = 0; i < numBuckets; i++) {
		  auto& bucket = table[i];
		  for (const auto& item : bucket) {
			  out << item.key << "\t" << item.value << endl;
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

#endif //_HT_BUCKETS_H
