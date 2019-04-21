#include "pch.h"
#include "Graf.h"
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <windows.h>
#include <list>
#include <algorithm>

#pragma region Czas

void Graf::StartCounter()
{
	LARGE_INTEGER li;
	if (!QueryPerformanceFrequency(&li))
		cout << "QueryPerformanceFrequency failed!\n";

	PCFreq = double(li.QuadPart) / 1000.0;

	QueryPerformanceCounter(&li);
	CounterStart = li.QuadPart;
}
double Graf::GetCounter()
{
	LARGE_INTEGER li;
	QueryPerformanceCounter(&li);
	return double(li.QuadPart - CounterStart) / PCFreq;
}

#pragma endregion




Graf* Graf::fromFile(string fileName)
{
	int size;
	int **tab;
		fstream f;
		f.open(fileName, ios::in);
		if (!f.good())
			return nullptr;
		f >> size;
		tab = new int*[size];
		for (int i = 0; i < size; i++) {
			tab[i] = new int[size];
			for (int j = 0; j < size; j++)
				f >> tab[i][j];
		}
		f.close();
		return new Graf(size, tab);
}
#pragma region alorytm_TSP
int *Graf::TSPgreed() {
	int min = INT_MAX;
	int minIndex;
	TSPValue = 0;
	int* tab = new int[size + 1];
	for (int i = 0; i < size; i++) {
		tab[i] = i;
	}
	tab[size] = 0;
	for (int i = 0; i < size-2; i++) {
		for (int j = i+1; j < size; j++) {
			if (min > graf[tab[i]][tab[j]]){
				minIndex = j;
				min = graf[tab[i]][tab[j]];
			}
		}
		TSPValue += min;
		min = tab[i + 1];
		tab[i + 1] = tab[minIndex];
		tab[minIndex] = min;
		min = INT_MAX;
	}
	TSPValue += graf[tab[size - 2]][tab[size - 1]];
	TSPValue += graf[tab[size - 1]][tab[size - 0]];
	return tab;
}


int* Graf::TabuTSPswap(int iterations, int tabuLength) {
	//inicjalizacja
	int i = 0;
	int* bestPath = new int[size + 1];
	int bestPathValue = 0;
	int* currentPath = new int[size + 1];
	int currentPathValue = 0;
	int moveValue;
	node bestmove, bestMoveIndex;
	int bestMoveValue = INT_MIN;
	list<node> TabuList;
	

	//wyznaczenie pierwotnej ścieżki i obliczenie jej wagi
	bestPath = TSPgreed();
	bestPathValue = TSPValue;
	for (int i = 0; i <= size; i++) {
		currentPath[i] = bestPath[i];
	}
	currentPathValue = bestPathValue;

	//inicjalizacja tabu
	for (int i = 0; i < tabuLength; i++)
		TabuList.push_front({ -1,-1 });

	//główna pętla
	do {

		//odnajdywanie najlepszego ruchu
		for (int i = 1; i < size - 1; i++) {
			//przypadek szczegolny (para powiazana)
			moveValue = graf[currentPath[i]][currentPath[i -1]];
			moveValue += graf[currentPath[i + 1]][currentPath[i + 2]];
			moveValue -= graf[currentPath[i - 1]][currentPath[i + 1]];
			moveValue -= graf[currentPath[i]][currentPath[i + 2]];

			//zaakceptowanie ruchu
			if (moveValue > bestMoveValue) {
				if (find(TabuList.begin(), TabuList.end(), node({ currentPath[i],currentPath[i+1] })) == TabuList.end() ||
					//aspiracja
					currentPathValue - moveValue < bestPathValue) {
					bestmove = { currentPath[i],currentPath[i + 1] };
					bestMoveValue = moveValue;
					bestMoveIndex = {i,i+1};
				}
			}
			for (int j = i + 2; j < size; j++) {
				//wyliczenie wartości ruchu
				moveValue =  graf[currentPath[i]][currentPath[i + 1]];
				moveValue += graf[currentPath[i - 1]][currentPath[i]];
				moveValue += graf[currentPath[j]][currentPath[j + 1]];
				moveValue += graf[currentPath[j - 1]][currentPath[j]];
				moveValue -= graf[currentPath[j]][currentPath[i + 1]];
				moveValue -= graf[currentPath[i - 1]][currentPath[j]];
				moveValue -= graf[currentPath[i]][currentPath[j + 1]];
				moveValue -= graf[currentPath[j - 1]][currentPath[i]];
				
				//zaakceptowanie ruchu
				if (moveValue > bestMoveValue) {
					if (find(TabuList.begin(), TabuList.end(), node({ currentPath[i],currentPath[j] })) == TabuList.end() || 
						//aspiracja
						currentPathValue - moveValue < bestPathValue) {
						bestmove = { currentPath[i],currentPath[j] };
						bestMoveValue = moveValue;
						bestMoveIndex = {i,j};
					}
				}

			}
		}

		//utworzenie nowej ścieżki
		if (bestMoveValue == INT_MIN)
			return nullptr;
		currentPathValue -= bestMoveValue;
		moveValue = currentPath[bestMoveIndex.a];//taktyczny bufor zgodny z powszechna konwencja nazewnictwa
		currentPath[bestMoveIndex.a] = currentPath[bestMoveIndex.b];
		currentPath[bestMoveIndex.b] = moveValue;

		//aktualizacja najlepszej sciezki
		if (currentPathValue < bestPathValue) {
			bestPathValue = currentPathValue;
			for (int i = 0; i <= size; i++)
				bestPath[i] = currentPath[i];
		}

		//dodanie do tabu
		TabuList.push_front(bestmove);
		TabuList.pop_back();

		//zakończenie iteracji
		bestMoveValue = INT_MIN;
		i++;
	} while (i < iterations);
	
	//zakończenie algorytmu
	TSPValue = bestPathValue;
	delete[] currentPath;
	return bestPath;
}

int * Graf::TabuTSPinsert(int iterations, int tabuLength)
{
	//inicjalizacja
	int i = 0;
	int* bestPath = new int[size + 1];
	int bestPathValue = 0;
	int* currentPath = new int[size + 1];
	int currentPathValue = 0;
	int moveValue;
	int bestmove;
	node bestMoveIndex;
	int bestMoveValue = INT_MIN;
	list<int> TabuList;


	//wyznaczenie pierwotnej ścieżki i obliczenie jej wagi
	bestPath = TSPgreed();
	bestPathValue = TSPValue;
	for (int i = 0; i <= size; i++) {
		currentPath[i] = bestPath[i];
	}
	currentPathValue = bestPathValue;

	//inicjalizacja tabu
	for (int i = 0; i < tabuLength; i++)
		TabuList.push_front(-1);

	//główna pętla
	do {

		//odnajdywanie najlepszego ruchu
		for (int i = 1; i < size - 1; i++) {
			//przypadek szczegolny (para powiazana)
			moveValue = graf[currentPath[i]][currentPath[i - 1]];
			moveValue += graf[currentPath[i + 1]][currentPath[i + 2]];
			moveValue -= graf[currentPath[i - 1]][currentPath[i + 1]];
			moveValue -= graf[currentPath[i]][currentPath[i + 2]];

			//zaakceptowanie ruchu
			if (moveValue > bestMoveValue) {
				if (find(TabuList.begin(), TabuList.end(), currentPath[i]) == TabuList.end() ||
					//aspiracja
					currentPathValue - moveValue < bestPathValue) {
					bestmove = currentPath[i];
					bestMoveValue = moveValue;
					bestMoveIndex = { i,i + 1 };
				}
			}
			for (int j = i + 2; j < size; j++) {
				//wyliczenie wartości ruchu
				moveValue = graf[currentPath[i]][currentPath[i + 1]];
				moveValue += graf[currentPath[i - 1]][currentPath[i]];
				moveValue += graf[currentPath[j]][currentPath[j + 1]];
				moveValue -= graf[currentPath[i-1]][currentPath[i + 1]];
				moveValue -= graf[currentPath[i]][currentPath[j]];
				moveValue -= graf[currentPath[i]][currentPath[j + 1]];

				//zaakceptowanie ruchu
				if (moveValue > bestMoveValue) {
					if (find(TabuList.begin(), TabuList.end(), currentPath[i]) == TabuList.end() ||
						//aspiracja
						currentPathValue - moveValue < bestPathValue) {
						bestmove = currentPath[i];
						bestMoveValue = moveValue;
						bestMoveIndex = { i,j };
					}
				}

			}
		}

		//utworzenie nowej ścieżki
		if (bestMoveValue == INT_MIN)
			return nullptr;
		currentPathValue -= bestMoveValue;
		moveValue = currentPath[bestMoveIndex.a];//taktyczny bufor zgodny z powszechna konwencja nazewnictwa
		for (int i = bestMoveIndex.a; i < bestMoveIndex.b; i++) {
			currentPath[i] = currentPath[i + 1];
		}
		currentPath[bestMoveIndex.b] = moveValue;

		//aktualizacja najlepszej sciezki
		if (currentPathValue < bestPathValue) {
			bestPathValue = currentPathValue;
			for (int i = 0; i <= size; i++)
				bestPath[i] = currentPath[i];
		}

		//dodanie do tabu
		TabuList.push_front(bestmove);
		TabuList.pop_back();

		//zakończenie iteracji
		bestMoveValue = INT_MIN;
		i++;
	} while (i < iterations);

	//zakończenie algorytmu
	TSPValue = bestPathValue;
	delete[] currentPath;
	return bestPath;
}
int * Graf::TabuTSPswapTime(double Mseconds, int tabuLength)
{
	//inicjalizacja
	int i = 0;
	int* bestPath = new int[size + 1];
	int bestPathValue = 0;
	int* currentPath = new int[size + 1];
	int currentPathValue = 0;
	int moveValue;
	node bestmove, bestMoveIndex;
	int bestMoveValue = INT_MIN;
	list<node> TabuList;


	//wyznaczenie pierwotnej ścieżki i obliczenie jej wagi
	bestPath = TSPgreed();
	bestPathValue = TSPValue;
	for (int i = 0; i <= size; i++) {
		currentPath[i] = bestPath[i];
	}
	currentPathValue = bestPathValue;

	//inicjalizacja tabu
	for (int i = 0; i < tabuLength; i++)
		TabuList.push_front({ -1,-1 });
	//główna pętla
	StartCounter();
	do {

		//odnajdywanie najlepszego ruchu
		for (int i = 1; i < size - 1; i++) {
			//przypadek szczegolny (para powiazana)
			moveValue = graf[currentPath[i]][currentPath[i - 1]];
			moveValue += graf[currentPath[i + 1]][currentPath[i + 2]];
			moveValue -= graf[currentPath[i - 1]][currentPath[i + 1]];
			moveValue -= graf[currentPath[i]][currentPath[i + 2]];

			//zaakceptowanie ruchu
			if (moveValue > bestMoveValue) {
				if (find(TabuList.begin(), TabuList.end(), node({ currentPath[i],currentPath[i + 1] })) == TabuList.end() ||
					//aspiracja
					currentPathValue - moveValue < bestPathValue
					) {
					bestmove = { currentPath[i],currentPath[i + 1] };
					bestMoveValue = moveValue;
					bestMoveIndex = { i,i + 1 };
				}
			}
			for (int j = i + 2; j < size; j++) {
				//wyliczenie wartości ruchu
				moveValue = graf[currentPath[i]][currentPath[i + 1]];
				moveValue += graf[currentPath[i - 1]][currentPath[i]];
				moveValue += graf[currentPath[j]][currentPath[j + 1]];
				moveValue += graf[currentPath[j - 1]][currentPath[j]];
				moveValue -= graf[currentPath[j]][currentPath[i + 1]];
				moveValue -= graf[currentPath[i - 1]][currentPath[j]];
				moveValue -= graf[currentPath[i]][currentPath[j + 1]];
				moveValue -= graf[currentPath[j - 1]][currentPath[i]];

				//zaakceptowanie ruchu
				if (moveValue > bestMoveValue) {
					if (find(TabuList.begin(), TabuList.end(), node({ currentPath[i],currentPath[j] })) == TabuList.end() ||
						//aspiracja
						currentPathValue - moveValue < bestPathValue
						) {
						bestmove = { currentPath[i],currentPath[j] };
						bestMoveValue = moveValue;
						bestMoveIndex = { i,j };
					}
				}

			}
		}

		//utworzenie nowej ścieżki
		if (bestMoveValue == INT_MIN)
			return nullptr;
		currentPathValue -= bestMoveValue;
		moveValue = currentPath[bestMoveIndex.a];//taktyczny bufor zgodny z powszechna konwencja nazewnictwa
		currentPath[bestMoveIndex.a] = currentPath[bestMoveIndex.b];
		currentPath[bestMoveIndex.b] = moveValue;

		//aktualizacja najlepszej sciezki
		if (currentPathValue < bestPathValue) {
			bestPathValue = currentPathValue;
			for (int i = 0; i <= size; i++)
				bestPath[i] = currentPath[i];
		}

		//dodanie do tabu
		TabuList.push_front(bestmove);
		TabuList.pop_back();

		//zakończenie iteracji
		bestMoveValue = INT_MIN;
		i++;
	} while (Mseconds > GetCounter());

	//zakończenie algorytmu
	TSPValue = bestPathValue;
	delete[] currentPath;
	return bestPath;
}
int * Graf::TabuTSPinsertTime(double Mseconds, int tabuLength)
{
	//inicjalizacja
	int i = 0;
	int* bestPath = new int[size + 1];
	int bestPathValue = 0;
	int* currentPath = new int[size + 1];
	int currentPathValue = 0;
	int moveValue;
	int bestmove;
	node bestMoveIndex;
	int bestMoveValue = INT_MIN;
	list<int> TabuList;


	//wyznaczenie pierwotnej ścieżki i obliczenie jej wagi
	bestPath = TSPgreed();
	bestPathValue = TSPValue;
	for (int i = 0; i <= size; i++) {
		currentPath[i] = bestPath[i];
	}
	currentPathValue = bestPathValue;

	//inicjalizacja tabu
	for (int i = 0; i < tabuLength; i++)
		TabuList.push_front(-1);

	StartCounter();
	//główna pętla
	do {

		//odnajdywanie najlepszego ruchu
		for (int i = 1; i < size - 1; i++) {
			//przypadek szczegolny (para powiazana)
			moveValue = graf[currentPath[i]][currentPath[i - 1]];
			moveValue += graf[currentPath[i + 1]][currentPath[i + 2]];
			moveValue -= graf[currentPath[i - 1]][currentPath[i + 1]];
			moveValue -= graf[currentPath[i]][currentPath[i + 2]];

			//zaakceptowanie ruchu
			if (moveValue > bestMoveValue) {
				if (find(TabuList.begin(), TabuList.end(), currentPath[i]) == TabuList.end() ||
					//aspiracja
					currentPathValue - moveValue < bestPathValue
					) {
					bestmove = currentPath[i];
					bestMoveValue = moveValue;
					bestMoveIndex = { i,i + 1 };
				}
			}
			for (int j = i + 2; j < size; j++) {
				//wyliczenie wartości ruchu
				moveValue = graf[currentPath[i]][currentPath[i + 1]];
				moveValue += graf[currentPath[i - 1]][currentPath[i]];
				moveValue += graf[currentPath[j]][currentPath[j + 1]];
				moveValue -= graf[currentPath[i - 1]][currentPath[i + 1]];
				moveValue -= graf[currentPath[i]][currentPath[j]];
				moveValue -= graf[currentPath[i]][currentPath[j + 1]];

				//zaakceptowanie ruchu
				if (moveValue > bestMoveValue) {
					if (find(TabuList.begin(), TabuList.end(), currentPath[i]) == TabuList.end() ||
						//aspiracja
						currentPathValue - moveValue < bestPathValue
					){
						bestmove = currentPath[i];
						bestMoveValue = moveValue;
						bestMoveIndex = { i,j };
					}
				}

			}
		}

		//utworzenie nowej ścieżki
		if (bestMoveValue == INT_MIN)
			return nullptr;
		currentPathValue -= bestMoveValue;
		moveValue = currentPath[bestMoveIndex.a];//taktyczny bufor zgodny z powszechna konwencja nazewnictwa
		for (int i = bestMoveIndex.a; i < bestMoveIndex.b; i++) {
			currentPath[i] = currentPath[i + 1];
		}
		currentPath[bestMoveIndex.b] = moveValue;

		//aktualizacja najlepszej sciezki
		if (currentPathValue < bestPathValue) {
			bestPathValue = currentPathValue;
			for (int i = 0; i <= size; i++)
				bestPath[i] = currentPath[i];
		}

		//dodanie do tabu
		TabuList.push_front(bestmove);
		TabuList.pop_back();

		//zakończenie iteracji
		bestMoveValue = INT_MIN;
		i++;
	} while (Mseconds > GetCounter());

	//zakończenie algorytmu
	TSPValue = bestPathValue;
	delete[] currentPath;
	return bestPath;
}


#pragma endregion


int Graf::getSize()
{
	return size;
}

string Graf::toString()
{
	string ret = "";
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++)
			ret += to_string(graf[i][j]) + " ";
		ret += "\n";
	}
	return ret;
}
Graf::Graf(int size, int** graf) {
	this->size = size;
	this->graf = graf;
}

Graf::Graf()
{
	
}


Graf::~Graf()
{
	for (int i = 0; i < size; i++) {
		delete[] graf[i];
	}
	delete[] graf;
}
Graf::Graf(int size, bool symetry)
{
	srand(time(NULL) );
	this->size = size;
	graf = new int*[size];
	for (int i = 0; i < size; i++) {
		graf[i] = new int[size];
		graf[i][i] = 0;
	}
	if (symetry) {
		for (int i = 0; i < size; i++) {
			for (int j = i + 1; j < size; j++) {
				graf[i][j] = rand() % 90 + 10;
				graf[j][i] = graf[i][j];
			}
		}
	}
	else {
		for (int i = 0; i < size; i++) {
			for (int j = 0; j < size; j++) {
				if (i != j)
					graf[i][j] = rand() % 90 + 10;
			}
		}
	}
	
}
