#ifndef _GRAPHICS_HPP
#define _GRAPHICS_HPP

#ifdef __cplusplus
extern "C"{
#endif

//colors
#define COL8_000000		0
#define COL8_FF0000		1
#define COL8_00FF00		2
#define COL8_FFFF00		3
#define COL8_0000FF		4
#define COL8_FF00FF		5
#define COL8_00FFFF		6
#define COL8_FFFFFF		7
#define COL8_C6C6C6		8
#define COL8_840000		9
#define COL8_008400		10
#define COL8_848400		11
#define COL8_000084		12
#define COL8_840084		13
#define COL8_008484		14
#define COL8_848484		15

class GRAPHICS {
private:
	int xsize,ysize;
public:
	unsigned char *vram;
	void init(unsigned char *vram,int xsize,int ysize);
	void init_screen();
	void init_mouse_cursor8(char bc);
	void init_window8(char *title);
	void boxfill8(unsigned char c, int x0, int y0, int x1, int y1);
	void putfont8(int x, int y, char color, char *font);
	void putfonts8_asc(int x,int y,char c,char *s);
	void putblock8_8(int pxsize, int pysize, int px0, int py0, char *buf, int bxsize);
};

//graphics.c
void init_palette(void);
void set_palette(int start, int end, unsigned char *rgb);

#ifdef __cplusplus
}
#endif
#endif
