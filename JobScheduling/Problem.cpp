#include "Problem.h"
const double EPS = 1e-6;
using namespace std;

float Problem::loadDeadlineProb(string filename, Problem& prob)
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

float Problem::loadNonDeadlineProb(string filename, Problem& prob)
{
    GeneralJob tempJob;
    ifstream in(filename);
    string line;
    string str1, str2, str3;
    prob.jobs.clear();
    prob.deadLine = 0;
    prob.firstReleaseTime = -1;
    while (getline(in, line))
    {
        stringstream word(line);
        word >> str1;
        word >> str2;

        tempJob.releaseTime = atof(str1.c_str());
        tempJob.processTime = atof(str2.c_str());

        prob.jobs.push_back(tempJob);

        if (tempJob.releaseTime + tempJob.processTime > prob.deadLine + EPS) {
            prob.deadLine = tempJob.releaseTime + tempJob.processTime;
        }


        if ((prob.firstReleaseTime == -1) || (prob.firstReleaseTime > tempJob.releaseTime + EPS)) {
            prob.firstReleaseTime = tempJob.releaseTime;
        }
    }
    in.close();
    prob.job_num = prob.jobs.size();

    return 0;
}