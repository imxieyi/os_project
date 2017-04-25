#ifndef _MEMORY_HPP
#define _MEMORY_HPP

#ifdef __cplusplus
extern "C"{
#endif

//内存管理部分
#define EFLAGS_AC_BIT		0x00040000
#define CR0_CACHE_DISABLE	0x60000000
#define MEMMAN_FREES		4000
#define MEMMAN_ADDR			0x003c0000
struct FREEINFO{
	unsigned int addr,size;
};
class MEMMAN {
private:
	int frees,maxfrees,lostsize,losts;
public:
	struct FREEINFO freeinfo[MEMMAN_FREES];
	MEMMAN();
	unsigned int total();
	unsigned int alloc(unsigned int size);
	int free(unsigned int addr,unsigned int size);
	unsigned int alloc_4k(unsigned int size);
	int free_4k(unsigned int addr,unsigned int size);
};
int load_cr0(void);
void store_cr0(int cr0);
unsigned int memtest(unsigned int start,unsigned int end);

#ifdef __cplusplus
}
#endif
#endif
