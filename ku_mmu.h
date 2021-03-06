//
// Created by USER on 2021-04-27 (027).
//

#ifndef KU_MMU_KU_MMU_H
#define KU_MMU_KU_MMU_H

/* 플래그 */
#define ZERO (0b00000000)
#define PRESENT (0b00000010)
#define SET_INVALID (0b11111110)
#define SET_VALID (0b11111111)
#define GET_PD (0b11000000)
#define GET_PMD (0b00110000)
#define GET_PT (0b00001100)
#define GET_OFFSET (0b00000011)

#include <memory.h>
#include <stdlib.h>
#include <stdio.h>

/* 구조체 */
typedef union pte{
    char pte_val;
    struct{
        char b0 : 1;
        char b1 : 1;
        char b2 : 1;
        char b3 : 1;
        char b4 : 1;
        char b5 : 1;
        char b6 : 1;
        char b7 : 1;
    }bits;
}pte;

typedef struct page{
    void* next;
    pte kuPte[4];
    void** p_mem;
    void* parent;
}page;

typedef struct PCB{
    int pid;
    page* PD_BR;
    void* next;
}PCB;

/* 전역변수 */
void* ku_mmu_pfn = NULL;
page** ku_mmu_swap_space = NULL;

void* ku_mmu_free_list = NULL;
void* ku_mmu_busy_list = NULL;

void* ku_mmu_PCBs = NULL;


/* 오프셋 */
int get_PD_offset(const int VirAdd){
    int pd_num = GET_PD & VirAdd;
    pd_num = pd_num >> 6;
    return pd_num;
}
int get_PMD_offset(const int VirAdd){
    int pmd_num = GET_PMD & VirAdd;
    pmd_num = pmd_num>> 4;
    return pmd_num;
}
int get_PT_offset(const int VirAdd){
    int pt_num = GET_PT & VirAdd;
    pt_num =pt_num >> 2;
    return pt_num;
}
int get_offset_offset(const int VirAdd){
    int offset = GET_OFFSET & VirAdd;
    return offset;
}


/* 초기화 */
void* ku_make_pfn_list(void* const p_mem,const unsigned int  p_mem_size){
    unsigned int page_num = p_mem_size/4;

    ku_mmu_pfn = malloc(sizeof(page));
    if(ku_mmu_pfn == NULL){
        return NULL;
    }
    ku_mmu_free_list = ku_mmu_pfn;
    page* iter = ku_mmu_pfn;

    for(int i = 0;i<4;i++){
        iter->kuPte[i].pte_val &= ZERO;
    }
    iter->p_mem = malloc(sizeof(int*)*4);
    if(iter->p_mem == NULL){
    	return NULL;
    }
    *(iter->p_mem) = p_mem;
    iter->parent = NULL;
    iter->next = NULL;


    page_num--;
    if(page_num > 1){
        for(int i = 0;i<page_num;i++){

		iter->next = malloc(sizeof (page));
            iter = iter->next;
            if(iter==NULL){
                return NULL;
            }
            for(int j = 0;j<4;j++){
                iter->kuPte[j].pte_val &= ZERO;
	        }
     	    iter->p_mem = malloc(sizeof(int*)*4);
            *(iter->p_mem) = (((int*)p_mem) + i);
            iter->parent = NULL;
            iter->next = NULL;
        }

	return ku_mmu_pfn;
    }
    else{
        return ku_mmu_pfn;
    }
}
void* ku_make_swap_space(unsigned  int swap_size){
    unsigned int swap_num = swap_size/4;

    ku_mmu_swap_space = malloc(sizeof(page*)*swap_num);
    if(ku_mmu_swap_space == NULL){
        return NULL;
    }
    for(int i =0;i<swap_num;i++){
        *(ku_mmu_swap_space+i) = malloc(sizeof(page));
        for(int j = 0;j<4;j++){
            (*(ku_mmu_swap_space+i))->kuPte[j].pte_val &= ZERO;
        }
        (*(ku_mmu_swap_space+i))->p_mem=malloc(sizeof(int*)*4);
        *((*(ku_mmu_swap_space+i))->p_mem)=((int*)(ku_mmu_swap_space))+i;
        (*(ku_mmu_swap_space+i))->parent = NULL;
        (*(ku_mmu_swap_space+i))->next = NULL;
    }
    return ku_mmu_swap_space;

}

void* ku_mmu_init (unsigned int mem_size ,
                  unsigned int swap_size){
    void* p_mem = malloc(mem_size);
    if(p_mem == NULL){
        return NULL;
    }

    ku_mmu_pfn = ku_make_pfn_list(p_mem,mem_size);
    if(ku_mmu_pfn == NULL){
        return NULL;
    }
    ku_mmu_swap_space = ku_make_swap_space(swap_size);
    if(ku_mmu_swap_space == NULL){
        return NULL;
    }

    memset(p_mem,ZERO,mem_size);

    return p_mem;
}

/* 프로세스 선택(생성) */
void add_page_busy_list(page* const busy_page, const int isTable){
    static page* last_page = NULL;
    if(isTable == 1){
        return;
    }
    if(busy_page){
        if(!ku_mmu_busy_list){
            busy_page->next=NULL;
            ku_mmu_busy_list = busy_page;
            last_page = busy_page;
        }else{
            busy_page->next =NULL;
            last_page = busy_page;
	    ku_mmu_busy_list= busy_page;
        }
    }
}
page* get_free_page(const int isTable){
    page* free_page = NULL;
    if(ku_mmu_free_list != NULL){
        free_page = ku_mmu_free_list;
        ku_mmu_free_list = free_page->next;
        add_page_busy_list(free_page,isTable);
    }else{
        return NULL;
    }
    return free_page;
}

void* make_PCB_return_PD_BR(const int pid){

    PCB * iter = ku_mmu_PCBs;
    while(iter != NULL){
        if(iter->pid == pid){
        	return *(iter->PD_BR->p_mem);
	}
        iter = iter->next;
    }
    iter = malloc(sizeof(PCB));
    if(iter == NULL){
    	return NULL;
    }
    iter->pid = pid;
    page* PD = get_free_page(1);
    if(PD==NULL){
        return NULL;
    }
    iter->PD_BR = PD;
    iter->next = NULL;

    if(ku_mmu_PCBs == NULL){
    	ku_mmu_PCBs = iter;
    }
    return *(PD->p_mem);
}
int ku_run_proc (char pid,
                 void** ku_cr3){
    *ku_cr3 = make_PCB_return_PD_BR(pid);

    if(*ku_cr3 == NULL){
        return -1;
    }
    return 0;
}

/* 프로세스 메모리 할당 */
void* page_to_swap_space(page* parent_page, int index){
    page* page_to_swap = *((parent_page->p_mem)+index);
    char pte_to_swap = parent_page->kuPte[index].pte_val;

    char swap_offset = pte_to_swap >> 1;

    ku_mmu_swap_space[swap_offset] = page_to_swap;

    parent_page->kuPte[index].pte_val &= SET_INVALID;

    return page_to_swap;
}

void* page_from_swap_space(char pte_to_return){
    page* page_to_return = NULL;

    char return_offset = pte_to_return >> 1;

    page_to_return = ku_mmu_swap_space[return_offset];

    for(int i = 0;i<4;i++){
        if(page_to_return ==  *(((page**)((page*)page_to_return->parent)->p_mem)+i)){
            ((page*)(page_to_return->parent))->kuPte[i].pte_val &= SET_VALID;
            break;
        }
    }
    return page_to_return;
}

page* get_busy_page(char pte){
    page* busy_page = NULL;
    if(ku_mmu_busy_list){
        busy_page = ku_mmu_busy_list;
        ku_mmu_free_list = busy_page->next;

        for(int i = 0;i<4;i++) {
            if (busy_page == *(((page**)((page*)busy_page->parent)->p_mem)+i)) {
                page_to_swap_space(busy_page->parent,i);
                busy_page = page_from_swap_space(pte);
                break;
            }
        }
    }
    return busy_page;
}

PCB* get_pcb(const int pid){

    PCB * iter = ku_mmu_PCBs;
    while(iter != NULL){
        if(iter->pid == pid){
            return iter;
        }
        iter = iter->next;
    }
    return NULL;
}

void* allocate_page(const int pid, const int VirAdd){
    int pd_num = get_PD_offset(VirAdd);
    int pmd_num = get_PMD_offset(VirAdd);
    int pt_num = get_PT_offset(VirAdd);
    int offset = get_offset_offset(VirAdd);

    PCB* pcb = get_pcb(pid);

    page* PD;
    page* PMD;
    page* PT;
    page* pfn;

    char p_de;
    char pm_de;
    char p_te;

    PD = (pcb->PD_BR);
    p_de = (PD->kuPte[pd_num].pte_val);

    if(p_de & PRESENT){
        PMD = (page*)*((page**)(PD->p_mem + pd_num));
    }else{
        page* newPage = get_free_page(1);
        if(newPage == NULL){
            return NULL;
        }
        *((page**)(PD->p_mem)+pd_num) = newPage;
        PMD = newPage;
        PMD->parent = PD;
        PD->kuPte[pd_num].pte_val |= PRESENT;
    }

    pm_de = PMD->kuPte[pmd_num].pte_val;
    if(pm_de & PRESENT){
        PT = (page*)*((page**)(PMD->p_mem + pmd_num));
    }else{
        page* newPage = get_free_page(1);
        if(newPage == NULL){
            return NULL;
        }
        *((page**)(PMD->p_mem)+pmd_num) = newPage;
        PT = newPage;
        PT->parent = PMD;
        PMD->kuPte[pmd_num].pte_val |= PRESENT;
    }

    p_te = PT->kuPte[pt_num].pte_val;
    if(p_te & PRESENT){
        pfn = (page*)*((page**)(PT->p_mem + pt_num));
    }else if(pm_de == ZERO){
        page* newPage = get_free_page(0);
        if(newPage == NULL){
            return NULL;
        }
        *((page**)(PT->p_mem)+pt_num) = newPage;
        pfn = newPage;
        pfn->parent = PT;
        PT->kuPte[pt_num].pte_val |= PRESENT;
    }else{

	    pfn = get_busy_page(p_te);
    }
    return pfn;
}

int ku_page_fault (char pid ,
                   char va){
    void* result =  allocate_page(pid,va);

    if(result == NULL){
        return -1;
    }else{
        return 0;
    }
}
int ku_traverse(void *, char, void *);



#endif //KU_MMU_KU_MMU_H

