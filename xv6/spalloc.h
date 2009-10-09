struct spalloc_page_elmt
{
    char *mem;
    int freebytes; 
    int next_free_at;
    struct spalloc_page_elmt *next;
    //would need 114 ints to store one bit per record

};

typedef struct spalloc_page_elmt spalloc_page;

spalloc_page* head_spalloc_page;
spalloc_page* curr_spalloc_page;
int hasOnePage;

char* spalloc(int i);
