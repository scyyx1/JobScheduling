#include "Population.h"
using namespace std;

int main()
{
    Problem prob;
    string filename("GeneralJobList1.txt");
    prob.load_problem(filename, prob);
    prob.solution_num = 10;
    
    Population currentPop;
    int totalIteration = 3000;
    int curIteration = 0;
    
    currentPop.init_population(prob);

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
        nextPop.crossover(prob);
        
        nextPop.mutation(prob, totalIteration, curIteration, currentPop);
        if (curIteration == 671) {
            index = 0;
            for (int i = 0; i < prob.solution_num; i++) {
                cout << "Fitness " << currentPop.solutions[i].fitness << endl;
                if (currentPop.solutions[i].fitness > currentPop.solutions[index].fitness) {
                    index = i;
                }
            }
        }
        nextPop.replacement(nextPop, currentPop, prob);

        currentPop = nextPop;
        curIteration++;
        cout << curIteration << endl;
    }

    index = 0;
    for (int i = 0; i < prob.solution_num; i++) {
        cout << "Fitness " << currentPop.solutions[i].fitness << endl;
        if (currentPop.solutions[i].fitness > currentPop.solutions[index].fitness) {
            index = i;
        }
    }

 //   for (int i = 0; i < currentPop.solutions[index].jobStartTimes.size(); i++) {
   //     Jobs* curJob = new Jobs(jobStartTimes[i], prob.jobs[i].processTime);
     //   jobs.push_back(*curJob);
  //      cout << jobs[i].startTime << " " << jobs[i].processTime << endl;
    //}

   

    return 0;
}