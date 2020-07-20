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
Population::Solution bestSolution;
double bestFitness;

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
    Problem prob;
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
}

int main() {
    for (int i = 0; i < 1; i++) {
        uint64_t time = timeSinceEpochMillisec();
        iteration(i);
        time = timeSinceEpochMillisec() - time;
        cout << " Time spent: " << time << " miliseconds" << endl;
    }

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

    ifstream in("TestFile0.txt");
    int job_num = 0;
    string line;
    string str1, str2, str3;
    float height = 0.8f;
    while (getline(in, line))
    {
        job_num++;
    }
    in.clear();
    in.seekg(0, ios::beg);
    float* positions = new float[job_num * 8];
    in.seekg(0, fstream::beg);
    int position_index = 0;
    int vertex_num = 0;

    while (getline(in, line))
    {
        stringstream word(line);
        word >> str1;
        word >> str2;
        word >> str3;

        float releaseTime = atof(str1.c_str()); 
        float deadLine = atof(str2.c_str());
        float processTime = atof(str3.c_str());

        positions[position_index] = releaseTime/20-1.0;    
        positions[position_index + 1] = height;            
        positions[position_index + 2] = deadLine/20-1.0;   
        positions[position_index + 3] = height;            
        positions[position_index + 4] = deadLine/20-1.0;   
        positions[position_index + 5] = height-0.01;       
        positions[position_index + 6] = releaseTime/20-1.0;
        positions[position_index + 7] = height-0.01;       
        position_index += 8;
        height -= 0.08;
    }

    unsigned int* indices = new unsigned int[job_num*6];
    int indices_index = 0;
    int job_index = 0;
    while (job_index < job_num) {
        indices[indices_index] = 0+ job_index*4;    
        indices[indices_index+1] = 1 + job_index*4; 
        indices[indices_index+2] = 2 + job_index*4; 
        indices[indices_index+3] = 2 + job_index*4; 
        indices[indices_index+4] = 3 + job_index*4; 
        indices[indices_index+5] = 0 + job_index*4; 
        indices_index += 6;
        job_index++;
    }
    unsigned int buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, 4 * job_num * 2 * sizeof(float), positions, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);

    unsigned int ibo;
    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, job_num * 6 * sizeof(unsigned int), indices, GL_STATIC_DRAW);


    ShaderProgramSource source = ParseShader("Basic.shader");

    unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);
    glUseProgram(shader);

    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);
     
        glDrawElements(GL_TRIANGLES, job_num * 6, GL_UNSIGNED_INT, nullptr);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteProgram(shader);

    glfwTerminate();
    return 0;
}