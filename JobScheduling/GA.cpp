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

Population::Solution bestSolution;
double bestFitness;
Problem prob;
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
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

/*
struct ShaderProgramSource {
    std::string VertexSource;
    std::string FragmentSource;
};

static ShaderProgramSource ParseShader(const std::string& filepath) {
    std::ifstream stream(filepath);

    enum class ShaderType {
        NONE = -1, VERTEX = 0, FRAGMENT = 1
    };

    std::string line;
    std::stringstream ss[2];
    ShaderType type = ShaderType::NONE;
    while (getline(stream, line)) {
        if (line.find("#shader") != std::string::npos) {
            if (line.find("vertex") != std::string::npos) {
                type = ShaderType::VERTEX;
            }
            else if (line.find("fragment") != std::string::npos) {
                type = ShaderType::FRAGMENT;
            }
        }
        else {
            ss[(int)type] << line << '\n';
        }
    }

    return { ss[0].str(), ss[1].str() };
}

static unsigned int CompileShader(unsigned int type, const std::string& source) {
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE) {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);
        std::cout << "Fail to compile" << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") 
         << " shader!" << std::endl;
        std::cout << message << std::endl;
        glDeleteShader(id);
        return 0;
    }

    return id;
}

static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader) {
    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}*/
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

    float segment = SCREEN_HEIGHT / (prob.job_num + 1);
    GLfloat* lineVertices = new GLfloat[prob.job_num * 6];
    GLfloat* redLineVertices = new GLfloat[prob.job_num * 6];
    int position_index = 0;
    int vertex_num = 0;

    vector<JobList> bestJobList;
    for (int i = 0; i < bestSolution.jobStartTimes.size(); i++) {
        JobList* curJob = new JobList(bestSolution.jobStartTimes[i], prob.jobs[i].processTime, prob.jobs[i].releaseTime, prob.jobs[i].deadLine);
        bestJobList.push_back(*curJob);
        //   cout << jobs[i].startTime << " " << jobs[i].processTime << endl;
    }

    std::sort(bestJobList.begin(), bestJobList.end(),
        [](const JobList& a, const JobList& b)
        {
            return a.jobReleasingTime <= b.jobReleasingTime;
            
        }
    );

    for (int i = 0; i < prob.job_num; i++) {
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

    for (int i = 0; i < prob.job_num; i++) {
        redLineVertices[position_index] = bestJobList[i].startTime * 30;
        redLineVertices[position_index + 1] = height;
        redLineVertices[position_index + 2] = 0;
        redLineVertices[position_index + 3] = (bestJobList[i].startTime + bestJobList[i].processTime)* 30;
        redLineVertices[position_index + 4] = height;
        redLineVertices[position_index + 5] = 0;
        position_index += 6;
        height -= segment;
    }
    /*
        while (getline(in, line))
        {
            stringstream word(line);
            word >> str1;
            word >> str2;
            word >> str3;

            float releaseTime = atof(str1.c_str());
            float deadLine = atof(str2.c_str());
            float processTime = atof(str3.c_str());
            lineVertices[position_index] = releaseTime * 30;
            lineVertices[position_index + 1] = height;
            lineVertices[position_index + 2] = 0;
            lineVertices[position_index + 3] = deadLine*30;
            lineVertices[position_index + 4] = height;
            lineVertices[position_index + 5] = 0;
            position_index += 6;
            height -= segment;
        }
    */



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
        glVertexPointer(3, GL_FLOAT, 0, lineVertices);
        glDrawArrays(GL_LINES, 0, 2 * prob.job_num);
        glColor3f(1, 0, 0);
        glVertexPointer(3, GL_FLOAT, 0, redLineVertices);
        glDrawArrays(GL_LINES, 0, 2 * prob.job_num);

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
        iteration(i);
        time = timeSinceEpochMillisec() - time;
        cout << " Time spent: " << time << " miliseconds" << endl;
    }
    draw();
    return 0;
}

