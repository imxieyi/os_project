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
	unsigned char *buf;
	int bxsize,bysize,vx0,vy0,col_inv,height,flags;//height越大越上层
	SHEET(SHEETCTRL *ctl);
	void setbuf(unsigned char *buf,int xsize,int ysize,int col_inv);
	void updown(int height);
	void refresh(int bx0,int by0,int bx1,int by1);
	void refreshsub(int bx0,int by0,int bx1,int by1);
	void slide(int vx0,int vy0);
	void free();
};
class SHEETCTRL {
public:
	unsigned char *vram;
	int xsize,ysize,top;
	SHEET *sheets[MAX_SHEETS];
	SHEET sheets0[MAX_SHEETS];
	SHEET *allocsheet();
};
struct SHEETCTRL *sheetctrl_init(struct MEMMAN *memman,unsigned char *vram,int xsize,int ysize);

#ifdef __cplusplus
}
#endif
#endif
