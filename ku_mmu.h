//
// Created by USER on 2021-04-27 (027).
//

#ifndef KU_MMU_KU_MMU_H
#define KU_MMU_KU_MMU_H

#define ZERO (0b00000000)
#define PRESENT (0b00000010)

#include <memory.h>

void* ku_mmu_free_list = NULL;
void* ku_mmu_busy_list = NULL;
void* ku_mmu_swap_space = NULL;

typedef struct page{
    void* next;
    void* pmem;
}page;

void ku_make_free_list(void* pmem, unsigned int  pmem_size){
    unsigned int page_num = pmem_size/4;
    int* iter_pmem = pmem;

    ku_mmu_free_list = malloc(sizeof(page));
    page* iter = ku_mmu_free_list;

    iter->pmem = iter_pmem;
    iter->next = NULL;
    page_num--;
    if(page_num > 1){
        for(int i = 0;i<page_num;i++){
            iter->next = malloc(sizeof (page));
            iter = iter->next;
            iter->pmem = iter_pmem+i;
            iter->next = NULL;
        }
    }
    else{
        return;
    }
}
void add_page_free_list(page* busy_page){
    // todo: 이것도 나중에 대답올라오는 것 보고 할지말지 정하기
}

void add_page_busy_list(page* busy_page){
    static page* last_page = NULL;
    if(busy_page){
        if(!ku_mmu_busy_list){
            busy_page->next=NULL;
            ku_mmu_busy_list = busy_page;
            last_page = ku_mmu_busy_list;
        }else{
            busy_page->next =NULL;
            last_page->next = busy_page;
            last_page = busy_page;
        }
    }
}
page* get_free_page(void* pmem){
    page* free_page = NULL;
    if(ku_mmu_free_list){
        free_page = ku_mmu_free_list;
        ku_mmu_free_list = free_page->next;
        add_page_busy_list(free_page);
    }
}
page* get_busy_page(){
    page* busy_page = NULL;
    if(ku_mmu_busy_list){
        busy_page = ku_mmu_busy_list;
        ku_mmu_free_list = busy_page->next;
    }
    return busy_page;
}
page* get_swap_page(){
    // todo: swap_page를 가져와서 다시 CPU위에 올리기
}


void destroy_free_list(){
    page* iter =ku_mmu_free_list;
    page* formal = NULL;
    while(iter){
        formal = iter;
        iter = iter+1;
        if(formal){
            free(formal);
        }
    }
}
void destroy_busy_list(){
    page* iter =ku_mmu_busy_list;
    page* formal = NULL;
    while(iter){
        formal = iter;
        iter = iter+1;
        if(formal){
            free(formal);
        }
    }
}

void* ku_mmu_init (unsigned int mem_size ,
                  unsigned int swap_size){
    // todo: 여기서 swapspace와 pmem을 구분해야한다.
    unsigned int total_size = sizeof(char)*(mem_size + swap_size);
    void* p_mem = malloc(total_size);
    if(p_mem == NULL){
        return NULL;
    }

    ku_make_free_list(p_mem,total_size);

    memset(p_mem,ZERO,total_size);

    return p_mem;
}
int ku_run_proc (char pid,
                 struct ku_pte **ku_cr3){

}
int ku_page_fault (char pid ,
                   char va){

}
int ku_traverse(void *, char, void *);

#endif //KU_MMU_KU_MMU_H
