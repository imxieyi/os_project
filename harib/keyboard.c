#include "headers.h"

void wait_kbc_sendready(void){
	//等待键盘控制电路准备完毕
	for(;;)
		if((io_in8(PORT_KEYSTA)&KEYSTA_SEND_NOTREADY)==0)
			return;
}

void init_keyboard(void){
	wait_kbc_sendready();
	io_out8(PORT_KEYCMD,KEYCMD_WRITE_MODE);
	wait_kbc_sendready();
	io_out8(PORT_KEYDAT,KBC_MODE);
	return;
}

struct FIFO8 keybuf;
//来自PS/2键盘的中断
void inthandler21(int *esp){
	unsigned char data;
	io_out8(PIC0_OCW2, 0x61);	//通知PIC IRQ-01已经受理完毕
	data=io_in8(PORT_KEYDAT);
	fifo8_put(&keybuf,data);
	return;
}
