#include <io.h>
#include <windows.h>
#include <gdiplus.h>
#include <iostream>
#include <fstream>
#include <string>
#include <queue>
#include <fmt/format.h>
#include <fmt/ostream.h>
#include <ctime>
#include "GA/Galgo.hpp"
#include <filesystem>
#include <execution>
#include <assert.h>
using namespace std;


void runTestFit();
void runTestTime();

int gJobNum = 30;
int gGenerations = 5000;
int gJobStart = 50;
int gJobFinish = 1000; 
int gJobStep = 50;
int gRunTestTime = 0;
int gRunTestFit = 0;
int gJob_r = 1.5*gJobNum;

int input = 0;

struct Job
{
	int job;
	int order;
	int r;
	int f;
	bool removed;
};

struct JobOverlap
{
	int num;
	int time;
	vector<Job> overlap;
};

struct Block
{
	int num;
	int time;
	vector<Job> job;
};

vector<Job> job;
vector<Job> job_init;

struct GeneralUnitJob
{
	int job;
	int order;
	float r;
	float f;
	bool removed;
};

struct generalUnitJobOverlap
{
	int num;
	float time;
	vector<GeneralUnitJob> overlap;
};

struct generalUnitJobBlock
{
	int num;
	float time;
	float length;
	vector<GeneralUnitJob> job;
};

vector<GeneralUnitJob> generalUnitJob;
vector<GeneralUnitJob> generalUnitJob_init;

struct GeneralJob
{
	int job;
	int order;
	float r;
	float p;
	float f;
	float s;
	float c;
	bool removed;
};

struct generalJobOverlap
{
	int num;
	float time;
	vector<GeneralJob> overlap;
};

struct generalJobBlock
{
	int num;
	float time;
	float length;
	vector<GeneralJob> job;
};

struct optimalGeneralBlocks
{
	float block_obj;
	vector<generalJobBlock> blocks;
};

vector<GeneralJob> generalJob;
vector<GeneralJob> generalJob_init;
deque<optimalGeneralBlocks> optimalBlocks;

std::istream& operator>>(std::istream& i, Job& j)
{
	i >> j.r >> j.f;
	return i;
}

std::istream& operator>>(std::istream& i, GeneralUnitJob& j)
{
	i >> j.r >> j.f;
	return i;
}

std::istream& operator>>(std::istream& i, GeneralJob& j)
{
	i >> j.r >> j.f >> j.p;
	return i;
}

// std::ostream& operator<<(std::ostream& o, Job& j)
// {
// 	o << fmt::format("Order={}, Index={}, Release={}, Finish={}", j.order, j.job, j.r, j.f);
// 	return o;
// }

bool operator<(const Job& a, const Job& b)
{
	return a.r < b.r || (a.r == b.r && a.f < b.f);
}

bool operator<(const GeneralUnitJob& a, const GeneralUnitJob& b)
{
	return a.r < b.r || (a.r == b.r && a.f < b.f);
}

bool operator<(const GeneralJob& a, const GeneralJob& b)
{
	return a.r < b.r || (a.r == b.r && a.f < b.f);
}

void openfile()
{
	//Open a file
	fstream file("JobList2.txt");
	{
		int i = 1;
		while (file)
		{
			file >> job.emplace_back();
			job.back().job = i;
			++i;
		}
	}

	//Initial Job_init
	job_init.resize(job.size());
	for (size_t i = 0; i < job.size(); i++)
	{
		job_init[i] = job[i];
	}

	std::sort(job.begin(), job.end()); //Sort

	//Initial order of jobs
	for (size_t i = 0; i < job.size(); i++)
	{
		job[i].order = i;
		job[i].removed = false;
	}
}

void openGenreralUnitFile()
{
	//Open a file
	fstream file("GeneralUnitJobList1.txt");
	{
		int i = 1;
		while (file)
		{
			file >> generalUnitJob.emplace_back();
			generalUnitJob.back().job = i;
			++i;
		}
	}

	//Initial generalUnitJob_init
	generalUnitJob_init.resize(generalUnitJob.size());
	for (size_t i = 0; i < generalUnitJob.size(); i++)
	{
		generalUnitJob_init[i] = generalUnitJob[i];
	}

	std::sort(generalUnitJob.begin(), generalUnitJob.end()); //Sort

	//Initial order of jobs
	for (size_t i = 1; i < generalUnitJob.size(); i++)
	{
		generalUnitJob[i].order = i;
		generalUnitJob[i].removed = false;
	}
}

void openGenreralFile()
{
	//Open a file
	fstream file("GeneralJobList1.txt");
	{
		int i = 1;
		while (file)
		{
			file >> generalJob.emplace_back();
			generalJob.back().job = i;
			++i;
		}
	}

	//Initial generalJob_init
	generalJob_init.resize(generalJob.size());
	for (size_t i = 0; i < generalJob.size(); i++)
	{
		generalJob_init[i] = generalJob[i];
	}

	std::sort(generalJob.begin(), generalJob.end()); //Sort

	//Initial order of jobs
	for (size_t i = 1; i < generalJob.size(); i++)
	{
		generalJob[i].order = i;
		generalJob[i].removed = false;
	}
}

void drawFile(HDC hdc)
{
	Gdiplus::Graphics graphics(hdc);
	Gdiplus::Pen pen(Gdiplus::Color(255, 0, 0, 0));

	//Draw Jobs without Blocks
	for (size_t i = 1; i < job.size(); i++)
	{
		graphics.DrawLine(&pen, 20 * job[i].r, 20 * i + 10, 20 * job[i].f, 20 * i + 10);
	}

	//Draw Ruler
	graphics.DrawLine(&pen, 20, 10, 1260, 10);

	for (size_t i = 1; i <= 1260 / 20; i++)
	{
		graphics.DrawLine(&pen, 20 * i, 5, 20 * i, 10);
	}
}

void drawInitFile(HDC hdc)
{
	Gdiplus::Graphics graphics(hdc);
	Gdiplus::Pen pen(Gdiplus::Color(255, 0, 0, 0));

	//Draw Jobs without Blocks
	for (size_t i = 0; i < job_init.size(); i++)
	{
		graphics.DrawLine(&pen, 20 * job_init[i].r, 20 * (i + 1) + 10, 20 * job_init[i].f, 20 * (i + 1) + 10);
	}

	//Draw Ruler
	graphics.DrawLine(&pen, 20, 10, 1260, 10);

	for (size_t i = 1; i <= 1260 / 20; i++)
	{
		graphics.DrawLine(&pen, 20 * i, 5, 20 * i, 10);
	}
}

void drawGeneralUnitFile(HDC hdc)
{
	Gdiplus::Graphics graphics(hdc);
	Gdiplus::Pen pen(Gdiplus::Color(255, 0, 0, 0));

	//Draw Jobs without Blocks
	for (size_t i = 1; i < generalUnitJob.size(); i++)
	{
		graphics.DrawLine(&pen, 20 * generalUnitJob[i].r, 20 * i + 10, 20 * generalUnitJob[i].f, 20 * i + 10);
	}

	//Draw Ruler
	graphics.DrawLine(&pen, 20, 10, 1260, 10);

	for (size_t i = 1; i <= 1260 / 20; i++)
	{
		graphics.DrawLine(&pen, 20 * i, 5, 20 * i, 10);
	}
}

void drawGeneralUnitInitFile(HDC hdc)
{
	Gdiplus::Graphics graphics(hdc);
	Gdiplus::Pen pen(Gdiplus::Color(255, 0, 0, 0));

	//Draw Jobs without Blocks
	for (size_t i = 0; i < generalUnitJob_init.size(); i++)
	{
		graphics.DrawLine(&pen, 20 * generalUnitJob_init[i].r, 20 * (i + 1) + 10, 20 * generalUnitJob_init[i].f,
		                  20 * (i + 1) + 10);
	}

	//Draw Ruler
	graphics.DrawLine(&pen, 20, 10, 1260, 10);

	for (size_t i = 1; i <= 1260 / 20; i++)
	{
		graphics.DrawLine(&pen, 20 * i, 5, 20 * i, 10);
	}
}

void drawGeneralFile(HDC hdc)
{
	Gdiplus::Graphics graphics(hdc);
	Gdiplus::Pen pen(Gdiplus::Color(255, 0, 0, 0));

	//Draw Jobs without Blocks
	for (size_t i = 1; i < generalJob.size(); i++)
	{
		graphics.DrawLine(&pen, 20 * generalJob[i].r, 20 * i + 10, 20 * generalJob[i].f, 20 * i + 10);
	}

	//Draw Ruler
	graphics.DrawLine(&pen, 20, 10, 1260, 10);

	for (size_t i = 1; i <= 1260 / 20; i++)
	{
		graphics.DrawLine(&pen, 20 * i, 5, 20 * i, 10);
	}
}

void drawGeneralInitFile(HDC hdc)
{
	Gdiplus::Graphics graphics(hdc);
	Gdiplus::Pen pen(Gdiplus::Color(255, 0, 0, 0));

	//Draw Jobs without Blocks
	for (size_t i = 0; i < generalJob_init.size(); i++)
	{
		graphics.DrawLine(&pen, 20 * generalJob_init[i].r, 20 * (i + 1) + 10, 20 * generalJob_init[i].f,
		                  20 * (i + 1) + 10);
	}

	//Draw Ruler
	graphics.DrawLine(&pen, 20, 10, 1260, 10);

	for (size_t i = 1; i <= 1260 / 20; i++)
	{
		graphics.DrawLine(&pen, 20 * i, 5, 20 * i, 10);
	}
}

void approAlgI(HDC hdc, HWND hWnd, RECT rect)
{
	vector<Job> job_1;
	deque<Job> remainingJobs;
	vector<JobOverlap> job_overlap;
	vector<Block> blocks;
	int cur_num = 0;
	int blocksNum = 0;
	Gdiplus::Graphics graphics(hdc);
	Gdiplus::Pen pen(Gdiplus::Color(255, 255, 0, 0));

	//Initial job_1
	job_1.resize(job.size());

	for (size_t i = 0; i < job.size(); i++)
	{
		job_1[i] = job[i];
	}

	//Initial remainingJobs
	for (size_t i = 1; i < job_1.size(); i++)
	{
		remainingJobs.emplace_back(job_1[i]);
	}

	auto maxf = max_element(std::begin(job_1), std::end(job_1),
		[](const Job & a, const Job & b)
		{
			return a.f < b.f;
		}
	)->f;

	int minr = job_1[1].r;

	//Unit version 2-Approximation Algorithm I
	rect.left = 700;
	rect.top = 50;
	DrawText(hdc, TEXT("Unit version 2-Approximation Algorithm I"), -1, &rect,
	         NULL);

	int removedNum = job_1.size() - 1;

	//Core Algorithm
	while (removedNum > 0)
	{
		job_overlap.clear();
		job_overlap.resize(maxf);
		int maxNum = 0;
		int maxt = minr;
		for (size_t j = minr; j < maxf; j++)
		{
			job_overlap[j].num = 0;
			job_overlap[j].time = j;
			for (int k = 1; k <= remainingJobs.size(); k++)
			{
				if (job_1[k].r <= j && j < job_1[k].f && !job_1[k].removed)
				{
					job_overlap[j].num++;
					job_overlap[j].overlap.emplace_back(job_1[k]);
				}
			}
			// Whether the number of overlapped jobs is maximum
			if (job_overlap[j].num > maxNum)
			{
				maxNum = job_overlap[j].num;
				maxt = j;
			}
		}
		//Remove Jobs
		for (int n = 0; n < maxNum; n++)
		{
			remainingJobs[job_overlap[maxt].overlap[n].order - 1].removed = true;
			job_1[job_overlap[maxt].overlap[n].order].removed = true;
			removedNum--;
		}
		//Blocks information
		blocks.resize(job_1.size());
		for (size_t b = 0; b < maxNum; b++)
		{
			blocks[cur_num].job.emplace_back(job_overlap[maxt].overlap[b]);
		}
		blocks[cur_num].num = maxNum;
		blocks[cur_num].time = job_overlap[maxt].time;
		blocksNum++;

		// Print information
		TCHAR szBuffer[100];
		wsprintf(szBuffer, TEXT("Blocks %i time: %i Jobs in block: "),
		         cur_num + 1,
		         blocks[cur_num].time
		);
		GetClientRect(hWnd, &rect);
		rect.left = 700;
		rect.top = 100 + 20 * cur_num;
		DrawText(hdc, szBuffer, -1, &rect,
		         NULL);

		for (size_t a = 0; a < maxNum; a++)
		{
			TCHAR Buffer[10];
			wsprintf(Buffer, TEXT("%i"), blocks[cur_num].job[a].order);
			GetClientRect(hWnd, &rect);
			rect.left = 920 + 20 * a;
			rect.top = 100 + 20 * cur_num;
			DrawText(hdc, Buffer, -1, &rect,
			         NULL);
		}

		cur_num++;
	}
	for (int i = 0; i < blocksNum; i++)
	{
		graphics.DrawLine(&pen, 20 * blocks[i].time, 20 * (blocks[i].job[0].order) - 9 + 10, 20 * blocks[i].time,
		                  20 * (blocks[i].job.back().order) + 9 + 10);
		graphics.DrawLine(&pen, 20 * (blocks[i].time + 1), 20 * (blocks[i].job[0].order) - 9 + 10,
		                  20 * (blocks[i].time + 1), 20 * (blocks[i].job.back().order) + 9 + 10);
	}
}

void approAlgII(HDC hdc, HWND hWnd, RECT rect)
{
	vector<Job> job_1;
	deque<Job> remainingJobs;
	vector<vector<JobOverlap>> job_overlap;
	vector<Block> blocks;
	int cur_num = 0;
	int blocksNum = 0;
	Gdiplus::Graphics graphics(hdc);
	Gdiplus::Pen pen(Gdiplus::Color(255, 255, 0, 0));

	//Initial job_1
	job_1.resize(job.size());

	for (size_t i = 0; i < job.size(); i++)
	{
		job_1[i] = job[i];
		job_1[i].removed = false;
	}

	//Initial remainingJobs
	for (size_t i = 1; i < job_1.size(); i++)
	{
		remainingJobs.emplace_back(job_1[i]);
	}

	//Unit version 2-Approximation Algorithm II
	rect.left = 700;
	rect.top = 50;
	DrawText(hdc, TEXT("Unit version 2-Approximation Algorithm II"), -1, &rect,
	         NULL);

	job_overlap.resize(remainingJobs.size() + 1);

	int removedNum = job_1.size() - 1;

	//Core Algorithm
	while (removedNum > 0)
	{
		int i = 0;
		for (size_t n = 1; n <= remainingJobs.size(); n++)
		{
			if (!remainingJobs[n - 1].removed)
			{
				i = n;
				break;
			}
		}
		// int i = remainingJobs.front().order;
		int maxNum = 1;
		int maxj = 0;
		auto interval = job_1[i].f - job_1[i].r;
		assert(interval > 0);
		auto& cur_overlap = job_overlap[i];
		cur_overlap.resize(interval);
		for (int j = 0; j < interval; j++)
		{
			cur_overlap[j].num = 0;
			cur_overlap[j].time = job_1[i].r + j;

			//Compare remaining jobs whether overlapping with the current job
			for (size_t k = 1; k <= remainingJobs.size(); k++)
			{
				//Whether overlapping
				if (job_1[k].r <= cur_overlap[j].time && cur_overlap[j].time < job_1[k].f && !job_1[k].removed)
				{
					cur_overlap[j].num++;
					cur_overlap[j].overlap.emplace_back(job_1[k]);
				}
			}
			//Whether the number of overlapped jobs is maximum
			if (cur_overlap[j].num > maxNum)
			{
				maxNum = cur_overlap[j].num;
				maxj = j;
			}
		}
		//Remove Jobs
		for (int n = 0; n < maxNum; n++)
		{
			remainingJobs[job_overlap[i][maxj].overlap[n].order - 1].removed = true;
			job_1[job_overlap[i][maxj].overlap[n].order].removed = true;
			removedNum--;
		}

		//Blocks information
		blocks.resize(job_1.size());
		for (size_t b = 0; b < maxNum; b++)
		{
			blocks[cur_num].job.emplace_back(job_overlap[i][maxj].overlap[b]);
		}
		blocks[cur_num].num = maxNum;
		blocks[cur_num].time = job_overlap[i][maxj].time;
		blocksNum ++;

		TCHAR szBuffer[100];
		wsprintf(szBuffer, TEXT("Blocks %i time: %i Jobs in block: "),
		         cur_num + 1,
		         blocks[cur_num].time
		);
		GetClientRect(hWnd, &rect);
		rect.left = 700;
		rect.top = 100 + 20 * cur_num;
		DrawText(hdc, szBuffer, -1, &rect,
		         NULL);

		for (size_t a = 0; a < maxNum; a++)
		{
			TCHAR Buffer[10];
			wsprintf(Buffer, TEXT("%i"), blocks[cur_num].job[a].order);
			GetClientRect(hWnd, &rect);
			rect.left = 920 + 20 * a;
			rect.top = 100 + 20 * cur_num;
			DrawText(hdc, Buffer, -1, &rect,
			         NULL);
			// cout << blocks[cur_num].job[a].order << " ";
		}
		cur_num++;
		cout << endl;
	}
	for (int i = 0; i < blocksNum; i++)
	{
		graphics.DrawLine(&pen, 20 * blocks[i].time, 20 * (blocks[i].job[0].order) - 9 + 10, 20 * blocks[i].time,
		                  20 * (blocks[i].job.back().order) + 9 + 10);
		graphics.DrawLine(&pen, 20 * (blocks[i].time + 1), 20 * (blocks[i].job[0].order) - 9 + 10,
		                  20 * (blocks[i].time + 1), 20 * (blocks[i].job.back().order) + 9 + 10);
	}
}

void optAlg(HDC hdc, HWND hWnd, RECT rect)
{
	vector<Job> job_1;
	vector<Job> job_2;
	queue<Job> remainingELI;
	queue<Job> remainingJobs;
	queue<Job> remainingJobs_2;
	vector<JobOverlap> job_overlap;
	vector<Block> blocks;
	int cur_num = 0;
	Gdiplus::Graphics graphics(hdc);
	Gdiplus::Pen pen(Gdiplus::Color(255, 255, 0, 0));

	//Initial job_1
	job_1.resize(job.size());

	for (size_t i = 0; i < job.size(); i++)
	{
		job_1[i] = job[i];
	}

	job_2.resize(job.size());

	//Initial job_2
	for (size_t i = 0; i < job.size(); i++)
	{
		job_2[i] = job[i];
	}

	//Initial remainingJobs
	for (size_t i = 1; i < job.size(); i++)
	{
		remainingJobs.emplace(job[i]);
	}

	job_overlap.resize(remainingJobs.size() + 1);

	int order = 1;

	job_1.erase(job_1.begin() + 0);

	while (!job_1.empty())
	{
		int num = 0;
		pen.SetColor(Gdiplus::Color(255, 0, 0, 255));
		Job ELI = *min_element(std::begin(job_1), std::end(job_1),
			[](const Job & a, const Job & b)
			{
				return a.f < b.f;
			}
		);
		remainingELI.emplace(ELI);
		for (int k = 0; k < job_1.size(); k++)
		{
			if (job_1[k].r < ELI.f)
			{
				num++;
			}
		}
		for (size_t i = 0; i < num; i++)
		{
			job_1.erase(job_1.begin() + 0);
		}
		graphics.DrawLine(&pen, 20 * remainingELI.back().r, 20 * remainingELI.back().order + 10,
		                  20 * remainingELI.back().f, 20 * remainingELI.back().order + 10);
		// cout <<"Order: "<< remainingELI.back().order << " job: " << remainingELI.back().r << " ";
	}

	//Optimal Algorithm
	rect.left = 700;
	rect.top = 50;
	DrawText(hdc, TEXT("Optimal Algorithm"), -1, &rect,
	         NULL);

	blocks.resize(remainingELI.size());

	while (!remainingJobs.empty())
	{
		int j = remainingELI.front().order;
		job_overlap[j].time = remainingELI.front().f - 1;
		job_overlap[j].num = 0;
		for (int k = 1; k <= remainingJobs.size(); k++)
		{
			if (job_2[k].r < remainingELI.front().f)
			{
				job_overlap[j].num++;
				job_overlap[j].overlap.emplace_back(job_2[k]);
			}
		}
		//Remove overlapped jobs
		//Remove the first ELI
		for (int n = 0; n < job_overlap[j].num; n++)
		{
			remainingJobs.pop();
			job_2.erase(job_2.begin() + 0);
		}
		remainingELI.pop();
		//Blocks information
		for (size_t b = 0; b < job_overlap[j].num; b++)
		{
			blocks[cur_num].job.emplace_back(job_overlap[j].overlap[b]);
		}
		blocks[cur_num].num = job_overlap[j].num;
		blocks[cur_num].time = job_overlap[j].time;

		//Print information
		// cout << "Blocks " << cur_num + 1 << " time: " << blocks[cur_num].time <<" num: "<<blocks[cur_num].num << " Order of jobs in the block: ";
		TCHAR szBuffer[100];
		wsprintf(szBuffer, TEXT("Blocks %i time: %i Jobs in block: "),
		         cur_num + 1,
		         blocks[cur_num].time
		);
		GetClientRect(hWnd, &rect);
		rect.left = 700;
		rect.top = 100 + 20 * cur_num;
		DrawText(hdc, szBuffer, -1, &rect,
		         NULL);
		for (size_t a = 0; a < blocks[cur_num].num; a++)
		{
			TCHAR Buffer[10];
			wsprintf(Buffer, TEXT("%i"), blocks[cur_num].job[a].order);
			GetClientRect(hWnd, &rect);
			rect.left = 920 + 20 * a;
			rect.top = 100 + 20 * cur_num;
			DrawText(hdc, Buffer, -1, &rect,
			         NULL);
			// cout << blocks[cur_num].job[a].order << " ";
		}
		cur_num++;
		cout << endl;
	}
	pen.SetColor(Gdiplus::Color(255, 255, 0, 0));
	for (int i = 0; i < blocks.size(); i++)
	{
		graphics.DrawLine(&pen, 20 * blocks[i].time, 20 * (blocks[i].job[0].order) - 9 + 10, 20 * blocks[i].time,
		                  20 * (blocks[i].job.back().order) + 9 + 10);
		graphics.DrawLine(&pen, 20 * (blocks[i].time + 1), 20 * (blocks[i].job[0].order) - 9 + 10,
		                  20 * (blocks[i].time + 1), 20 * (blocks[i].job.back().order) + 9 + 10);
	}
}

void approGeneralUnitAlg(HDC hdc, HWND hWnd, RECT rect)
{
	vector<GeneralUnitJob> job_1;
	vector<GeneralUnitJob> job_2;
	queue<GeneralUnitJob> remainingELI;
	queue<GeneralUnitJob> remainingJobs;
	vector<generalUnitJobOverlap> job_overlap;
	vector<generalUnitJobBlock> blocks;
	int cur_num = 0;
	Gdiplus::Graphics graphics(hdc);
	Gdiplus::Pen pen(Gdiplus::Color(255, 255, 0, 0));

	//Initial job_1
	job_1.resize(generalUnitJob.size());

	for (size_t i = 0; i < generalUnitJob.size(); i++)
	{
		job_1[i] = generalUnitJob[i];
	}

	job_2.resize(generalUnitJob.size());

	//Initial job_2
	for (size_t i = 0; i < generalUnitJob.size(); i++)
	{
		job_2[i] = generalUnitJob[i];
	}

	//Initial remainingJobs
	for (size_t i = 1; i < generalUnitJob.size(); i++)
	{
		remainingJobs.emplace(generalUnitJob[i]);
	}

	job_overlap.resize(remainingJobs.size() + 1);

	int order = 1;

	job_1.erase(job_1.begin() + 0);

	while (!job_1.empty())
	{
		int num = 0;
		pen.SetColor(Gdiplus::Color(255, 0, 0, 255));
		GeneralUnitJob ELI = *min_element(std::begin(job_1), std::end(job_1),
		                                  [](const GeneralUnitJob& a, const GeneralUnitJob& b)
		                                  {
			                                  return a.f < b.f;
		                                  }
		);
		remainingELI.emplace(ELI);

		//Remove intervals before ELI
		for (int k = 0; k < job_1.size(); k++)
		{
			if (job_1[k].r < ELI.f)
			{
				num++;
			}
		}
		for (size_t i = 0; i < num; i++)
		{
			job_1.erase(job_1.begin() + 0);
		}
		graphics.DrawLine(&pen, 20 * remainingELI.back().r, float(20 * remainingELI.back().order + 10),
		                  20 * remainingELI.back().f, float(20 * remainingELI.back().order + 10));
	}

	//Optimal Algorithm
	rect.left = 700;
	rect.top = 50;
	DrawText(hdc, TEXT("General Unit Job 2-Aprroximation Algorithm"), -1, &rect, NULL);

	blocks.resize(remainingELI.size());

	while (!remainingJobs.empty())
	{
		int j = remainingELI.front().order;
		job_overlap[j].time = remainingELI.front().f - 1;
		job_overlap[j].num = 0;
		blocks[cur_num].length = 1;
		for (int k = 1; k <= remainingJobs.size(); k++)
		{
			if (job_2[k].r <= remainingELI.front().f - 1)
			{
				job_overlap[j].num++;
				job_overlap[j].overlap.emplace_back(job_2[k]);
			}
			if (job_2[k].r < remainingELI.front().f && job_2[k].r > remainingELI.front().f - 1 && blocks[cur_num].length
				< 2 - remainingELI.front().f + job_2[k].r)
			{
				job_overlap[j].num++;
				job_overlap[j].overlap.emplace_back(job_2[k]);
				blocks[cur_num].length = 2 - remainingELI.front().f + job_2[k].r;
			}
		}

		//Remove the first ELI&&Remove overlapped jobs
		for (int n = 0; n < job_overlap[j].num; n++)
		{
			remainingJobs.pop();
			job_2.erase(job_2.begin() + 0);
		}
		remainingELI.pop();

		//Blocks information
		for (size_t b = 0; b < job_overlap[j].num; b++)
		{
			blocks[cur_num].job.emplace_back(job_overlap[j].overlap[b]);
		}
		blocks[cur_num].num = job_overlap[j].num;
		blocks[cur_num].time = job_overlap[j].time;

		//Print information
		const auto buf = fmt::format(
			L"Blocks {} time: {:0.2f} Jobs in block: ",
			cur_num + 1,
			blocks[cur_num].time);
		GetClientRect(hWnd, &rect);
		rect.left = 700;
		rect.top = 100 + 20 * cur_num;
		DrawText(hdc, buf.c_str(), -1, &rect, NULL);
		for (size_t a = 0; a < blocks[cur_num].num; a++)
		{
			TCHAR Buffer[10];
			wsprintf(Buffer, TEXT("%i"), blocks[cur_num].job[a].order);
			GetClientRect(hWnd, &rect);
			rect.left = 940 + 20 * a;
			rect.top = 100 + 20 * cur_num;
			DrawText(hdc, Buffer, -1, &rect, NULL); //Print job order in the block
		}
		cur_num++;
		cout << endl;
	}
	pen.SetColor(Gdiplus::Color(255, 255, 0, 0));
	for (int i = 0; i < blocks.size(); i++)
	{
		graphics.DrawLine(&pen, 20 * blocks[i].time, float(20 * (blocks[i].job[0].order) - 9 + 10), 20 * blocks[i].time,
		                  float(20 * (blocks[i].job.back().order) + 9 + 10));
		graphics.DrawLine(&pen, 20 * (blocks[i].time + blocks[i].length), float(20 * (blocks[i].job[0].order) - 9 + 10),
		                  20 * (blocks[i].time + blocks[i].length), float(20 * (blocks[i].job.back().order) + 9 + 10));
	}
}

void geneToBlocks(galgo::GeneticAlgorithm<double> ga, deque<GeneralJob>& generalJob_1, deque<generalJobBlock>& generalBlocks,vector<double> *worstGenes = nullptr)
{
	if(input == 14 && worstGenes)
	{
		auto& worstGenesRef = *worstGenes;
		for (size_t i = 0; i < generalJob.size() - 1; i++)
		{
			generalJob[i + 1].s = worstGenesRef[i];
			generalJob[i + 1].c = worstGenesRef[i] + generalJob[i + 1].p;
		}
	}
	else
	{
		for (size_t i = 0; i < generalJob.size() - 1; i++)
		{
			generalJob[i + 1].s = ga.pop(0)->getParam()[i];
			generalJob[i + 1].c = ga.pop(0)->getParam()[i] + generalJob[i + 1].p;
		}
	}

	generalJob_1.resize(generalJob.size() - 1);
	for (size_t i = 0; i < generalJob.size() - 1; i++)
	{
		generalJob_1[i] = generalJob[i + 1];
	}
	std::sort(generalJob_1.begin(), generalJob_1.end(),
		[](const GeneralJob & a, const GeneralJob & b)
		{
			return a.s < b.s || a.s == b.s && a.s + a.p < b.s + b.p;
			// return a.s < b.s;
		});
	for (size_t i = 0; i < generalJob_1.size(); i++)
	{
		if (i == 0)
		{
			generalJobBlock general_block_first = {
				1, generalJob_1[i].s, generalJob_1[i].p, {generalJob_1[i]}
			};
			generalBlocks.emplace_back(general_block_first);
		}
		else
		{
			if (generalJob_1[i].s <= generalBlocks.back().time + generalBlocks.back().length)
			{
				generalBlocks.back().job.emplace_back(generalJob_1[i]);
				generalBlocks.back().num = generalBlocks.back().job.size();
				generalBlocks.back().length = max_element(generalBlocks.back().job.begin(),
					generalBlocks.back().job.end(),
					[](const GeneralJob & a, const GeneralJob & b)
					{
						return a.c < b.c;
					})->c - generalBlocks.back().time;
			}
			if (generalJob_1[i].s > generalBlocks.back().time + generalBlocks.back().length)
			{
				generalJobBlock general_block_first = {
					1, generalJob_1[i].s, generalJob_1[i].p, {generalJob_1[i]}
				};
				generalBlocks.emplace_back(general_block_first);
			}
		}
	}
}

// objective class example
template <typename T>
class MyObjective
{


public:
	// minimizing f(x,y) = (1 - x)^2 + 100 * (y - x^2)^2
	static std::vector<T> Objective(const std::vector<T>& x)
	{
		static vector<GeneralJob> generalJob_obj;
		static vector<generalJobBlock> generalBlocks_obj;
		for (size_t i = 0; i < generalJob.size() - 1; i++)
		{
			generalJob[i + 1].s = x[i];
			generalJob[i + 1].c = x[i] + generalJob[i + 1].p;
		}
		size_t block_num = 0;
		generalJob_obj.resize(generalJob.size() - 1);
		generalBlocks_obj.resize(generalJob_obj.size());
		for (size_t i = 0; i < generalJob.size() - 1; i++)
		{
			generalJob_obj[i] = generalJob[i + 1];
		}
		std::sort(generalJob_obj.begin(), generalJob_obj.end(),
			[](const GeneralJob & a, const GeneralJob & b)
			{
				// return a.s < b.s;
				return a.s < b.s || a.s == b.s && a.s + a.p < b.s + b.p;
			});
		for (size_t i = 0; i < generalJob_obj.size(); i++)
		{
			if (i == 0)
			{
				auto& block = generalBlocks_obj[block_num++];
				block.num = 1;
				block.time = generalJob_obj[i].s;
				block.length = generalJob_obj[i].p;
				block.job.clear();
				block.job.emplace_back(generalJob_obj[i]);
			}
			else
			{
				auto &last_block = generalBlocks_obj[block_num - 1];
				if (generalJob_obj[i].s <= last_block.time + last_block.length)
				{
					last_block.job.emplace_back(generalJob_obj[i]);
					last_block.num = last_block.job.size();
					last_block.length = max_element(last_block.job.begin(),
					                                              last_block.job.end(),
					                                              [](const GeneralJob& a, const GeneralJob& b)
					                                              {
						                                              return a.c < b.c;
					                                              })->c - last_block.time;
				}
				if (generalJob_obj[i].s > last_block.time + last_block.length)
				{
					auto& block = generalBlocks_obj[block_num++];
					block.num = 1;
					block.time = generalJob_obj[i].s;
					block.length = generalJob_obj[i].p;
					block.job.clear();
					block.job.emplace_back(generalJob_obj[i]);
				}
			}
		}

		T obj = 0.0;

		for (size_t i = 0; i < block_num; i++)
		{
			obj -= generalBlocks_obj[i].length;
		}

		return {obj};
	}

	// NB: GALGO maximize by default so we will maximize -f(x,y)
};

// constraints example:
// 1) x * y + x - y + 1.5 <= 0
// 2) 10 - x * y <= 0
template <typename T>
std::vector<T> MyConstraint(const std::vector<T>& x)
{
	T Constraint = -12;
	return {Constraint};
}

// NB: a penalty will be applied if one of the constraints is > 0 
// using the default adaptation to constraint(s) method

using namespace Gdiplus;
#pragma comment (lib,"Gdiplus.lib")

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int main(int argc, char* argv[])
{
	for(int i = 1; i < argc; ++i)
	{
		std::string cur(argv[i]);
#define PARAM(name, var) \
		if(cur == #name)\
		{\
			var = stoi(argv[++i]);\
			cout << #var " = " << var << endl;\
		}
		PARAM(gen, gGenerations)
		else PARAM(start, gJobStart)
		else PARAM(finish, gJobFinish)
		else PARAM(step, gJobStep)
		else PARAM(runtime, gRunTestTime)
		else PARAM(runfit, gRunTestFit)
		else PARAM(testnum, gJobNum)
	}

	if (gRunTestTime)
	{
		fmt::print("running tests from {} to {}, step={}, generations={}\n",
			gJobStart, gJobFinish, gJobStep, gGenerations);
		runTestTime();
		return 0;
	}

	if(gRunTestFit)
	{
		fmt::print("running tests from {} to {}, step={}, generations={}\n",
			gJobStart, gJobFinish, gJobStep, gGenerations);
		runTestFit();
		return 0;
	}

	auto hInstance = GetModuleHandle(NULL);
	HWND hWnd;
	MSG msg;
	WNDCLASS wndClass;
	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;

	openfile();
	openGenreralUnitFile();
	openGenreralFile();

	// Initialize GDI+.
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc = WndProc;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hInstance = hInstance;
	wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndClass.lpszMenuName = NULL;
	wndClass.lpszClassName = TEXT("JobScheduling");

	RegisterClass(&wndClass);

	hWnd = CreateWindow(
		TEXT("JobScheduling"), // window class name
		TEXT("Job Scheduling"), // window caption
		WS_OVERLAPPEDWINDOW, // window style
		250,
		0,
		// CW_USEDEFAULT, // initial x position
		// CW_USEDEFAULT, // initial y position
		1293,
		831,
		// CW_USEDEFAULT, // initial x size
		// CW_USEDEFAULT, // initial y size
		NULL, // parent window handle
		NULL, // window menu handle
		hInstance, // program instance handle
		NULL); // creation parameters

	ShowWindow(hWnd, SW_SHOW);
	UpdateWindow(hWnd);

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	GdiplusShutdown(gdiplusToken);
}

void runTest_runningtime_jobnum(std::filesystem::path mTestFolder, size_t jobnum)
{
	std::random_device rd;  //Will be used to obtain a seed for the random number engine
	std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
	std::uniform_real_distribution<float> randr(1.0, gJob_r);
	std::uniform_real_distribution<float> randp(0.5, 3.0);
	std::uniform_real_distribution<float> randlength(1.0, 5.0);
	//Out put fitness result
	ofstream runningTimeFile(mTestFolder / fmt::format("tests_{}.csv", jobnum));
	for (size_t runnum = 0; runnum < 10; runnum++)
	{	
		const auto begin_time = std::chrono::high_resolution_clock::now();
		generalJob.clear();

		float ave_job_length = 0;
		float ave_interval_length = 0;

		for (size_t j = 1; j < jobnum + 1; j++)
		{
			generalJob.emplace_back(GeneralJob());
			generalJob.back().r = randr(gen);
			generalJob.back().p = randp(gen);
			float intervalLength = randlength(gen) + generalJob.back().p;
			generalJob.back().f = generalJob.back().r + intervalLength;
		}

		GeneralJob first;
		first.p = 0;
		ave_job_length = std::accumulate(generalJob.begin(), generalJob.end(), first, [](auto & a, auto & b)
			{
				GeneralJob tmp; tmp.p = a.p + b.p;
				return tmp;
			}).p / jobnum;

		for (size_t i = 0; i < generalJob.size(); i++)
		{
			ave_interval_length += (generalJob[i].f - generalJob[i].r) / jobnum;
		}

		std::sort(generalJob.begin(), generalJob.end()); //Sort

		//Initial order of jobs
		for (size_t i = 1; i < generalJob.size(); i++)
		{
			generalJob[i].order = i;
			generalJob[i].removed = false;
		}

		// defining lower bound LB and upper bound UB
		std::vector<double> LB;
		std::vector<double> UB;

		LB.resize(generalJob.size() - 1);
		UB.resize(generalJob.size() - 1);

		for (size_t i = 0; i < generalJob.size() - 1; i++)
		{
			// assert(LB[i] < UB[i]);
			LB[i] = generalJob[i + 1].r;
			UB[i] = generalJob[i + 1].f - generalJob[i + 1].p;
		}

		// initiliazing genetic algorithm
		galgo::GeneticAlgorithm<double> ga(MyObjective<double>::Objective, 100, LB, UB, gGenerations, false);

		// setting constraints
		//ga.Constraint = MyConstraint;

		// running genetic algorithm
		ga.run();
		ga.createPopulation();;

		// initializing best result and previous best result
		float bestResult = ga.pop(0)->getTotal();
		float prevBestResult = bestResult;

		// outputting results 
		if (ga.output) ga.print();

		// starting population evolution
		for (ga.nogen = 1; ga.nogen <= ga.nbgen; ++ga.nogen)
		{
			// evolving population
			ga.pop.evolution();

			// getting best current result
			bestResult = ga.pop(0)->getTotal();

			// if ((bestResult - prevBestResult) < 10)
			// {
			// 	break;
			// }

			// checking convergence
			if (ga.tolerance != 0.0)
			{
				if (fabs(bestResult - prevBestResult) < fabs(ga.tolerance))
				{
					break;
				}
				prevBestResult = bestResult;
			}
		}

		// gene to blocks
		deque<GeneralJob> generalJob_1;
		deque<generalJobBlock> generalBlocks;

		generalJob_1.clear();
		generalBlocks.clear();

		geneToBlocks(ga, generalJob_1, generalBlocks);

		float best_blocks_length = 0;

		for (size_t i = 0; i < generalBlocks.size(); i++)
		{
			best_blocks_length += generalBlocks[i].length;
		}

		const auto end_time = std::chrono::high_resolution_clock::now();
		const std::chrono::duration<double> delta_time
			= end_time - begin_time;

		fmt::print(runningTimeFile,
			"{}, {}, {}, {}, {}, {}, {}",
			jobnum,
			delta_time.count(),
			best_blocks_length,
			generalBlocks.size(),
			best_blocks_length/ generalBlocks.size(),
			ave_interval_length,
			ave_job_length
			);

		runningTimeFile << std::endl;
	}
	runningTimeFile.close();
}

void runTest_fitness_generation(std::filesystem::path mTestFolder, size_t jobnum)
{
	std::random_device rd;  //Will be used to obtain a seed for the random number engine
	std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
	std::uniform_real_distribution<float> randr(1.0, gJob_r);
	std::uniform_real_distribution<float> randp(0.5, 3.0);
	std::uniform_real_distribution<float> randlength(1.0, 5.0);
	//Out put fitness result
	ofstream fitnessFile(mTestFolder / fmt::format("{}tests.csv", jobnum));
	generalJob.clear();

	for (size_t j = 1; j < jobnum + 1; j++)
	{
		generalJob.emplace_back(GeneralJob());
		generalJob.back().r = randr(gen);
		generalJob.back().p = randp(gen);
		float intervalLength = randlength(gen) + generalJob.back().p;
		generalJob.back().f = generalJob.back().r + intervalLength;
	}

	std::sort(generalJob.begin(), generalJob.end()); //Sort

	//Initial order of jobs
	for (size_t i = 1; i < generalJob.size(); i++)
	{
		generalJob[i].order = i;
		generalJob[i].removed = false;
	}

	// defining lower bound LB and upper bound UB
	std::vector<double> LB;
	std::vector<double> UB;

	LB.resize(generalJob.size() - 1);
	UB.resize(generalJob.size() - 1);

	for (size_t i = 0; i < generalJob.size() - 1; i++)
	{
		// assert(LB[i] < UB[i]);
		LB[i] = generalJob[i + 1].r;
		UB[i] = generalJob[i + 1].f - generalJob[i + 1].p;
	}

	// initiliazing genetic algorithm
	galgo::GeneticAlgorithm<double> ga(MyObjective<double>::Objective, 100, LB, UB, gGenerations, false);

	// setting constraints
	//ga.Constraint = MyConstraint;

	// running genetic algorithm
	ga.run();
	ga.createPopulation();

	vector<float> best_fitness;
	best_fitness.clear();

	// initializing best result and previous best result
	float bestResult = ga.pop(0)->getTotal();
	float prevBestResult = bestResult;
	// outputting results 
	if (ga.output) ga.print();

	auto e_start = std::chrono::high_resolution_clock::now();
	// starting population evolution
	for (ga.nogen = 1; ga.nogen <= ga.nbgen; ++ga.nogen)
	{
		auto start = std::chrono::high_resolution_clock::now();
		// evolving population
		ga.pop.evolution();

		// getting best current result
		bestResult = ga.pop(0)->getTotal();

		//For later fitness print
		best_fitness.emplace_back(ga.pop.getSumFitness());

		auto end = std::chrono::high_resolution_clock::now();
		
		std::chrono::duration<float> delta = end - start;
		std::chrono::duration<float> elapsed = end - e_start;

		fmt::print(fitnessFile, "{}, {}, {}, {}", ga.nogen, ga.pop.getSumFitness(), delta.count(), elapsed.count());
		fitnessFile << endl;


		// checking convergence
		if (ga.tolerance != 0.0)
		{
			if (fabs(bestResult - prevBestResult) < fabs(ga.tolerance))
			{
				break;
			}
			prevBestResult = bestResult;
		}
	}
	fitnessFile.close();
}


void runTestFit()
{
	std::filesystem::path mTestFolder = fmt::format("FitnessTests/test_{}", time(0));
	create_directories(mTestFolder);
	std::vector<size_t> jobnums;
	int jobnum = gJobNum;
	runTest_fitness_generation(mTestFolder, jobnum);
}

void runTestTime()
{
	std::filesystem::path mTestFolder = fmt::format("RunTimeTests/test_{}", time(0));
	create_directories(mTestFolder);
	std::vector<size_t> jobnums;
	for (size_t jobnum = gJobStart; jobnum <= gJobFinish; jobnum += gJobStep)
	{
		jobnums.push_back(jobnum);
	}
	std::for_each(
		// std::execution::par,
		jobnums.begin(), jobnums.end(), 
		[&](auto jobnum)
		{
			runTest_runningtime_jobnum(mTestFolder, jobnum);
		});
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message,
                         WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;

	switch (message)
	{
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);

		//Clear Window
		RECT rect;
		HBRUSH hBr;
		SetRect(&rect, 0, 0, 2000, 2000);
		hBr = CreateSolidBrush(RGB(255, 255, 255));
		FillRect(hdc, &rect, hBr);

		switch (input)
		{
		case 1:
			drawFile(hdc);
			approAlgI(hdc, hWnd, rect);
			break;
		case 2:
			drawFile(hdc);
			approAlgII(hdc, hWnd, rect);
			break;
		case 3:
			drawFile(hdc);
			optAlg(hdc, hWnd, rect);
			break;
		case 4:
			drawGeneralUnitFile(hdc);
			approGeneralUnitAlg(hdc, hWnd, rect);
			break;
		case 5: case 14:
			{
				drawGeneralFile(hdc);
				// defining lower bound LB and upper bound UB
				std::vector<double> LB;
				std::vector<double> UB;

				const auto bufTitle = fmt::format(
					L"General Job Genetic Algorithm with Generation Number: {}",
					gGenerations);
				rect.left = 700;
				rect.top = 50;
				DrawText(hdc, bufTitle.c_str(), -1, &rect,
					NULL);

				LB.resize(generalJob.size() - 1);
				UB.resize(generalJob.size() - 1);

				for (size_t i = 0; i < generalJob.size() - 1; i++)
				{
					// assert(LB[i] < UB[i]);
					LB[i] = generalJob[i + 1].r;
					UB[i] = generalJob[i + 1].f - generalJob[i + 1].p;
				}

				// initiliazing genetic algorithm
				galgo::GeneticAlgorithm<double> ga(MyObjective<double>::Objective, 100, LB, UB, gGenerations, false);

				// setting constraints
				ga.Constraint = MyConstraint;

				// running genetic algorithm
				ga.run();
				ga.createPopulation();

				vector<float> best_fitness;

				// initializing best result and previous best result
				float bestResult = ga.pop(0)->getTotal();
				float prevBestResult = bestResult;


				vector worstGenes = ga.pop(0)->getParam();

				// outputting results 
				if (ga.output) ga.print();

				// starting population evolution
				for (ga.nogen = 1; ga.nogen <= ga.nbgen; ++ga.nogen)
				{
					// evolving population
					ga.pop.evolution();

					// getting best current result
					bestResult = ga.pop(0)->getTotal();

					//For later fitness print
					best_fitness.emplace_back(ga.pop.getSumFitness());

					// checking convergence
					if (ga.tolerance != 0.0)
					{
						if (fabs(bestResult - prevBestResult) < fabs(ga.tolerance))
						{
							break;
						}
						prevBestResult = bestResult;
					}
				}

				//Out put fitness result
				ofstream fitnessFile;
				fitnessFile.open("Fitness_Result.csv");
				for (size_t i = 0; i < best_fitness.size(); i++)
				{
					fitnessFile << best_fitness[i] << endl;
				}
				fitnessFile.close();

				deque<GeneralJob> generalJob_1;
				deque<generalJobBlock> generalBlocks;

				geneToBlocks(ga, generalJob_1, generalBlocks, &worstGenes);

				Gdiplus::Graphics graphics(hdc);
				Gdiplus::Pen pen(Gdiplus::Color(255, 0, 255, 200));
				
				for (size_t i = 0; i < generalJob_1.size(); i++)
				{
					graphics.DrawLine(&pen, 20 * generalJob_1[i].s, float(20 * generalJob_1[i].order + 10),
						20 * generalJob_1[i].c, float(20 * generalJob_1[i].order + 10));
				}

				pen.SetColor(Gdiplus::Color(255, 255, 0, 0));

				for (int i = 0; i < generalBlocks.size(); i++)
				{
					auto y_1 = min_element(generalBlocks[i].job.begin(), generalBlocks[i].job.end(),
					                       [](const GeneralJob& a, const GeneralJob& b)
					                       {
						                       return a.order < b.order;
					                       }
					)->order;
					auto y_2 = max_element(generalBlocks[i].job.begin(), generalBlocks[i].job.end(),
					                       [](const GeneralJob& a, const GeneralJob& b)
					                       {
						                       return a.order < b.order;
					                       }
					)->order;
					graphics.DrawLine(&pen, 20 * generalBlocks[i].time, float(20 * y_1 - 9 + 10),
					                  20 * generalBlocks[i].time, float(20 * y_2 + 9 + 10));
					graphics.DrawLine(&pen, 20 * (generalBlocks[i].time + generalBlocks[i].length),
					                  float(20 * y_1 - 9 + 10), 20 * (generalBlocks[i].time + generalBlocks[i].length),
					                  float(20 * y_2 + 9 + 10));
				}

				float general_block_length = 0;

				for (size_t i = 0; i < generalBlocks.size(); i++)
				{
					general_block_length += generalBlocks[i].length;
				}

				const auto blockLength_str = fmt::format(
					L"Total active time: {:0.2f}",
					general_block_length);
				GetClientRect(hWnd, &rect);
				rect.left = 700;
				rect.top = 75;
				SetTextColor(hdc, RGB(200, 0, 200));
				DrawText(hdc, blockLength_str.c_str(), -1, &rect, NULL);


				SetTextColor(hdc, RGB(0, 0, 0));
				for (size_t i = 0; i < generalBlocks.size(); i++)
				{
					const auto buf = fmt::format(
						L"Blocks {} time: {:0.2f} Jobs in block: ",
						i + 1,
						generalBlocks[i].time);
					GetClientRect(hWnd, &rect);
					rect.left = 700;
					rect.top = 100 + 20 * i;
					DrawText(hdc, buf.c_str(), -1, &rect, NULL);
					for (size_t a = 0; a < generalBlocks[i].num; a++)
					{
						TCHAR Buffer[10];
						wsprintf(Buffer, TEXT("%i"), generalBlocks[i].job[a].order);
						GetClientRect(hWnd, &rect);
						rect.left = 940 + 20 * a;
						rect.top = 100 + 20 * i;
						DrawText(hdc, Buffer, -1, &rect, NULL);
						// cout << blocks[cur_num].job[a].order << " ";
					}
				}
				break;
			}
		case 6:
			drawInitFile(hdc);
			break;
		case 7:
			drawFile(hdc);
			break;
		case 8:
			drawGeneralUnitInitFile(hdc);
			break;
		case 9:
			drawGeneralUnitFile(hdc);
			break;
		case 10:
			drawGeneralInitFile(hdc);
			break;
		case 11:
			drawGeneralFile(hdc);
			break;
		case 12:
		{
			cout << "Running time && Job num test" << endl;
			runTestTime();
			break;
		}
		case 13:
		{
			cout << "Fitness Convergence test" << endl;
			runTestFit();
			break;
		}
		}
		EndPaint(hWnd, &ps);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_KEYDOWN:
		switch (wParam)
		{
		case '1':
			input = 1;
			InvalidateRect(hWnd, NULL, NULL);
			UpdateWindow(hWnd);
			// RedrawWindow(hWnd, &rect, NULL, RDW_UPDATENOW | RDW_VALIDATE);
			break;
		case '2':
			input = 2;
			InvalidateRect(hWnd, NULL, NULL);
			UpdateWindow(hWnd);
			break;
		case '3':
			input = 3;
			InvalidateRect(hWnd, NULL, NULL);
			UpdateWindow(hWnd);
			break;
		case '4':
			input = 4;
			InvalidateRect(hWnd, NULL, NULL);
			UpdateWindow(hWnd);
			break;
		case '5':
			input = 5;
			InvalidateRect(hWnd, NULL, NULL);
			UpdateWindow(hWnd);
			break;
		case 'Q':
			input = 6;
			InvalidateRect(hWnd, NULL, NULL);
			UpdateWindow(hWnd);
			break;
		case 'A':
			input = 7;
			InvalidateRect(hWnd, NULL, NULL);
			UpdateWindow(hWnd);
			break;
		case 'W':
			input = 8;
			InvalidateRect(hWnd, NULL, NULL);
			UpdateWindow(hWnd);
			break;
		case 'S':
			input = 9;
			InvalidateRect(hWnd, NULL, NULL);
			UpdateWindow(hWnd);
			break;
		case 'E':
			input = 10;
			InvalidateRect(hWnd, NULL, NULL);
			UpdateWindow(hWnd);
			break;
		case 'D':
			input = 11;
			InvalidateRect(hWnd, NULL, NULL);
			UpdateWindow(hWnd);
			break;
		case 'T':
			input = 12;
			InvalidateRect(hWnd, NULL, NULL);
			UpdateWindow(hWnd);
			break;
		case 'G':
			input = 13;
			InvalidateRect(hWnd, NULL, NULL);
			UpdateWindow(hWnd);
			break;
		case '6':
			input = 14;
			InvalidateRect(hWnd, NULL, NULL);
			UpdateWindow(hWnd);
			break;
		}
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
} // WndProc
