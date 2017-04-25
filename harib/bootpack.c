#include <stdio.h>
#include "headers.h"

extern struct FIFO8 keybuf;
extern struct FIFO8 mousebuf;
/*unsigned int memtest(unsigned int start,unsigned int end){
	unsigned int cr0;
	cr0=load_cr0();
	cr0|=CR0_CACHE_DISABLE;
	store_cr0(cr0);

	unsigned int i,*p,old,pat0=0x66996699,pat1=0x99669966;
	for(i=start;i<=end;i+=0x400000){//改进：vmware要求内存必须是4MB的倍数，所以直接以4MB为单位检查内存
		p=(unsigned int *)(i+0x3ffffc);
		old=*p;
		*p=pat0;
		*p^=0xffffffff;
		if(*p!=pat1){
			not_memory:
			*p=old;
			break;
		}
		*p^=0xffffffff;
		if(*p!=pat0)goto not_memory;
		*p=old;
	}

	cr0=load_cr0();
	cr0&=~CR0_CACHE_DISABLE;
	store_cr0(cr0);
	return i;
}*/

void HariMain(void)
{
	struct BOOTINFO *binfo=(struct BOOTINFO *)0x0ff0;

	init_gdtidt();
	init_pic();
	io_sti();//取消CPU中断禁止，因为IDT/PIC已初始化完成
	init_palette();
	init_screen(binfo->vram,binfo->scrnx,binfo->scrny);
	
	char s[15];
	sprintf(s,"mem: %dMB",memtest(0x00400000,0xbfffffff)/0x400000*4);
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
