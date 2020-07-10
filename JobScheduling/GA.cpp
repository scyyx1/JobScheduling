#include <iostream>
#include <string.h>
#include <vector>
#include <stdlib.h>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <math.h>
#include <algorithm>
#include <set>

using namespace std;

class GeneralJob {
    public:
        int index;
        double releaseTime;
        double processTime;
        double deadLine;
        double startTime;
        double latestStartTime;
        double endTime;
};

class GeneralJobBlock {
    public:
        double blockStartTime;
        double blockEndTime;
};

class Problem {
    public:
        int job_num;
        int POP_SIZE;
        vector<GeneralJob> jobs;
        void load_problem(string filename, Problem &prob);
};

class Solution {
    public:
        double fitness;
        vector<double> jobStartTimes;
        void compute_fitness(Problem prob);
        
};

class Population {
    public:
        vector<Solution> pop;
        void init_population(Problem prob);
        void selection(Population &currentPop);
        void crossover(Problem &prob);
        void mutation(Problem prob);
        void replacement(Population &nextPop, Problem prob);
};

//return a random number between 0 and 1
float rand_01()
{
    float number;
    number = (float) rand();
    number = number/RAND_MAX;
    return number;
}

//return a random nunber ranging from min to max (inclusive)
int rand_int(int min, int max)
{
    int div = max-min+1;
    int val =rand() % div + min;
    return val;
}

double rand_double(double min, double max)
{

    double val = min + 1.0 * rand() / RAND_MAX * (max - min);
    return val;
}

void Problem::load_problem(string filename, Problem &prob)
{
    GeneralJob tempJob;
    ifstream in(filename);
    string line;
    string str1, str2, str3;
    while(getline(in, line))
    {
        stringstream word(line);
        word >> str1;
        word >> str2;
        word >> str3;

        tempJob.releaseTime = atof(str1.c_str());
        tempJob.deadLine = atof(str2.c_str());
        tempJob.processTime = atof(str3.c_str());
        tempJob.latestStartTime = tempJob.deadLine - tempJob.processTime;

        prob.jobs.push_back(tempJob);
    }
    in.close();
    prob.job_num = prob.jobs.size();
    return;
}

void Population::init_population(Problem prob)
{
    int i, j, t, u, v, k;
    const double EPS = 1e-6; // used to compare two double values
    for(i = 0; i < prob.POP_SIZE; i++)
    {
        Solution tempSolution;
        for(j = 0; j < prob.job_num; j++)
        {
            double tempD = rand_double(prob.jobs[j].releaseTime, prob.jobs[j].latestStartTime);
            tempSolution.jobStartTimes.push_back(tempD);
        }
        pop.push_back(tempSolution);

    }


}

void Solution::compute_fitness(Problem prob) {
    vector<GeneralJobBlock> generalBlocks;
    vector<vector<double>> jobLists(jobStartTimes.size());
    for (int i = 0; i < jobLists.size(); i++) {
        jobLists[i].resize(jobStartTimes.size());
    }

}


int main()
{
    Problem prob;
    string filename("GeneralJobList1.txt");
    prob.load_problem(filename, prob);

    Population currentPop;
    Population nextPop;
    currentPop.init_population(prob);

    for(int i = 0; i < prob.jobs.size(); i++)
    {
        cout << prob.jobs[i].releaseTime << " " << prob.jobs[i].deadLine << " " << prob.jobs[i].processTime << endl;
    }

    return 0;
}