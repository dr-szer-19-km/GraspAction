#include "algo.hpp"
#include "env.hpp"
#include <algorithm>
#include <fstream>
#include <set>
#include <cstdlib>
#include <iostream>
#include <ctime>

/*
struct row
{
    int timeExec_set;   // czas zakonczenia pracy wybranych procesorow
    std::set<int> procSet; // numery procesorow z tym samym czasem zakonczenia pracy
};
*/

algo::algo(env& e) : en(e){}
algo1::algo1(env& e) : algo(e){
	coreFreedomTime.resize(e.numCores);
	std::fill(coreFreedomTime.begin(), coreFreedomTime.end(), 0);
}

int algo1::local_search(int which, int ind1, int ind2, std::ofstream &out, bool flagaOut)
{
    greedy(which);
    if (ind1+ind2)        // jesli a=0 i b=0 to swap sie nie wykona
        std::swap(jobList[ind1], jobList[ind2]);
    int result = generateSolution(out,flagaOut);
    if (ind1+ind2)        // powrot do pierwotnej kolejnosci
        std::swap(jobList[ind1], jobList[ind2]);
    return result;
}

void algo1::greedy(int which)
{
    /*
    case 0: sortowanie wg: rosnacych timeArrival -> rosnacych numCores
    case 1: sortowanie wg: rosnacych timeArrival -> rosnacych numCores -> rosnacych timeExec
    case 2: sortowanie wg: rosnacych timeArrival -> malejacych numCores -> malejacych timeExec
    */

    switch(which)
    {
    case 0:
        std::sort(jobList.begin(), jobList.end(), [](const std::unique_ptr<task>& t1, const std::unique_ptr<task>& t2) -> bool {
			return ((t1->timeArrival ^ t2->timeArrival) ? t1->timeArrival < t2->timeArrival : t1->numCores < t2-> numCores); //bitowe mogą być szybsze
            });
        break;
    case 1:
        std::sort(jobList.begin(), jobList.end(), [](const std::unique_ptr<task>& t1, const std::unique_ptr<task>& t2) -> bool {
            return ((t1->timeArrival ^ t2->timeArrival) ? t1->timeArrival < t2->timeArrival :
                   ((t1->numCores ^ t2-> numCores) ? t1->numCores < t2->numCores : t1->timeExec < t2-> timeExec)) ;
            });
        break;
    case 2:
        std::sort(jobList.begin(), jobList.end(), [](const std::unique_ptr<task>& t1, const std::unique_ptr<task>& t2) -> bool {
            return ((t1->timeArrival ^ t2->timeArrival) ? t1->timeArrival < t2->timeArrival :
                   ((t1->numCores ^ t2-> numCores) ? t1->numCores > t2->numCores : t1->timeExec > t2-> timeExec)) ;
            });
        break;
    }
}

int algo1::calcFreeCores(int when)const
{
	int ret = 0;
	for(const auto & i : coreFreedomTime)
	{
		if(i <= when)ret++;
	}
	return ret;
}

int algo1::generateSolution(std::ofstream &out, bool flagaOut)
{
    //std::cout<<"Sorting done.\n Please wait until solving is done.\n"<<std::endl;

    //if(getVerbose())en.listJobsByTimeExec();

    /*
    std::vector<row> proc_num_array; // wektor roznych czasow zakonczenia pracy procesorow
    row element;
    element.timeExec_set=0;
    for (int j=0; j<e.numCores; j++)
        element.procSet.insert(j);
    proc_num_array.push_back(element);  //wstawianie pierwszego zbioru o nazwie 0 zawierajacego wszystkie cory

    int newExecTime;

	std::vector<std::unique_ptr<task> >::iterator it;
    for(it=jobList.begin(); it<=jobList.end(); it++)
	{
		auto &i = *it;




	}






    while(jobList.size() != 0)
    {
        auto &i = jobList.front();
        for( const auto &j : proc_num_array)
            if(j->timeExec_set <= i->timeArrival && j->procSet.size() >= i->numCores)   // dodawanie rozwiazania
            {
                newExecTime = j->timeExec_set + i->timeExec;
                element.timeExec_set = newExecTime;
                for(const auto &k : proc_num_array)
                    if(k->timeExec_set == newExecTime)
                    {

                    }

                break;
            }
	}

*/


    std::fill(coreFreedomTime.begin(), coreFreedomTime.end(), 0);
    lastJobExecTime = 0;
    std::vector<std::unique_ptr<task> >::iterator it;
    for(it=jobList.begin(); it!=jobList.end(); it++)
    {
        auto &i = *it;
        std::set<time_task_t> timez;
        for( const auto &j : coreFreedomTime) //różne możliwe czasy zwolnień procesorów
        {
            if(j >= i->timeArrival)timez.insert(j); //nie rozpatrujemy czasu przed przybyciem zadania
            else timez.insert(i->timeArrival);
        }
        while(timez.size() != 0){ //czy w danym czasie jest dość wolnych proców. można to ulepszyć
            auto act = timez.begin();
            if(calcFreeCores(*act) >= i->numCores) //jest
            {
                if(flagaOut)
                    out<<i->id<<" "<<*act<<" "<<(i->timeExec + *act)<<" ";
                if(int(i->timeExec + *act) > lastJobExecTime)
                    lastJobExecTime = int(i->timeExec + *act);
                int req = i->numCores;
                for(int j = 0; j < coreFreedomTime.size(); j++) //nowe czasy zakończenia dla proców
                {
                    if(coreFreedomTime[j] <= *act){
                        req--;
                        coreFreedomTime[j] = *act + i->timeExec;
                        if(flagaOut)
                            out<<j<<" ";
                    }
                    if(!req)break;
                }
                if(flagaOut)
                    out<<"\n";
                break;
            }
            timez.erase(timez.begin()); //nie ma
            //std::cout << " _" << *act << "_ ";
        }
        if(timez.size() == 0) //nie udało się uszeregować.
        {
            std::cerr<<"Algo error! (Prosimy Pana prof. Drozdowskiego o łagodny wymiar kary.)"<<std::endl; //zawsze powinno się udać przy obecnej konstrukcji.
            for(const auto &z : timez)std::cerr<<z<<",";
            abort();
        }
        //*act = 0;
    }

    //std::cout << " _" << *act << "_ ";
    int Solution = lastJobExecTime;
    lastJobExecTime = 0;
    return Solution;
}

void algo1::generateSuperSolution()
{
	std::cout<<"Sorting for greedy...\n" << std::flush;
    std::ofstream out(this->outp);
	int newSolution;
	int BestSolution=0;
	auto & jobList = en.getTaskList();
    unsigned AlgTime = en.maxTime*0.9;
    //unsigned AlgTime = 15;
    int Which=0, X=0, Y=0;
    bool flagaOut = false;
    srand (time(NULL));
	for (int which=0; which<3; which++)
    {
        greedy(which);
        newSolution = generateSolution(out,flagaOut);
        std::cout << "." << newSolution << "<-- greedy\n"<<std::flush;
        if (!which || (which && newSolution < BestSolution))
        {
            BestSolution = newSolution;
            std::cout << "." << newSolution << "<-- greedy (Best)\n"<<std::flush;
            Which = which;
            X = 0;
            Y = 0;
        }
        std::vector<int> vector_i;
        for(int i=0; i<jobList.size(); i++)
            vector_i.push_back(i);
        std::vector<int>::iterator index_it = vector_i.begin();

        for (int i = rand() % (jobList.size()); vector_i.size()-1 && (double(clock())/double(CLOCKS_PER_SEC) < double(AlgTime/3*(which+1))); i = rand() % (jobList.size()-1))
        {
            index_it = std::find(vector_i.begin(), vector_i.end(), i);
            if(index_it != vector_i.end())
            {
                for(int j=0; j<jobList.size() && (double(clock())/double(CLOCKS_PER_SEC) < double(AlgTime/3*(which+1))); j++)
                {
                    if (i!=j)
                    {
                        newSolution = local_search(which,i,j,out,flagaOut);// poszukiwanie dwoch indeksow do zamienienia; local search algo time
                        std::cout << " " << newSolution << "<-- local search dla " << i << "," << j <<std::endl;
                    }
                    if(newSolution < BestSolution)
                    {
                        std::cout << newSolution << "<-- local search (Best)\n" <<std::flush;
                        BestSolution = newSolution;
                        Which = which;
                        X = i;
                        Y = j;
                    }
                }
                vector_i.erase(index_it);
            }
        }
    }
    flagaOut = true;
    BestSolution = local_search(Which,X,Y,out,flagaOut); // wpisywanie najlepszego rozwiazania do outputu
    out.close();
    std::cout<<"\nBestSolution: "<<BestSolution<<std::endl;
	std::cout<<"Algo done"<<std::endl;
}
