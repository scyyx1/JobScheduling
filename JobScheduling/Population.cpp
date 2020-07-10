#include "Population.h"
void Population::init_population(Problem prob)
{
    int i, j;
    const double EPS = 1e-6; // used to compare two double values
    for (i = 0; i < prob.POP_SIZE; i++)
    {
        Solution tempSolution;
        tempSolution.jobStartTimes.clear();
        for (j = 0; j < prob.job_num; j++)
        {
            tempSolution.fitness = 0;
            
            double tempD = rand_double(prob.jobs[j].releaseTime, prob.jobs[j].latestStartTime);
            tempSolution.jobStartTimes.push_back(tempD);

        }
        tempSolution.compute_fitness(prob);
        pop.push_back(tempSolution);
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

void Population::Solution::compute_fitness(Problem prob) {

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