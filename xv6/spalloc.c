#include "spalloc.h"

char* spalloc(int i)
{
    char* newMem; 
    spalloc_page* sp_page;

    if( hasOnePage == 0)
    {
        newMem = kalloc(4096);
        hasOnePage = 1;
        sp_page = (spalloc_page*)newMem;
        sp_page->next = 0;
        sp_page->mem = newMem;

        sp_page->freebytes = 4096;
        sp_page->next_free_at = 0;
        sp_page->freebytes -= i;
        sp_page->next_free_at = sp_page->next_free_at + i;

        head_spalloc_page = sp_page;
        curr_spalloc_page = sp_page;
        return sp_page->mem;
    }

    if(i < curr_spalloc_page->freebytes)
    {
        curr_spalloc_page->freebytes -= i;
        curr_spalloc_page->next_free_at += i;
        return curr_spalloc_page->mem[curr_spalloc_page->next_free_at];
    }
    else
    {
        spalloc_page* new_sp_page;

        newMem = kalloc(4096);
        new_sp_page = (spalloc_page*)newMem;
        sp_page->next = new_sp_page;
        new_sp_page = 0;
        new_sp_page->freebytes = 4096;
        new_sp_page->next_free_at = 0;
        sp_page = new_sp_page;
        sp_page->freebytes -= i;
        sp_page->next_free_at += i;
    }
    
}
