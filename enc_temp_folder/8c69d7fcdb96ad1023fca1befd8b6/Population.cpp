#include "Population.h"
#include <math.h>
using namespace  std;
void Population::init_population(Problem prob)
{
    int i, j;
    const double EPS = 1e-6; // used to compare two double values
    for (i = 0; i < prob.solution_num; i++)
    {
        Solution *tempSolution = new Solution();
        tempSolution->jobStartTimes.clear();
        for (j = 0; j < prob.job_num; j++)
        {
            tempSolution->fitness = 0;
            
            double tempD = rand_double(prob.jobs[j].releaseTime, prob.jobs[j].latestStartTime);
            tempSolution->jobStartTimes.push_back(tempD);

        }
        tempSolution->compute_fitness(prob);
        solutions.push_back(*tempSolution);
    }
}

void Population::selection(Population& currentPop, Problem prob)
{
    double totalFitness = 0, cumFitness = 0;
    vector<double> slice;
    vector<double> cumSlice;
    solutions.clear();
    int i, j;

    //total fitness
    for (i = 0; i < prob.solution_num; i++)
    {
        totalFitness += currentPop.solutions[i].fitness;
    }

    //slice of each individual
    for (i = 0; i < prob.solution_num; i++)
    {
        slice.push_back((currentPop.solutions[i].fitness) / totalFitness);
        cumSlice.push_back(slice[i] + cumFitness);
        cumFitness = cumSlice[i];
    }

    // copy selected solutions into next population
    for (i = 0; i < prob.solution_num; i++)
    {
        float randnum = rand_01();
        for (j = 0; j < prob.solution_num; j++)
        {
            if (randnum <= cumSlice[j])
            {
                solutions.push_back(currentPop.solutions[j]);
                break;
            }
        }
    }
    return;
}

void Population::Solution::compute_fitness(Problem prob) {

    vector<GeneralJobBlock> generalBlocks;
    vector<Jobs> jobs;
    fitness = 0;
    for (int i = 0; i < jobStartTimes.size(); i++) {
        Jobs* curJob = new Jobs(jobStartTimes[i], prob.jobs[i].processTime);
        jobs.push_back(*curJob);
     //   cout << jobs[i].startTime << " " << jobs[i].processTime << endl;
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

}

void Population::crossover(Problem& prob) {
    for (int i = 0; i < prob.solution_num; i ++) {
        if (i == prob.solution_num - 1) {
            for (int j = 0; j < prob.job_num; j++) {
                double alpha = 0.5;
                solutions[i].jobStartTimes[j] = (1 - alpha) * solutions[i].jobStartTimes[j] + alpha * solutions[0].jobStartTimes[j];

            }
        }
        else {
            for (int j = 0; j < prob.job_num - 1; j++) {
                double alpha = 0.5;
                solutions[i].jobStartTimes[j] = (1 - alpha) * solutions[i].jobStartTimes[j] + alpha * solutions[i+1].jobStartTimes[j];
            }
        }
        solutions[i].compute_fitness(prob);
       
    }
}

void Population::mutation(Problem prob, int totalIteration, int currentIteration, Population lastPopulation) {

    for (int i = 0; i < prob.solution_num; i++) {
        for (int j = 0; j < prob.job_num; j++) {
            
            double needMutation = rand_double(0, 1);
            if (needMutation < 0.2) {
                continue;
            }
            //this->solutions[i].jobStartTimes[j] = rand_double(prob.jobs[j].releaseTime,prob.jobs[j].latestStartTime);
            if (currentIteration == 671) {
                int index = 0;
                for (int i = 0; i < prob.solution_num; i++) {
                    cout << "Fitness " << this->solutions[i].fitness << endl;
                    if (this->solutions[i].fitness > this->solutions[index].fitness) {
                        index = i;
                    }
                }
            }
            double rd = rand_double(0, 1);
            double threshold = 1 - pow(rd, pow(1 - currentIteration / totalIteration, 5));
            double lastStartTime = lastPopulation.solutions[i].jobStartTimes[j];
            if (rd >= 0.5) {
                double boundary = prob.jobs[j].latestStartTime - lastStartTime;
                this->solutions[i].jobStartTimes[j] = boundary * threshold + lastStartTime;
            }
            else {
                double boundary = lastStartTime - prob.jobs[j].releaseTime;
                this->solutions[i].jobStartTimes[j] = lastStartTime - boundary * threshold;
            }
            this->solutions[i].compute_fitness(prob);
        }
    }
    cout << "out" << endl;

}

void Population::replacement(Population& nextPop, Population currentPop, Problem prob) {
    double parentFitness = 0;
    double childFitness = 0;
    for (int i = 0; i < prob.solution_num; i++) {
        parentFitness = currentPop.solutions[i].fitness;
        childFitness = nextPop.solutions[i].fitness;
        if (childFitness <= parentFitness) {
            continue;
        }
        else {
            nextPop.solutions[i] = currentPop.solutions[i];
        }
    }
    
}

