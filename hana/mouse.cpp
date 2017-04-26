#include "include/headers.h"
#include "include/fifo.hpp"
#include "include/inputdevices.hpp"

void enable_mouse(struct MOUSE_DEC *mdec){
	//激活鼠标
	wait_kbc_sendready();
	io_out8(PORT_KEYCMD,KEYCMD_SENDTO_MOUSE);
	wait_kbc_sendready();
	io_out8(PORT_KEYDAT,MOUSECMD_ENABLE);
	//如果成功，键盘控制器返回ACK(0xfa)
	mdec->phase=-1;//等待0xfa
	return;
}

int mouse_decode(struct MOUSE_DEC *mdec,unsigned char dat){
	if(mdec->phase==-1){
		if(dat==0xfa)
			mdec->phase=0;
		return 0;
	}
	mdec->buf[mdec->phase]=dat;
	if(!mdec->phase && (dat&0xc8)!=0x08)
		return 0;
	if(!(mdec->phase=(mdec->phase+1)%3)){
		mdec->btn=mdec->buf[0]&0x07;
		mdec->x=mdec->buf[1];
		mdec->y=mdec->buf[2];
		if(mdec->buf[0]&0x10)
			mdec->x|=0xffffff00;
		if(mdec->buf[0]&0x20)
			mdec->y|=0xffffff00;
		mdec->y=-mdec->y;
		return 1;
	}
	return 0;
}

FIFO *mousebuf;
//来自PS/2鼠标的中断
void inthandler2c(int *esp){
	unsigned char data;
	io_out8(PIC1_OCW2,0x64);
	io_out8(PIC0_OCW2,0x62);
	data=io_in8(PORT_KEYDAT);
	mousebuf->put(data);
	return;
}
