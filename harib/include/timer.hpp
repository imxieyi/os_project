#ifndef _TIMER_HPP
#define _TIMER_HPP

#ifdef __cplusplus
extern "C"{
#endif

#define PIT_CTRL	0x0043
#define PIT_CNT0	0x0040
#define MAX_TIMER	500
enum TIMER_FLAG {FREE,ALLOC,USING};
class TIMER {
private:
	unsigned char data;
public:
	class FIFO *fifo;
	TIMER_FLAG flag;
	unsigned int timeout;
	void set(unsigned int timeout);
	void push();
	void free();
	TIMER *init(class FIFO *fifo,unsigned int data);
	void init(unsigned int data);
};
class TIMERCTRL {
public:
	unsigned int count,next,inuse;
	TIMER *timers[MAX_TIMER];
	TIMER timer[MAX_TIMER];
	TIMER *alloc();
	void interrupt();
	void regist(TIMER *timer);
};
void init_pit();
void asm_inthandler20();
void inthandler20(int *esp);

#ifdef __cplusplus
}
#endif
#endif
