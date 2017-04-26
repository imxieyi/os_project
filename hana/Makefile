TOOLPATH = ../tolset/z_tools/

OBJS_BOOTPACK = build/bootpack.obj build/naskfunc.obj build/hankaku.obj build/graphics.obj \
	 build/gdtidt.obj build/interrupt.obj build/fifo.obj build/keyboard.obj build/mouse.obj \
	 build/memory.obj build/sheet.obj build/timer.obj

MAKE     = $(TOOLPATH)make.exe -r
NASK     = $(TOOLPATH)nask.exe
CXX      = g++ -I./depends -I. -I./include -std=gnu++11 -Wall -Wextra -nostdlib -nostdinc -nostartfiles -nodefaultlibs -fno-builtin -c -fno-rtti -fpermissive -fno-exceptions -fpic
# -fno-stack-check -fno-stack-protector -mno-stack-arg-probe 这些参数即使加上也不能使用相应的特性
OBJ2BIM  = $(TOOLPATH)obj2bim.exe
MAKEFONT = $(TOOLPATH)makefont.exe
BIN2OBJ  = $(TOOLPATH)bin2obj.exe
BIM2HRB  = $(TOOLPATH)bim2hrb.exe
RULEFILE = depends/hanaos.rul
EDIMG    = $(TOOLPATH)edimg.exe
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

%.obj : %.cpp Makefile
	$(CXX) -o build/$*.obj $*.cpp

hankaku.bin : hankaku.txt Makefile
	$(MAKEFONT) hankaku.txt build/hankaku.bin

hankaku.obj : hankaku.bin Makefile
	$(BIN2OBJ) build/hankaku.bin build/hankaku.obj _hankaku

bootpack.bim : bootpack.obj naskfunc.obj hankaku.obj graphics.obj gdtidt.obj timer.obj \
			   interrupt.obj fifo.obj keyboard.obj mouse.obj memory.obj sheet.obj Makefile
	$(OBJ2BIM) @$(RULEFILE) out:build/bootpack.bim stack:3136k map:build/bootpack.map \
		$(OBJS_BOOTPACK)
# 3MB+64KB=3136KB

bootpack.hana : bootpack.bim Makefile
	$(BIM2HRB) build/bootpack.bim build/bootpack.hana 0

hanaos.sys : asmhead.bin bootpack.hana Makefile
#	cmd /c copy /B build\asmhead.bin+build\bootpack.hana build\hanaos.sys
	cat build/asmhead.bin build/bootpack.hana > build/hanaos.sys

hanaos.img : ipl.bin hanaos.sys Makefile
	$(EDIMG)   imgin:$(TOOLPATH)fdimg0at.tek \
		wbinimg src:build/ipl.bin len:512 from:0 to:0 \
		copy from:build/hanaos.sys to:@: \
		imgout:build/hanaos.img

# コマンド

img :
	$(MAKE) hanaos.img

run :
	$(MAKE) img
	$(COPY) build/hanaos.img $(TOOLPATH)qemu/fdimage0.bin
#	$(COPY) build\hanaos.img ..\tolset\z_tools\qemu\fdimage0.bin
	$(MAKE) -C $(TOOLPATH)qemu

clean :
	-$(DEL) build/*.*

src_only :
	$(MAKE) clean
	-$(DEL) hanaos.img
