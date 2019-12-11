#ifndef _PARSER_HPP_
#define _PARSER_HPP_

#include <string>

class env;

class parser{
	protected:
	env& en;

	bool verbose{false};
	unsigned limT{0};

    //std::string fileToOpen{"Sandia-Ross-2001-1.1-cln.swf"};
	//std::string fileToOpen{"2Sandia-Ross-2001-1.1-cln.swf"};
	//std::string fileToOpen{"CIEMAT-Euler-2008-1.swf"};
	//std::string fileToOpen{"DAS2-fs0-2003-1.swf"};
	//std::string fileToOpen{"LANL-CM5-1994-4.1-cln.swf"};
	//std::string fileToOpen{"SDSC-SP2-1998-4.2-cln.swf"};
	//std::string fileToOpen{"test.swf"};
	//std::string fileToOpen{"test1.swf"};
	//std::string fileToOpen{"test2.swf"};
	std::string fileToOpen{"test3.swf"};


	std::string fileToWrite{"output.txt"};


	public:
    unsigned maxTime{0};

	parser(env&);

	bool parseOpts(std::string params);

	bool parse(const char* cc);
	inline bool parse()
	{
		return this->parse(fileToOpen.c_str());
	}

	inline bool getVerbose()const {return verbose;}
	inline std::string getOutputFileName()const {return fileToWrite;}
};

#endif
