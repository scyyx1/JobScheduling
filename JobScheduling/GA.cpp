#include "Population.h"
using namespace std;

int main()
{
    Problem prob;
    string filename("GeneralJobList1.txt");
    prob.load_problem(filename, prob);
    prob.POP_SIZE = 1;
    Population currentPop;
    Population nextPop;
    currentPop.init_population(prob);
    nextPop.selection(currentPop, prob);

    for(int i = 0; i < prob.jobs.size(); i++)
    {
        cout << prob.jobs[i].releaseTime << " " << prob.jobs[i].deadLine << " " << prob.jobs[i].processTime << endl;
    }

    return 0;
}