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

static _hk_mem_stat_info_t _hk_info_;
static unsigned char _hk_start_ = 0;
static std::map<void*,unsigned long long int> _hk_alloc_rec_;

void _hk_start_mem_stat()
{
	_hk_start_ = 1;
	_hk_info_.alloced_ = 0;
	_hk_info_.released_ = 0;
	_hk_info_.mapped_ = 0;
	_hk_info_.unmapped_ = 0;
	_hk_alloc_rec_.clear();
}

void _hk_stop_mem_stat(const char * save_file)
{
	(void)save_file;
	_hk_start_ = 0;
	printf("alloced:%lld\n",_hk_info_.alloced_);
	printf("released:%lld\n",_hk_info_.released_);
	printf("mapped:%lld\n",_hk_info_.mapped_);
	printf("unmapped:%lld\n",_hk_info_.unmapped_);
}

void _hk_mem_alloced_inc(void* va, size_t size)
{
	if(unlikely(_hk_start_ == 0)) return;	
	if(unlikely(va == NULL)) return;	
	_hk_info_.alloced_ += size;
	_hk_alloc_rec_[va] = size;
}

void _hk_mem_released_inc(void* va)
{
	if(unlikely(_hk_start_ == 0)) return;
	if(unlikely(va == NULL)) return;	
	_hk_info_.released_ += _hk_alloc_rec_[va];
	_hk_alloc_rec_[va] = 0;
}

//
// ------------------------ hooks ------------------------ //
//
typedef void* (*some_malloc_fn)(size_t);
void * someMalloc(size_t size)
{
	some_malloc_fn raw;
	raw=(some_malloc_fn)dlsym(RTLD_NEXT,"someMalloc");
	void *va = raw(size);
	_hk_mem_alloced_inc(va,size);
	return va;
}

typedef void* (*some_free_fn)(void*);
void * someFree(void* va)
{
	some_free_fn raw;
	raw=(some_free_fn)dlsym(RTLD_NEXT,"someFree");
	void *ret = raw(va);
	_hk_mem_released_inc(va);
	return ret;
}
