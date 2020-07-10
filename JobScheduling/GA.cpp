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
        GeneralJobBlock(double sTime, double eTime) {
            blockStartTime = sTime;
            blockEndTime = eTime;
        }
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

class Jobs {
public:
    double startTime;
    double processTime;
    Jobs(double sTime, double pTime) {
        startTime = sTime;
        processTime = pTime;
    }
};

class Population {
    public:
        vector<Solution> pop;
        void init_population(Problem prob);
        void selection(Population &currentPop, Problem prob);
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
    int i, j;
    const double EPS = 1e-6; // used to compare two double values
    for(i = 0; i < prob.POP_SIZE; i++)
    {
        Solution tempSolution;
        tempSolution.jobStartTimes.clear();
        for(j = 0; j < prob.job_num; j++)
        {
            tempSolution.fitness = 0;
            double tempD = rand_double(prob.jobs[j].releaseTime, prob.jobs[j].latestStartTime);
            tempSolution.jobStartTimes.push_back(tempD);
            
        }
        tempSolution.compute_fitness(prob);
        pop.push_back(tempSolution);
    }
}



void Solution::compute_fitness(Problem prob) {

    vector<GeneralJobBlock> generalBlocks;
    vector<Jobs> jobs;

    for (int i = 0; i < jobStartTimes.size(); i++) {
        Jobs* curJob = new Jobs(jobStartTimes[i], prob.jobs[i].processTime);
        jobs.push_back(*curJob);
        cout << jobs[i].startTime << " " << jobs[i].processTime << endl;
    }

    std::sort(jobs.begin(), jobs.end(),
        [](const Jobs& a, const Jobs& b)
        {
            return a.startTime < b.startTime || a.startTime == b.startTime && a.startTime + a.processTime < b.startTime + b.processTime;
        }
    );

    GeneralJobBlock* firstBlock = new GeneralJobBlock(jobs[0].startTime, jobs[0].processTime + jobs[0].startTime);
    generalBlocks.push_back(*firstBlock);
    for (int i = 1; i < jobs.size(); i++) {
        if (jobs[i].startTime > generalBlocks.back().blockEndTime) {
            GeneralJobBlock* block = new GeneralJobBlock(jobs[i].startTime, jobs[i].processTime + jobs[i].startTime);
            generalBlocks.push_back(*block);
        }
        else
        {
            if ((jobs[i].startTime + jobs[i].processTime) > generalBlocks.back().blockEndTime) {
                generalBlocks.back().blockEndTime = jobs[i].startTime + jobs[i].processTime;
            }
        }
    }

    for (int i = 0; i < generalBlocks.size(); i++) {
        fitness += (generalBlocks[i].blockEndTime - generalBlocks[i].blockStartTime);
    }
    for (int i = 0; i < jobStartTimes.size(); i++) {
        cout << jobs[i].startTime << " " << jobs[i].processTime << endl;
    }

}

void Population::selection(Population& currentPop, Problem prob)
{
    double totalFitness = 0, cumFitness = 0;
    vector<double> slice;
    vector<double> cumSlice;
    pop.clear();
    int i, j;

    //total fitness
    for (i = 0; i < prob.POP_SIZE; i++)
    {
        totalFitness += currentPop.pop[i].fitness;
    }

    //slice of each individual
    for (i = 0; i < prob.POP_SIZE; i++)
    {
        slice.push_back((currentPop.pop[i].fitness) / totalFitness);
        cumSlice.push_back(slice[i] + cumFitness);
        cumFitness = cumSlice[i];
    }

    // copy selected solutions into next population
    for (i = 0; i < prob.POP_SIZE; i++)
    {
        float randnum = rand_01();
        for (j = 0; j < prob.POP_SIZE; j++)
        {
            if (randnum <= cumSlice[j])
            {
                pop.push_back(currentPop.pop[j]);
                break;
            }
        }
    }
    return;
}
int main()
{
    Problem prob;
    string filename("GeneralJobList1.txt");
    prob.load_problem(filename, prob);
    prob.POP_SIZE = 1;
    Population currentPop;
    Population nextPop;
    currentPop.init_population(prob);
    nextPop.selection(currentPop, prob);

    for(int i = 0; i < prob.jobs.size(); i++)
    {
        cout << prob.jobs[i].releaseTime << " " << prob.jobs[i].deadLine << " " << prob.jobs[i].processTime << endl;
    }

    return 0;
}