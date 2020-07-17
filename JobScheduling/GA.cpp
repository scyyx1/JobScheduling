#include "Population.h"
#include <GLFW/glfw3.h>
#include <chrono>
#include <cstdint>
#include <iostream>

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

uint64_t timeSinceEpochMillisec() {
    using namespace std::chrono;
    return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}

int iteration(int testIndex)
{

    /*GLFWwindow* window;

    if (!glfwInit())
        return -1;

    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);
        glBegin(GL_TRIANGLES);
        glVertex2f(-0.5f, -0.5f);
        glVertex2f(0, 0.5f);
        glVertex2f(0.5f, -0.5f);
        glEnd();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0; */
   
    Problem prob;
    string filename("TestFile0.txt");
    prob.load_problem(filename, prob);
    prob.solution_num = 10;
    prob.crossover_rate = 0.8;
    prob.mutation_rate = 0.2;

    Population currentPop;
    int totalIteration = 10;
    int curIteration = 0;
    
    currentPop.init_population(prob);
    bestSolution = currentPop.solutions[0];
    bestFitness = currentPop.solutions[0].fitness;
    updateBestSolution(currentPop, prob);
    currentPop.localSearch(prob);

    //int index = 0;
    //for (int i = 0; i < prob.solution_num; i++) {
    //    cout << "Initial Fitness " << currentPop.solutions[i].fitness << endl;
    //    if (currentPop.solutions[i].fitness > currentPop.solutions[index].fitness) {
    //        index = i;
    //    }
    //}

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

    //index = 0;
    //for (int i = 0; i < prob.solution_num; i++) {
    //    cout << "Fitness " << currentPop.solutions[i].fitness << endl;
    //    if (currentPop.solutions[i].fitness > currentPop.solutions[index].fitness) {
    //        index = i;
    //    }
    //}

    cout << "TestID:" << testIndex << endl << " BestFitness: " << bestFitness << endl;

 //   for (int i = 0; i < currentPop.solutions[index].jobStartTimes.size(); i++) {
   //     Jobs* curJob = new Jobs(jobStartTimes[i], prob.jobs[i].processTime);
     //   jobs.push_back(*curJob);
  //      cout << jobs[i].startTime << " " << jobs[i].processTime << endl;
    //}

   

    return 0;
}

int main() {
    for (int i = 0; i < 10; i++) {
        uint64_t time = timeSinceEpochMillisec();
        iteration(i);
        time = timeSinceEpochMillisec() - time;
        cout << " Time spent: " << time << " miliseconds" << endl;
    }
    return 0;
}