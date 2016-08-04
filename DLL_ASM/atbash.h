#include <Windows.h>
#ifndef ATBASH_H
#define ATBASH_H

#define DECLDIR __declspec(dllexport)

extern "C"
{
	DECLDIR int Cipher(DWORD x, DWORD y);
}

#endif