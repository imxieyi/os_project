#ifndef _SHEET_HPP
#define _SHEET_HPP

#ifdef __cplusplus
extern "C"{
#endif

#define MAX_SHEETS		256
#define SHEET_USE		1
class SHEETCTRL;
class SHEET {
public:
	SHEETCTRL *sctrl;
	class GRAPHICS *graphics;
	int bxsize,bysize,vx0,vy0,col_inv,height,flags;//height越大越上层
	SHEET(SHEETCTRL *ctl);
	void updown(int height);
	void refresh(int bx0,int by0,int bx1,int by1);
	void refreshsub(int bx0,int by0,int bx1,int by1,int h0,int h1);
	void refreshmap(int vx0,int vy0,int vx1,int vy1,int h0);
	void slide(int vx0,int vy0);
	void free();
};
class SHEETCTRL {
private:
	class MEMMAN *memman;
public:
	unsigned char *vram,*map;
	int xsize,ysize,top;
	SHEET *sheets[MAX_SHEETS];
	SHEET sheets0[MAX_SHEETS];
	SHEET *allocsheet(int xsize,int ysize,int col_inv);
	void setmemman(class MEMMAN *memman);
};
struct SHEETCTRL *sheetctrl_init(class MEMMAN *memman,unsigned char *vram,int xsize,int ysize);

#ifdef __cplusplus
}
#endif
#endif
