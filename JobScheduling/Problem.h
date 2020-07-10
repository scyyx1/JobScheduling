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

class Problem {
    class GeneralJob {
    public:
        int index;
        double releaseTime;
        double processTime;
        double deadLine;
        double startTime;
        double latestStartTime;
        double endTime;
    };

public:
    int job_num;
    int POP_SIZE;
    vector<GeneralJob> jobs;
    void load_problem(string filename, Problem& prob);
    
};