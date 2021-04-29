//
// Created by USER on 2021-04-27 (027).
//

#ifndef KU_MMU_KU_MMU_H
#define KU_MMU_KU_MMU_H

#define ZERO (0b00000000)
#define PRESENT (0b00000010)
#define GET_PD (0b11000000)
#define GET_PMD (0b00110000)
#define GET_PT (0b00001100)
#define GET_OFFSET (0b00000011)

#include <memory.h>

void* ku_mmu_free_list = NULL;
void* ku_mmu_busy_list = NULL;
void* ku_mmu_swap_space = NULL;
void* ku_mmu_PCBs = NULL;


typedef struct page{
    void* next;
    void* p_mem;
}page;

typedef struct PCB{
    int pid;
    page* PD_BR;
    int program_counter;
    void* next;
}PCB;

int get_PD_num(int VirAdd){
    int pd_num = GET_PD & VirAdd;
    pd_num >> 6;
    return pd_num;
}
int get_PMD_num(int VirAdd){
    int pmd_num = GET_PMD & VirAdd;
    pmd_num >> 4;
    return pmd_num;
}
int get_PT_num(int VirAdd){
    int pt_num = GET_PT & VirAdd;
    pt_num >> 2;
    return pt_num;
}
int get_offset_num(int VirAdd){
    int offset = GET_OFFSET & VirAdd;
    return offset;
}

void ku_make_free_list(void* p_mem, unsigned int  p_mem_size){
    unsigned int page_num = p_mem_size/4;
    int* iter_p_mem = page_num;

    ku_mmu_free_list = malloc(sizeof(page));
    page* iter = ku_mmu_free_list;

    iter->p_mem = iter_p_mem;
    iter->next = NULL;
    page_num--;
    if(page_num > 1){
        for(int i = 0;i<page_num;i++){
            iter->next = malloc(sizeof (page));
            iter = iter->next;
            iter->p_mem = iter_p_mem+i;
            iter->next = NULL;
        }
    }
    else{
        return;
    }
}

// todo: ku_make_swap_space(p_mem,mem_size);

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
page* get_free_page(){
    page* free_page = NULL;
    if(ku_mmu_free_list){
        free_page = ku_mmu_free_list;
        ku_mmu_free_list = free_page->next;
        add_page_busy_list(free_page);
    }
    return free_page;
}
page* get_busy_page(){
    page* busy_page = NULL;
    if(ku_mmu_busy_list){
        busy_page = ku_mmu_busy_list;
        ku_mmu_free_list = busy_page->next;
    }
    return busy_page;
}
page* set_swap_page(int VirAdd){
    // todo: swap_page 를 가져와서 다시 CPU 위에 올리기
}
page* remapping_page(){


}
PCB* get_pcb(int pid){
    PCB * iter = ku_mmu_PCBs;
    while(iter->next = NULL){
        if(iter->pid == pid){
            return iter;
        }
        iter = iter->next;
    }
    return NULL;
}

void* allocate_page(int pid, int VirAdd){
    int pd_num = get_PD_num(VirAdd);
    int pmd_num = get_PMD_num(VirAdd);
    int pt_num = get_PT_num(VirAdd);
    int offset = get_offset_num(VirAdd);

    PCB* pcb = get_pcb(pid);

    int* pde = (int*)((pcb->PD_BR)->p_mem)+pd_num;

    int* pfn = NULL;

    int* p_mde = (int*)(pde)+pmd_num;
    if(p_mde == NULL){
        if(p_mde = get_free_page()){
            return NULL;
        }
    }
    int* pte = (int*)(p_mde)+pt_num;
    if(pte == NULL){
        if(pte = get_free_page()){
            return NULL;
        }
        *(int*)(((page*)pte)->p_mem) |= PRESENT;
        pfn = (int*)(pte)+offset;
    }else{
        if(*(int*)(((page*)pte)->p_mem) & PRESENT){
            pfn = (int*)(pte)+offset;
        }else{
            // todo: get page from swap_space;
        }
    }
    return pfn;
}



struct ku_pte* set_process(int pid){
    static int program_counter = 0;

    PCB * iter = ku_mmu_PCBs;
    while(iter->next = NULL){
        if(iter->pid == pid){
            return iter->PD_BR->p_mem;
        }
        iter = iter->next;
    }
    iter->next = malloc(sizeof(PCB));
    ((PCB*)(iter->next))->pid = pid;
    page* PD = get_free_page();
    ((PCB*)(iter->next))->PD_BR = PD;
    ((PCB*)(iter->next))->next = NULL;
    ((PCB*)(iter->next))->program_counter = program_counter;
    program_counter++;

    return PD->p_mem;
}

PCB* get_PCB(int pid){
    // todo: optimize make process and get program counter (possibly hash_map)
}

int check_mapped(int VirAdd){

}



void* ku_mmu_init (unsigned int mem_size ,
                  unsigned int swap_size){
    // todo: 여기서 swap_space 와 p_ mem 을 구분해야한다.
    unsigned int total_size = sizeof(char)*(mem_size + swap_size);
    void* p_mem = malloc(total_size);
    if(p_mem == NULL){
        return NULL;
    }

    ku_make_free_list(p_mem,mem_size);
    // todo: ku_make_swap_space(p_mem,mem_size);

    memset(p_mem,ZERO,total_size);

    return p_mem;
}
int ku_run_proc (char pid,
                 struct ku_pte **ku_cr3){
    *ku_cr3 = set_process(pid);
}
int ku_page_fault (char pid ,
                   char va){
// todo : va 자리에 1) 차지하고 있는 애를 스왑 스페이스로 내쫓고, 자리 내주기 2) 또 자리가 없으면 그냥 0
    return allocate_page(pid,va);

}
int ku_traverse(void *, char, void *);


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
        iter = iter->next;
        if(formal){
            free(formal);
        }
    }
}
void destroy_swap_list(){
    page* iter =ku_mmu_swap_space;
    page* formal = NULL;
    while(iter){
        formal = iter;
        iter = iter->next;
        if(formal){
            free(formal);
        }
    }
}
void destroy_PCB_list(){
    page* iter =ku_mmu_PCBs;
    page* formal = NULL;
    while(iter){
        formal = iter;
        iter = iter->next;
        if(formal){
            free(formal);
        }
    }
}

#endif //KU_MMU_KU_MMU_H

