;MOUSE.ASM
;Provides routines for mouse stuff

[section .text]

global _setMouseStatus
global _setCursorStatus
global _setMousePosition
global _setCursorPosition

_setMouseStatus:
_setCursorStatus:
	push bp
	mov bp, sp
	
	mov ax, [bp+4] ;BYTE STATUS
	int 0x33
	
	pop bp
	ret

_setMousePosition:
_setCursorPosition:
	push bp
	mov bp, sp
	
	mov cx, [bp+4] ;WORD X
	mov dx, [bp+6] ;WORD Y
	mov ax, 4
	int 0x33
	
	pop bp
	ret
	
