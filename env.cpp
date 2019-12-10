#include "env.hpp"

#include <memory>

env::env()
{
}

int env::tryAddJob(std::unique_ptr<task> t)
{
	if(t->timeArrival < 0|| t->id < 0)
		return 1;
	if(numJobs == tasks.size())return 2;
	if(t->numCores > numCores)return 3;
    if(t->timeExec <= 0)return 4;
	if(t->numCores <= 0)return 5;

	tasks.push_back(std::move(t));

	return 0;
}
#include <iostream>
void env::listJobsByTimeExec()
{
	std::cout<<"###SUMMARY###"<<std::endl;
	std::cout<<"We have "<<numJobs<<" jobs"<<std::endl;

	int jobz = 0;
	for(const auto& i : tasks)
	{
		std::cout<<"Time arrival: "<<i->timeArrival<<std::endl;
		std::cout<<"id:"<<i->id<<", numCores:"<<i->numCores<<", timeArrival:"<<i->timeArrival
			<<", timeExec:"<<i->timeExec<<std::endl;
			jobz++;
	}
	std::cout<<"Counted jobs:"<<jobz<<std::endl;


	std::cout<<"\n##############"<<std::endl;
}
