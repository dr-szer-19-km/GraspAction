#include "parser.hpp"
#include "env.hpp"

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>

parser::parser(env& e) : en(e) {}

#define RED(ss, tp) \
int tp; \
ss>>a;

bool parser::parseOpts(std::string params)
{
	std::istringstream buf(params);
	std::string i;

	while(buf >> i)
	{
		std::string optsym = i.substr(0,2);

		if(optsym == "-v"){
			std::cout<<"Verbose output requested. Note that it will drastically prolong execution time.\n";
			verbose = true;
		}else if(optsym == "-b"){
			limT = std::stoi(i.substr(2));
			std::cout<<"Set maximum allowed tasks to "<<limT<<" \n";
		}else if(optsym == "-f"){
			fileToOpen = i.substr(2);
			std::cout<<"Will read file: "<<fileToOpen<<"\n";
        }else if(optsym == "-t"){
			maxTime = std::stoi(i.substr(2));
			std::cout<<"Algorithm run time in seconds: "<<maxTime<<"\n";
		}else if(optsym == "-o"){
			fileToWrite = i.substr(2);
			std::cout<<"Will write to file: "<<fileToOpen<<"\n";
		}else if(optsym == "-h" || optsym == "h" || optsym == "/?"){
			std::cout<<"Available switches:\n";
			std::cout<<std::setw(20)<<"-h"<<std::setw(50)<<"opens this help\n";
			std::cout<<std::setw(20)<<"-f[FILENAME]"<<std::setw(50)<<"opens file to read (input)\n";
			std::cout<<std::setw(20)<<"-o[FILENAME]"<<std::setw(50)<<"opens file to write (output)\n";
			std::cout<<std::setw(20)<<"-v"<<std::setw(50)<<"verbose output\n";
			std::cout<<std::setw(20)<<"-b[LIMIT]"<<std::setw(50)<<"set upper task limit to parse\n";
			std::cout<<std::setw(20)<<"-t[LIMIT]"<<std::setw(50)<<"set max runtime of algorithm\n";
			std::cout<<"\nSample command: szereg.exe -b1000 -fSandia-Ross-cln.swf\n";
			return false;
		}else{
			std::cout<<"Unrecognized option. Skipping :"<<i<<"\n";
		}
	}
	return true;
}

bool parser::parse(const char* filename /* =fileToOpen */)
{

	std::ifstream reader(filename);
	if(!reader){
		std::cerr<<"Error opening file."<<std::endl;
		return false;
	}

	if(maxTime<5)
        en.maxTime = maxTime;
    else
    {
        en.maxTime = 5;
        std::cout<<"Set time is too short, algorithm runtime is now set to 5 seconds."<<std::endl;
    }


	for( std::string line; getline( reader, line ); )
	{
		if(line.size() == 0)continue;
		if(line == "\r")continue;
		if(line.at(0) == ';'){
			line.erase(line.begin());
			std::istringstream iss(line);

			std::string opt;
			iss>>opt;

			if(opt == "MaxNodes:" || opt == "MaxProcs:"){
				RED(iss,a);
				en.numCores = std::max(en.numCores, a);
				std::cout<<"Preparing "<<en.numCores<<" cores"<<std::endl;
			}
			if(opt == "MaxRecords:" || opt == "MaxJobs:"){
				RED(iss,a);
				en.numJobs = std::max(en.numJobs, a);
				std::cout<<"Expecting "<<en.numJobs<<" jobs"<<std::endl;
				if(en.numJobs > limT && limT != 0){
					en.numJobs = limT;
					std::cout<<"Warning, truncated job amount to limT value, more jobs will be ignored"<<std::endl;
				}
			}

		}
		else{
			int jnum, ncores, /*tarr, texc,*/ trash;
			time_task_t texc, tarr;
			std::istringstream iss(line);

			iss>>jnum>>tarr>>trash>>texc>>ncores;

			if(getVerbose())std::cout<<"Parsing: "<<jnum<<"\n";

			switch(en.tryAddJob(std::make_unique<task>(jnum, ncores, tarr, texc)))
			{
				case 1:
					if(getVerbose())std::cout<<"Failed to add job (malformed entry), id: "<<jnum<<"\n";
					break;
				case 2:
					if(getVerbose())std::cout<<"Failed to add job (job queue full), id: "<<jnum<<"\n";
					break;
				case 3:
					if(getVerbose())std::cout<<"Failed to add job (job requires more cores than max declared), id: "<<jnum<<"\n";
					break;
				case 4:
					if(getVerbose())std::cout<<"Failed to add job (job has zero or less exec time), id: "<<jnum<<"\n";
					break;
				case 5:
					if(getVerbose())std::cout<<"Failed to add job (job requires zero or less cores), id: "<<jnum<<"\n";
					break;
			}

		}
	}
	std::cout<<"Parser is done."<<std::endl;
	reader.close();
	return true;
}
