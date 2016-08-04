#include <iostream>
#include <windows.h> 
#include <stdio.h> 
#include <iostream>
#include <fstream>
#include <string>
#include <list>
#include <future>
#include <vector>

class MainModel
{
private:
	int threads;		//used threads
	bool useCpp;		//cpp/asm flag
	LPWSTR file;		//filepath
	char* alphabet;		//alphabet
	//private thread method
	void threadRun( std::vector<int> ids, int procID);
public:
	MainModel();
	void setThreads(int Threads);	//setter for threads
	void useCppDll();				//setter for C++ dll
	void useAsmDll();				//setter for asm dll
	void setFile(LPWSTR file);		//setter for filepath
	LPWSTR getFile();				//getter for filepath
	//main function of the class, return the time of the execution in miliseconds
	long long start();
};

