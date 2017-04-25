#include <stdio.h>
#include "headers.h"

extern struct FIFO8 keybuf;
extern struct FIFO8 mousebuf;

void wait_kbc_sendready(void);
void init_keyboard(void);
void enable_mouse(struct MOUSE_DEC *mdec);
int mouse_decode(struct MOUSE_DEC *mdec,unsigned char dat);

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
	struct MOUSE_DEC mdec;
	enable_mouse(&mdec);
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
				if(mouse_decode(&mdec,i)){
					sprintf(s,"[lcr %4d %4d]",mdec.x,mdec.y);
					if(mdec.btn&0x01)
						s[1]='L';
					if(mdec.btn&0x02)
						s[3]='R';
					if(mdec.btn&0x04)
						s[2]='C';
					boxfill8(binfo->vram,binfo->scrnx,COL8_008484,32,16,32+15*8-1,31);
					putfonts8_asc(binfo->vram,binfo->scrnx,32,16,COL8_FFFFFF,s);
					//移动鼠标指针
					boxfill8(binfo->vram,binfo->scrnx,COL8_008484,mx,my,mx+15,my+15);
					mx+=mdec.x;
					my+=mdec.y;
					mx=mx<0?0:mx;
					my=my<0?0:my;
					mx=mx>binfo->scrnx-16?binfo->scrnx-16:mx;
					my=my>binfo->scrny-16?binfo->scrny-16:my;
					sprintf(s,"(%3d,%3d)",mx,my);
					boxfill8(binfo->vram,binfo->scrnx,COL8_008484,0,0,79,15);
					putfonts8_asc(binfo->vram,binfo->scrnx,0,0,COL8_FFFFFF,s);
					putblock8_8(binfo->vram,binfo->scrnx,16,16,mx,my,mcursor,16);
				}
			}
		}
	}
}

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
