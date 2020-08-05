#include "Population.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>


using namespace std;

class JobList {
public:
    double jobReleasingTime;
    double jobDeadline;
    double startTime;
    double processTime;
    JobList(double sTime, double pTime, double rTime, double dTime) {
        jobDeadline = dTime;
        jobReleasingTime = rTime;
        startTime = sTime;
        processTime = pTime;
    }
};

Population::Solution bestSolutionForNewGA;
Population::Solution bestSolutionForOriginGA;
Population::Solution bestSolutionForNonDeadLine;
double bestFitness;
Problem deadLineProblem;
Problem nonDeadlineProblem;
float longestDeadline = 0;
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
void updateBestSolutionForNewGA(Population pop, Problem prob) {
    for (int i = 0; i < prob.solution_num; i++) {
        if (pop.solutions[i].fitness < bestFitness) {
            bestSolutionForNewGA = pop.solutions[i];
            bestFitness = pop.solutions[i].fitness;
        }
    }
}

void updateBestSolutionForOriginGA(Population pop, Problem prob) {
    for (int i = 0; i < prob.solution_num; i++) {
        if (pop.solutions[i].fitness < bestFitness) {
            bestSolutionForOriginGA = pop.solutions[i];
            bestFitness = pop.solutions[i].fitness;
        }
    }
}

void updateBestSolutionForNonDeadline(Population pop, Problem prob) {
    for (int i = 0; i < prob.solution_num; i++) {
        if (pop.solutions[i].fitness < bestFitness) {
            bestSolutionForOriginGA = pop.solutions[i];
            bestFitness = pop.solutions[i].fitness;
        }
    }
}
uint64_t timeSinceEpochMillisec() {
    using namespace std::chrono;
    return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}

void newGeneticAlgorithm(int testIndex)
{   
    string filename("TestFile0.txt");
    deadLineProblem.loadDeadlineProb(filename, deadLineProblem);
    deadLineProblem.solution_num = 10;
    deadLineProblem.crossover_rate = 0.8;
    deadLineProblem.mutation_rate = 0.2;

    Population currentPop;
    int totalIteration = 10;
    int curIteration = 0;
    
    currentPop.initDeadlinePop(deadLineProblem);
    bestSolutionForNewGA = currentPop.solutions[0];
    bestFitness = currentPop.solutions[0].fitness;
    updateBestSolutionForNewGA(currentPop, deadLineProblem);
    currentPop.newLocalSearch(deadLineProblem);

    while (curIteration < totalIteration) {
        Population nextPop;
        nextPop.newSelection(currentPop, deadLineProblem);
        //cout << "selection done" << endl;
        nextPop.newCrossover(deadLineProblem);
        //cout << "crossover done" << endl;
        nextPop.newMutation(deadLineProblem, totalIteration, curIteration, currentPop);
        //cout << "mutation done" << endl;
        nextPop.newReplacement(nextPop, currentPop, deadLineProblem);
        //cout << "replacement done" << endl;
        if (curIteration % 100 == 0) {
           nextPop.newLocalSearch(deadLineProblem);
            //cout << "local search done" << endl;
        }
        
        currentPop = nextPop;
        updateBestSolutionForNewGA(currentPop, deadLineProblem);
        curIteration++;
        //cout << curIteration << endl;
    }

    cout << "New Genetic Algorithm: TestID:" << testIndex << endl << " BestFitness: " << bestFitness << endl;
}

void originalGeneticAlgorithm(int testIndex) {
    string filename("TestFile0.txt");
    longestDeadline = deadLineProblem.loadDeadlineProb(filename, deadLineProblem);
    deadLineProblem.solution_num = 10;
    deadLineProblem.crossover_rate = 0.8;
    deadLineProblem.mutation_rate = 0.2;

    Population currentPop;
    int totalIteration = 10;
    int curIteration = 0;

    currentPop.initDeadlinePop(deadLineProblem);
    bestSolutionForOriginGA = currentPop.solutions[0];
    bestFitness = currentPop.solutions[0].fitness;
    updateBestSolutionForOriginGA(currentPop, deadLineProblem);

 
    while (curIteration < totalIteration) {
        Population nextPop;
        nextPop.originalSelection(currentPop, deadLineProblem);
        //cout << "selection done" << endl;
        nextPop.originalCrossover(deadLineProblem);
        //cout << "crossover done" << endl;
        nextPop.originalMutation(deadLineProblem, totalIteration, curIteration, currentPop);
        //cout << "mutation done" << endl;
        nextPop.originalReplacement(nextPop, currentPop, deadLineProblem);
        //cout << "replacement done" << endl;
        currentPop = nextPop;
        updateBestSolutionForNewGA(currentPop, deadLineProblem);
        curIteration++;
        //cout << curIteration << endl;
    }

    cout << "Original Genetic Algorithm: TestID:" << testIndex << endl << " BestFitness: " << bestFitness << endl;
}

void nonDeadlineGeneticAlgorithm(int testIndex) {
    string filename("TestFile0.txt");
    longestDeadline = nonDeadlineProblem.loadNonDeadlineProb(filename, nonDeadlineProblem);
    nonDeadlineProblem.solution_num = 10;
    nonDeadlineProblem.crossover_rate = 0.8;
    nonDeadlineProblem.mutation_rate = 0.2;

    Population currentPop;
    int totalIteration = 10;
    int curIteration = 0;

    currentPop.initNonDeadlinePop(nonDeadlineProblem);
    bestSolutionForNonDeadLine = currentPop.solutions[0];
    bestFitness = currentPop.solutions[0].fitness;
    updateBestSolutionForNonDeadline(currentPop, nonDeadlineProblem);
    currentPop.nonDeadlineLocalSearch(nonDeadlineProblem);

    while (curIteration < totalIteration) {
        Population nextPop;
        nextPop.nonDeadlineSelection(currentPop, nonDeadlineProblem);
        //cout << "selection done" << endl;
        nextPop.nonDeadlineCrossover(nonDeadlineProblem);
        //cout << "crossover done" << endl;
        nextPop.nonDeadlineMutation(nonDeadlineProblem, totalIteration, curIteration, currentPop);
        //cout << "mutation done" << endl;
        nextPop.nonDeadlineReplacement(nextPop, currentPop, nonDeadlineProblem);
        //cout << "replacement done" << endl;
        if (curIteration % 100 == 0) {
            nextPop.nonDeadlineLocalSearch(nonDeadlineProblem);
            //cout << "local search done" << endl;
        }

        currentPop = nextPop;
        updateBestSolutionForNonDeadline(currentPop, nonDeadlineProblem);
        curIteration++;
        //cout << curIteration << endl;
    }
    cout << "Non DeadLine Problem: TestID:" << testIndex << endl << " BestFitness: " << bestFitness << endl;
}

int draw() {
    GLFWwindow* window;

    if (!glfwInit())
        return -1;

    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK) {
        std::cout << "Error!" << std::endl;
    }

    std::cout << glGetString(GL_VERSION) << std::endl;

    glViewport(0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT); // specifies the part of the window to which OpenGL will draw (in pixels), convert from normalised to pixels
    glMatrixMode(GL_PROJECTION); // projection matrix defines the properties of the camera that views the objects in the world coordinate frame. Here you typically set the zoom factor, aspect ratio and the near and far clipping planes
    glLoadIdentity(); // replace the current matrix with the identity matrix and starts us a fresh because matrix transforms such as glOrpho and glRotate cumulate, basically puts us at (0, 0, 0)
    glOrtho(0, SCREEN_WIDTH, 0, SCREEN_HEIGHT, 0, 1); // essentially set coordinate system
    glMatrixMode(GL_MODELVIEW); // (default matrix mode) modelview matrix defines how your objects are transformed (meaning translation, rotation and scaling) in your world
    glLoadIdentity(); // same as above comment

    string line;
    string str1, str2, str3;

    float height = 0.9 * SCREEN_HEIGHT;

    float segment = SCREEN_HEIGHT / (deadLineProblem.job_num + 1);
    int position_index = 0;
    int vertex_num = 0;

    vector<JobList> bestJobList;
    for (int i = 0; i < bestSolutionForNewGA.jobStartTimes.size(); i++) {
        JobList* curJob = new JobList(bestSolutionForNewGA.jobStartTimes[i], deadLineProblem.jobs[i].processTime, deadLineProblem.jobs[i].releaseTime, deadLineProblem.jobs[i].deadLine);
        bestJobList.push_back(*curJob);
        //   cout << jobs[i].startTime << " " << jobs[i].processTime << endl;
    }

    std::sort(bestJobList.begin(), bestJobList.end(),
        [](const JobList& a, const JobList& b)
        {
            return a.jobReleasingTime < b.jobReleasingTime;

        }
    );

    GLfloat* lineVertices = new GLfloat[deadLineProblem.job_num * 6];
    GLfloat* redLineVertices = new GLfloat[deadLineProblem.job_num * 6];
    GLfloat* rulerVertices = new GLfloat[(ceil(longestDeadline) + 2) * 6];

    for (int i = 0; i < deadLineProblem.job_num; i++) {
        lineVertices[position_index] = bestJobList[i].jobReleasingTime * 30;
        lineVertices[position_index + 1] = height;
        lineVertices[position_index + 2] = 0;
        lineVertices[position_index + 3] = bestJobList[i].jobDeadline * 30;
        lineVertices[position_index + 4] = height;
        lineVertices[position_index + 5] = 0;
        position_index += 6;
        height -= segment;
    }
    position_index = 0;
    height = 0.9 * SCREEN_HEIGHT;

    for (int i = 0; i < deadLineProblem.job_num; i++) {
        redLineVertices[position_index] = bestJobList[i].startTime * 30;
        redLineVertices[position_index + 1] = height;
        redLineVertices[position_index + 2] = 0;
        redLineVertices[position_index + 3] = (bestJobList[i].startTime + bestJobList[i].processTime) * 30;
        redLineVertices[position_index + 4] = height;
        redLineVertices[position_index + 5] = 0;
        position_index += 6;
        height -= segment;
    }

    position_index = 0;
    height = 0.95 * SCREEN_HEIGHT;
    int interval = 0;
    
    for (int i = 0; i < ceil(longestDeadline)+1; i++) {
        rulerVertices[position_index] = (interval) * 30;
        rulerVertices[position_index + 1] = height;
        rulerVertices[position_index + 2] = 0;
        rulerVertices[position_index + 3] = (interval) * 30;
        rulerVertices[position_index + 4] = height + 10;
        rulerVertices[position_index + 5] = 0;
        position_index += 6;
        interval += 1;
    }
    rulerVertices[position_index] = 0;
    rulerVertices[position_index + 1] = height;
    rulerVertices[position_index + 2] = 0;
    rulerVertices[position_index + 3] = ceil(longestDeadline) * 30;
    rulerVertices[position_index + 4] = height;
    rulerVertices[position_index + 5] = 0;


    // Loop until the user closes the window
    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);

        // Render OpenGL here
        glEnable(GL_LINE_SMOOTH);
        glEnable(GL_LINE_STIPPLE);
        glPushAttrib(GL_LINE_BIT);
        glLineWidth(4);
        glEnableClientState(GL_VERTEX_ARRAY);
        glColor3f(1, 1, 1);
        glVertexPointer(3, GL_FLOAT, 0, rulerVertices);
        glDrawArrays(GL_LINES, 0, 2 * (3 + ceil(longestDeadline)));
        glVertexPointer(3, GL_FLOAT, 0, lineVertices);
        glDrawArrays(GL_LINES, 0, 2 * deadLineProblem.job_num);
        glColor3f(1, 0, 0);
        glVertexPointer(3, GL_FLOAT, 0, redLineVertices);
        glDrawArrays(GL_LINES, 0, 2 * deadLineProblem.job_num);

        glDisableClientState(GL_VERTEX_ARRAY);


        // Swap front and back buffers
        glfwSwapBuffers(window);

        // Poll for and process events
        glfwPollEvents();
    }

    glfwTerminate();

}



int main() {
    for (int i = 0; i < 1; i++) {
        uint64_t time = timeSinceEpochMillisec();
        newGeneticAlgorithm(i);
        originalGeneticAlgorithm(i);
        nonDeadlineGeneticAlgorithm(i);
        time = timeSinceEpochMillisec() - time;
        cout << " Time spent: " << time << " miliseconds" << endl;
    }
    //draw();
    return 0;
}

