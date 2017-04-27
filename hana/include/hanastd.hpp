#ifndef _HANASTD_HPP
#define _HANASTD_HPP

namespace hanastd {
	template <typename T>
	int strlen(T const& str){
		auto i=&str;
		for(;*i!=0;i++);
		return i-&str+1;
	}
}

#endif
