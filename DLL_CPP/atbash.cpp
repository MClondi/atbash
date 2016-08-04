//Author: Marcel Janotta
#include "atbash.h"
#include <string>
#include <cstring>
#include <windows.h>

#define DLL_EXPORT
#define ASCII_GAP 32


//Our actual function, cipheres a string with Atbash method
//message - pointer to the message we will cipher
//alphabet - pointer to the alphabet
//result - pointer to allocated memory where the result will be saved

extern "C"
{
	DECLDIR char * __stdcall Cipher(char * message, char * alphabet, char * result)
	{
		int reslut_indicator = 0;						// int value to watch the result
		for (int i = 0; i < strlen(message); i++)		// iterating through the message
		{
			bool match_found = FALSE;					// flag to check if a match was found
			for (int j = 0; j < strlen(alphabet); j++)	// iterating through the alphabet
			{
				if (message[i] == alphabet[j])			
				{
					result[reslut_indicator] = alphabet[strlen(alphabet) - j - 1]; // getting the char from the oposite end of the alphabet
					match_found = TRUE;
				}
				else if (message[i] == alphabet[j] + ASCII_GAP)						// checking if the char isn't a small letter
				{
					result[reslut_indicator] = alphabet[strlen(alphabet) - j - 1] + ASCII_GAP; // adding 32 to get the small ASCII letter
					match_found = TRUE;
				}
			}
			if (!match_found)
			{
				result[reslut_indicator] = message[i];  // if no match was found we save the not reconized char
			}
			reslut_indicator++;
		}
		result[reslut_indicator] = '\0';
		return result;
	}
}