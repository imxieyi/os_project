#ifndef _FIFO_HPP
#define _FIFO_HPP

#ifdef __cplusplus
extern "C"{
#endif

#define FLAGS_OVERRUN	0x0001
class FIFO {
private:
	unsigned char *buf;
	class MEMMAN *memman;
	int r,w,size,free,flags;
public:
	FIFO(class MEMMAN *memman,int size);
	int put(unsigned char data);
	int get();
	void remove();
	int status();
};

#ifdef __cplusplus
}
#endif
#endif
