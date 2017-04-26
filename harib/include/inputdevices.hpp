#ifndef _INPUTDEVICES_HPP
#define _INPUTDEVICES_HPP

#ifdef __cplusplus
extern "C"{
#endif

//键盘鼠标部分
#define PORT_KEYDAT		0x0060
#define PORT_KEYSTA		0x0064
#define PORT_KEYCMD		0x0064
#define KEYSTA_SEND_NOTREADY	0x02
#define KEYCMD_WRITE_MODE		0x60
#define KBC_MODE				0x47
#define KEYCMD_SENDTO_MOUSE		0xd4
#define MOUSECMD_ENABLE			0xf4

//keyboard.c
void wait_kbc_sendready(void);
void init_keyboard(void);
void asm_inthandler21();

//mouse.c
struct MOUSE_DEC {
	unsigned char buf[3];
	char phase;
	int x,y,btn;
};
void enable_mouse(struct MOUSE_DEC *mdec);
int mouse_decode(struct MOUSE_DEC *mdec,unsigned char dat);
void asm_inthandler2c();

#ifdef __cplusplus
}
#endif
#endif
