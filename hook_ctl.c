#include <stdio.h>
#include <string>

extern void _hk_start_mem_stat();
extern void _hk_stop_mem_stat(const char* save_file);

int main(int c,char**v)
{
	if(c != 2){
		printf("usage:%s [cmd]\n cmd:start or stop\n",v[0]);
		return 0;
	}
	if(std::string(v[1]) == "start"){
		_hk_start_mem_stat();
		return 0;
	}
	if(std::string(v[1]) == "stop"){
		_hk_stop_mem_stat("mem_stat.info");
		return 0;
	}
	return 0;
}
