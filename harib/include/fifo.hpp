#ifndef _FIFO_H
#define _FIFO_H

#ifdef __cplusplus
extern "C"{
#endif

#define FLAGS_OVERRUN	0x0001
class FIFO {
private:
	unsigned char *buf;
	int r,w,size,free,flags;
public:
	FIFO(int size);
	FIFO(int size,unsigned char *buf);
	int put(unsigned char data);
	int get();
	int status();
};

#ifdef __cplusplus
}
#endif
#endif
