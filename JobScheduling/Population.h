#include "Problem.h"

using namespace std;

class Population {

public:
    class Solution {
    public:
        double fitness;
        double turnAroundTime;
        vector<double> jobStartTimes;
        void deadlineComputeFitness(Problem prob);
        void nonDeadlineComputeFitness(Problem prob);
    };

    class GeneralJobBlock {
    public:
        double blockStartTime;
        double blockEndTime;
        GeneralJobBlock(double sTime, double eTime) {
            blockStartTime = sTime;
            blockEndTime = eTime;
        }
    };

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
    void newSelection(Population& currentPop, Problem prob);
    void newCrossover(Problem& prob);
    void newMutation(Problem prob, int totalIteration, int currentIteration, Population lastPopulation);
    void newReplacement(Population& nextPop, Population currentPop, Problem prob);
    void originalSelection(Population& currentPop, Problem prob);
    void originalCrossover(Problem& prob);
    void originalMutation(Problem prob, int totalIteration, int currentIteration, Population lastPopulation);
    void originalReplacement(Population& nextPop, Population currentPop, Problem prob);
    void newLocalSearch(Problem prob);
    void initNonDeadlinePop(Problem prob);
    void nonDeadlineSelection(Population& currentPop, Problem prob);
    void nonDeadlineCrossover(Problem& prob);
    void nonDeadlineMutation(Problem prob, int totalIteration, int currentIteration, Population lastPopulation);
    void nonDeadlineReplacement(Population& nextPop, Population currentPop, Problem prob);
    void nonDeadlineLocalSearch(Problem prob);
    double rand_double(double min, double max)
    {

        double val = min + 1.0 * rand() / RAND_MAX * (max - min);
        return val;
    }
    //return a random number between 0 and 1
    float rand_01()
    {
        float number;
        number = (float)rand();
        number = number / RAND_MAX;
        return number;
    }

    //return a random nunber ranging from min to max (inclusive)
    int rand_int(int min, int max)
    {
        int div = max - min + 1;
        int val = rand() % div + min;
        return val;
    }

    
};