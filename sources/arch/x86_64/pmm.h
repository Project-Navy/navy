#pragma once 

#include <navy/handover.h>
#include <navy/range.h>

#define MMAP_IO_BASE ((uintptr_t) 0xffff800000000000)
#define MMAP_KERNEL_BASE ((uintptr_t) 0xffffffff80000000)

struct alloc_metadata 
{
    size_t size;
    size_t page_count;
};

void pmm_init(struct handover *handover);
size_t get_usable_pages(void);
struct range pmm_alloc(size_t amount);
void pmm_free(struct range range);
void pmm_unused(range_t range);
range_t get_pmm_metadata(void *ptr);