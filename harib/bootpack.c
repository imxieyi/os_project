#include<stdio.h>
#include<headers.h>

void HariMain(void)
{
	struct BOOTINFO *binfo=(struct BOOTINFO *)0x0ff0;

	init_gdtidt();
	init_pic();
	io_sti();	//取消CPU中断禁止，因为IDT/PIC已初始化完成
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

	io_out8(PIC0_IMR, 0xf9);	//允许PIC1和键盘发送中断(11111001)
	io_out8(PIC1_IMR, 0xef);	//允许鼠标发送中断(11101111)
	
	for (;;) {
		io_hlt();
	}
}
