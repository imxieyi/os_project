default:ipl.nas helloos_nas.img

ipl.bin:ipl.nas Makefile
	../tolset/z_tools/nask.exe ipl.nas ipl.bin ipl.lst

helloos_nas.img:ipl.bin Makefile
	../tolset/z_tools/edimg.exe imgin:../tolset/z_tools/fdimg0at.tek wbinimg src:ipl.bin len:512 from:0 to:0 imgout:helloos_nas.img
