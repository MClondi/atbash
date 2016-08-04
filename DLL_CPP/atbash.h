#ifndef ATBASH_H
#define ATBASH_H

#define DECLDIR __declspec(dllexport)

extern "C"
{
	DECLDIR char * __stdcall Cipher(char* message, char* alphabet, char* result);
}

#endif