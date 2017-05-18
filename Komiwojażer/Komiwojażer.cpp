// Komiwoja¿er.cpp: Okreœla punkt wejœcia dla aplikacji konsoli.
//

#include "stdafx.h"
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <ctime>

using namespace std;

double scoreGenes(vector<int> genes, double x[], double y[]);
vector<int> randPermute();
double scoreAll(vector<vector<int>> genes, double xLoc[], double yLoc[]);
vector<vector<int>> evolve(vector<vector<int>> genes, double xLoc[], double yLoc[]);
vector<vector<int>> sort(vector<vector<int>> genes, int scores[]);
vector<vector<int>> crossover(vector<vector<int>> genes);
vector<vector<int>> mutate(vector<vector<int>> genes);
vector<int> selectBest(vector<vector<int>> genes, double xLoc[], double yLoc[]);
int findIndex(vector<int> mother, int value);



int main()
{
	srand(time(NULL));

	fstream plik;
	plik.open("dane.txt");
	double xLoc[100], yLoc[100];
	vector<vector<int>> genes;

	for (int i = 0; i < 100; i++)
	{
		plik >> xLoc[i]; //pozbywamy siê id miasta
		plik >> xLoc[i];
		plik >> yLoc[i];
	}

	for (int i = 0; i < 100; i++)
		genes.push_back(randPermute());
	
	double score = scoreGenes(genes[0], xLoc, yLoc);

	cout << "Zadanie 1. Odleglosc losowej trasy: ";
	cout << score << endl;
	cout << "Zadanie 2. Stan obliczen: " << endl;

	int average = 0;
	int max=INT32_MAX;
	vector<int> best;
	double limit = 100;

	for (int i = 0; i < limit; i++)
	{
		genes = evolve(genes, xLoc, yLoc);
		int tempMax = scoreAll(genes, xLoc, yLoc);
		if (tempMax < max)
		{
			max = tempMax;
			best = selectBest(genes, xLoc, yLoc);
		}
		if ((int)(i / limit * 100) % 5 == 0)
			cout << "\r" << (int)(i/limit*100) << "%   ";
	}
	cout << "\r100%" << endl;
	average = 0;
	for (int i = 0; i < 100; i++)
		average += scoreGenes(genes[i], xLoc, yLoc);
	average /= 100;
	cout << "Najlepsza odkryta odleglosc: " << max << endl;
	cout << "Najlepsza odkryta sekwencja miast:" << endl;
	for (int i = 0; i < 99; i++)
		cout << best[i] << "->";
	cout << best[99] << endl;

	cin.get();
}

vector<vector<int>> evolve(vector<vector<int>> genes, double xLoc[], double yLoc[])
{
	int scores[100], j;

	for (int i = 0; i < 100; i++)
		scores[i] = scoreGenes(genes[i], xLoc, yLoc);

	//InsertSort by score
	genes = sort(genes, scores);

	//Fill new generation with 5 times winner, 3 times 2nd place, 2 times 3rd place, 1 time next 80. Fill rest with rand
	vector<vector<int>> newEvolution;
	for (int i = 0; i < 80; i++)
		newEvolution.push_back(genes[0]);
	//for (int i = 0; i < 3; i++)
	//	newEvolution.push_back(genes[1]);
	//for (int i = 0; i < 2; i++)
	//	newEvolution.push_back(genes[2]);
	//for (int i = 0; i < 80; i++)
	//	newEvolution.push_back(genes[i]);
	for (int i = 0; i < 20; i++)
		newEvolution.push_back(randPermute());

	//Crossover
	//Each 'speciman' becomes a father. Then each of his genes has 10% chance of being replaced by random mother's gene
	genes = crossover(genes);

	//Mutate
	genes = mutate(genes);

	return newEvolution;
}

vector<vector<int>> sort(vector<vector<int>> genes, int scores[])
{
	int j = 0;
	for (int i = 0; i < 100; i++)
	{
		j = i;
		while (j > 0 && scores[j] < scores[j - 1])
		{
			swap(genes[j], genes[j - 1]);
			j--;
		}
	}
	return genes;
}

vector<vector<int>> crossover(vector<vector<int>> genes)
{
	for (int i = 0; i < 100; i++)
	{
		vector<int> father = genes[i];
		vector<int> mother = genes[rand() % 50];

		for (int j = 0; j < 100; j++)
			if (rand() % 100 < 10)
			{
				int index = findIndex(father, mother[j]);
				swap(father[j], father[index]);
			}

		genes[i] = father;
	}
	return genes;
}

vector<vector<int>> mutate(vector<vector<int>> genes)
{
	for (int j = 0; j<100; j++)
		for (int i = 0; i < 100; i++)
			if (rand() % 100 < 5)
				swap(genes[j][i], genes[j][rand() % 100]);
	return genes;
}

int findIndex(vector<int> vec, int value)
{
	for (int i = 0; i < 100; i++)
		if (vec[i] == value)
			return i;
}

double scoreAll(vector<vector<int>> genes, double xLoc[], double yLoc[])
{
	double max = INT32_MAX;
	for (int i = 0; i < 100; i++)
	{
		int temp = scoreGenes(genes[i], xLoc, yLoc);
		if (temp < max)
			max = temp;
	}
	return max;
}

double scoreGenes(vector<int> genes, double xLoc[], double yLoc[])
{
	int id;
	double x, oldx, y, oldy;
	double score=0;

	oldx = xLoc[genes[0]-1];
	oldy = yLoc[genes[0]-1];

	for (int i = 1; i < 100; i++)
	{
		x = xLoc[genes[i]-1];
		y = yLoc[genes[i]-1];

		score += sqrt((x - oldx)*(x - oldx) + (y - oldy)*(y - oldy));

		oldx = x;
		oldy = y;
	}

	score += sqrt((xLoc[0] - xLoc[99])*(xLoc[0] - xLoc[99]) + (yLoc[0] - yLoc[99])*(yLoc[0] - yLoc[99]));

	return score;
}

vector<int> selectBest(vector<vector<int>> genes, double xLoc[], double yLoc[])
{
	int scores[100];

	for (int i = 0; i < 100; i++)
		scores[i] = scoreGenes(genes[i], xLoc, yLoc);

	//InsertSort by score
	genes = sort(genes, scores);

	return genes[0];
}

vector<int> randPermute()
{
	vector<int> temp;
	for (int i = 0; i < 100; i++)
		temp.push_back(i + 1);
	for (int i = 0; i < 100; i++)
		swap(temp[rand() % 100], temp[rand() % 100]);
	return temp;
}