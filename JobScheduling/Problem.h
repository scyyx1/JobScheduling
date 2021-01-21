#pragma once
#include <iostream>
#include <string.h>
#include <vector>
#include <stdlib.h>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <math.h>
#include <algorithm>
#include <set>
using namespace std;

// The problem class defining the problem loaded from .txt file
class Problem {

    // The inner class recording essential information of each job
    class GeneralJob {

    public:
        // The index of each jobs
        int index;
        double releaseTime;
        double processTime;
        double deadLine;
        double startTime;
        // The latest time each job should start
        double latestStartTime;
    };

public:
    // Number of jobs in the problem
    int jobNumber;
    // Number of solutions in the population
    int solutionNumber;
    // Time the first job release
    double firstReleaseTime;
    // Time the last job finishes
    double deadLine;
    double mutationRate;
    double crossoverRate;
	double totalJobLength;
    // Vector of jobs
    vector<Problem::GeneralJob> jobs;
    // The method to load unlimited job problem
    double loadDeadlineProb(string filename, Problem& prob);
    // The method to load limited job problem
    float loadNonDeadlineProb(string filename, Problem& prob);
};