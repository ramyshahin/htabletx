#ifndef _TX_POLICY_H
#define _TX_POLICY_H

#include <immintrin.h>
#include <windows.h>
#include <Synchapi.h>

typedef std::function<void(void)> Action;

class TxNon
{
private:
	SRWLOCK lock; // reader-writer lock

public:
	__forceinline TxNon()
	{
		InitializeSRWLock(&lock);
	}

	__forceinline void shared(const Action& act)
	{
		AcquireSRWLockShared(&lock);
		act();
		ReleaseSRWLockShared(&lock);
	}

	__forceinline void exclusive(const Action& act)
	{
		AcquireSRWLockExclusive(&lock);
		act();
		ReleaseSRWLockExclusive(&lock);
	}
}; // TxNon

template<size_t RETRY_COUNT=0>
class TxRTM
{
private:
	SRWLOCK lock; // reader-writer lock

#ifdef TX_DIAGNOSTICS
	size_t totalTx;
	size_t totalRetry;
	size_t totalLocks;
#endif //TX_DIAGNOSTICS

protected:
	__forceinline void TxEnd()
	{
		_xend();
	}

public:
	__forceinline TxRTM()
	{
#ifdef TX_DIAGNOSTICS
		totalTx = 0;
		totalRetry = 0;
		totalLocks = 0;
#endif //TX_DIAGNOSTICS
	}

	__forceinline void shared(const Action& act)
	{
		volatile unsigned int xbegin_ret;
		unsigned int r = 0;
		bool tx = false;
		while (r <= RETRY_COUNT) {
#ifdef TX_DIAGNOSTICS
			InterlockedIncrement(&totalTx);
#endif //TX_DIAGNOSTICS
			xbegin_ret = _xbegin();
			if (xbegin_ret == _XBEGIN_STARTED) {
				tx = true;
				break;
			}
			r++;
#ifdef TX_DIAGNOSTICS
			InterlockedIncrement(&totalRetry);
#endif //TX_DIAGNOSTICS
		}

		if (!tx) {
			AcquireSRWLockShared(&lock);
#ifdef TX_DIAGNOSTICS
			InterlockedIncrement(&totalLocks);
#endif //TX_DIAGNOSTICS
		}

		act();

		if (tx) {
			_xend();
		}
		else {
			ReleaseSRWLockShared(&lock);
		}
	}

	__forceinline void exclusive(const Action& act)
	{
		volatile unsigned int xbegin_ret;
		unsigned int r = 0;
		bool tx = false;
		while (r <= RETRY_COUNT) {
#ifdef TX_DIAGNOSTICS
			InterlockedIncrement(&totalTx);
#endif //TX_DIAGNOSTICS
			xbegin_ret = _xbegin();
			if (xbegin_ret == _XBEGIN_STARTED) {
				tx = true;
				break;
			}
			r++;
#ifdef TX_DIAGNOSTICS
			InterlockedIncrement(&totalRetry);
#endif //TX_DIAGNOSTICS
		}

		if (!tx) {
			AcquireSRWLockExclusive(&lock);
#ifdef TX_DIAGNOSTICS
			InterlockedIncrement(&totalLocks);
#endif //TX_DIAGNOSTICS
		}

		act();

		if (tx) {
			_xend();
		}
		else {
			ReleaseSRWLockExclusive(&lock);
		}
	} // exclusive

#ifdef TX_DIAGNOSTICS
	__forceinline void ResetCounts() {
		totalTx = 0;
		totalRetry = 0;
		totalLocks = 0;
	}

	__forceinline size_t GetTxCount() const {
		return totalTx;
	}

	__forceinline size_t GetRetryCount() const {
		return totalRetry;
	}

	__forceinline size_t GetLockCount() const {
		return totalLocks;
	}
#endif //TX_DIAGNOSTICS
}; // TxRTM

#endif // _TX_POLICY_H