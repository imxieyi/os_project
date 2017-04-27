#include "include/headers.h"
#include "include/timer.hpp"
#include "include/fifo.hpp"
#include "include/memory.hpp"

TIMERCTRL *timerctrl;

void init_pit(){
	io_out8(PIT_CTRL,0x34);
	io_out8(PIT_CNT0,0x9c);
	io_out8(PIT_CNT0,0x2e);
	timerctrl->count=0;
	timerctrl->next=0xffffffff;
	timerctrl->inuse=0;
	for(int i=0;i<MAX_TIMER;i++)
		timerctrl->timer[i].flag=FREE;
	return;
}

void inthandler20(int *esp){
	io_out8(PIC0_OCW2,0x60);
	timerctrl->interrupt();
	return;
}

void TIMERCTRL::interrupt(){
	count++;
	if(next>count)return;
	int i;
	for(i=0;i<inuse;i++){
		if(timers[i]->timeout>count)break;
		timers[i]->push();
	}
	inuse-=i;
	for(int j=0;j<inuse;j++)timers[j]=timers[i+j];
	if(inuse>0)next=timers[0]->timeout;
	else next=0xffffffff;
	return;
}

void TIMERCTRL::regist(TIMER *timer){
	int i;
	for(i=0;i<inuse;i++)
		if(timers[i]->timeout>=timer->timeout)
			break;
	for(int j=inuse;j>i;j--)
		timers[j]=timers[j-1];
	inuse++;
	timers[i]=timer;
	next=timers[0]->timeout;
	return;
}

TIMER *TIMERCTRL::alloc(){
	for(int i=0;i<MAX_TIMER;i++)
		if(timer[i].flag==FREE){
			timer[i].flag=ALLOC;
			return &timer[i];
		}
	return 0;
}

void TIMER::push(){
	flag=ALLOC;
	fifo->put(data);
}

void TIMER::free(){
	flag=FREE;
	return;
}

TIMER *TIMER::init(FIFO *fifo,int data){
	this->fifo=fifo;
	this->data=data;
	return this;
}

void TIMER::setdata(int data){
	this->data=data;
}

void TIMER::set(unsigned int timeout){
	this->timeout=timerctrl->count+timeout;
	flag=USING;
	int e=io_load_eflags();
	io_cli();
	timerctrl->regist(this);
	io_store_eflags(e);
	return;
}
