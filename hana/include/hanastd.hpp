#ifndef _HANASTD_HPP
#define _HANASTD_HPP

namespace hanastd {
	template <typename T>
	auto strlen(T str){
		auto i=str;
		for(;*i!=0;i++);
		return i-str+1;
	}
}

#endif
