; haribote-os
; TAB=4

; 相关BOOT_INFO
CYLS	EQU		0x0ff0
LEDS	EQU		0x0ff1
VMODE	EQU		0x0ff2
SCRNX	EQU		0x0ff4
SCRNY	EQU		0x0ff6
VRAM	EQU		0x0ff8

		ORG		0xc200			; 本文件被装载到内存0xc200处
		MOV		AL,0x13			; VGA显卡，320x200x8位彩色
		MOV		AH,0x00
		INT		0x10
		MOV		BYTE [VMODE],8	; 记录画面模式
		MOV		WORD [SCRNX],320
		MOV		WORD [SCRNY],240
		MOV		DWORD [VRAM],0x000a0000
		
; 用BIOS获取键盘灯状态
		MOV		AH,0x02
		INT		0x16			; 键盘BIOS
		MOV		[LEDS],AL

fin:
		HLT
		JMP		fin
