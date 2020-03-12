;VIDEO.ASM
;Provides routines for video (VGA,CGA,etc) in a very fast way

[section .text]

global _vgaPlotPixel
global _plotPixel
global _plotVideoPixel
global _setPixel
global _setVideoPixel
global _setVideo
global _setVideoMode
global _setVideoPalette
global _setPalette

_videoAddress dw 0xA000

_vgaPlotPixel:
_plotPixel:
_plotVideoPixel:
_setPixel:
_setVideoPixel:
	push bp
	mov bp, sp
	push es
	
	mov ax, _videoAddress ;change to video address segment
	mov es, ax
	
	mov si, [bp+4] ;word Y
	mov di, si
	shl di, 8
	mov ax, si
	shl ax, 6
	add di, ax ;Y is shifted to left 6,8 and this gives us ((320*Y)+X)=P
	add di, [bp+6] ;word X
	
	mov al, [es:di] ;return byte COLOR
	xor ah, ah ;clear higher byte of ah to only leave al
	
	pop es
	pop bp
	ret
	
_vgaFetchPixel:
_vgaGetPixel:
_getPixel:
_getVideoPixel:
	push bp
	mov bp, sp
	push es
	
	mov ax, _videoAddress ;change to video address segment
	mov es, ax
	
	mov si, [bp+4] ;word Y
	mov di, si
	shl di, 8
	mov ax, si
	shl ax, 6
	add di, ax ;Y is shifted to left 6,8 and this gives us ((320*Y)+X)=P
	add di, [bp+6] ;word X
	
	mov [es:di], [bp+8] ;byte COLOR
	
	pop es
	pop bp
	ret

_setVideo:
_setVideoMode:
	push bp
	mov bp, sp
	
	mov al, [bp+4] ;byte MODE
	xor ah, ah
	int 10h
	
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
	
	mov bx, [bp+4] ;word N
	mov si, [bp+6] ;ptr RAW PALETTE ENTRIES
	;use CX as iterator
	xor cx, cx ;set to zero
	mov dx, 03C8h
	xor al, al ;output 0
	out dx, al
	mov dx, 03C9h
.forLoop:
	cmp cx, bx
	jge short .end
	
	lodsb
	shr al, 2
	out dx, al
	
	lodsb
	shr al, 2
	out dx, al
	
	lodsb
	shr al, 2
	out dx, al
	
	jmp short .forLoop
.end:
	pop bp
	ret
