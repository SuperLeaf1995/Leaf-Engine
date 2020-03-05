[section .text]

_videoAddress dw 0xA000

global _plotPixel
_plotPixel:
	push ebp
	mov ebp, esp
	push esi
	push ebx
	
	mov edx, [ebp+8]
	mov eax, [ebp+12]
	mov ecx, [ebp+16]
	mov ebx, _videoAddress
	xor esi, esi
	mov si, ax
	shl esi, 6
	and eax, 65535
	shl eax, 8
	add esi, eax
	xor eax, eax
	mov ax, dx
	add eax, esi
	add ebx, eax
	mov [eax], cl
	
	pop ebx
	pop esi
	pop ebp
	ret

global _setVideo:
_setVideo:
	push ebp
	mov ebp, esp
	push eax
	
	mov al, [ebp+8]
	xor ah, ah
	int 10h
	
	pop eax
	pop ebp
	ret

global _soundStart
_soundStart:
	push ebp
	mov ebp, esp
	sub esp, 24
	mov eax, 1193180
	xor edx, edx
	div [ebp+8]
	mov [ebp-12], eax
	sub esp, 8
	push 128
	push 67
	call _outportb
	add esp, 16
	mov eax, [ebp-12]
	and eax, 255
	sub esp, 8
	push eax
	push 66
	call _outportb
	add esp, 16
	mov eax, [ebp-12]
	and eax, 255
	sar eax, 8
	and eax, 255
	sub esp, 8
	push eax
	push 66
	call _outportb
	add esp, 16
	sub esp, 12
	push 97
	call _inportb
	add esp, 16
	mov [ebp-13], al
	mov al, [ebp-13]
	or eax, 3
	cmp [ebp-13], al
	je .skipExtraBytes
	mov al, [ebp-13]
	or eax, 3
	and eax, 255
	sub esp, 8
	push eax
	push 97
	call _outportb
	add esp, 16
.skipExtraBytes:
	ret

global _soundStop
_soundStop:
	push ebp
	mov ebp, esp
	sub esp, 24
	sub esp, 12
	push 97
	call _inportb
	add esp, 16
	and eax, -4
	mov [ebp-9], al
	mov al, [ebp-9]
	and 255, eax
	sub esp, 8
	push eax
	push 97
	call _outportb
	add esp, 16
	ret
