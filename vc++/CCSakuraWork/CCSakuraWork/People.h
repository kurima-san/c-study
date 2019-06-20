#pragma once

#define CHARBUFF 124

struct guest
{
	char name[CHARBUFF];
	int moveTime;
	int getoffTime;
	int buyTime;
	int numCarry;
	int numBought;
};

struct shopkeeper
{
	int numStock;
};


