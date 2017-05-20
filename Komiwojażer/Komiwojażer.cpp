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
vector<vector<int>> sort(vector<vector<int>> genes, vector<int> scores);
vector<vector<int>> crossover(vector<vector<int>> genes);
vector<vector<int>> mutate(vector<vector<int>> genes);
vector<int> selectBest(vector<vector<int>> genes, double xLoc[], double yLoc[]);
int findIndex(vector<int> mother, int value);



int main()
{
	srand(1);

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
	double limit = 1000;
	int repetitions = 50;
	
	for (int k = 0;k < repetitions;k++)
	{
		genes.clear();
		for (int i = 0; i < 100; i++)
			genes.push_back(randPermute());
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
				cout << "\r" << (int)(i / limit * 100) << "%   ";
		}
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
	vector<int> scores, j;

	for (int i = 0; i < 100; i++)
		scores.push_back(scoreGenes(genes[i], xLoc, yLoc));

	//InsertSort by score
	genes = sort(genes, scores);

	//Fill new generation with 30 times winner and 1 time next 80. Fill rest with rand
	vector<vector<int>> newEvolution;
	for (int i = 0; i < 30; i++)
		newEvolution.push_back(genes[0]);
	for (int i = 0; i < 50; i++)
		newEvolution.push_back(genes[i]);
	for (int i = 0; i < 20; i++)
		newEvolution.push_back(randPermute());

	//Crossover
	//Each 'speciman' becomes a father. Then each of his genes has 10% chance of being replaced by random mother's gene
	newEvolution = crossover(newEvolution);

	//Mutate
	newEvolution = mutate(newEvolution);

	return newEvolution;
}

vector<vector<int>> sort(vector<vector<int>> genes, vector<int> scores)
{
	int j = 0;

	for (int i = 0; i < 100; i++)
	{
		j = i;
		while (j > 0 && scores[j] < scores[j - 1])
		{
			swap(genes[j], genes[j - 1]);
			swap(scores[j], scores[j - 1]);
			j--;
		}
	}
	return genes;
}

vector<vector<int>> crossover(vector<vector<int>> genes)
{
	for (int i = 1; i < 100; i++)
	{
		vector<int> father = genes[i];
		vector<int> mother = genes[rand() % 50];

		//1% chance that we insert mother's sequence instead of mothers
		for (int j = 0; j < 100; j++)
			if (rand() % 1000 < 42)
			{
				/*int index = findIndex(father, mother[j]);*/
				int index = findIndex(mother, father[j]);
				int index2 = findIndex(father, mother[(index + 1) % 100]);
				int index3 = findIndex(father, mother[(index + 2) % 100]);
				swap(father[(j + 1) % 100], father[index2]);
				swap(father[(j + 2) % 100], father[index3]);
			}

		genes[i] = father;
	}
	return genes;
}

void reverse(vector<int> &vec, int start, int end)
{
	if (start > end)
		swap(start, end);

	int limit = end - start;

	for (int i = 0;i < limit / 2;i++)
		swap(vec[start + i], vec[end - i]);
}

vector<vector<int>> mutate(vector<vector<int>> genes)
{
	for (int j = 1; j < 100; j++)
		for (int i = 0; i < 100-2; i++)
			if (rand() % 1000 < 5)
				reverse(genes[j], i, rand() % 100);
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
	vector<int> scores;

	for (int i = 0; i < 100; i++)
		scores.push_back(scoreGenes(genes[i], xLoc, yLoc));

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