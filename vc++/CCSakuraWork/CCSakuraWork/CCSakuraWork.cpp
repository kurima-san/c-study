// CCSakuraWork.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include <windows.h>
#include <stdio.h>
#include <time.h>
#include "People.h"

#pragma comment(lib, "winmm.lib")

HANDLE shop;
shopkeeper akito;

void buy(LPVOID data) {
	guest *human = (guest*)data;
	bool isShopping = true;
	while (isShopping) {
		//go to the shop
		Sleep(human->moveTime);

		fprintf_s(stdout, "%s arrived at the shop\n", human->name);
		WaitForSingleObject(shop, INFINITE);
		fprintf_s(stdout, "%s start shopping\n", human->name);
		Sleep(human->buyTime);

		if (akito.numStock >= human->numCarry) {
			human->numBought += human->numCarry;
			akito.numStock -= human->numCarry;
		}
		else if (akito.numStock > 0) {
			human->numBought += akito.numStock;
			akito.numStock = 0;
			isShopping = false;
		}
		else {
			isShopping = false;
		}
		fprintf_s(stdout, "%s has %d, akito has %d\n", human->name, human->numBought, akito.numStock);
		ReleaseMutex(shop);

		//go to home
		Sleep(human->moveTime);
		Sleep(human->getoffTime);
	}
}

int main()
{
	akito.numStock = 100;

	guest sakura;
	sakura.buyTime = 1000;
	sakura.moveTime = 1000;
	sakura.getoffTime = 3000;
	sakura.numCarry = 5;
	sakura.numBought = 0;
	sprintf_s(sakura.name, "sakura");

	guest tomoyo;
	tomoyo.buyTime = 2000;
	tomoyo.moveTime = 2000;
	tomoyo.getoffTime = 1000;
	tomoyo.numCarry = 3;
	tomoyo.numBought = 0;
	sprintf_s(tomoyo.name, "tomoyo");
	
	int numGuest = 2;
	HANDLE threadHandler[2];
	DWORD threadID[2];
	shop = CreateMutex(NULL, FALSE, NULL);
	fprintf_s(stdout, "start\n");
	threadHandler[0] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)buy, (LPVOID)&sakura, 0, &threadID[0]);
	threadHandler[1] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)buy, (LPVOID)&tomoyo, 0, &threadID[1]);

	for (int i = 0; i < numGuest; i++) {
		WaitForSingleObject(threadHandler[i], INFINITE);
		CloseHandle(threadHandler[i]);
	}

	fprintf_s(stdout, "<final result>\nsakura has %d, tomoyo has %d, akito has %d\n", sakura.numBought, tomoyo.numBought, akito.numStock);
	CloseHandle(shop);
    return 0;
}

