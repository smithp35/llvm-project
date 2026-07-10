	.macro VECTOR_TABLE_ENTRY
	.balign 128
	b __cexception_handler
	.endm


	.globl _start
	.type _start, %function
	.globl __bootcode_start
	.type __bootcode_start, %function
	.section .text.init.enter, "ax", %progbits
_start:
	adrp x0, __stack
	add x0, x0, :lo12: __stack
	mov sp, x0
	b __bootcode_start


	.section .init.vector, "ax", %progbits
	.globl __cvector_table
	.type __cvector_table, %function
	.globl __cexception_handler
	.type __cexception_handler, %function
__cvector_table:
	VECTOR_TABLE_ENTRY
	VECTOR_TABLE_ENTRY
	VECTOR_TABLE_ENTRY
	VECTOR_TABLE_ENTRY
	VECTOR_TABLE_ENTRY
	VECTOR_TABLE_ENTRY
	VECTOR_TABLE_ENTRY
	VECTOR_TABLE_ENTRY
	VECTOR_TABLE_ENTRY
	VECTOR_TABLE_ENTRY
	VECTOR_TABLE_ENTRY
	VECTOR_TABLE_ENTRY
	VECTOR_TABLE_ENTRY
	VECTOR_TABLE_ENTRY
	VECTOR_TABLE_ENTRY
	VECTOR_TABLE_ENTRY
