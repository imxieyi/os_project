#include "include/headers.h"
#include "include/memory.hpp"
#include "include/sheet.hpp"

//图层控制
SHEETCTRL *sheetctrl_init(struct MEMMAN *memman,unsigned char *vram,int xsize,int ysize){
	int i;
	SHEETCTRL *ctl=(SHEETCTRL *)memman->alloc_4k(sizeof(SHEETCTRL));
	if(ctl==0){
		return 0;
	}
	ctl->vram=vram;
	ctl->xsize=xsize;
	ctl->ysize=ysize;
	ctl->top=-1;
	for(i=0;i<MAX_SHEETS;i++){
		ctl->sheets0[i].flags=0;//未使用
		ctl->sheets0[i].sctrl=ctl;
	}
	return ctl;
}

SHEET *SHEETCTRL::allocsheet(){
	SHEET *sht;
	int i;
	for(i=0;i<MAX_SHEETS;i++)
		if(!(sheets0[i].flags)){
			sht=&sheets0[i];
			sht->flags=SHEET_USE;
			sht->height=-1;
			return sht;
		}
	return 0;//无空闲图层
}

void SHEET::setbuf(unsigned char *buf,int xsize,int ysize,int col_inv){
	this->buf=buf;
	this->bxsize=xsize;
	this->bysize=ysize;
	this->col_inv=col_inv;
	return;
}

void SHEET::updown(int height){
	int h,old=this->height;
	height=height>sctrl->top+1?sctrl->top+1:height;
	height=height<-1?-1:height;
	this->height=height;
	//重排列sheets[]
	if(old>height){
		if(height>=0){
			for(h=old;h>height;h--){
				sctrl->sheets[h]=sctrl->sheets[h-1];
				sctrl->sheets[h]->height=h;
			}
			sctrl->sheets[height]=this;
		}else{
			if(sctrl->top>old)
				for(h=old;h<sctrl->top;h++){
					sctrl->sheets[h]=sctrl->sheets[h+1];
					sctrl->sheets[h]->height=h;
				}
			sctrl->top--;
		}
		refreshsub(vx0,vy0,vx0+bxsize,vy0+bysize);
	}else if(old<height){
		if(old>=0){
			for(h=old;h<height;h++){
				sctrl->sheets[h]=sctrl->sheets[h+1];
				sctrl->sheets[h]->height=h;
			}
			sctrl->sheets[height]=this;
		}else{
			for(h=sctrl->top;h>=height;h--){
				sctrl->sheets[h+1]=sctrl->sheets[h];
				sctrl->sheets[h+1]->height=h+1;
			}
			sctrl->sheets[height]=this;
			sctrl->top++;
		}
		refreshsub(vx0,vy0,vx0+bxsize,vy0+bysize);
	}
	return;
}

void SHEET::refresh(int bx0, int by0, int bx1, int by1){
	if (height >= 0)
		refreshsub(vx0 + bx0, vy0 + by0, vx0 + bx1, vy0 + by1);
	return;
}

void SHEET::refreshsub(int vx0, int vy0, int vx1, int vy1){
	int h, bx, by, vx, vy, bx0, by0, bx1, by1;
	unsigned char *buf, c, *vram = sctrl->vram;
	SHEET *sht;
	for (h = 0; h <= sctrl->top; h++) {
		sht = sctrl->sheets[h];
		buf = sht->buf;
		bx0 = vx0 - sht->vx0;
		by0 = vy0 - sht->vy0;
		bx1 = vx1 - sht->vx0;
		by1 = vy1 - sht->vy0;
		if (bx0 < 0) { bx0 = 0; }
		if (by0 < 0) { by0 = 0; }
		if (bx1 > sht->bxsize) { bx1 = sht->bxsize; }
		if (by1 > sht->bysize) { by1 = sht->bysize; }
		for (by = by0; by < by1; by++) {
			vy = sht->vy0 + by;
			for (bx = bx0; bx < bx1; bx++) {
				vx = sht->vx0 + bx;
				c = buf[by * sht->bxsize + bx];
				if (c != sht->col_inv) {
					vram[vy * sctrl->xsize + vx] = c;
				}
			}
		}
	}
	return;
}

void SHEET::slide(int vx0,int vy0){
	int old_vx0 = this->vx0, old_vy0 = this->vy0;
	this->vx0 = vx0;
	this->vy0 = vy0;
	if (height >= 0) {
		refreshsub(old_vx0, old_vy0, old_vx0 + bxsize, old_vy0 + bysize);
		refreshsub(vx0, vy0, vx0 + bxsize, vy0 + bysize);
	}
	return;
}

void SHEET::free(){
	if(height>=0)
		updown(-1);
	flags=0;
	return;
}
