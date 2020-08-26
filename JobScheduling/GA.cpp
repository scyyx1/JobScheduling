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

// The class used to compute blocks
class JobList {
public:
    // Detail information of each jobs in the solution
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
Population::Solution bestSolutionForNonDeadline;
double bestFitnessNewGA;
double bestFitnessOriginGA;
double bestFitnessNonDeadline;
Problem deadLineProblem;
Problem nonDeadlineProblem;
float originalLongestDeadline = 0;
float newLongestDeadline = 0;

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

// Updates the best solution and record it for new genetic algorithm
void updateBestSolutionForNewGA(Population pop, Problem prob) {
    for (int i = 0; i < prob.solutionNumber; i++) {
        // Uses naive accept function which accepts better solutions only
        if (pop.solutions[i].fitness < bestFitnessNewGA) {
            bestSolutionForNewGA = pop.solutions[i];
            bestFitnessNewGA = pop.solutions[i].fitness;
        }
    }
}

// Updates the best solution and record it for original genetic algorithm
void updateBestSolutionForOriginGA(Population pop, Problem prob) {
    for (int i = 0; i < prob.solutionNumber; i++) {
        // Uses naive accept function which accepts better solutions only
        if (pop.solutions[i].fitness < bestFitnessOriginGA) {
            bestSolutionForOriginGA = pop.solutions[i];
            bestFitnessOriginGA = pop.solutions[i].fitness;
        }
    }
}

// Updates the best solution and record it for genetic algorithm of limited job problem
void updateBestSolutionForNonDeadline(Population pop, Problem prob) {
    for (int i = 0; i < prob.solutionNumber; i++) {
        // Uses naive accept function which accepts better solutions only
        if (pop.solutions[i].fitness < bestFitnessNonDeadline) {
            bestSolutionForNonDeadline = pop.solutions[i];
            bestFitnessNonDeadline = pop.solutions[i].fitness;
        }
    }
}

// Returns the current time in millisecond
uint64_t timeSinceEpochMillisec() {
    using namespace std::chrono;
    return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}

// New genetic algorithm
void newGeneticAlgorithm(int testIndex)
{   
    string filename("TestFile0.txt");
    newLongestDeadline = deadLineProblem.loadDeadlineProb(filename, deadLineProblem);
    deadLineProblem.solutionNumber = 10;
    deadLineProblem.crossoverRate = 0.8;
    deadLineProblem.mutationRate = 0.2;

    Population currentPop;
    int totalIteration = 10;
    int curIteration = 0;
    
    currentPop.initDeadlinePop(deadLineProblem);
    bestSolutionForNewGA = currentPop.solutions[0];
    bestFitnessNewGA = currentPop.solutions[0].fitness;
    updateBestSolutionForNewGA(currentPop, deadLineProblem);
    // Local search
    currentPop.newLocalSearch(deadLineProblem);

    while (curIteration < totalIteration) {
        Population nextPop;
        // Select populaition
        nextPop.newSelection(currentPop, deadLineProblem);
        // Apply crossover
        nextPop.newCrossover(deadLineProblem);
        // Apply mutation
        nextPop.newMutation(deadLineProblem, totalIteration, curIteration, currentPop);
        // Apply replacement function
        nextPop.newReplacement(nextPop, currentPop, deadLineProblem);
        // Apply local search every 100 iteration
        if (curIteration % 100 == 0) {
           nextPop.newLocalSearch(deadLineProblem);
        }
        
        // Update counter and best solution
        currentPop = nextPop;
        updateBestSolutionForNewGA(currentPop, deadLineProblem);
        curIteration++;
    }

    cout << "New Genetic Algorithm: TestID:" << testIndex << endl << " BestFitness: " << bestFitnessNewGA << endl;
}

// Original genetic algorithm
void originalGeneticAlgorithm(int testIndex) {
    string filename("TestFile0.txt");
    originalLongestDeadline = deadLineProblem.loadDeadlineProb(filename, deadLineProblem); 
    deadLineProblem.solutionNumber = 10;
    deadLineProblem.crossoverRate = 0.8;
    deadLineProblem.mutationRate = 0.2;

    Population currentPop;
    int totalIteration = 10;
    int curIteration = 0;

    currentPop.initDeadlinePop(deadLineProblem);
    bestSolutionForOriginGA = currentPop.solutions[0];
    bestFitnessOriginGA = currentPop.solutions[0].fitness;
    updateBestSolutionForOriginGA(currentPop, deadLineProblem);

    while (curIteration < totalIteration) {
        Population nextPop;
        // Apply original selection
        nextPop.originalSelection(currentPop, deadLineProblem);
        //Apply original crossover
        nextPop.originalCrossover(deadLineProblem);
        // Apply original mutation
        nextPop.originalMutation(deadLineProblem, totalIteration, curIteration, currentPop);
        // Apply original replacement function
        nextPop.originalReplacement(nextPop, currentPop, deadLineProblem);

        // Updates the counter and best solution
        currentPop = nextPop;
        updateBestSolutionForOriginGA(currentPop, deadLineProblem);
        curIteration++; 
    }

    cout << "Original Genetic Algorithm: TestID:" << testIndex << endl << " BestFitness: " << bestFitnessOriginGA << endl;
}

// Genetic algorithm for limited job algorithm
void nonDeadlineGeneticAlgorithm(int testIndex) {

    // Read jobs from file and initialize problem
    string filename("TestFile0.txt");
    nonDeadlineProblem.loadNonDeadlineProb(filename, nonDeadlineProblem);
    nonDeadlineProblem.solutionNumber = 10;
    nonDeadlineProblem.crossoverRate = 0.8;
    nonDeadlineProblem.mutationRate = 0.2;

    Population currentPop;
    int totalIteration = 10;
    int curIteration = 0;

    currentPop.initNonDeadlinePop(nonDeadlineProblem);
    bestSolutionForNonDeadline = currentPop.solutions[0];
    bestFitnessNonDeadline = currentPop.solutions[0].fitness;
    updateBestSolutionForNonDeadline(currentPop, nonDeadlineProblem);
    // Apply local search before the first iteration
    currentPop.nonDeadlineLocalSearch(nonDeadlineProblem);

    while (curIteration < totalIteration) {
        Population nextPop;
        // Apply selection for limited job problem
        nextPop.nonDeadlineSelection(currentPop, nonDeadlineProblem);
        // Apply crossover for limited job problem
        nextPop.nonDeadlineCrossover(nonDeadlineProblem);
        // Apply mutation for limited job problem
        nextPop.nonDeadlineMutation(nonDeadlineProblem, totalIteration, curIteration, currentPop);
        // Apply replacement for limited job problem
        nextPop.nonDeadlineReplacement(nextPop, currentPop, nonDeadlineProblem);
        // Apply local search every 100 iterations
        if (curIteration % 100 == 0) {
            nextPop.nonDeadlineLocalSearch(nonDeadlineProblem);
        }

        // Update counter and best solution
        currentPop = nextPop;
        updateBestSolutionForNonDeadline(currentPop, nonDeadlineProblem);
        curIteration++;
    }

    cout << "Non DeadLine Problem: TestID:" << testIndex << endl << " BestFitness: " << bestFitnessNonDeadline << endl;
}

// Method to visualize the result
int draw() {
    GLFWwindow* originalWindow;
    GLFWwindow* newWindow;
    if (!glfwInit())
        return -1; 
    newWindow = glfwCreateWindow(640, 480, "New", NULL, NULL);
    originalWindow = glfwCreateWindow(640, 480, "Original", NULL, NULL);
    glfwMakeContextCurrent(newWindow);
   
    if (glewInit() != GLEW_OK) {
        std::cout << "Error!" << std::endl;
    }

    std::cout << glGetString(GL_VERSION) << std::endl;
    // Specifies the part of the window to which OpenGL will draw (in pixels), convert from normalised to pixels
    glViewport(0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT);
    // Projection matrix defines the properties of the camera that views the objects in the world coordinate frame. Here you typically set the zoom factor, aspect ratio and the near and far clipping planes
    glMatrixMode(GL_PROJECTION); 
    // Replace the current matrix with the identity matrix and starts us a fresh because matrix transforms such as glOrpho and glRotate cumulate, basically puts us at (0, 0, 0)
    glLoadIdentity(); 
    // Essentially set coordinate system
    glOrtho(0, SCREEN_WIDTH, 0, SCREEN_HEIGHT, 0, 1); 
    // (default matrix mode) modelview matrix defines how your objects are transformed (meaning translation, rotation and scaling) in your world
    glMatrixMode(GL_MODELVIEW); 
    // Same as above comment
    glLoadIdentity(); 

    float height = 0.9 * SCREEN_HEIGHT;

    float segment = SCREEN_HEIGHT / (deadLineProblem.jobNumber + 1);
    int position_index = 0;
    int vertex_num = 0;

    vector<JobList> bestJobList;
    for (int i = 0; i < bestSolutionForNewGA.jobStartTimes.size(); i++) {
        JobList* curJob = new JobList(bestSolutionForNewGA.jobStartTimes[i], deadLineProblem.jobs[i].processTime, deadLineProblem.jobs[i].releaseTime, deadLineProblem.jobs[i].deadLine);
        bestJobList.push_back(*curJob);
    }

    // Sort jobs according to releasing time
    std::sort(bestJobList.begin(), bestJobList.end(),
        [](const JobList& a, const JobList& b)
        {
            return a.jobReleasingTime < b.jobReleasingTime;
        }
    );

    GLfloat* lineVertices = new GLfloat[deadLineProblem.jobNumber * 6];
    GLfloat* redLineVertices = new GLfloat[deadLineProblem.jobNumber * 6];
    GLfloat* rulerVertices = new GLfloat[((int)ceil(newLongestDeadline) + 2) * 6];

    for (int i = 0; i < deadLineProblem.jobNumber; i++) {
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

    for (int i = 0; i < deadLineProblem.jobNumber; i++) {
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
    
    for (int i = 0; i < ceil(newLongestDeadline)+1; i++) {
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
    rulerVertices[position_index + 3] = ceil(newLongestDeadline) * 30;
    rulerVertices[position_index + 4] = height;
    rulerVertices[position_index + 5] = 0;

    glClear(GL_COLOR_BUFFER_BIT);

    // Render OpenGL here
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_LINE_STIPPLE);
    glPushAttrib(GL_LINE_BIT);
    glLineWidth(4);
    glEnableClientState(GL_VERTEX_ARRAY);
    glColor3f(1, 1, 1);
    glVertexPointer(3, GL_FLOAT, 0, rulerVertices);
    glDrawArrays(GL_LINES, 0, 2 * (3 + ceil(newLongestDeadline)));
    glVertexPointer(3, GL_FLOAT, 0, lineVertices);
    glDrawArrays(GL_LINES, 0, 2 * deadLineProblem.jobNumber);
    glColor3f(1, 0, 0);
    glVertexPointer(3, GL_FLOAT, 0, redLineVertices);
    glDrawArrays(GL_LINES, 0, 2 * deadLineProblem.jobNumber);

    glDisableClientState(GL_VERTEX_ARRAY);


    // Swap front and back buffers
    glfwSwapBuffers(newWindow);

    // Poll for and process events
    glfwPollEvents();
    

    //start of the original window
    glfwMakeContextCurrent(originalWindow);

    if (glewInit() != GLEW_OK) {
        std::cout << "Error!" << std::endl;
    }

    std::cout << glGetString(GL_VERSION) << std::endl;

    // Specifies the part of the window to which OpenGL will draw (in pixels), convert from normalised to pixels
    glViewport(0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT); 
    // Projection matrix defines the properties of the camera that views the objects in the world coordinate frame. Here you typically set the zoom factor, aspect ratio and the near and far clipping planes
    glMatrixMode(GL_PROJECTION); 
    // Replace the current matrix with the identity matrix and starts us a fresh because matrix transforms such as glOrpho and glRotate cumulate, basically puts us at (0, 0, 0)
    glLoadIdentity();
    // Essentially set coordinate system
    glOrtho(0, SCREEN_WIDTH, 0, SCREEN_HEIGHT, 0, 1); 
    // (default matrix mode) modelview matrix defines how your objects are transformed (meaning translation, rotation and scaling) in your world
    glMatrixMode(GL_MODELVIEW); 
    // Same as above comment
    glLoadIdentity(); 

    height = 0.9 * SCREEN_HEIGHT;

    segment = SCREEN_HEIGHT / (deadLineProblem.jobNumber + 1);
    position_index = 0;
    vertex_num = 0;

    bestJobList.clear();
    for (int i = 0; i < bestSolutionForOriginGA.jobStartTimes.size(); i++) {
        JobList* curJob = new JobList(bestSolutionForOriginGA.jobStartTimes[i], deadLineProblem.jobs[i].processTime, deadLineProblem.jobs[i].releaseTime, deadLineProblem.jobs[i].deadLine);
        bestJobList.push_back(*curJob);
    }

    // Sort all jobs according to job releasing time
    std::sort(bestJobList.begin(), bestJobList.end(),
        [](const JobList& a, const JobList& b)
        {
            return a.jobReleasingTime < b.jobReleasingTime;
        }
    );

    lineVertices = new GLfloat[deadLineProblem.jobNumber * 6];              
    redLineVertices = new GLfloat[deadLineProblem.jobNumber * 6];           
    rulerVertices = new GLfloat[(ceil(originalLongestDeadline) + 2) * 6]; 

    for (int i = 0; i < deadLineProblem.jobNumber; i++) {
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

    for (int i = 0; i < deadLineProblem.jobNumber; i++) {
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
    interval = 0;

    for (int i = 0; i < ceil(originalLongestDeadline) + 1; i++) {
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
    rulerVertices[position_index + 3] = ceil(originalLongestDeadline) * 30;
    rulerVertices[position_index + 4] = height;
    rulerVertices[position_index + 5] = 0;


    // Loop until the user closes the window
    while (!glfwWindowShouldClose(originalWindow))
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
        glDrawArrays(GL_LINES, 0, 2 * (3 + ceil(originalLongestDeadline))); 
        glVertexPointer(3, GL_FLOAT, 0, lineVertices);                     
        glDrawArrays(GL_LINES, 0, 2 * deadLineProblem.jobNumber);            
        glColor3f(1, 0, 0);                                                
        glVertexPointer(3, GL_FLOAT, 0, redLineVertices);                  
        glDrawArrays(GL_LINES, 0, 2 * deadLineProblem.jobNumber);            

        glDisableClientState(GL_VERTEX_ARRAY);


        // Swap front and back buffers
        glfwSwapBuffers(originalWindow);

        // Poll for and process events
        glfwPollEvents();
    }

    glfwTerminate();

}

// The main function
int main() {
    int iteration = 1;
    for (int i = 0; i < iteration; i++) {
        uint64_t time = timeSinceEpochMillisec();
        newGeneticAlgorithm(i);        
        originalGeneticAlgorithm(i);          
        nonDeadlineGeneticAlgorithm(i);       
        time = timeSinceEpochMillisec() - time;
        cout << " Time spent: " << time << " miliseconds" << endl;
    }
    draw();
    return 0;
}

