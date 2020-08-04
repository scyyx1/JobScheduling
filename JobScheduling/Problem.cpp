#include "Problem.h"
using namespace std;

float Problem::load_problem(string filename, Problem& prob)
{
    GeneralJob tempJob;
    ifstream in(filename);
    string line;
    string str1, str2, str3;
    prob.jobs.clear();
    float longestDeadline = 0;
    while (getline(in, line))
    {
        stringstream word(line);
        word >> str1;
        word >> str2;
        word >> str3;

        tempJob.releaseTime = atof(str1.c_str());
        tempJob.deadLine = atof(str2.c_str());
        tempJob.processTime = atof(str3.c_str());
        tempJob.latestStartTime = tempJob.deadLine - tempJob.processTime;
        if (tempJob.deadLine > longestDeadline) {
            longestDeadline = tempJob.deadLine;
        }

        prob.jobs.push_back(tempJob);
    }
    in.close();
    prob.job_num = prob.jobs.size();
    return longestDeadline;
}