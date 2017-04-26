; naskfunc
; TAB=4

[FORMAT "WCOFF"]				; オブジェクトファイルを作るモード	
[INSTRSET "i486p"]				; 486の命令まで使いたいという記述
[BITS 32]						; 32ビットモード用の機械語を作らせる
[FILE "naskfunc.nas"]			; ソースファイル名情報

		GLOBAL	_io_hlt, _io_cli, _io_sti, _io_stihlt
		GLOBAL	_io_in8,  _io_in16,  _io_in32
		GLOBAL	_io_out8, _io_out16, _io_out32
		GLOBAL	_io_load_eflags, _io_store_eflags
		GLOBAL	_load_gdtr, _load_idtr
		GLOBAL	_load_cr0, _store_cr0
		GLOBAL	_memtest, _asm_inthandler20
		GLOBAL	_asm_inthandler21, _asm_inthandler27, _asm_inthandler2c
		EXTERN	_inthandler20, _inthandler21, _inthandler27, _inthandler2c

[SECTION .text]

_io_hlt:	; void io_hlt(void);
		HLT
		RET

_io_cli:	; void io_cli(void);
		CLI
		RET

_io_sti:	; void io_sti(void);
		STI
		RET

_io_stihlt:	; void io_stihlt(void);
		STI
		HLT
		RET

_io_in8:	; int io_in8(int port);
		MOV		EDX,[ESP+4]		; port
		MOV		EAX,0
		IN		AL,DX
		RET

_io_in16:	; int io_in16(int port);
		MOV		EDX,[ESP+4]		; port
		MOV		EAX,0
		IN		AX,DX
		RET

_io_in32:	; int io_in32(int port);
		MOV		EDX,[ESP+4]		; port
		IN		EAX,DX
		RET

_io_out8:	; void io_out8(int port, int data);
		MOV		EDX,[ESP+4]		; port
		MOV		AL,[ESP+8]		; data
		OUT		DX,AL
		RET

_io_out16:	; void io_out16(int port, int data);
		MOV		EDX,[ESP+4]		; port
		MOV		EAX,[ESP+8]		; data
		OUT		DX,AX
		RET

_io_out32:	; void io_out32(int port, int data);
		MOV		EDX,[ESP+4]		; port
		MOV		EAX,[ESP+8]		; data
		OUT		DX,EAX
		RET

_io_load_eflags:	; int io_load_eflags(void);
		PUSHFD		; 将当前寄存器的值压入栈
		POP		EAX
		RET

_io_store_eflags:	; void io_store_eflags(int eflags);
		MOV		EAX,[ESP+4]
		PUSH	EAX
		POPFD		; 从栈顶弹出寄存器的值
		RET

_load_gdtr:		; void load_gdtr(int limit, int addr);
		MOV		AX,[ESP+4]		; limit
		MOV		[ESP+6],AX
		LGDT	[ESP+6]
		RET

_load_idtr:		; void load_idtr(int limit, int addr);
		MOV		AX,[ESP+4]		; limit
		MOV		[ESP+6],AX
		LIDT	[ESP+6]
		RET

_load_cr0:		; int load_cr0(void);
		MOV		EAX,CR0
		RET

_store_cr0:		; void store_cr0(int cr0);
		MOV		EAX,[ESP+4]
		MOV		CR0,EAX
		RET

; 改进：vmware要求内存必须是4MB的倍数，所以直接以4MB为单位检查内存
_memtest:
	PUSH	EBP
	MOV	EBP,ESP
	SUB	ESP,24
	CALL	_load_cr0
	MOV	DWORD [-4+EBP],EAX
	LEA	EAX,DWORD [-4+EBP]
	OR	DWORD [EAX],1610612736
	SUB	ESP,12
	PUSH	DWORD [-4+EBP]
	CALL	_store_cr0
	ADD	ESP,16
	MOV	DWORD [-20+EBP],1721329305
	MOV	DWORD [-24+EBP],-1721329306
	MOV	EAX,DWORD [8+EBP]
	MOV	DWORD [-8+EBP],EAX
L2:
	MOV	EAX,DWORD [-8+EBP]
	CMP	EAX,DWORD [12+EBP]
	JBE	L5
	JMP	L3
L5:
	MOV	EAX,DWORD [-8+EBP]
	ADD	EAX,4194300
	MOV	DWORD [-12+EBP],EAX
	MOV	EAX,DWORD [-12+EBP]
	MOV	EAX,DWORD [EAX]
	MOV	DWORD [-16+EBP],EAX
	MOV	EDX,DWORD [-12+EBP]
	MOV	EAX,DWORD [-20+EBP]
	MOV	DWORD [EDX],EAX
	MOV	EDX,DWORD [-12+EBP]
	MOV	EAX,DWORD [-12+EBP]
	MOV	EAX,DWORD [EAX]
	NOT	EAX
	MOV	DWORD [EDX],EAX
	MOV	EAX,DWORD [-12+EBP]
	MOV	EAX,DWORD [EAX]
	CMP	EAX,DWORD [-24+EBP]
	JE	L6
L7:
	MOV	EDX,DWORD [-12+EBP]
	MOV	EAX,DWORD [-16+EBP]
	MOV	DWORD [EDX],EAX
	JMP	L3
L6:
	MOV	EDX,DWORD [-12+EBP]
	MOV	EAX,DWORD [-12+EBP]
	MOV	EAX,DWORD [EAX]
	NOT	EAX
	MOV	DWORD [EDX],EAX
	MOV	EAX,DWORD [-12+EBP]
	MOV	EAX,DWORD [EAX]
	CMP	EAX,DWORD [-20+EBP]
	JE	L8
	JMP	L7
L8:
	MOV	EDX,DWORD [-12+EBP]
	MOV	EAX,DWORD [-16+EBP]
	MOV	DWORD [EDX],EAX
	LEA	EAX,DWORD [-8+EBP]
	ADD	DWORD [EAX],4194304
	JMP	L2
L3:
	CALL	_load_cr0
	MOV	DWORD [-4+EBP],EAX
	LEA	EAX,DWORD [-4+EBP]
	AND	DWORD [EAX],-1610612737
	SUB	ESP,12
	PUSH	DWORD [-4+EBP]
	CALL	_store_cr0
	ADD	ESP,16
	MOV	EAX,DWORD [-8+EBP]
	LEAVE
	RET

_asm_inthandler20:
		PUSH	ES
		PUSH	DS
		PUSHAD
		MOV		EAX,ESP
		PUSH	EAX
		MOV		AX,SS
		MOV		DS,AX
		MOV		ES,AX
		CALL	_inthandler20
		POP		EAX
		POPAD
		POP		DS
		POP		ES
		IRETD

_asm_inthandler21:
		PUSH	ES
		PUSH	DS
		PUSHAD
		MOV		EAX,ESP
		PUSH	EAX
		MOV		AX,SS
		MOV		DS,AX
		MOV		ES,AX
		CALL	_inthandler21
		POP		EAX
		POPAD
		POP		DS
		POP		ES
		IRETD

_asm_inthandler27:
		PUSH	ES
		PUSH	DS
		PUSHAD
		MOV		EAX,ESP
		PUSH	EAX
		MOV		AX,SS
		MOV		DS,AX
		MOV		ES,AX
		CALL	_inthandler27
		POP		EAX
		POPAD
		POP		DS
		POP		ES
		IRETD

_asm_inthandler2c:
		PUSH	ES
		PUSH	DS
		PUSHAD
		MOV		EAX,ESP
		PUSH	EAX
		MOV		AX,SS
		MOV		DS,AX
		MOV		ES,AX
		CALL	_inthandler2c
		POP		EAX
		POPAD
		POP		DS
		POP		ES
		IRETD

