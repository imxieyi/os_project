#include "fifo.hpp"

FIFO::FIFO(int size,unsigned char *buf){
	this->size=size;
	this->buf=buf;
	this->free=size;
	this->flags=0;
	this->w=0;
	this->r=0;
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
