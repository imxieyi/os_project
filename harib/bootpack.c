void io_hlt(void);
//void write_mem8(int addr,int data);

void HariMain(void)
{
/*
	int i;
	char *p;
	for(i=0xa0000;i<=0xaffff;i++){
		p=(char *)i;
		*p=i&0x0f;
		//write_mem8(i,i&0x0f);	//MOV BYTE [i],15
	}
*/
	char *p;
	int i;
	p=(char*)0xa0000;
	for(i=0;i<=0xffff;i++){
		*(p+i)=i&0x0f;
	}
	while(1){
		io_hlt();
	}

}
