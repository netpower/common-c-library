/*
 * KThread.h
 *
 *  Created on: 2014/10/27
 *      Author: Max.Chiu
 *      Email: Kingsleyyau@gmail.com
 */

#ifndef _INC_KThread_
#define _INC_KThread_

#include <stdio.h>
#include <stdlib.h>

#ifndef _WIN32
#include <pthread.h>
#endif

#include "KMutex.h"
#include "KRunnable.h"

#ifndef _WIN32
class KThread
{
public:
	KThread();
	KThread(KRunnable *runnable);
	virtual ~KThread();
	long start(KRunnable *runnable = NULL);
	void stop();
	void sleep(uint32_t msec);
	bool isRunning() const;
	long getThreadId() const;
protected:
	virtual void onRun();
private:
	KRunnable *m_pKRunnable;
	bool m_isRunning;
	pthread_t m_pthread_t;
	static void *thread_proc_func(void *args);
};
#else

#include <windows.h>
#define pthread_t DWORD
#define uint32_t DWORD

class KThread
{
public:
	KThread();
	KThread(KRunnable *runnable);
	virtual ~KThread();
	pthread_t start(KRunnable *runnable = NULL);
	void stop();
	void sleep(uint32_t msec);
	bool isRunning() const;
	pthread_t getThreadId() const;
protected:
	virtual void onRun();
private:
	KRunnable *m_pKRunnable;
	bool m_isRunning;
	HANDLE m_pthread_t;
	DWORD m_threadId;
#ifndef _WIN32
	static void *thread_proc_func(void *args);
#else
	static DWORD WINAPI thread_proc_func(void *args);
#endif
};
#endif

#endif
