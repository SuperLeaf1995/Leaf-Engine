;VIDEO.ASM
;Provides routines for video (VGA,CGA,etc) in a very fast way

[section .text]

global _plotPixel
global _plotVideoPixel
global _setPixel
global _setVideoPixel
global _setVideo
global _setVideoMode
global _setVideoPalette
global _setPalette

_videoAddress dw 0xA000

_plotPixel:
_plotVideoPixel:
_setPixel:
_setVideoPixel:
	push bp
	mov bp, sp
	push ax
	push es
	push di
	
	mov ax, _videoAddress
	mov es, ax
	mov di, [bp+8] ;WORD Y
	shl di, 8
	mov ax, [bp+8] ;WORD Y
	shl ax, 6
	add di, ax ;Y is shifted to left 6,8 and this gives us ((320*Y)+X)=P
	add di, [bp+10] ;BYTE X
	
	mov [es:di], [bp+12] ;BYTE COLOR
	
	pop di
	pop es
	pop ax
	pop bp
	ret

_setVideo:
_setVideoMode:
	push bp
	mov bp, sp
	push ax
	
	mov al, [bp+8] ;BYTE MODE
	xor ah, ah
	int 10h
	
	pop ax
	pop bp
	ret
	
_getVideo:
_getVideoMode:
	push bp
	mov bp, sp
	
	mov ah, 0x0F
	int 10h
	
	;C compiler expects result on AX
	xor ah, ah ;clear high part of AX to leave AL
	pop bp
	ret
	
_setPalette:
_setVideoPalette:
	push bp
	mov bp, sp
	push ax
	
	mov bx, [bp+8] ;WORD N
	mov si, [bp+10] ;PTR (WORD) RAW PALETTE ENTRIES
	;use CX as iterator
	xor cx, cx ;set to zero
	mov dx, 03C8h
	xor al, al ;output 0
	out dx, al
	mov dx, 03C9h
.forLoop:
	cmp cx, bx
	jge .end
	
	lodsb
	shr al, 2
	out dx, al
	
	lodsb
	shr al, 2
	out dx, al
	
	lodsb
	shr al, 2
	out dx, al
	
	jmp .forLoop
.end:
	pop ax
	pop bp
	ret
