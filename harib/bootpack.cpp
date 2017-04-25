#include <stdio.h>
#include "headers.h"

extern struct FIFO8 keybuf;
extern struct FIFO8 mousebuf;

#ifdef __cplusplus
extern "C"{
#endif
void HariMain(void)
{
	struct BOOTINFO *binfo=(struct BOOTINFO *)0x0ff0;

	init_gdtidt();
	init_pic();
	io_sti();//取消CPU中断禁止，因为IDT/PIC已初始化完成

	struct SHEETCTRL *shtctl;
	struct SHEET *sht_back,*sht_mouse;
	unsigned char *buf_back,buf_mouse[256];
	
	unsigned int memtotal;
	struct MEMMAN *memman=(struct MEMMAN *)MEMMAN_ADDR;
	memtotal=memtest(0x00400000,0xbfffffff);
	memman_init(memman);
	memman_free(memman,0x00001000,0x0009e000);
	memman_free(memman,0x00400000,memtotal-0x00400000);
	char s[15];

	init_palette();
	shtctl=sheetctrl_init(memman,binfo->vram,binfo->scrnx,binfo->scrny);
	sht_back=sheet_alloc(shtctl);
	sht_mouse=sheet_alloc(shtctl);
	buf_back=(unsigned char *)memman_alloc_4k(memman,binfo->scrnx*binfo->scrny);
	sheet_setbuf(sht_back,buf_back,binfo->scrnx,binfo->scrny,-1);
	sheet_setbuf(sht_mouse,buf_mouse,16,16,99);
	init_screen(buf_back,binfo->scrnx,binfo->scrny);
	
	int mx=(binfo->scrnx-16)/2,my=(binfo->scrny-28-16)/2;
	init_mouse_cursor8(buf_mouse,99);
	
	sheet_slide(shtctl,sht_back,0,0);
	sheet_slide(shtctl,sht_mouse,mx,my);
	sheet_updown(shtctl,sht_back,0);
	sheet_updown(shtctl,sht_mouse,1);
	sprintf(s,"(%3d,%3d)",mx,my);
	putfonts8_asc(buf_back,binfo->scrnx,0,0,COL8_FFFFFF,s);
	sprintf(s,"mem: %dMB free:%dKB",memtotal/0x400000*4,memman_total(memman)/1024);
	putfonts8_asc(buf_back,binfo->scrnx,0,32,COL8_840084,s);
	sheet_refresh(shtctl,sht_back,0,0,binfo->scrnx,48);

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
				boxfill8(buf_back,binfo->scrnx,COL8_008484,0,16,15,31);
				putfonts8_asc(buf_back,binfo->scrnx,0,16,COL8_FFFFFF,s);
				sheet_refresh(shtctl,sht_back,0,16,16,32);
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
					boxfill8(buf_back,binfo->scrnx,COL8_008484,32,16,32+15*8-1,31);
					putfonts8_asc(buf_back,binfo->scrnx,32,16,COL8_FFFFFF,s);
					sheet_refresh(shtctl,sht_back,32,16,32+15*8,32);
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
					sheet_refresh(shtctl,sht_back,0,0,80,16);
					sheet_slide(shtctl,sht_mouse,mx,my);
				}
			}
		}
	}
}
#ifdef __cplusplus
}
#endif
