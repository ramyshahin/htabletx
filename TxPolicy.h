#ifndef _TX_POLICY_H
#define _TX_POLICY_H

#include <immintrin.h>

class TxNon
{
public:
	__forceinline bool TxBegin()
	{
		return true;
	}

	__forceinline void TxEnd()
	{

	}
}; // TxNon

template<size_t RETRY_COUNT=0>
class TxRTM
{
public:
	__forceinline bool TxBegin()
	{
		volatile unsigned int xbegin_ret;
		unsigned int r = 0;
		while (r < RETRY_COUNT)
		{
			xbegin_ret = _xbegin();
			if (xbegin_ret == _XBEGIN_STARTED)
			{
				return true;
			}
			r++;
		}
		cout << "lock elision failed. Status: " << std::hex << xbegin_ret << endl;
		return false;
	}

	__forceinline void TxEnd()
	{
		_xend();
	}
}; // TxRTM

#endif // _TX_POLICY_H