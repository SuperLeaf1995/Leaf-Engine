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
	push ax
	
	mov ax, [bp+8] ;WORD STATUS
	int 0x33
	
	pop ax
	pop bp
	ret

_setMousePosition:
_setCursorPosition:
	push bp
	mov bp, sp
	push ax
	push dx
	
	mov cx, [bp+8] ;WORD X
	mov dx, [bp+10] ;WORD Y
	mov ax, 4
	int 0x33
	
	pop dx
	pop ax
	pop bp
	ret
	
