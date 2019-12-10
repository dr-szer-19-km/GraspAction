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

struct numPair
{
    int elem1;
    int elem2;
};

algo::algo(env& e) : en(e){}
algo1::algo1(env& e) : algo(e){
	coreFreedomTime.resize(e.numCores);
	std::fill(coreFreedomTime.begin(), coreFreedomTime.end(), 0);
}

int algo1::local_search(int which, int a, int b, bool flaga)
{
    greedy(which);
    if (a+b)        // jesli a=0 i b=0 to swap sie nie wykona
        std::swap(jobList[a], jobList[b]);
    return generateSolution(flaga);
}

void algo1::greedy(int which) const
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
    case 1:
        std::sort(jobList.begin(), jobList.end(), [](const std::unique_ptr<task>& t1, const std::unique_ptr<task>& t2) -> bool {
            return ((t1->timeArrival ^ t2->timeArrival) ? t1->timeArrival < t2->timeArrival :
                   ((t1->numCores ^ t2-> numCores) ? t1->numCores < t2->numCores : t1->timeExec < t2-> timeExec)) ;
            });
    case 2:
        std::sort(jobList.begin(), jobList.end(), [](const std::unique_ptr<task>& t1, const std::unique_ptr<task>& t2) -> bool {
            return ((t1->timeArrival ^ t2->timeArrival) ? t1->timeArrival < t2->timeArrival :
                   ((t1->numCores ^ t2-> numCores) ? t1->numCores > t2->numCores : t1->timeExec > t2-> timeExec)) ;
            });
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

int algo1::generateSolution(bool flaga)
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



        for( const auto &j : coreFreedomTime) //różne możliwe czasy zwolnień procesorów
        {
            if(j >= i->timeArrival)timez.insert(j); //nie rozpatrujemy czasu przed przybyciem zadania
            else timez.insert(i->timeArrival);
        }
        */
    std::ofstream out(this->outp);
    int lastJobExecTime{0};
    auto &i = jobList.front();
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
            //if(flaga)
                out<<i->id<<" "<<*act<<" "<<(i->timeExec + *act)<<" ";
            if(int(i->timeExec + *act) > lastJobExecTime)
                lastJobExecTime = int(i->timeExec + *act);
            int req = i->numCores;
            for(int j = 0; j < coreFreedomTime.size(); j++) //nowe czasy zakończenia dla proców
            {
                if(coreFreedomTime[j] <= *act){
                    req--;
                    coreFreedomTime[j] = *act + i->timeExec;
                    //if(flaga)
                        out<<j<<" ";
                }
                if(!req)break;
            }
            //if(flaga)
                out<<"\n";
            break;
        }
        timez.erase(timez.begin()); //nie ma
    }
    if(timez.size() == 0) //nie udało się uszeregować.
    {
        std::cerr<<"Algo error! (Prosimy Pana prof. Drozdowskiego o łagodny wymiar kary.)"<<std::endl; //zawsze powinno się udać przy obecnej konstrukcji.
        for(const auto &z : timez)std::cerr<<z<<",";
        abort();
    }
    jobList.erase(jobList.begin());

    int Solution = lastJobExecTime;
    //if (flaga)
    out.close();
    return Solution;
}

void algo1::generateSuperSolution()
{
	std::cout<<"Sorting for greedy...\n" << std::flush;

	int Solution;
	int newSolution;
	int BestSolution;
	int rand1, rand2;
    std::vector<numPair> pairVector;
    numPair elem;
    int size1;
    int size2;
    //unsigned AlgTime = en.maxTime*0.9;
    unsigned AlgTime = 15;
    int Which, X, Y;
    bool flaga = false;
    clock_t actTics = clock();
    double actTime = double(actTics)/CLOCKS_PER_SEC;
    int pom;
    int vectorSize;
    bool flagaElem = false;

	for (int which=0; which<3; which++)
    {
        greedy(which);
        Solution = generateSolution(flaga);
        if (!which || (which && Solution < BestSolution))
        {
            BestSolution = Solution;
            Which = which;
            X = 0;
            Y = 0;
        }
        while(double(clock())/double(CLOCKS_PER_SEC) < double(AlgTime/3*(which+1)))
        {                       // poszukiwanie dwoch indeksow do zamienienia; local search algo time
            vectorSize = pairVector.size();
            while(vectorSize == pairVector.size())
            {
                rand1 = rand() % jobList.size();
                rand2 = rand1;
                while(rand1 == rand2)
                    rand2 = rand() % jobList.size();
                if(rand1 > rand2)               // rand 1 zawsze bedzie mniejszy od rand 2 od teraz
                {
                    pom = rand1;
                    rand1 = rand2;
                    rand2 = pom;
                }
                elem.elem1 = rand1;
                elem.elem2 = rand2;
                for (const auto &j : pairVector)    // sprawdzanie czy dana para jest rozna od poprzednich
                  if(j.elem1 == elem.elem1)
                    if(j.elem2 == elem.elem2)
                    {
                        flagaElem = true;
                        break;
                    }
                if(!flagaElem)
                    pairVector.push_back(elem);     // jesli tak - insert
                flagaElem = false;
            }
            newSolution = local_search(which,rand1,rand2,flaga);
            if (newSolution < BestSolution)
            {
                BestSolution = newSolution;
                Which = which;
                X = rand1;
                Y = rand2;
                std::cout<<"A" << std::flush;
            }
        }
        pairVector.clear();
    }
    flaga = true;
    BestSolution = local_search(Which,X,Y,flaga); // wpisywanie najlepszego rozwiazania do outputu

	std::cout<<"Algo done"<<std::endl <<std::flush;;
}
