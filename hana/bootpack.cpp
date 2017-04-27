#include <stdio.h>
#include "include/headers.h"
#include "include/timer.hpp"
#include "include/fifo.hpp"
#include "include/memory.hpp"
#include "include/graphics.hpp"
#include "include/sheet.hpp"
#include "include/inputdevices.hpp"

extern TIMERCTRL *timerctrl;

#ifdef __cplusplus
extern "C"{
#endif
void HanaMain(void)
{
	struct BOOTINFO *binfo=(struct BOOTINFO *)0x0ff0;

	unsigned int memtotal;
	memtotal=memtest(0x00400000,0xbfffffff);
	auto *memman=(MEMMAN*)MEMMAN_ADDR;
	memman->free(0x00001000,0x0009e000);
	memman->free(0x00400000,memtotal-0x00400000);

	timerctrl=(TIMERCTRL*)memman->alloc(sizeof(TIMERCTRL));

	init_gdtidt();
	init_pic();
	io_sti();//取消CPU中断禁止，因为IDT/PIC已初始化完成
	init_pit();//定时器
	io_out8(PIC0_IMR, 0xf8);//允许PIT,PIC1和键盘发送中断(11111000)
	io_out8(PIC1_IMR, 0xef);//允许鼠标发送中断(11101111)

	//auto fifo=FIFO(memman,128);
	FIFO *fifo=(FIFO*)memman->alloc(sizeof(FIFO));
	fifo->init(memman,128);

	auto timer1=timerctrl->alloc()->init(fifo,10);
	auto timer2=timerctrl->alloc()->init(fifo,3);
	auto timer3=timerctrl->alloc()->init(fifo,1);
	timer1->set(1000);
	timer2->set(300);
	timer3->set(50);
	
	char s[128];

	init_palette();
	auto shtctl=sheetctrl_init(memman,binfo->vram,binfo->scrnx,binfo->scrny);
	auto sht_back=shtctl->allocsheet(binfo->scrnx,binfo->scrny,-1);
	auto sht_mouse=shtctl->allocsheet(16,16,99);
	auto sht_win=shtctl->allocsheet(160,52,-1);
	sht_back->graphics->init_screen();
	
	int mx=(binfo->scrnx-16)/2,my=(binfo->scrny-28-16)/2;
	sht_mouse->graphics->init_mouse_cursor8(99);
	
	sht_back->slide(0,0);
	sht_mouse->slide(mx,my);
	sht_back->updown(0);
	sht_mouse->updown(2);

	sht_win->graphics->init_window8("timer");
	sht_win->slide(80,72);
	sht_win->updown(1);
	sprintf(s,"(%3d,%3d)",mx,my);
	sht_back->graphics->putfonts8_asc(0,0,COL8_FFFFFF,s);
	sprintf(s,"mem: %dMB free:%dKB",memtotal/0x400000*4,memman->total()/1024);
	sht_back->graphics->putfonts8_asc(0,32,COL8_840084,s);
	sht_back->refresh(0,0,binfo->scrnx,48);

	init_keyboard(fifo,256);
	struct MOUSE_DEC mdec;
	enable_mouse(fifo,512,&mdec);

	int i=0;
	int count=0;
	for (;;) {
		count++;

		io_cli();
		if(fifo->status()==0){
			io_sti();
		}else{
			i=fifo->get();
			io_sti();
			if(i>=256&&i<=511){
				sprintf(s,"%02X",i-256);
				sht_back->putstring(0,16,COL8_FFFFFF,COL8_008484,s);
			} else if(i>=512&&i<=767){
				if(mouse_decode(&mdec,i-512)){
					sprintf(s,"[lcr %4d %4d]",mdec.x,mdec.y);
					if(mdec.btn&0x01)
						s[1]='L';
					if(mdec.btn&0x02)
						s[3]='R';
					if(mdec.btn&0x04)
						s[2]='C';
					sht_back->putstring(32,16,COL8_FFFFFF,COL8_008484,s);
					//移动鼠标指针
					mx+=mdec.x;
					my+=mdec.y;
					mx=mx<0?0:mx;
					my=my<0?0:my;
					mx=mx>binfo->scrnx-1?binfo->scrnx-1:mx;
					my=my>binfo->scrny-1?binfo->scrny-1:my;
					sprintf(s,"(%3d,%3d)",mx,my);
					sht_back->putstring(0,0,COL8_FFFFFF,COL8_008484,s);
					sht_mouse->slide(mx,my);
				}
			} else if(i==10){
				sht_back->putstring(0,64,COL8_FFFFFF,COL8_008484,"10[sec]");
				sprintf(s,"%010d",count);
				sht_win->putstring(40,28,COL8_000000,COL8_C6C6C6,s);
			} else if(i==3){
				sht_back->putstring(0,80,COL8_FFFFFF,COL8_008484,"3[sec]");
				count=0;
			} else if(i==1){
				timer3->setdata(0);
				sht_back->graphics->boxfill8(COL8_FFFFFF,8,96,15,111);
				timer3->set(50);
				sht_back->refresh(8,96,16,112);
			} else if(i==0) {
				timer3->setdata(1);
				sht_back->graphics->boxfill8(COL8_008484,8,96,15,111);
				timer3->set(50);
				sht_back->refresh(8,96,16,112);
			}
		}
	}
}
#ifdef __cplusplus
}
#endif
