#include "tsm.h"
#define INT_MAX 2147483647

TSM::TSM(int** graph, int numV) :n(numV), timeLimit(0)
{
    pathLength = INT_MAX;
    for (int i = 0; i < n; i++)
        finalPath.push_back(i);
    this->graph = new int* [numV];
    for (int i = 0; i < numV; i++)
    {
        this->graph[i] = new int[numV];
        for (int j = 0; j < numV; j++) this->graph[i][j] = graph[i][j];
    }
    seed = 0;
    startTime = chrono::high_resolution_clock::now();
}
TSM::~TSM()
{
    for (int i = 0; i < n; i++) delete[] graph[i];
    delete[] graph;
}
bool TSM::checkTimeOut()
{
    auto now = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = now - startTime;
    if ((elapsed.count() * 1000.0) >= this->m_timeLimit) return true;
    return false;
}
ULONG TSM::m_rand()
{
    return (((seed = seed * 214013L + 2531011L) >> 16) & 0x7fff);
}
void TSM::preProcess()
{
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
        {
            if (graph[i][j] == 0 && i != j) graph[i][j] = -1;
        }
}
void TSM::randomSwap(vector<int>& list) {
    //srand(time(0));
    for (int i = list.size() - 1; i > 0; --i) {
        int j = m_rand() % (i + 1);
        swap(list[i], list[j]);
    }
}
int TSM::nearestNeighbour(int start, bool* visited) {
    int minDist = INT_MAX, minIndex = INT_MAX;
    for (int i = 0; i < n; i++) {
        if (graph[start][i] != -1 && graph[start][i] < minDist && !visited[i]) {
            minDist = graph[start][i];
            minIndex = i;
        }
    }
    return minIndex;
}
vector<int> TSM::solveNN(int start)
{
    vector<int> path;
    int current = start;
    bool* visited = new bool[n];
    for (int i = 0; i < n; i++) visited[i] = 0;
    while (true) {
        visited[current] = true;
        path.push_back(current);

        int next = nearestNeighbour(current, visited);
        if (next == INT_MAX) break;
        current = next;
    }
    return path;
}
void TSM::NNThread(int iters)
{
    bool isRev = 0;
    int fLen = INT_MAX;
    for (int i = 0; i < n; i++)
    {
        auto tour = solveNN(i);
        if (tour.size() == n)
        {
            bool p;
            int len = calculateDistance(tour, 0, p);
            if (len < fLen)
            {
                fLen = len;
                finalPath = tour;
                isRev = p;
            }
        }
    }
    if (isRev) reverse(finalPath.begin(), finalPath.end());
    pathLength = fLen;
    auto tour = finalPath;
    int finDist = pathLength;
    //srand(time(0) - iters);
    seed = time(0) - iters * 18;
    for (int i = 0; i < iters; i++)
    {
        if (checkTimeOut()) break;
        int r = m_rand() % 4;
        bool p = (r % 2 == 0) ? 1 : 0;
        if (r - 2 < 0)
        {
            twoOpt(tour);
            threeOpt(tour, p);
        }
        else
        {
            threeOpt(tour, p);
            twoOpt(tour);
        }
        int tmp = calculateDistance(tour, 1, *(bool*)0);
        if (tmp < finDist)
        {
            finDist = tmp;
            finalPath = tour;
        }
        randomSwap(tour);
    }
    pathLength = finDist;
}
string TSM::pathWithStart(vector<int> circuit, int start)
{
    string tmp = "";
    int pos = -1;
    for (int i = 0; i < circuit.size(); i++) {
        if (circuit[i] == start)
        {
            pos = i; break;
        }
    }
    vector<int> v;
    for (int i = pos; i < circuit.size(); i++) v.push_back(circuit[i]);
    for (int i = 0; i < pos; i++) v.push_back(circuit[i]);
    //v.push_back(circuit[pos]);
    // for (int i = 0; i < v.size(); i++) {
        // tmp += (char)(v[i] + 65);
        // tmp += " ";
    // }
    // tmp += (char)(v[0] + 65);
    // return tmp;
	for (int i = 0;i< v.size();i++)
	{
		tmp += to_string(v[i]);
		tmp += " ";
	}
	tmp += to_string(v[0]);
	return tmp;
}
string Traveling(int** graph, int numV, int start, int& pathLength)
{
    int iters, milisec;
    if (numV >= 18) { iters = 750; milisec = 9000; }
    else if (numV >= 15) { iters = 850; milisec = 7777; }
    else { iters = 1000; milisec = 5555; }
    TSM tsm(graph, numV);
    tsm.m_timeLimit = milisec;
    tsm.preProcess();  
    tsm.NNThread(iters);
    pathLength = tsm.pathLength;
    return tsm.pathWithStart(tsm.finalPath, start);
}

int TSM::calculateDistance(const vector<int>& tour, bool ori,bool& isRev) {
    //need to check reverse
    int totalDistance = 0, revDist=0;
    vector<int> rev = tour;
    reverse(rev.begin(), rev.end());
    bool valid = 1, validRev = 1;
;    for (int i = 0; i < tour.size() - 1; ++i) {
        totalDistance += graph[tour[i]][tour[i + 1]];
        revDist += graph[rev[i]][rev[i + 1]];
        if (graph[tour[i]][tour[i + 1]] == -1) valid = 0;
        if (graph[rev[i]][rev[i + 1]] == -1) validRev = 0;
    }
    totalDistance += graph[tour.back()][tour.front()];
    revDist += graph[rev.back()][rev.front()];
    if (graph[tour.back()][tour.front()] == -1) valid = 0;
    if (graph[rev.back()][rev.front()] == -1) validRev = 0;
    if (!valid) totalDistance = INT_MAX;
    if (!validRev) revDist = INT_MAX;
    if (ori) return totalDistance;
    if (!ori) isRev = totalDistance > revDist ? 1 : 0;
    return  totalDistance < revDist ? totalDistance : revDist;
}

void TSM::twoOpt(vector<int>& tour) {
    bool improvement = true;
    while (improvement && !checkTimeOut()) {
        improvement = false;
        for (int i = 0; i < (int)(tour.size()) - 2 ; ++i) {
            for (int j = i + 2; j < tour.size(); ++j) {
                vector<int> newTour = tour;
                reverse(newTour.begin() + i + 1, newTour.begin() + j + 1); 
                bool isRev = 0;
                if (calculateDistance(newTour, 0, isRev) < calculateDistance(tour, 1, isRev)) {
                    tour = newTour;
                    if (isRev) reverse(tour.begin(), tour.end());
                    improvement = true;
                   // cout << "Improved\n";
                }
            }
        }
    }
}

void TSM::threeOpt(std::vector<int>& tour, bool case5)
{
    bool improvement = true;
    while (improvement && !checkTimeOut()) {
        improvement = false;
        for (int i = 0; i < (int)(tour.size()) - 4; ++i) {
            for (int j = i + 2; j < (int)(tour.size()) - 2; ++j) {
                for (int k = j + 2; k < (int)(tour.size()); k++) {
                    bool isRev = 0;
                    int finalDist = calculateDistance(tour, 1, isRev);
                    int num = 0;
                    bool finalRev = 0;
                    //2-opt case 1
                    vector<int> newTour1 = tour;
                    reverse(newTour1.begin() + i + 1, newTour1.begin() + j + 1);
                    int tmp = calculateDistance(newTour1, 0, isRev);
                    if (tmp < finalDist) {
                        finalDist = tmp;
                        num = 1;
                        finalRev = isRev;
                    }
                    //2-opt case 2
                    vector<int> newTour2 = tour;
                    reverse(newTour2.begin() + i + 1, newTour2.begin() + k + 1);
                    tmp = calculateDistance(newTour2, 0, isRev);
                    if (tmp < finalDist) {
                        finalDist = tmp;
                        num = 2;
                        finalRev = isRev;
                    }
                    //2-opt case 3
                    vector<int> newTour3 = tour;
                    reverse(newTour3.begin() + j + 1, newTour3.begin() + k + 1);
                    tmp = calculateDistance(newTour3, 0, isRev);
                    if (tmp < finalDist) {
                        finalDist = tmp;
                        num = 3;
                        finalRev = isRev;
                    }
                    //3-opt case 1
                    vector<int> newTour4 = tour;
                    reverse(newTour4.begin() + i + 1, newTour4.begin() + j + 1);
                    reverse(newTour4.begin() + j + 1, newTour4.begin() + k + 1);
                    tmp = calculateDistance(newTour4, 0, isRev);
                    if (tmp < finalDist) {
                        finalDist = tmp;
                        num = 4;
                        finalRev = isRev;
                    }
                    //3-opt case 2
                    vector<int> newTour5 = tour;
                    reverse(newTour5.begin() + i + 1, newTour5.begin() + k + 1);
                    reverse(newTour5.begin() + i + 1, newTour5.begin() + (i + k - j + 1));
                    reverse(newTour5.begin() + (i + k - j + 1), newTour5.begin() + k + 1);
                    tmp = calculateDistance(newTour5, 0, isRev);
                    if (tmp < finalDist) {
                        finalDist = tmp;
                        num = 5;
                        finalRev = isRev;
                    }
                    //3-opt case 3
                    vector<int> newTour6 = tour;
                    reverse(newTour6.begin() + i + 1, newTour6.begin() + k + 1);
                    reverse(newTour6.begin() + i + 1, newTour6.begin() + (i + k - j + 1));
                    tmp = calculateDistance(newTour6, 0, isRev);
                    if (tmp < finalDist) {
                        finalDist = tmp;
                        num = 6;
                        finalRev = isRev;
                    }
                    //3-opt case 4
                    vector<int> newTour7 = tour;
                    reverse(newTour7.begin() + i + 1, newTour7.begin() + k + 1);
                    reverse(newTour7.begin() + (i + k - j + 1), newTour7.begin() + k + 1);
                    tmp = calculateDistance(newTour7, 0, isRev);
                    if (tmp < finalDist) {
                        finalDist = tmp;
                        num = 7;
                        finalRev = isRev;
                    }
                    if (num > 0)
                    {
                        if (num == 1) tour = newTour1;
                        if (num == 2) tour = newTour2;
                        if (num == 3) tour = newTour3;
                        if (num == 4) tour = newTour4;
                        if (num == 5) tour = newTour5;
                        if (num == 6) tour = newTour6;
                        if (num == 7) tour = newTour7;
                        if (finalRev) reverse(tour.begin(), tour.end());
                        improvement = true;
                    }
                }
            }
        }
        //3-opt case 5
        if(case5)
        for (int i = 0; i < (int)(tour.size()); i++)
        {
            vector<int> newtour = tour;
            //change pivot
            int c = 0;
            for (int t = i; t < (int)(tour.size()); t++)
                newtour[c++] = tour[t];
            for (int t = 0; t < i; t++)
                newtour[c++] = tour[t];
            //check case 5
            for (int j = 2; j < (int)(tour.size()) - 2; j++)
            {
                vector<int> n2 = newtour;
                reverse(n2.begin() + 1, n2.begin() + j + 1);
                reverse(n2.begin() + j + 1, n2.end());
                bool isRev = 0;
                if (calculateDistance(n2, 0, isRev) < calculateDistance(tour, 1, isRev)) {
                    tour = n2;
                    if (isRev) reverse(tour.begin(), tour.end());
                    improvement = true;
                }
                    
            }
        }
    }
 }
