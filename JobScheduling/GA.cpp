#include "Population.h"
using namespace std;
Population::Solution bestSolution;
double bestFitness;

void updateBestSolution(Population pop, Problem prob) {
    for (int i = 0; i < prob.solution_num; i++) {
        if (pop.solutions[i].fitness < bestFitness) {
            bestSolution = pop.solutions[i];
            bestFitness = pop.solutions[i].fitness;
        }
    }
}

int main()
{
    Problem prob;
    string filename("GeneralJobList1.txt");
    prob.load_problem(filename, prob);
    prob.solution_num = 10;
    Population currentPop;
    int totalIteration = 1000;
    int curIteration = 0;
    
    currentPop.init_population(prob);
    bestSolution = currentPop.solutions[0];
    bestFitness = currentPop.solutions[0].fitness;
    updateBestSolution(currentPop, prob);
    currentPop.localSearch(prob);

    int index = 0;
    for (int i = 0; i < prob.solution_num; i++) {
        cout << "Initial Fitness " << currentPop.solutions[i].fitness << endl;
        if (currentPop.solutions[i].fitness > currentPop.solutions[index].fitness) {
            index = i;
        }
    }

    while (curIteration < totalIteration) {
        Population nextPop;
        nextPop.selection(currentPop, prob);
        //cout << "selection done" << endl;
        nextPop.crossover(prob);
        //cout << "crossover done" << endl;
        nextPop.mutation(prob, totalIteration, curIteration, currentPop);
        //cout << "mutation done" << endl;
        nextPop.replacement(nextPop, currentPop, prob);
        //cout << "replacement done" << endl;
        if (curIteration % 100 == 0) {
           nextPop.localSearch(prob);
            //cout << "local search done" << endl;
        }
        
        currentPop = nextPop;
        updateBestSolution(currentPop, prob);
        curIteration++;
        //cout << curIteration << endl;
    }

    index = 0;
    for (int i = 0; i < prob.solution_num; i++) {
        cout << "Fitness " << currentPop.solutions[i].fitness << endl;
        if (currentPop.solutions[i].fitness > currentPop.solutions[index].fitness) {
            index = i;
        }
    }

    cout << "BestFitness " << bestFitness << endl;

 //   for (int i = 0; i < currentPop.solutions[index].jobStartTimes.size(); i++) {
   //     Jobs* curJob = new Jobs(jobStartTimes[i], prob.jobs[i].processTime);
     //   jobs.push_back(*curJob);
  //      cout << jobs[i].startTime << " " << jobs[i].processTime << endl;
    //}

   

    return 0;
}