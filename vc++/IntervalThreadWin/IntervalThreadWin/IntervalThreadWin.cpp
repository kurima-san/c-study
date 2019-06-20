// IntervalThreadWin.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//
#include "stdafx.h"
#include <windows.h>
#include <mmsystem.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <vector>

#pragma comment(lib, "winmm.lib")
#define NUMTHREAD 4

#define USEMUTEX

HANDLE threadMutex;
int gCounter;
std::vector<int> threadQueue;


void CALLBACK intervalSample(UINT uTimerID, UINT uMsg, DWORD dwUser, DWORD dummy1, DWORD dummy2) {
	// タイマー割り込みの処理
	//(*(int*)dwUser)++;//引数渡し例count
	int *count = (int*)dwUser;//引数渡し例count
	*count++;
	fprintf(stdout, "Timer interval %d\n", *count);
	return;
}

void process(int waitTime) {
	for (int i = 0; i < 5; i++) {
		fprintf_s(stdout, "!try to start threadA(%d) process\n", waitTime);
		threadQueue.push_back(waitTime);
#ifdef USEMUTEX
		WaitForSingleObject(threadMutex, INFINITE);
#endif
		fprintf_s(stdout, "start threadA(%d) process\n", waitTime);
		int prev = gCounter;
		Sleep(waitTime);
		gCounter += waitTime;
		fprintf_s(stdout, "finish threadA(%d) process %d -> %d\n", waitTime, prev, gCounter);
		if (!threadQueue.empty()) {
			threadQueue.erase(threadQueue.begin());
		}
		if (!threadQueue.empty()) {
			fprintf_s(stdout, "threadA queue =");
			for (auto itr = threadQueue.begin(); itr != threadQueue.end(); itr++) {
				fprintf_s(stdout, " %d", *itr);
			}
			fprintf_s(stdout, "\n\n");
			//fprintf_s(stdout, "next threadA(%d) process will start\n\n", *threadQueue.begin());
		}
		else {
			fprintf_s(stdout, "finish all\n\n");
		}
#ifdef USEMUTEX
		ReleaseMutex(threadMutex);
#endif
	}
}

void threadSampleP(LPVOID *data) {
	fprintf_s(stdout, "waitTime pointer = %x\n", (unsigned int)data);
	process(*(int*)data);
}

void threadSample(LPVOID data) {
	process((int)data);
}

int main() {
	fprintf(stdout, "Pass with variable\n");

	int count = 0;
	HANDLE threadHandler[NUMTHREAD];
	DWORD threadID[NUMTHREAD];
	threadMutex = CreateMutex(NULL, FALSE, NULL);	//ミューテックス生成
	for (int i = 0; i < NUMTHREAD; i++) {
		int waitTime = (i + 1) * 100;
		fprintf_s(stdout, "create thread %d\n", waitTime);
		threadHandler[i] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)threadSample, (LPVOID)waitTime, 0, &threadID[i]);
	}

	for (int i = 0; i < NUMTHREAD; i++) {
		WaitForSingleObject(threadHandler[i], INFINITE);
		CloseHandle(threadHandler[i]);
	}

	CloseHandle(threadMutex);
	fprintf(stdout, "End \n");

	fprintf(stdout, "\n\n\nPass with pointer\n");

	threadMutex = CreateMutex(NULL, FALSE, NULL);	//ミューテックス生成
	for (int i = 0; i < NUMTHREAD; i++) {
		int waitTime = (i + 1) * 100;
		fprintf_s(stdout, "create thread %d\n", waitTime);
		threadHandler[i] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)threadSampleP, (LPVOID)&waitTime, 0, &threadID[i]);
		//waitTimeのポインターを渡す場合はCreateThreadがスタートする前に次のforでwaitTimeが更新されるといざ前のCreateThreadが実行される際に値が変わっている
		//どうやらwaitTimeのポインターはforで次のループに移っても同じポインターを使いまわすらしい，だからいつの間にか次のループで上書きされている
		//この場合Sleepでまっても回避できるがSleepの必要時間は環境依存なのでポインターを渡す行為を避けるべきである
		Sleep(100);
	}

	for (int i = 0; i < NUMTHREAD; i++) {
		WaitForSingleObject(threadHandler[i], INFINITE);
		CloseHandle(threadHandler[i]);
	}

	CloseHandle(threadMutex);
	fprintf(stdout, "End \n");

	MMRESULT timerID = timeSetEvent(1000,// 間隔[ms]
		0,// 分解能
		intervalSample,// 割り込み関数
		(DWORD)&count,// ユーザーパラメータ
		TIME_PERIODIC | TIME_CALLBACK_FUNCTION// 動作フラグ
	);
	if (!timerID) {
		fprintf(stderr, "ERROR:Failed\n");
		return -2;
	}
	int i;
	fprintf_s(stdout, "input int to finish>>");
	scanf_s("%d",&i);
	timeKillEvent(timerID);
	
	return 0;
}
