#include "include/fifo.hpp"
#include "include/memory.hpp"

FIFO::FIFO(MEMMAN *memman,int size){
	this->size=size;
	memman=memman;
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
	if(free==0){
		flags|=FLAGS_OVERRUN;
		return -1;
	}
	buf[w]=data;
	w=(w+1)%size;
	free--;
	return 0;
}

int FIFO::get(){
	int data;
	if(free==size){
		return -1;
	}
	data=buf[r];
	r=(r+1)%size;
	free++;
	return data;
}

int FIFO::status(){
	return size-free;
}
