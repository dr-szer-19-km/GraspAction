#ifndef _ALGO_HPP_
#define _ALGO_HPP_

#include "env.hpp"
#include <vector>
#include <string>


class algo{
protected:
    env& en;

    bool verbose{false};
    std::string outp{"output.txt"};

public:
    algo(env &);
    virtual int generateSolution(std::ofstream &out, bool flagaOut) = 0;
    inline bool getVerbose()const {return verbose;}
};

class algo1 : public algo{
	std::vector <time_task_t> coreFreedomTime;

	int calcFreeCores(int when) const;
	void greedy(int which);
	int local_search(int which, int ind1, int ind2, std::ofstream &out, bool flagaOut);
	int lastJobExecTime = 0;

public:
    algo1(env & e, bool v, std::string o) : algo1(e)
    {
        verbose = v;
        outp = o;
    }
    algo1(env & e);
    std::vector<std::unique_ptr<task> > &jobList = en.getTaskList();
    int generateSolution(std::ofstream &out, bool flagaOut) override;
    void generateSuperSolution();
};

#endif
