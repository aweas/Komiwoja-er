// Komiwoja¿er.cpp: Okreœla punkt wejœcia dla aplikacji konsoli.
//

#include "stdafx.h"
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <ctime>

#define citiesNumber 100

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
	srand(time(NULL));

	fstream plik;
	plik.open("dane100.txt");
	double xLoc[citiesNumber], yLoc[citiesNumber];
	vector<vector<int>> genes;

	for (int i = 0; i < citiesNumber; i++)
	{
		plik >> xLoc[i]; //pozbywamy siê id miasta
		plik >> xLoc[i];
		plik >> yLoc[i];
	}

	vector<int> best;
	
	double generations = 0;

	double repetitions = 2000;
	vector<double> score;

	//vector<int> crossRate = { 42,42,42,42,42,42 }; //fill with best option
	//vector<int> mutationRate = { 5 };

	vector<int> history;
	vector<int> historyFar;

	//According to statistics, best results were achieved when:
	//bestCopies = 30
	//twoCopies = 0
	//threeCopies = 0
	//restCopies = 50
	//crossRate = 42
	//mutationRate = ?

	int theBestest = INT32_MAX;
	int bestIndex;
	vector<int> bestGenes;


	//for (int exp = 0; exp < mutationRate.size(); exp++)
	//{
		//Restart randomness seed and genes
		srand(time(NULL));
		genes.clear();		

		for (int i = 0; i < citiesNumber; i++)
			genes.push_back(randPermute());

		int average = 0;
		for (int k = 0; k < repetitions; k++)
		{
			int tempMax = 0;

			//for (int i = 0; i < generations; i++)
			genes = evolve(genes, xLoc, yLoc, 30, 0, 0, 50, 42, 5);
				
			tempMax = scoreAll(genes, xLoc, yLoc);

			if (tempMax < theBestest)
			{
				theBestest = tempMax;
				//bestIndex = exp;
	
				bestGenes = selectBest(genes, xLoc, yLoc);
			}

			score.push_back(tempMax);
			average += tempMax;
			int percent = (k+1) / repetitions * 100;
			if(percent%2==0)
				printf("\rComputation: %i%c \t", percent,'%');
		}

		//Statistics measuring
		average /= repetitions;

		double variation = 0;
		for (int i = 0; i < score.size(); i++)
			variation += (score[i] - average)*(score[i] - average);
		variation /= repetitions;
		variation = sqrt(variation);

		printf("\r\t\t\t\n");
		printf("Average: %i\nVariation: %f\n", average, variation);
		score.clear();

		history.push_back(average);
		historyFar.push_back(average);

		//if ((exp+1) % 6 == 0)
		//{
			//int best = smallest(history);
			//printf("\nBest experiment out of six: %i, average: %i\n", best+1, history[best]);
			//history.clear();
			//for (int i = 0;i < 6;i++)
				//crossRate.push_back(crossRate[best]);
			//if (exp == 8 || exp == 11)
			//{
			//	int bestFar = smallest(historyFar);
			//	printf("Best experiment so far: %i, average: %i\n", bestFar, historyFar[bestFar]);

			//	for (int i = 0; i < 6 && exp==8; i++)
			//	{
			//		bestCopies.push_back(bestCopies[bestFar]);
			//		twoCopies.push_back(twoCopies[bestFar]);
			//		threeCopies.push_back(threeCopies[bestFar]);
			//		restCopies.push_back(restCopies[bestFar]);
			//	}
			//	for (int i = 0; i < 3 && exp == 11; i++)
			//	{
			//		crossRate.push_back(bestCopies[bestFar]);
			//	}
			//}
		//}
	//}

	int bestFar = smallest(historyFar);
	//printf("Best experiment so far: %i, average: %i\n\n", bestFar, historyFar[bestFar]);

	cout << "Best result: " << theBestest << " for experiment " << bestIndex << endl;

	for (int i = 0;i < 98;i++)
		cout << bestGenes[i] << "->";
	cout << bestGenes[99];

	std::cin.get();
}

vector<vector<int>> evolve(vector<vector<int>> genes, double xLoc[], double yLoc[], int bestCopies, int twoCopies, int threeCopies, int restCopies, int crossRate, int mutationRate)
{
	vector<int> scores, j;

	for (int i = 0; i < citiesNumber; i++)
		scores.push_back(scoreGenes(genes[i], xLoc, yLoc));

	//InsertSort by score
	genes = sort(genes, scores);

	//Fill new generation with 5 times winner, 3 times 2nd place, 2 times 3rd place, 1 time next 80. Fill rest with rand
	vector<vector<int>> newEvolution;
	for (int i = 0; i < bestCopies; i++)
		newEvolution.push_back(genes[0]);
	for (int i = 0; i < restCopies; i++)
		newEvolution.push_back(genes[i]);
	for (int i = 0; i < citiesNumber -bestCopies-twoCopies-threeCopies-restCopies; i++)
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

	for (int i = 0; i < citiesNumber; i++)
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
	for (int i = 1; i < citiesNumber; i++)
	{
		vector<int> father = genes[i];
		vector<int> mother = genes[rand() % 50];

		//1% chance that we insert mother's sequence instead of mothers
		for (int j = 0; j < citiesNumber; j++)
			if (rand() % 1000 < rate)
			{
				/*int index = findIndex(father, mother[j]);*/
				int index = findIndex(mother, father[j]);
				int index2 = findIndex(father, mother[(index + 1) % citiesNumber]);
				int index3 = findIndex(father, mother[(index + 2) % citiesNumber]);
				swap(father[(j + 1) % citiesNumber], father[index2]);
				swap(father[(j + 2) % citiesNumber], father[index3]);
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
		swap(vec[start+i], vec[end-i]);
}

vector<vector<int>> mutate(vector<vector<int>> genes, int rate)
{
	for (int j = 1; j < citiesNumber; j++)
		for (int i = 0; i < citiesNumber; i++)
			if (rand() % 1000 < rate)
				reverse(genes[j], i, rand() % citiesNumber);
	return genes;
}

int findIndex(vector<int> vec, int value)
{
	for (int i = 0; i < citiesNumber; i++)
		if (vec[i] == value)
			return i;
}

double scoreAll(vector<vector<int>> genes, double xLoc[], double yLoc[])
{
	double max = INT32_MAX;
	for (int i = 0; i < citiesNumber; i++)
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

	for (int i = 1; i < citiesNumber; i++)
	{
		x = xLoc[genes[i]-1];
		y = yLoc[genes[i]-1];

		score += sqrt((x - oldx)*(x - oldx) + (y - oldy)*(y - oldy));

		oldx = x;
		oldy = y;
	}

	score += sqrt((xLoc[0] - xLoc[citiesNumber-1])*(xLoc[0] - xLoc[citiesNumber-1]) + (yLoc[0] - yLoc[citiesNumber-1])*(yLoc[0] - yLoc[citiesNumber-1]));

	return score;
}

vector<int> selectBest(vector<vector<int>> genes, double xLoc[], double yLoc[])
{
	vector<int> scores;

	for (int i = 0; i < citiesNumber; i++)
		scores.push_back(scoreGenes(genes[i], xLoc, yLoc));

	//InsertSort by score
	genes = sort(genes, scores);

	return genes[0];
}

vector<int> randPermute()
{
	vector<int> temp;
	for (int i = 0; i < citiesNumber; i++)
		temp.push_back(i + 1);
	for (int i = 0; i < citiesNumber; i++)
		swap(temp[rand() % citiesNumber], temp[rand() % citiesNumber]);
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