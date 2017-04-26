#include "include/fifo.hpp"
#include "include/memory.hpp"

FIFO::FIFO(MEMMAN *memman,int size){
	this->size=size;
	this->memman=memman;
	buf=(unsigned char *)memman->alloc(size);
	free=size;
	flags=0;
	w=0;
	r=0;
	return;
}

void FIFO::remove(){
	memman->free((unsigned int)buf,size);
	return;
}

int FIFO::put(unsigned char data){
	if(this->free==0){
		this->flags|=FLAGS_OVERRUN;
		return -1;
	}
	this->buf[this->w]=data;
	this->w=(this->w+1)%this->size;
	this->free--;
	return 0;
}

int FIFO::get(){
	int data;
	if(this->free==this->size){
		return -1;
	}
	data=this->buf[this->r];
	this->r=(this->r+1)%this->size;
	this->free++;
	return data;
}

int FIFO::status(){
	return this->size-this->free;
}
