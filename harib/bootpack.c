#include<stdio.h>
#include<headers.h>

void HariMain(void)
{
	struct BOOTINFO *binfo=(struct BOOTINFO *)0x0ff0;

	init_gdtidt();
	init_palette();
	init_screen(binfo->vram,binfo->scrnx,binfo->scrny);
	
	putfonts8_asc(binfo->vram,binfo->scrnx,8,8,COL8_FFFFFF,"ABC 123");
	putfonts8_asc(binfo->vram,binfo->scrnx,31,31,COL8_000000,"Project OS.");
	putfonts8_asc(binfo->vram,binfo->scrnx,30,30,COL8_FFFFFF,"Project OS.");
	
	char s[15];
	sprintf(s,"scrnx = %d",binfo->scrnx);
	putfonts8_asc(binfo->vram,binfo->scrnx,16,64,COL8_840084,s);
	
	char mcursor[256];
	int mx=152,my=78;
	init_mouse_cursor8(mcursor,COL8_008484);
	putblock8_8(binfo->vram,binfo->scrnx,16,16,mx,my,mcursor,16);
	
	for (;;) {
		io_hlt();
	}
}
