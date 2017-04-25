#include <stdio.h>
#include "include/headers.h"
#include "include/fifo.hpp"
#include "include/memory.hpp"
#include "include/sheet.hpp"

extern FIFO *keybuf;
extern FIFO *mousebuf;

#ifdef __cplusplus
extern "C"{
#endif
void HariMain(void)
{
	struct BOOTINFO *binfo=(struct BOOTINFO *)0x0ff0;

	init_gdtidt();
	init_pic();
	io_sti();//取消CPU中断禁止，因为IDT/PIC已初始化完成

	SHEETCTRL *shtctl;
	SHEET *sht_back,*sht_mouse;
	unsigned char *buf_back,buf_mouse[256];
	
	unsigned int memtotal;
	//struct MEMMAN *memman=(struct MEMMAN *)MEMMAN_ADDR;
	MEMMAN *memman=(MEMMAN*)MEMMAN_ADDR;
	memtotal=memtest(0x00400000,0xbfffffff);
	//memman->init(memman);
	memman->free(0x00001000,0x0009e000);
	memman->free(0x00400000,memtotal-0x00400000);
	char s[15];

	init_palette();
	shtctl=sheetctrl_init(memman,binfo->vram,binfo->scrnx,binfo->scrny);
	sht_back=shtctl->allocsheet();
	sht_mouse=shtctl->allocsheet();
	buf_back=(unsigned char *)memman->alloc_4k(binfo->scrnx*binfo->scrny);
	sht_back->setbuf(buf_back,binfo->scrnx,binfo->scrny,-1);
	sht_mouse->setbuf(buf_mouse,16,16,99);
	init_screen(buf_back,binfo->scrnx,binfo->scrny);
	
	int mx=(binfo->scrnx-16)/2,my=(binfo->scrny-28-16)/2;
	init_mouse_cursor8(buf_mouse,99);
	
	sht_back->slide(0,0);
	sht_mouse->slide(mx,my);
	sht_back->updown(0);
	sht_mouse->updown(1);
	sprintf(s,"(%3d,%3d)",mx,my);
	putfonts8_asc(buf_back,binfo->scrnx,0,0,COL8_FFFFFF,s);
	sprintf(s,"mem: %dMB free:%dKB",memtotal/0x400000*4,memman->total()/1024);
	putfonts8_asc(buf_back,binfo->scrnx,0,32,COL8_840084,s);
	sht_back->refresh(0,0,binfo->scrnx,48);

	init_keyboard();
	struct MOUSE_DEC mdec;
	enable_mouse(&mdec);
	io_out8(PIC0_IMR, 0xf9);//允许PIC1和键盘发送中断(11111001)
	io_out8(PIC1_IMR, 0xef);//允许鼠标发送中断(11101111)
	
	unsigned char keybuff[32];
	keybuf=&FIFO(32,keybuff);
	unsigned char mousebuff[128];
	mousebuf=&FIFO(128,mousebuff);

	unsigned char i;
	for (;;) {
		io_cli();
		if(keybuf->status()+mousebuf->status()==0){
			io_stihlt();
		}else{
			if(keybuf->status()!=0){
				i=keybuf->get();
				io_sti();
				sprintf(s,"%02X",i);
				boxfill8(buf_back,binfo->scrnx,COL8_008484,0,16,15,31);
				putfonts8_asc(buf_back,binfo->scrnx,0,16,COL8_FFFFFF,s);
				sht_back->refresh(0,16,16,32);
			} else if(mousebuf->status()!=0){
				i=mousebuf->get();
				io_sti();
				if(mouse_decode(&mdec,i)){
					sprintf(s,"[lcr %4d %4d]",mdec.x,mdec.y);
					if(mdec.btn&0x01)
						s[1]='L';
					if(mdec.btn&0x02)
						s[3]='R';
					if(mdec.btn&0x04)
						s[2]='C';
					boxfill8(buf_back,binfo->scrnx,COL8_008484,32,16,32+15*8-1,31);
					putfonts8_asc(buf_back,binfo->scrnx,32,16,COL8_FFFFFF,s);
					sht_back->refresh(32,16,32+15*8,32);
					//移动鼠标指针
					mx+=mdec.x;
					my+=mdec.y;
					mx=mx<0?0:mx;
					my=my<0?0:my;
					mx=mx>binfo->scrnx-16?binfo->scrnx-16:mx;
					my=my>binfo->scrny-16?binfo->scrny-16:my;
					sprintf(s,"(%3d,%3d)",mx,my);
					boxfill8(buf_back,binfo->scrnx,COL8_008484,0,0,79,15);
					putfonts8_asc(buf_back,binfo->scrnx,0,0,COL8_FFFFFF,s);
					sht_back->refresh(0,0,80,16);
					sht_mouse->slide(mx,my);
				}
			}
		}
	}
}
#ifdef __cplusplus
}
#endif
