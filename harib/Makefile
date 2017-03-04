TOOLPATH = ../tolset/z_tools/
INCPATH  = ../tolset/z_tools/haribote/

MAKE     = $(TOOLPATH)make.exe -r
NASK     = $(TOOLPATH)nask.exe
CC1      = $(TOOLPATH)cc1.exe -I$(INCPATH) -Os -Wall -quiet
GAS2NASK = $(TOOLPATH)gas2nask.exe -a
OBJ2BIM  = $(TOOLPATH)obj2bim.exe
MAKEFONT = $(TOOLPATH)makefont.exe
BIN2OBJ  = $(TOOLPATH)bin2obj.exe
BIM2HRB  = $(TOOLPATH)bim2hrb.exe
RULEFILE = $(TOOLPATH)haribote/haribote.rul
EDIMG    = $(TOOLPATH)edimg.exe
IMGTOL   = $(TOOLPATH)imgtol.com
COPY     = cp
DEL      = rm

# デフォルト動作

default :
	$(MAKE) img

# ファイル生成規則

ipl.bin : ipl.nas Makefile
	$(NASK) ipl.nas ./build/ipl.bin ./build/ipl.lst

asmhead.bin : asmhead.nas Makefile
	$(NASK) asmhead.nas build/asmhead.bin build/asmhead.lst

bootpack.gas : bootpack.c Makefile
	$(CC1) -o build/bootpack.gas bootpack.c

bootpack.nas : bootpack.gas Makefile
	$(GAS2NASK) build/bootpack.gas build/bootpack.nas

bootpack.obj : bootpack.nas Makefile
	$(NASK) build/bootpack.nas build/bootpack.obj build/bootpack.lst

naskfunc.obj : naskfunc.nas Makefile
	$(NASK) naskfunc.nas build/naskfunc.obj build/naskfunc.lst

hankaku.bin : hankaku.txt Makefile
	$(MAKEFONT) hankaku.txt build/hankaku.bin

hankaku.obj : hankaku.bin Makefile
	$(BIN2OBJ) build/hankaku.bin build/hankaku.obj _hankaku

bootpack.bim : bootpack.obj naskfunc.obj hankaku.obj Makefile
	$(OBJ2BIM) @$(RULEFILE) out:build/bootpack.bim stack:3136k map:build/bootpack.map \
		build/bootpack.obj build/naskfunc.obj build/hankaku.obj
# 3MB+64KB=3136KB

bootpack.hrb : bootpack.bim Makefile
	$(BIM2HRB) build/bootpack.bim build/bootpack.hrb 0

haribote.sys : asmhead.bin bootpack.hrb Makefile
	# copy /B asmhead.bin+bootpack.hrb haribote.sys
	cat build/asmhead.bin build/bootpack.hrb > build/haribote.sys

haribote.img : ipl.bin haribote.sys Makefile
	$(EDIMG)   imgin:$(TOOLPATH)fdimg0at.tek \
		wbinimg src:build/ipl.bin len:512 from:0 to:0 \
		copy from:build/haribote.sys to:@: \
		imgout:build/haribote.img

# コマンド

img :
	$(MAKE) haribote.img

run :
	$(MAKE) img
	$(COPY) haribote.img $(TOOLPATH)qemu/fdimage0.bin
	$(MAKE) -C $(TOOLPATH)qemu

install :
	$(MAKE) img
	$(IMGTOL) w a: haribote.img

clean :
	-$(DEL) build/*.*

src_only :
	$(MAKE) clean
	-$(DEL) haribote.img
