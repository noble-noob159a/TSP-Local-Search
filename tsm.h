#pragma once
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <chrono>
typedef unsigned long long ULONG;
using namespace std;

class TSM
{
	friend string Traveling(int** graph, int numV, int start, int& pathLength);
private:
	int** graph;
	int n;
	ULONG seed;
	bool timeLimit;
	chrono::high_resolution_clock::time_point startTime;
	double m_timeLimit = 9000;
	vector<int> finalPath;
	int pathLength;

public:
	TSM(int** graph, int numV=20);
	~TSM();

	int calculateDistance(const std::vector<int>& tour,bool ori,bool& isRev);
	void twoOpt(std::vector<int>& tour);
	void threeOpt(std::vector<int>& tour,bool case5);
	string pathWithStart(vector<int> circuit, int start);
	vector<int> solveNN(int start);
	int nearestNeighbour(int start, bool* visited);
	void NNThread(int iters);
	void preProcess();
	bool checkTimeOut();
	ULONG m_rand();
	void randomSwap(vector<int>& list);
};

string Traveling(int** graph, int numV, int start, int& pathLength);
