#include "MainModel.h"
#define cpp_library "DLL_CPP.dll"
#define asm_library "DLL_ASM.dll"

using namespace std;

typedef char* (__stdcall *CIPHER_PROC)(char *, char *, char *);

CIPHER_PROC cipher;
char ** to_cipher;
char ** result;

MainModel::MainModel(){
	this->useCpp = true;							// initial using C++ library
	this->alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";  // the alphabet used in the program
	this->file = L"";								// user has to choose a path
}

void MainModel::setThreads(int threads){
	this->threads = threads;
}

void MainModel::useAsmDll(){
	this->useCpp = false;
}

void MainModel::useCppDll(){
	this->useCpp = true;
}

void MainModel::setFile(LPWSTR file){
	this->file = file;
}

LPWSTR MainModel::getFile(){
	return file;
}

void MainModel::threadRun(vector<int> ids, int procID){
	for (int i = 0; i < ids.size(); i++){
		int indicator = ids.at(i);
		result[indicator] = cipher(to_cipher[indicator], alphabet, result[indicator]);
	}
}

long long MainModel::start() {

	fstream ciphered_file;
	list<string> lines;
	long long resultTime = 0;
	ciphered_file.open(file, fstream::in);
	int size;

	HINSTANCE hDll;
	hDll = LoadLibraryA(useCpp ? cpp_library : asm_library);	//loading choosen library
	if (hDll != NULL)
	{
		cipher = (CIPHER_PROC)GetProcAddress(hDll, "Cipher");	//loading the library function
		if (cipher != NULL)
		{
			//memory allocation sector
			try{
				while (!ciphered_file.eof())
				{
					string line;
					getline(ciphered_file, line);

					for (int i = 0; i < line.length(); i++)
					{
						switch (line[i])
						{
						case '¹': line[i] = 'a'; break;
						case '¥': line[i] = 'A'; break;
						case 'æ': line[i] = 'c'; break;
						case 'Æ': line[i] = 'C'; break;
						case 'ê': line[i] = 'e'; break;
						case 'Ê': line[i] = 'E'; break;
						case '³': line[i] = 'l'; break;
						case '£': line[i] = 'L'; break;
						case 'ñ': line[i] = 'n'; break;
						case 'Ñ': line[i] = 'N'; break;
						case 'ó': line[i] = 'o'; break;
						case 'Ó': line[i] = 'O'; break;
						case 'œ': line[i] = 's'; break;
						case 'Œ': line[i] = 'S'; break;
						case 'Ÿ': line[i] = 'z'; break;
						case '': line[i] = 'Z'; break;
						case '¿': line[i] = 'z'; break;
						case '¯': line[i] = 'Z'; break;
						default: break;
						}
					}
					lines.push_back(line);
				};

				size = lines.size();
				to_cipher = new char*[size];
				result = new char*[size];
				for (int i = 0; i < size; i++)
				{
					string line = lines.front();
					to_cipher[i] = new char[line.size() + 1];
					copy(line.begin(), line.end(), to_cipher[i]);
					to_cipher[i][line.size()] = 0;
					result[i] = new char[line.size() + 1];
					lines.pop_front();
				}

				if (size < threads)
					threads = size;
			}
			catch (bad_alloc& ba)
			{
				MessageBox(NULL, "Out of memory, please use a smaller file or free memory", "Error", MB_ICONERROR | MB_OK);
			}

			ciphered_file.close();
			// clearing the file before calculations
			ciphered_file.open(file, fstream::out | fstream::trunc);

			vector<thread *> thread_vec;
			vector<vector<int>> ids_vec;

			int remainder = size % threads;						//calculations to divide the lines equal between the thread
			size -= remainder;
			int division = (int)size / threads;
			int counter = 0;
			size += remainder;

			auto start_time = chrono::high_resolution_clock::now();	//starting to count the time

			for (int i = 0; i < threads; i++)						//preparation for thread runing
			{
				vector<int> ids;
				for (int j = 0; j < division; j++)
				{
					ids.push_back(counter);
					counter++;
				}
				if (remainder > 0)
				{
					ids.push_back(counter);
					counter++;
					remainder--;
				}
				ids_vec.push_back(ids);
			}

			for (int i = 0; i < threads; i++)						//creating threads
				thread_vec.push_back(new thread(&MainModel::threadRun, this, ids_vec.at(i), i));

			for (int i = 0; i < thread_vec.size(); i++)				//waiting to finish all tasks
				thread_vec.at(i)->join();

			auto stop_time = chrono::high_resolution_clock::now();	//stoping the time
			resultTime = chrono::duration_cast<chrono::milliseconds>(stop_time - start_time).count();

			for (int i = 0; i < size; i++)		//saving the ciphered lines
			{
				ciphered_file << result[i];
				if (i < size - 1)
					ciphered_file << '\n';
			}
			ciphered_file.close();

			//memory cleanup
			for (auto i : thread_vec)
				delete i;

			for (int i = 0; i < size; i++){
				delete to_cipher[i];
				delete result[i];
			}

			delete to_cipher;
			delete result;

		}
		FreeLibrary(hDll);
	}
	else
	{
		MessageBox(NULL, "DLL not found", "Error", MB_ICONERROR | MB_OK);
	}
	return resultTime;		//returns the time in miliseconds
}