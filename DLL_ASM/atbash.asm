;Author: Marcel Janotta
.386					
.model flat, stdcall	; flat memory model, stdcall convention
.code					; code fragment below

; Macro used to calculate the length of a string has two parameters:
; line - pointer to the begining of the string 
; len_data - pointer to a DWORD where the result will be saved

MsgLen MACRO line, len_data 
		mov edi, line				; moving the string to EDI register
		sub	ecx, ecx				; initializing ECX to 0
		not	ecx						; getting the biggest possible value for ECX
		sub	al, al					; initializing AL to 0
		cld							; cleaing direction flag
repne	scasb						; scanning the string in EDI, decrementing ECX every char
									; now EDI = - (length of string) - 2
									; the -2 comes from the initial value and for counting the string end char (0)
		neg	ecx						; EDI = (length of string) + 2
		dec	ecx						; EDI = (length of string) + 1
		dec	ecx						; EDI = length of string
		mov DWORD PTR len_data, ecx	; moving the result to our pointer
ENDM


; Our actual function, cipheres a string with Atbash method
; uses - makes the function to push the registers on the stack on the begin and pop them when quitting. We write here all our used registers, except the ACX, which will return the pointer to out result
; message - pointer to the message we will cipher
; alphabet - pointer to the alphabet
; result - pointer to allocated memory where the result will be saved

Cipher proc uses ebx ecx edx esi edi, message:PTR DWORD, alphabet:PTR DWORD, result:PTR DWORD 

	; Locals used in the code:
	LOCAL result_ptr	: DWORD ; For saving the pointer to the begin of the result	 
	LOCAL msg_len		: DWORD	; Variable fot the length of the message
	LOCAL alphabet_len	: DWORD	; Variable fot the length of the alphabet
	LOCAL correction	: BYTE	; Correction when we have a small caps letter

	mov eax, result					; saving result pointer
	mov DWORD PTR result_ptr, eax	; we dont need this value in the registers, because it will be used one more time only

	MsgLen message, msg_len			; finding message length

	cmp DWORD PTR msg_len, 0		; comparing to check if the string is not empty
	jne continue					; if not we continue
	mov edx, result					; if yes we move the result pointer to EDX to write a 0 there
	jmp empty_string				; jumping to the end of the procedure
	
	continue:						; we found a string and continue
	MsgLen alphabet, alphabet_len	; finding alphabet length	
									; loading pointers to registers:
	mov ebx, message				; the message we want to cipher to EBX
	mov edx, result					; the pointer to allocated memory to EDX
	mov edi, alphabet				; the aplhabet we will use to EDI

	mov esi, 0						; EDI will be the indicator for a loop
	loop_through_message:			; we will loop through every char in the message
		mov ecx, 0						; ECX will be the indicator for an inner loop
		mov al, [ebx]					; taking a letter from the message
		mov [edx], al					; saving the char in case not finding it in alphabet, this makes ignoring unrecognized signs
		loop_through_alphabet:			; loop through the chars in the alphabet
			mov ah, [edi + ecx]				; next alphabet letters
			cmp al, ah						; comparision with our char from the message
			je equal						; the letters were equal
			add ah, 32						; if not, we check if it isnt a small letter
											; small letters are moved +32 places in the ASCII table
			cmp al, ah						; comparision
			jne not_equal					; this was not this letter, we finish the checking																		
											; we have a small letter
			mov BYTE PTR correction, 32		; the correction variable will make the ciphered letter small if its needed
			jmp avoid_correction_erase		; jump to ciphering
			equal:
			mov BYTE PTR correction, 0		; the letter was big, so no correction is needed

			avoid_correction_erase:			; here begins the actuacl ciphering of a char
			mov eax, edi					; moving the alphabet to EAX
			add eax, DWORD PTR alphabet_len	; going to the end of the alphabet
			dec eax							; correction because we overjumped 
			sub eax, ecx					; and we subtract the indicator of the alphabet
											; the EAX is pointing now to the ciphered letter
											; we got the opposite letter, for ex. if the ECX was 2, we get n-2, were n is the alphabet length
			mov cl, [eax]					; we load the char to a byte register
			add cl, BYTE PTR correction		; adding the correction if it was a small letter
			mov [edx], cl					; moving the ciphered char to our result pointer
			jmp match_found					; ending loop 

			not_equal:						; if there was no hit, we take next letter			
			inc ecx							; incresaing the alphabet iterator
			cmp ecx, DWORD PTR alphabet_len	; checking if its the end of alphabet
			jne loop_through_alphabet		; starting the loop again

		match_found:					; we found a match, went through the whole alphabet and no match was found
		inc ebx							; taking next letter to cipher
		inc edx							; increasing the iterator to the result
		inc esi							; increasing the iterator of the outer loop
		cmp esi, DWORD PTR msg_len		; checking if end of the line
		jne loop_through_message		; if not, another loop starts

	empty_string:					; if the passed string was empty we end here at once
	mov al, 0						; we need to move 0 to the last place in the result
	mov [edx], al					; so we will know when the line ends
	mov eax, DWORD PTR result_ptr	; moving the pointer of the begin to EAX
	ret								; returning EAX, pointer to the ciphered string
Cipher endp 
end