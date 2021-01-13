#include "Population.h"
#include <math.h>
using namespace  std;
// Used to compare two double values
const double EPS = 1e-6; 

// The method to initialize the unlimited job problem
void Population::initDeadlinePop(Problem prob)
{
    int i, j;
    
    for (i = 0; i < prob.solutionNumber; i++)
    {
        Solution *tempSolution = new Solution();
        tempSolution->jobStartTimes.clear();
        // For every job, initialize a random double number from releasing time to latest start time as the job start time
        for (j = 0; j < prob.jobNumber; j++)
        {
            tempSolution->fitness = 0;
        
            // Generate a random number in the given range
            double tempD = rand_double(prob.jobs[j].releaseTime, prob.jobs[j].latestStartTime);
            tempSolution->jobStartTimes.push_back(tempD);
        }

        // Compute fitness for each solution
        tempSolution->deadlineComputeFitness(prob);
        solutions.push_back(*tempSolution);
        delete(tempSolution);
    }
}

// Selection function used to update population and uses RWS
void Population::newSelection(Population& currentPop, Problem prob)
{
    double totalFitness = 0, cumFitness = 0;
    vector<double> slice;
    vector<double> cumSlice;
    solutions.clear();
    int i, j;

    // Total fitness for all solutions in the population
    for (i = 0; i < prob.solutionNumber; i++)
    {
        totalFitness += currentPop.solutions[i].fitness;
    }

    // Slice of each individual
    for (i = 0; i < prob.solutionNumber; i++)
    {
        slice.push_back((currentPop.solutions[i].fitness) / totalFitness);
        cumSlice.push_back(slice[i] + cumFitness);
        cumFitness = cumSlice[i];
    }

    // Copy selected solutions into next population
    for (i = 0; i < prob.solutionNumber; i++)
    {
        float randnum = rand_01();
        for (j = 0; j < prob.solutionNumber; j++)
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

// COmpute the objective function for unlimited job problem
void Population::Solution::deadlineComputeFitness(Problem prob) {

    vector<GeneralJobBlock> generalBlocks;
    vector<Jobs> jobs;
    fitness = 0;
    for (int i = 0; i < jobStartTimes.size(); i++) {
        Jobs* curJob = new Jobs(jobStartTimes[i], prob.jobs[i].processTime);
        jobs.push_back(*curJob);
        delete(curJob);
    }

    // Sort all jobs according to job start time and finish time
    std::sort(jobs.begin(), jobs.end(),
        [](const Jobs& a, const Jobs& b)
        {
            return a.startTime < b.startTime || a.startTime == b.startTime && a.startTime + a.processTime < b.startTime + b.processTime;
        }
    );

    // Record all blocks
    GeneralJobBlock* firstBlock = new GeneralJobBlock(jobs[0].startTime, jobs[0].processTime + jobs[0].startTime);
    generalBlocks.push_back(*firstBlock);
    delete(firstBlock);
    for (int i = 1; i < jobs.size(); i++) {
        if (jobs[i].startTime > generalBlocks.back().blockEndTime) {
            GeneralJobBlock* block = new GeneralJobBlock(jobs[i].startTime, jobs[i].processTime + jobs[i].startTime);
            generalBlocks.push_back(*block);
            delete(block);
        }
        else
        {
            if ((jobs[i].startTime + jobs[i].processTime) > generalBlocks.back().blockEndTime) {
                generalBlocks.back().blockEndTime = jobs[i].startTime + jobs[i].processTime;
            }
        }
    }

    // Update the fitness which is the sum of all block length
    for (int i = 0; i < generalBlocks.size(); i++) {
        fitness += (generalBlocks[i].blockEndTime - generalBlocks[i].blockStartTime);
    }

    jobs.clear();
    vector <Jobs>().swap(jobs);
    generalBlocks.clear();
    vector <GeneralJobBlock>().swap(generalBlocks);
}

// New crossover function
void Population::newCrossover(Problem& prob) {

    for (int i = 0; i < prob.solutionNumber; i ++) {
        // Use a random double to decide whether should crossover
        double needCrossover = rand_double(0, 1);
        if (needCrossover > prob.crossoverRate + EPS) {
            continue;
        }

        // Change the job start time according to current time and the time in next solution
        if (i == prob.solutionNumber - 1) {
            for (int j = 0; j < prob.jobNumber; j++) {
                double alpha = 0.5;
                solutions[i].jobStartTimes[j] = (1 - alpha) * solutions[i].jobStartTimes[j] + alpha * solutions[0].jobStartTimes[j];

            }
        }
        else {
            for (int j = 0; j < prob.jobNumber - 1; j++) {
                double alpha = 0.5;
                solutions[i].jobStartTimes[j] = (1 - alpha) * solutions[i].jobStartTimes[j] + alpha * solutions[i+1].jobStartTimes[j];
            }
        }
        solutions[i].deadlineComputeFitness(prob);
       
    }
}

// New mutation function
void Population::newMutation(Problem prob, int totalIteration, int currentIteration, Population lastPopulation) {

    for (int i = 0; i < prob.solutionNumber; i++) {
        for (int j = 0; j < prob.jobNumber; j++) {
            
            double needMutation = rand_double(0, 1);
            if (needMutation > prob.mutationRate + EPS) {
                continue;
            }
            // Uses evolutionary mutation to generate the job start time
            double rd = rand_double(0, 1);
            double threshold = 1 - pow(rd, pow(1 - currentIteration / totalIteration, 0));
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

// New replacement function
void Population::newReplacement(Population& nextPop, Population currentPop, Problem prob) {
    double parentFitness = 0;
    double childFitness = 0;
    // Compare children and parent and choose the better solution
    for (int i = 0; i < prob.solutionNumber; i++) {
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

// New local search
void Population::newLocalSearch(Problem prob) {
    int count = 0;
    while (count < 100) {
        cout << count << endl;
        for (int i = 0; i < prob.solutionNumber; i++) {
            double bestFitness = solutions[i].fitness;
            for (int j = 0; j < prob.jobNumber; j++) {
                // Define the neighborhood
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

                // Choose the job start time from neighborhood
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

// Original selection function
void Population::originalSelection(Population& currentPop, Problem prob) {
    solutions.clear();
    int tour_size = 5;
    vector<Population::Solution> temp;
    double bestfitness = 0;
    int index = 0;
    for (int i = 0; i < prob.solutionNumber; i++)
    {
        // Uses random solutioon to select
        for (int j = 0; j < tour_size; j++)
        {
            int select = rand_int(0, prob.solutionNumber - 1);
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

// Original crossover function
void Population::originalCrossover(Problem& prob) {
    for (int i = 0; i < prob.solutionNumber; i++) {

        double needCrossover = rand_double(0, 1);
        if (needCrossover > prob.crossoverRate + EPS) {
            continue;
        }
        double alpha = rand_double(0, 1);
        // Decide job start according to current job start time and next job start time
        if (i == prob.solutionNumber - 1)
        {
            for (int j = 0; j < prob.jobNumber; j++)
            {
                solutions[i].jobStartTimes[j] = (1 - alpha) * solutions[i].jobStartTimes[j] + alpha * solutions[0].jobStartTimes[j];
            }
        }
        else
        {
            for (int j = 0; j < prob.jobNumber - 1; j++)
            {
                solutions[i].jobStartTimes[j] = (1 - alpha) * solutions[i].jobStartTimes[j] + alpha * solutions[i + 1].jobStartTimes[j];
            }
        }
        solutions[i].deadlineComputeFitness(prob);

    }
}

// Original mutation
void Population::originalMutation(Problem prob, int totalIteration, int currentIteration, Population lastPopulation) {
    for (int i = 0; i < prob.solutionNumber; i++) {
        for (int j = 0; j < prob.jobNumber; j++) {

            double needMutation = rand_double(0, 1);
            if (needMutation > prob.mutationRate + EPS) {
                continue;
            }
            // Generate a random number in the available range
            double start_time = rand_double(prob.jobs[j].releaseTime, prob.jobs[j].latestStartTime);
            this->solutions[i].jobStartTimes[j] = start_time;
            this->solutions[i].deadlineComputeFitness(prob);
        }
    }
}

// Original raplacement
void Population::originalReplacement(Population& nextPop, Population currentPop, Problem prob) {
    double parentFitness = 0;
    double childFitness = 0;
    // Chooses better solution in children and parents
    for (int i = 0; i < prob.solutionNumber; i++) {
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

// Initialize limited job problem
void Population::initNonDeadlinePop(Problem prob)
{
    int i, j;

    for (i = 0; i < prob.solutionNumber; i++)
    {
        Solution* tempSolution = new Solution();
        tempSolution->turnAroundTime = 0;
        tempSolution->jobStartTimes.clear();
        for (j = 0; j < prob.jobNumber; j++)
        {
            tempSolution->fitness = 0;

            // Get job start time from job release time and deadline which is manually defined
            double tempD = rand_double(prob.jobs[j].releaseTime, prob.deadLine);
            tempSolution->jobStartTimes.push_back(tempD);

            if (tempD + prob.jobs[j].processTime > tempSolution->turnAroundTime + EPS) {
                tempSolution->turnAroundTime = (tempD + prob.jobs[j].processTime);
            }
        }

        // Compute fitness for all solutions
        tempSolution->deadlineComputeFitness(prob);
        solutions.push_back(*tempSolution);
        delete(tempSolution);
    }
}

// Selection function for limited job problem using RWS
void Population::nonDeadlineSelection(Population& currentPop, Problem prob)
{
    double totalFitness = 0, cumFitness = 0;
    vector<double> slice;
    vector<double> cumSlice;
    solutions.clear();
    int i, j;

    // Total fitness
    for (i = 0; i < prob.solutionNumber; i++)
    {
        totalFitness += currentPop.solutions[i].fitness;
    }

    // Slice of each individual
    for (i = 0; i < prob.solutionNumber; i++)
    {
        slice.push_back((currentPop.solutions[i].fitness) / totalFitness);
        cumSlice.push_back(slice[i] + cumFitness);
        cumFitness = cumSlice[i];
    }

    // Copy selected solutions into next population
    for (i = 0; i < prob.solutionNumber; i++)
    {
        float randnum = rand_01();
        for (j = 0; j < prob.solutionNumber; j++)
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

// Function to compute fitness for limited job problem
void Population::Solution::nonDeadlineComputeFitness(Problem prob) {

    vector<GeneralJobBlock> generalBlocks;
    vector<Jobs> jobs;
    fitness = 0;
    for (int i = 0; i < jobStartTimes.size(); i++) {
        Jobs* curJob = new Jobs(jobStartTimes[i], prob.jobs[i].processTime);
        jobs.push_back(*curJob);
        delete(curJob);
    }

    // Sort all jobs in accordance with job start time and finish time
    std::sort(jobs.begin(), jobs.end(),
        [](const Jobs& a, const Jobs& b)
        {
            return a.startTime < b.startTime || a.startTime == b.startTime && a.startTime + a.processTime < b.startTime + b.processTime;
        }
    );

    // Record all blocks
    GeneralJobBlock* firstBlock = new GeneralJobBlock(jobs[0].startTime, jobs[0].processTime + jobs[0].startTime);
    generalBlocks.push_back(*firstBlock);
    delete(firstBlock);
    for (int i = 1; i < jobs.size(); i++) {
        if (jobs[i].startTime > generalBlocks.back().blockEndTime) {
            GeneralJobBlock* block = new GeneralJobBlock(jobs[i].startTime, jobs[i].processTime + jobs[i].startTime);
            generalBlocks.push_back(*block);
            delete(block);
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

    // Final fitness reflects both active time and turn around time 
    fitness = 0.7*fitness + 0.3*(turnAroundTime - prob.firstReleaseTime);
}

// Crossover function for limited job problem which is similar to new crossover funtion of unlimited job problem
void Population::nonDeadlineCrossover(Problem& prob) {

    for (int i = 0; i < prob.solutionNumber; i++) {
        double needCrossover = rand_double(0, 1);
        if (needCrossover > prob.crossoverRate + EPS) {
            continue;
        }

        if (i == prob.solutionNumber - 1) {
            for (int j = 0; j < prob.jobNumber; j++) {
                double alpha = 0.5;
                solutions[i].jobStartTimes[j] = (1 - alpha) * solutions[i].jobStartTimes[j] + alpha * solutions[0].jobStartTimes[j];

            }
        }
        else {
            for (int j = 0; j < prob.jobNumber - 1; j++) {
                double alpha = 0.5;
                solutions[i].jobStartTimes[j] = (1 - alpha) * solutions[i].jobStartTimes[j] + alpha * solutions[i + 1].jobStartTimes[j];
            }
        }
        solutions[i].nonDeadlineComputeFitness(prob);

    }
}

// Mutation function for limited job problem which is similar to new mutation funtion of unlimited job problem
void Population::nonDeadlineMutation(Problem prob, int totalIteration, int currentIteration, Population lastPopulation) {

    for (int i = 0; i < prob.solutionNumber; i++) {
        for (int j = 0; j < prob.jobNumber; j++) {

            double needMutation = rand_double(0, 1);
            if (needMutation > prob.mutationRate + EPS) {
                continue;
            }
            
            // Manually set the range to deadline
            double rd = rand_double(0, 1);
            double threshold = 1 - pow(rd, pow(1 - currentIteration / totalIteration, 1));
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

// Replacement function for limited job problem which is similar to new replacement funtion of unlimited job problem
void Population::nonDeadlineReplacement(Population& nextPop, Population currentPop, Problem prob) {
    double parentFitness = 0;
    double childFitness = 0;
    for (int i = 0; i < prob.solutionNumber; i++) {
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

// Local search function for limited job problem which is similar to local search funtion of unlimited job problem
void Population::nonDeadlineLocalSearch(Problem prob) {
    int count = 0;
    while (count < 100) {
        for (int i = 0; i < prob.solutionNumber; i++) {
            double bestFitness = solutions[i].fitness;
            for (int j = 0; j < prob.jobNumber; j++) {
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