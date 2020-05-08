.section .bss
.align 16
stack_bottom:
.skip 16384
stack_top:

.section .text
.global _entry
_entry:
	bl start
1:	hlt
	b 1b
