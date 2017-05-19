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
vector<vector<int>> evolve(vector<vector<int>> genes, double xLoc[], double yLoc[], int bestCopies, int twoCopies, int threeCopies, int restCopies, int crossRate, int mutationRate);
vector<vector<int>> sort(vector<vector<int>> genes, vector<int> scores);
vector<vector<int>> crossover(vector<vector<int>> genes, int rate);
vector<vector<int>> mutate(vector<vector<int>> genes, int rate);
vector<int> selectBest(vector<vector<int>> genes, double xLoc[], double yLoc[]);
int findIndex(vector<int> mother, int value);
int smallest(vector<int> vec);


int main()
{
	srand(0);

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

	vector<int> best;
	
	double limit = 1;
	int repetitions = 1;
	vector<double> score;

	vector<int> bestCopies = { 0,0,0,10,30,50,15,30,45 }; //fill with best option
	vector<int> twoCopies = { 0,0,0,0,0,0,10,20,30 };
	vector<int> threeCopies = { 0,0,0,0,0,0,5,10,15 };
	vector<int> restCopies = { 30,50,80,70,50,30,0,0,0 };
	vector<int> crossRate = { 10,10,10,10,10,10,10,10,10, 5,20,30 }; //fill with best option
	vector<int> mutationRate = { 10,10,10,10,10,10,10,10,10, 10,10,10, 5,20,30 };

	vector<int> history;
	vector<int> historyFar;

	for (int exp = 0; exp < mutationRate.size(); exp++)
	{
		//Restart randomness seed
		srand(0);
		int average = 0;
		for (int k = 0; k < repetitions; k++)
		{
			int tempMax = 0;

			for (int i = 0; i < limit; i++)
				genes = evolve(genes, xLoc, yLoc, bestCopies[exp], twoCopies[exp], threeCopies[exp], restCopies[exp], crossRate[exp], mutationRate[exp]);
				
			tempMax = scoreAll(genes, xLoc, yLoc);
			score.push_back(tempMax);
			average += tempMax;
			printf("\rRepetition: %i \t", k);
		}

		//Statistics measuring
		average /= repetitions;

		double variation = 0;
		for (int i = 0; i < score.size(); i++)
			variation += (score[i] - average)*(score[i] - average);
		variation /= repetitions;

		printf("\r\t\t\n");
		printf("Experiment: %i\nBest: %i\nVariation: %df\n", exp, average, variation);
		score.clear();

		history.push_back(average);
		historyFar.push_back(average);

		if ((exp+1) % 3 == 0)
		{
			int best = smallest(history);
			printf("\nBest experiment out of three: %i, average: %i\n", best, history[best]);
			history.clear();

			if (exp == 8 || exp == 11)
			{
				int bestFar = smallest(historyFar);
				printf("Best experiment so far: %i, average: %i\n", bestFar, historyFar[bestFar]);

				for (int i = 0; i < 6 && exp==8; i++)
				{
					bestCopies.push_back(bestCopies[bestFar]);
					twoCopies.push_back(twoCopies[bestFar]);
					threeCopies.push_back(threeCopies[bestFar]);
					restCopies.push_back(restCopies[bestFar]);
				}
				for (int i = 0; i < 3 && exp == 11; i++)
				{
					crossRate.push_back(bestCopies[bestFar]);
				}
			}
		}

		
	}

	std::cin.get();
}

vector<vector<int>> evolve(vector<vector<int>> genes, double xLoc[], double yLoc[], int bestCopies, int twoCopies, int threeCopies, int restCopies, int crossRate, int mutationRate)
{
	vector<int> scores, j;

	for (int i = 0; i < 100; i++)
		scores.push_back(scoreGenes(genes[i], xLoc, yLoc));

	//InsertSort by score
	genes = sort(genes, scores);

	//Fill new generation with 5 times winner, 3 times 2nd place, 2 times 3rd place, 1 time next 80. Fill rest with rand
	vector<vector<int>> newEvolution;
	for (int i = 0; i < bestCopies; i++)
		newEvolution.push_back(genes[0]);
	for (int i = 0; i < twoCopies; i++)
		newEvolution.push_back(genes[1]);
	for (int i = 0; i < threeCopies; i++)
		newEvolution.push_back(genes[2]);
	for (int i = 0; i < restCopies; i++)
		newEvolution.push_back(genes[i]);
	for (int i = 0; i < 100-bestCopies-twoCopies-threeCopies-restCopies; i++)
		newEvolution.push_back(randPermute());

	//Crossover
	//Each 'speciman' becomes a father. Then each of his genes has 10% chance of being replaced by random mother's gene
	newEvolution = crossover(newEvolution, crossRate);

	//Mutate
	newEvolution = mutate(newEvolution, mutationRate);

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

vector<vector<int>> crossover(vector<vector<int>> genes, int rate)
{
	for (int i = 1; i < 100; i++)
	{
		vector<int> father = genes[i];
		vector<int> mother = genes[rand() % 50];

		//1% chance that we insert mother's sequence instead of mothers
		for (int j = 0; j < 100; j++)
			if (rand() % 1000 < rate)
			{
				/*int index = findIndex(father, mother[j]);*/
				int index = findIndex(mother, father[j]);
				int index2 = findIndex(father, mother[(index + 1)%100]);
				swap(father[j], father[index2]);
			}

		genes[i] = father;
	}
	return genes;
}

vector<vector<int>> mutate(vector<vector<int>> genes, int rate)
{
	for (int j = 1; j<100; j++)
		for (int i = 0; i < 100; i++)
			if (rand() % 1000 < rate)
				swap(genes[j][rand()%100], genes[j][rand() % 100]);
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

int smallest(vector<int> vec)
{
	int min = vec[0];
	for (int i = 1; i < vec.size(); i++)
		if (vec[i] < min)
			min = vec[i];
	return findIndex(vec, min);
}