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
vector<vector<int>> evolve(vector<vector<int>> genes);

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

	//cout << "Zadanie 1. Odleglosc losowej trasy: ";
	//cout << score << endl;
	//cin.get();



}

vector<vector<int>> evolve(vector<vector<int>> genes, double xLoc[], double yLoc[])
{
	int scores[100], j;

	for (int i = 0; i < 100; i++)
		scores[i] = scoreGenes(genes[i], xLoc, yLoc);

	//Sort
	for (int i = 0; i < 100; i++)
	{
		j = i;
		while (j > 0 && scores[j] > scores[j - 1])
		{
			swap(genes[j], genes[j - 1]);
			j--;
		}
	}

	//Fill
	vector<vector<int>> newEvolution;
	for (int i = 0; i < 90; i++)
	{

	}


	//Crossover

	//Mutate
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

vector<int> randPermute()
{
	vector<int> temp;
	for (int i = 0; i < 100; i++)
		temp.push_back(i + 1);
	for (int i = 0; i < 100; i++)
		swap(temp[rand() % 100], temp[rand() % 100]);
	return temp;
}