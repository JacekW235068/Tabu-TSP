// PEA2.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include "Graf.h"
#include <iostream>

#include<list>
#include<algorithm>

int main()
{
	list<double> times;
	times.push_back(1.0);
	times.push_back(2.0);
	times.push_back(5.0);
	times.push_back(10.0);
	times.push_back(30.0);
	times.push_back(60.0);
	int i;
	Graf*g = Graf::fromFile("data58.txt");
	//int a, b;
	//cin >> a >> b;
	//g->TabuTSPswap(a,b);
	//g->TabuTSPinsert(a,b);
	//cout << g->TSPValue << " ";
		for (list<double>::iterator j = times.begin(); j != times.end(); ++j) {
			for (int i = 1; i < 10; i++) {
				g->TabuTSPinsertTime(*j * 0.5, g->getSize()*i / 10);
				cout << g->TSPValue << " ";
			}
		cout << endl;
		}
	}
