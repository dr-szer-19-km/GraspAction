#ifndef _ENV_HPP_
#define _ENV_HPP_

#ifdef _GNUG_
#define always_inline inline __attribute__((always_inline))
#else
#define always_inline inline
#endif

#include <vector>
#include <memory>

using time_task_t = int;

struct task{
	int id;
	int numCores;
	time_task_t timeArrival;
	time_task_t timeExec;

	task() = default;
	task(int i, int nc, time_task_t ta, time_task_t te) :
		id(i), numCores(nc), timeArrival(ta), timeExec(te) {}
};

class base_env{

public:
	int numJobs{0};
	int numCores{0};
	int maxTime{0};

	virtual int tryAddJob(std::unique_ptr<task>) = 0;

};

class env : public base_env{
	std::vector<std::unique_ptr<task> > tasks;

public:
	env();
	int tryAddJob(std::unique_ptr<task>) override;

	void listJobsByTimeExec();
	auto & getTaskList() { return tasks; }
};

#endif
