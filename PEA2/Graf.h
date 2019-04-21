#pragma once
#include<string>

using namespace std;

struct node{
	int a;
	int b;

	inline bool operator==(node n) {
		if (n.a == a && n.b == b)
			return true;
		if (n.a == b && n.b == a)
			return true;
		return false;
	}
};

class Graf
{
private:
	Graf(int size, int** graf);
	Graf();
	//rozmiar grafu
	int size;
	//tablica z macierzą sąsiedztwa
	int **graf;
	double PCFreq;
	__int64 CounterStart;
public:
	int TSPValue;
	int getSize();
	void StartCounter();
	double GetCounter();
	static Graf *fromFile(string fileName);
	int * TSPgreed();
	string toString();
	~Graf();
	Graf(int size, bool symetry);
	
	int* TabuTSPswap(int iterations, int tabuLength);
	int* TabuTSPinsert(int iterations, int tabuLength);
	int* TabuTSPswapTime(double Mseconds, int tabuLength);
	int* TabuTSPinsertTime(double Mseconds, int tabuLength);


};

