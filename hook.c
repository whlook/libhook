#include <stdio.h>
#include <unistd.h>
#include <dlfcn.h>
#include <map>

#define unlikely(x) __builtin_expect(!!(x), 0)
#define likely(x) __builtin_expect(!!(x), 1)

typedef struct _hk_mem_stat_info{
	unsigned long long int alloced_;
	unsigned long long int released_;
	unsigned long long int mapped_; // TODO
	unsigned long long int unmapped_; // TODO
} _hk_mem_stat_info_t;

class _HK_STAT_
{

public:
	_HK_STAT_()
	{
		_hk_info_.alloced_ = 0;
		_hk_info_.released_ = 0;
		_hk_info_.mapped_ = 0;
		_hk_info_.unmapped_ = 0;
		_hk_alloc_rec_.clear();
	}

	~_HK_STAT_()
	{
		printf("alloced:%lld\n",_hk_info_.alloced_);
		printf("released:%lld\n",_hk_info_.released_);
		printf("mapped:%lld\n",_hk_info_.mapped_);
		printf("unmapped:%lld\n",_hk_info_.unmapped_);
	}

	void _hk_mem_alloced_inc(void* va, size_t size)
	{
		if(unlikely(va == NULL)) return;	
		_hk_info_.alloced_ += size;
		_hk_alloc_rec_[va] = size;
	}

	void _hk_mem_released_inc(void* va)
	{
		if(unlikely(va == NULL)) return;	
		_hk_info_.released_ += _hk_alloc_rec_[va];
		_hk_alloc_rec_[va] = 0;
	}

private:
	_hk_mem_stat_info_t _hk_info_;
	std::map<void*,unsigned long long int> _hk_alloc_rec_;
};

static _HK_STAT_ _hk_stat_;
//
// ------------------------ hooks ------------------------ //
// dont hook system malloc/free
//
extern "C" {

typedef unsigned char * (*some_malloc_fn)(size_t);
unsigned char* some_malloc(size_t size)
{
	some_malloc_fn raw;
	raw=(some_malloc_fn)dlsym(RTLD_NEXT,"some_malloc");
	unsigned char*va = (unsigned char*)raw(size);
	_hk_stat_._hk_mem_alloced_inc((void*)va,size);
	return va;
}

typedef int (*some_free_fn)(unsigned char*);
int some_free(unsigned char* va)
{
	some_free_fn raw;
	raw=(some_free_fn)dlsym(RTLD_NEXT,"some_free");
	int ret = raw(va);
	_hk_stat_._hk_mem_released_inc((void*)va);
	return ret;
}

}
