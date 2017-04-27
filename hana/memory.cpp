#include "include/headers.h"
#include "include/memory.hpp"

MEMMAN::MEMMAN(){
	this->frees=0;	//可用信息数
	this->maxfrees=0;//frees的最大值
	this->lostsize=0;//释放失败内存大小和
	this->losts=0;	//释放失败次数
	return;
}

unsigned int MEMMAN::total(){
	unsigned int i,t=0;
	for(i=0;i<frees;i++)
		t+=freeinfo[i].size;
	return t;
}

unsigned int MEMMAN::alloc(unsigned int size){
	unsigned int i,a;
	for(i=0;i<this->frees;i++)
		if(this->freeinfo[i].size>=size){
			a=this->freeinfo[i].addr;
			this->freeinfo[i].addr+=size;
			this->freeinfo[i].size-=size;
			if(this->freeinfo[i].size==0){
				this->frees--;
				for(;i<this->frees;i++)
					this->freeinfo[i]=this->freeinfo[i+1];
			}
			return a;
		}
	return 0;
}

int MEMMAN::free(unsigned int addr,unsigned int size){
	int i,j;
	for(i=0;i<this->frees;i++)
		if(this->freeinfo[i].addr>addr)
			break;
	if(i>0)
		if(this->freeinfo[i-1].addr+this->freeinfo[i-1].size==addr){
			this->freeinfo[i-1].size+=size;
			if(i<this->frees)
				if(addr+size==this->freeinfo[i].addr){
					this->freeinfo[i-1].size+=this->freeinfo[i].size;
					this->frees--;
					for(;i<this->frees;i++)
						this->freeinfo[i]=this->freeinfo[i+1];
				}
			return 0;
		}
	if(i<this->frees)
		if(addr+size==this->freeinfo[i].addr){
			this->freeinfo[i].addr=addr;
			this->freeinfo[i].size+=size;
			return 0;
		}
	if(this->frees<MEMMAN_FREES){
		for(j=this->frees;j>i;j--)
			this->freeinfo[j]=this->freeinfo[j-1];
		this->frees++;
		if(this->maxfrees<this->frees)
			this->maxfrees=this->frees;
		this->freeinfo[i].addr=addr;
		this->freeinfo[i].size=size;
		return 0;
	}
	this->losts++;
	this->lostsize+=size;
	return -1;
}

unsigned int MEMMAN::alloc_4k(unsigned int size){
	size=(size+0xfff)&0xfffff000;
	return this->alloc(size);
}

int MEMMAN::free_4k(unsigned int addr,unsigned int size){
	size=(size+0xfff)&0xfffff000;
	return this->free(addr,size);
}
