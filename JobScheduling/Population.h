#include "Problem.h"

using namespace std;

// The population class containing specific number of solutions
class Population {

public:

    // Solution representation
    class Solution {
    public:
        // Objective value of the solution
        double fitness;
        // The time spent on finishing all jobs
        double turnAroundTime;
        // Vector of job start times of all jobs
        vector<double> jobStartTimes;
        // To methods used to compute the objective value of each solution
        void deadlineComputeFitness(Problem prob);
        void nonDeadlineComputeFitness(Problem prob);
    };

    // Blocks representing active time of CPU
    class GeneralJobBlock {
    public:
        // Start and end times of block
        double blockStartTime;
        double blockEndTime;
        GeneralJobBlock(double sTime, double eTime) {
            blockStartTime = sTime;
            blockEndTime = eTime;
        }
    };

    // Recording the start time and processing time of each jobs
    class Jobs {
    public:
        double startTime;
        double processTime;
        Jobs(double sTime, double pTime) {
            startTime = sTime;
            processTime = pTime;
        }
    };

public:
    vector<Population::Solution> solutions;

    void initDeadlinePop(Problem prob);

    // Developed selection, crossover, mutation and replacement functions for unlimited job problem
    void newSelection(Population& currentPop, Problem prob);
    void newCrossover(Problem& prob);
    void newMutation(Problem prob, int totalIteration, int currentIteration, Population lastPopulation);
    void newReplacement(Population& nextPop, Population currentPop, Problem prob);

    // Original selection, crossover, mutation and replacement functions for unlimited job problem
    void originalSelection(Population& currentPop, Problem prob);
    void originalCrossover(Problem& prob);
    void originalMutation(Problem prob, int totalIteration, int currentIteration, Population lastPopulation);
    void originalReplacement(Population& nextPop, Population currentPop, Problem prob);

    // Local search function
    void newLocalSearch(Problem prob);

    void initNonDeadlinePop(Problem prob);

    // Selection, crossover, mutation and replacement functions for limited job problem
    void nonDeadlineSelection(Population& currentPop, Problem prob);
    void nonDeadlineCrossover(Problem& prob);
    void nonDeadlineMutation(Problem prob, int totalIteration, int currentIteration, Population lastPopulation);
    void nonDeadlineReplacement(Population& nextPop, Population currentPop, Problem prob);
    void nonDeadlineLocalSearch(Problem prob);

    // Function to get a random double number within the indicated range
    double rand_double(double min, double max)
    {

        double val = min + 1.0 * rand() / RAND_MAX * (max - min);
        return val;
    }
    // Return a random number between 0 and 1
    float rand_01()
    {
        float number;
        number = (float)rand();
        number = number / RAND_MAX;
        return number;
    }

    // Return a random nunber ranging from min to max (inclusive)
    int rand_int(int min, int max)
    {
        int div = max - min + 1;
        int val = rand() % div + min;
        return val;
    }

    
};