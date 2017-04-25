TOOLPATH = ../tolset/z_tools/
INCPATH  = ../tolset/z_tools/haribote/

OBJS_BOOTPACK = build/bootpack.obj build/naskfunc.obj build/hankaku.obj build/graphics.obj \
	 build/gdtidt.obj build/interrupt.obj build/fifo.obj build/keyboard.obj build/mouse.obj \
	 build/memory.obj build/sheet.obj

MAKE     = $(TOOLPATH)make.exe -r
NASK     = $(TOOLPATH)nask.exe
CC1      = $(TOOLPATH)cc1.exe -I$(INCPATH) -I. -Os -Wall -quiet
GAS2NASK = $(TOOLPATH)gas2nask.exe -a
OBJ2BIM  = $(TOOLPATH)obj2bim.exe
MAKEFONT = $(TOOLPATH)makefont.exe
BIN2OBJ  = $(TOOLPATH)bin2obj.exe
BIM2HRB  = $(TOOLPATH)bim2hrb.exe
RULEFILE = $(TOOLPATH)haribote/haribote.rul
EDIMG    = $(TOOLPATH)edimg.exe
IMGTOL   = $(TOOLPATH)imgtol.com
#COPY     = cmd /c copy /y
#DEL      = cmd /c del /f
COPY	 = cp
DEL		 = rm

# デフォルト動作

default :
	$(MAKE) run

# ファイル生成規則

ipl.bin : ipl.nas Makefile
	$(NASK) ipl.nas ./build/ipl.bin ./build/ipl.lst

asmhead.bin : asmhead.nas Makefile
	$(NASK) asmhead.nas build/asmhead.bin build/asmhead.lst
	
naskfunc.obj : naskfunc.nas Makefile
	$(NASK) naskfunc.nas build/naskfunc.obj build/naskfunc.lst
	
%.gas : %.c Makefile
	$(CC1) -o build/$*.gas $*.c
	
%.nas : %.gas Makefile
	$(GAS2NASK) build/$*.gas build/$*.nas
	
%.obj : %.nas Makefile
	$(NASK) build/$*.nas build/$*.obj build/$*.lst

hankaku.bin : hankaku.txt Makefile
	$(MAKEFONT) hankaku.txt build/hankaku.bin

hankaku.obj : hankaku.bin Makefile
	$(BIN2OBJ) build/hankaku.bin build/hankaku.obj _hankaku

bootpack.bim : bootpack.obj naskfunc.obj hankaku.obj graphics.obj gdtidt.obj \
			   interrupt.obj fifo.obj keyboard.obj mouse.obj memory.obj sheet.obj Makefile
	$(OBJ2BIM) @$(RULEFILE) out:build/bootpack.bim stack:3136k map:build/bootpack.map \
		$(OBJS_BOOTPACK)
# 3MB+64KB=3136KB

bootpack.hrb : bootpack.bim Makefile
	$(BIM2HRB) build/bootpack.bim build/bootpack.hrb 0

haribote.sys : asmhead.bin bootpack.hrb Makefile
#	cmd /c copy /B build\asmhead.bin+build\bootpack.hrb build\haribote.sys
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
	$(COPY) build/haribote.img $(TOOLPATH)qemu/fdimage0.bin
#	$(COPY) build\haribote.img ..\tolset\z_tools\qemu\fdimage0.bin
	$(MAKE) -C $(TOOLPATH)qemu

install :
	$(MAKE) img
	$(IMGTOL) w a: haribote.img

clean :
	-$(DEL) build/*.*

src_only :
	$(MAKE) clean
	-$(DEL) haribote.img
