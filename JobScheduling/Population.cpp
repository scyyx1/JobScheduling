#include "Population.h"
#include <math.h>
using namespace  std;
const double EPS = 1e-6; // used to compare two double values
void Population::initDeadlinePop(Problem prob)
{
    int i, j;
    
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
        tempSolution->deadlineComputeFitness(prob);
        solutions.push_back(*tempSolution);
    }
}

void Population::newSelection(Population& currentPop, Problem prob)
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
            if ((randnum - cumSlice[j]) < EPS)
            {
                solutions.push_back(currentPop.solutions[j]);
                break;
            }
        }
    }
    return;
}

void Population::Solution::deadlineComputeFitness(Problem prob) {

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

void Population::newCrossover(Problem& prob) {

    for (int i = 0; i < prob.solution_num; i ++) {
        double needCrossover = rand_double(0, 1);
        if (needCrossover > prob.crossover_rate + EPS) {
            continue;
        }

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
        solutions[i].deadlineComputeFitness(prob);
       
    }
}

void Population::newMutation(Problem prob, int totalIteration, int currentIteration, Population lastPopulation) {

    for (int i = 0; i < prob.solution_num; i++) {
        for (int j = 0; j < prob.job_num; j++) {
            
            double needMutation = rand_double(0, 1);
            if (needMutation > prob.mutation_rate + EPS) {
                continue;
            }
            //this->solutions[i].jobStartTimes[j] = rand_double(prob.jobs[j].releaseTime,prob.jobs[j].latestStartTime);

            double rd = rand_double(0, 1);
            double threshold = 1 - pow(rd, pow(1 - currentIteration / totalIteration, 5));
            double lastStartTime = lastPopulation.solutions[i].jobStartTimes[j];
            if (0.5+EPS <= rd) {
                double boundary = prob.jobs[j].latestStartTime - lastStartTime;
                this->solutions[i].jobStartTimes[j] = boundary * threshold + lastStartTime;
            }
            else {
                double boundary = lastStartTime - prob.jobs[j].releaseTime;
                this->solutions[i].jobStartTimes[j] = lastStartTime - boundary * threshold;
            }
            this->solutions[i].deadlineComputeFitness(prob);
        }
    }
}

void Population::newReplacement(Population& nextPop, Population currentPop, Problem prob) {
    double parentFitness = 0;
    double childFitness = 0;
    for (int i = 0; i < prob.solution_num; i++) {
        parentFitness = currentPop.solutions[i].fitness;
        childFitness = nextPop.solutions[i].fitness;
        if (childFitness + EPS <= parentFitness) {
            continue;
        }
        else {
            nextPop.solutions[i] = currentPop.solutions[i];
        }
    }
    
}

void Population::newLocalSearch(Problem prob) {
    int count = 0;
    while (count < 100) {
        for (int i = 0; i < prob.solution_num; i++) {
            double bestFitness = solutions[i].fitness;
            for (int j = 0; j < prob.job_num; j++) {
                double upperBound;
                double lowerBound;
                double interval = prob.jobs[j].latestStartTime - prob.jobs[j].releaseTime;
                double range = 0.05 * interval;
                double originJobStartTime = solutions[i].jobStartTimes[j];
                upperBound = originJobStartTime + range;
                lowerBound = originJobStartTime - range;
                if (originJobStartTime - range + EPS < prob.jobs[j].releaseTime) {
                    upperBound = prob.jobs[j].releaseTime + 2 * range;
                    lowerBound = prob.jobs[j].releaseTime;
                }
                else if (originJobStartTime + range > prob.jobs[j].latestStartTime + EPS) {
                    lowerBound = prob.jobs[j].latestStartTime - 2 * range;
                    upperBound = prob.jobs[j].latestStartTime;
                }

                solutions[i].jobStartTimes[j] = rand_double(lowerBound, upperBound);
                solutions[i].deadlineComputeFitness(prob);
                double currentFitness = solutions[i].fitness;
                if (currentFitness < bestFitness) {
                    bestFitness = currentFitness;
                    continue;
                }
                else {
                    solutions[i].jobStartTimes[j] = originJobStartTime;
                    solutions[i].deadlineComputeFitness(prob);
                }
            }
        }
        count++; 
    }
}

void Population::originalSelection(Population& currentPop, Problem prob) {
    solutions.clear();
    int tour_size = 5;
    vector<Population::Solution> temp;
    double bestfitness = 0;
    int index = 0;
    for (int i = 0; i < prob.solution_num; i++)
    {

        for (int j = 0; j < tour_size; j++)
        {
            int select = rand_int(0, prob.solution_num - 1);
            temp.push_back(currentPop.solutions[select]);
        }
        bestfitness = temp[0].fitness;
        for (int k = 1; k < tour_size; k++)
        {
            if (temp[k].fitness < bestfitness)
            {
                bestfitness = temp[k].fitness;
                index = k;
            }
        }
        solutions.push_back(temp[index]);
        temp.clear();
        index = 0;

    }
}

void Population::originalCrossover(Problem& prob) {
    for (int i = 0; i < prob.solution_num; i++) {

        double alpha = rand_double(0, 1);

        if (i == prob.solution_num - 1)
        {
            for (int j = 0; j < prob.job_num; j++)
            {
                solutions[i].jobStartTimes[j] = (1 - alpha) * solutions[i].jobStartTimes[j] + alpha * solutions[0].jobStartTimes[j];
            }
        }
        else
        {
            for (int j = 0; j < prob.job_num - 1; j++)
            {
                solutions[i].jobStartTimes[j] = (1 - alpha) * solutions[i].jobStartTimes[j] + alpha * solutions[i + 1].jobStartTimes[j];
            }
        }
        solutions[i].deadlineComputeFitness(prob);

    }
}

void Population::originalMutation(Problem prob, int totalIteration, int currentIteration, Population lastPopulation) {
    for (int i = 0; i < prob.solution_num; i++) {
        for (int j = 0; j < prob.job_num; j++) {

            double start_time = rand_double(prob.jobs[j].releaseTime, prob.jobs[j].latestStartTime);
            this->solutions[i].jobStartTimes[j] = start_time;
            this->solutions[i].deadlineComputeFitness(prob);
        }
    }
}

void Population::originalReplacement(Population& nextPop, Population currentPop, Problem prob) {
    double parentFitness = 0;
    double childFitness = 0;
    for (int i = 0; i < prob.solution_num; i++) {
        parentFitness = currentPop.solutions[i].fitness;
        childFitness = nextPop.solutions[i].fitness;
        if (childFitness + EPS <= parentFitness) {
            continue;
        }
        else {
            nextPop.solutions[i] = currentPop.solutions[i];
        }
    }
}

void Population::initNonDeadlinePop(Problem prob)
{
    int i, j;

    for (i = 0; i < prob.solution_num; i++)
    {
        Solution* tempSolution = new Solution();
        tempSolution->turnAroundTime = 0;
        tempSolution->jobStartTimes.clear();
        for (j = 0; j < prob.job_num; j++)
        {
            tempSolution->fitness = 0;

            double tempD = rand_double(prob.jobs[j].releaseTime, prob.deadLine);
            tempSolution->jobStartTimes.push_back(tempD);

            if (tempD + prob.jobs[j].processTime > tempSolution->turnAroundTime + EPS) {
                tempSolution->turnAroundTime = (tempD + prob.jobs[j].processTime);
            }
        }
        tempSolution->deadlineComputeFitness(prob);
        solutions.push_back(*tempSolution);
    }
}

void Population::nonDeadlineSelection(Population& currentPop, Problem prob)
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
            if ((randnum - cumSlice[j]) < EPS)
            {
                solutions.push_back(currentPop.solutions[j]);
                break;
            }
        }
    }
    return;
}

void Population::Solution::nonDeadlineComputeFitness(Problem prob) {

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

    //cout << fitness << "after";

    fitness = 0.7*fitness + 0.3*(turnAroundTime - prob.firstReleaseTime);

    //cout << fitness << endl;
}

void Population::nonDeadlineCrossover(Problem& prob) {

    for (int i = 0; i < prob.solution_num; i++) {
        double needCrossover = rand_double(0, 1);
        if (needCrossover > prob.crossover_rate + EPS) {
            continue;
        }

        if (i == prob.solution_num - 1) {
            for (int j = 0; j < prob.job_num; j++) {
                double alpha = 0.5;
                solutions[i].jobStartTimes[j] = (1 - alpha) * solutions[i].jobStartTimes[j] + alpha * solutions[0].jobStartTimes[j];

            }
        }
        else {
            for (int j = 0; j < prob.job_num - 1; j++) {
                double alpha = 0.5;
                solutions[i].jobStartTimes[j] = (1 - alpha) * solutions[i].jobStartTimes[j] + alpha * solutions[i + 1].jobStartTimes[j];
            }
        }
        solutions[i].nonDeadlineComputeFitness(prob);

    }
}

void Population::nonDeadlineMutation(Problem prob, int totalIteration, int currentIteration, Population lastPopulation) {

    for (int i = 0; i < prob.solution_num; i++) {
        for (int j = 0; j < prob.job_num; j++) {

            double needMutation = rand_double(0, 1);
            if (needMutation > prob.mutation_rate + EPS) {
                continue;
            }
            //this->solutions[i].jobStartTimes[j] = rand_double(prob.jobs[j].releaseTime,prob.jobs[j].latestStartTime);

            double rd = rand_double(0, 1);
            double threshold = 1 - pow(rd, pow(1 - currentIteration / totalIteration, 5));
            double lastStartTime = lastPopulation.solutions[i].jobStartTimes[j];
            if (0.5 + EPS <= rd) {
                double boundary = prob.deadLine - lastStartTime;
                this->solutions[i].jobStartTimes[j] = boundary * threshold + lastStartTime;
            }
            else {
                double boundary = lastStartTime - prob.jobs[j].releaseTime;
                this->solutions[i].jobStartTimes[j] = lastStartTime - boundary * threshold;
            }
            this->solutions[i].nonDeadlineComputeFitness(prob);
        }
    }
}

void Population::nonDeadlineReplacement(Population& nextPop, Population currentPop, Problem prob) {
    double parentFitness = 0;
    double childFitness = 0;
    for (int i = 0; i < prob.solution_num; i++) {
        parentFitness = currentPop.solutions[i].fitness;
        childFitness = nextPop.solutions[i].fitness;
        if (childFitness + EPS <= parentFitness) {
            continue;
        }
        else {
            nextPop.solutions[i] = currentPop.solutions[i];
        }
    }

}

void Population::nonDeadlineLocalSearch(Problem prob) {
    int count = 0;
    while (count < 100) {
        for (int i = 0; i < prob.solution_num; i++) {
            double bestFitness = solutions[i].fitness;
            for (int j = 0; j < prob.job_num; j++) {
                double upperBound;
                double lowerBound;
                double interval = prob.deadLine - prob.jobs[j].releaseTime;
                double range = 0.05 * interval;
                double originJobStartTime = solutions[i].jobStartTimes[j];
                upperBound = originJobStartTime + range;
                lowerBound = originJobStartTime - range;
                if (originJobStartTime - range + EPS < prob.jobs[j].releaseTime) {
                    upperBound = prob.jobs[j].releaseTime + 2 * range;
                    lowerBound = prob.jobs[j].releaseTime;
                }
                else if (originJobStartTime + range > prob.deadLine + EPS) {
                    lowerBound = prob.deadLine - 2 * range;
                    upperBound = prob.deadLine;
                }

                solutions[i].jobStartTimes[j] = rand_double(lowerBound, upperBound);
                solutions[i].nonDeadlineComputeFitness(prob);
                double currentFitness = solutions[i].fitness;
                if (currentFitness < bestFitness) {
                    bestFitness = currentFitness;
                    continue;
                }
                else {
                    solutions[i].jobStartTimes[j] = originJobStartTime;
                    solutions[i].nonDeadlineComputeFitness(prob);
                }
            }
        }
        count++;
    }
}