#include <stdio.h>
#include "headers.h"

extern struct FIFO8 keybuf;
extern struct FIFO8 mousebuf;

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

void enable_mouse(void){
	//激活鼠标
	wait_kbc_sendready();
	io_out8(PORT_KEYCMD,KEYCMD_SENDTO_MOUSE);
	wait_kbc_sendready();
	io_out8(PORT_KEYDAT,MOUSECMD_ENABLE);
	return;//如果成功，键盘控制器返回ACK(0xfa)
}

void HariMain(void)
{
	struct BOOTINFO *binfo=(struct BOOTINFO *)0x0ff0;

	init_gdtidt();
	init_pic();
	io_sti();//取消CPU中断禁止，因为IDT/PIC已初始化完成
	init_palette();
	init_screen(binfo->vram,binfo->scrnx,binfo->scrny);
	
	putfonts8_asc(binfo->vram,binfo->scrnx,8,8,COL8_FFFFFF,"ABC 123");
	putfonts8_asc(binfo->vram,binfo->scrnx,31,31,COL8_000000,"Project OS.");
	putfonts8_asc(binfo->vram,binfo->scrnx,30,30,COL8_FFFFFF,"Project OS.");
	
	char s[15];
	sprintf(s,"scrnx = %d",binfo->scrnx);
	putfonts8_asc(binfo->vram,binfo->scrnx,16,64,COL8_840084,s);
	
	char *mcursor;
	int mx=152,my=78;
	init_mouse_cursor8(mcursor,COL8_008484);
	putblock8_8(binfo->vram,binfo->scrnx,16,16,mx,my,mcursor,16);

	init_keyboard();
	enable_mouse();
	io_out8(PIC0_IMR, 0xf9);//允许PIC1和键盘发送中断(11111001)
	io_out8(PIC1_IMR, 0xef);//允许鼠标发送中断(11101111)
	
	unsigned char keybuff[32];
	fifo8_init(&keybuf,32,keybuff);
	unsigned char mousebuff[128];
	fifo8_init(&mousebuf,128,mousebuff);

	unsigned char i;
	for (;;) {
		io_cli();
		if(fifo8_status(&keybuf)+fifo8_status(&mousebuf)==0){
			io_stihlt();
		}else{
			if(fifo8_status(&keybuf)!=0){
				i=fifo8_get(&keybuf);
				io_sti();
				sprintf(s,"%02X",i);
				boxfill8(binfo->vram,binfo->scrnx,COL8_008484,0,16,15,31);
				putfonts8_asc(binfo->vram,binfo->scrnx,0,16,COL8_FFFFFF,s);
			} else if(fifo8_status(&mousebuf)!=0){
				i=fifo8_get(&mousebuf);
				io_sti();
				sprintf(s,"%02X",i);
				boxfill8(binfo->vram,binfo->scrnx,COL8_008484,0,16,47,31);
				putfonts8_asc(binfo->vram,binfo->scrnx,0,16,COL8_FFFFFF,s);
			}
		}
	}
}
