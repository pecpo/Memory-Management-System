// All the main functions with respect to the MeMS are inplemented here
// read the function discription for more details

// NOTE: DO NOT CHANGE THE NAME OR SIGNATURE OF FUNCTIONS ALREADY PROVIDED
// you are only allowed to implement the functions 
// you can also make additional helper functions a you wish

// REFER DOCUMENTATION FOR MORE DETAILS ON FUNSTIONS AND THEIR FUNCTIONALITY

// add other headers as required

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/mman.h>

/*
Use this macro where ever you need PAGE_SIZE.
As PAGESIZE can differ system to system we should have flexibility to modify this 
macro to make the output of all system same and conduct a fair evaluation. 
*/

#define PAGE_SIZE 4096

typedef struct Node {
    int type; // 0 for HOLE, 1 for PROCESS
    void* start_addr;
    void* end_addr;
    struct Node* next;
    struct Node* prev;
} Node;

typedef struct Chain {
    size_t size; // Number of pages, also denotes total memory in that subchain
    size_t offset;
    struct Node* sub_chain;
    struct Chain* next;
    struct Chain* prev;
} Chain;

Node* internal_nodes_head;
Node* internal_nodes_ptr;
Chain* internal_chains_head;
Chain* internal_chains_ptr;
Chain* free_list_head;
int chainsCount;
int firstTime;
int baseVirtualAddress;

Node* internal_node_create();
Chain* internal_chain_create();

/*
Initializes all the required parameters for the MeMS system. The main parameters to be initialized are:
1. the head of the free list i.e. the pointer that points to the head of the free list
2. the starting MeMS virtual address from which the heap in our MeMS virtual address space will start.
3. any other global variable that you want for the MeMS implementation can be initialized here.
Input Parameter: Nothing
Returns: Nothing
*/

void mems_init(){
    chainsCount=0;
    firstTime=1;
    baseVirtualAddress=1000;
    free_list_head=NULL;
    internal_nodes_head=(Node*)mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    internal_nodes_ptr=internal_nodes_head;
    internal_chains_head=(Chain*)mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    internal_chains_ptr=internal_chains_head;
}

/*
This function will be called at the end of the MeMS system and its main job is to unmap the 
allocated memory using the munmap system call.
Input Parameter: Nothing
Returns: Nothing
*/

void mems_finish(){
    Chain* CurrentChain=free_list_head;
    while(CurrentChain->next!=NULL){
        if(munmap((void*)CurrentChain->sub_chain->start_addr,(CurrentChain->size))==-1){
            printf("mems_finish failed!\n");
            perror("munmap");
        }
        CurrentChain=CurrentChain->next;
    }
    munmap((void*)internal_chains_head,PAGE_SIZE);
    munmap((void*)internal_nodes_head,PAGE_SIZE);
    free_list_head=NULL;
    
}

/*
Allocates memory of the specified size by reusing a segment from the free list if 
a sufficiently large segment is available. 

Else, uses the mmap system call to allocate more memory on the heap and updates 
the free list accordingly.

Note that while mapping using mmap do not forget to reuse the unused space from mapping
by adding it to the free list.
Parameter: The size of the memory the user program wants
Returns: MeMS Virtual address (that is created by MeMS)
*/ 

void* mems_malloc(size_t size){
    size_t allocationSize=0;
    if(size%PAGE_SIZE==0){
        allocationSize = size;
    }
    else{
        allocationSize = ((size / PAGE_SIZE) + 1) * PAGE_SIZE;
    }
    if(firstTime){
        Node* newProcessNode=internal_node_create();
        newProcessNode->start_addr=mmap(NULL, allocationSize, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        newProcessNode->end_addr=newProcessNode->start_addr+(size-1);
        newProcessNode->type=1;
        if(size==allocationSize){
            newProcessNode->next=NULL;
            newProcessNode->prev=NULL;
            Chain* newChain=internal_chain_create();
            free_list_head=newChain;
            newChain->offset=baseVirtualAddress;
            newChain->sub_chain=newProcessNode;
            newChain->next=NULL;
            newChain->prev=NULL;
            newChain->size=allocationSize-1;
            firstTime=0;
            chainsCount++;
            return (void*)newChain->offset;
        }
        else{
            Node* newHoleNode=internal_node_create();
            newHoleNode->start_addr=newProcessNode->end_addr+1;
            newHoleNode->end_addr=newProcessNode->start_addr+allocationSize-1;
            newHoleNode->type=0;
            newProcessNode->next=newHoleNode;
            newProcessNode->prev=NULL;
            newHoleNode->next=NULL;
            newHoleNode->prev=newProcessNode;
            Chain* newChain=internal_chain_create();
            free_list_head=newChain;
            newChain->offset=baseVirtualAddress;
            newChain->sub_chain=newProcessNode;
            newChain->next=NULL;
            newChain->prev=NULL;
            newChain->size=allocationSize-1;
            firstTime=0;
            chainsCount++;
            return (void*)newChain->offset;
        }
    }

    Chain* currentChain = free_list_head;
    for(int i=0;i<chainsCount;i++){
        Node* currentNode = currentChain->sub_chain;
        while (currentNode != NULL) {
            if (currentNode->type == 0 && currentNode->end_addr-currentNode->start_addr+1 >=size) {
                void* old_end=currentNode->end_addr;
                currentNode->type = 1;
                currentNode->end_addr = currentNode->start_addr + size - 1;
                if(size==currentNode->end_addr-currentNode->start_addr+1 && currentNode->next!=NULL){
                    return (void*)currentChain->offset+(currentNode->start_addr-currentChain->sub_chain->start_addr);
                }
                else{
                    Node* newHoleNode=internal_node_create();
                    newHoleNode->start_addr=currentNode->end_addr+1;
                    newHoleNode->end_addr=old_end;
                    newHoleNode->type=0;
                    currentNode->next=newHoleNode;
                    newHoleNode->prev=currentNode;
                    newHoleNode->next=NULL;
                    return (void*)currentChain->offset+(currentNode->start_addr-currentChain->sub_chain->start_addr);
                }
            }
            currentNode = currentNode->next;
        }
        currentChain = currentChain->next;
    }

    currentChain = free_list_head;
    while(currentChain->next!=NULL){
        currentChain=currentChain->next;
    }
    Chain* newChain=internal_chain_create();
    currentChain->next=newChain;
    newChain->prev=currentChain;
    newChain->next=NULL;
    newChain->size=allocationSize-1;
    newChain->offset=currentChain->offset+currentChain->size+1;
    Node* newProcessNode=internal_node_create();
    newProcessNode->start_addr=mmap(NULL, allocationSize, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if(newProcessNode->start_addr==MAP_FAILED){
        perror("Mmap failed");
        exit(1);
    }
    newProcessNode->end_addr=newProcessNode->start_addr+(size-1);
    newProcessNode->type=1;
    newChain->sub_chain=newProcessNode;
    if(size==allocationSize){
        newProcessNode->next=NULL;
        newProcessNode->prev=NULL;
        chainsCount++;
        return (void*)newChain->offset;
    }
    else{
        Node* newHoleNode=internal_node_create();
        newHoleNode->start_addr=newProcessNode->end_addr+1;
        newHoleNode->end_addr=newProcessNode->start_addr+allocationSize-1;
        newHoleNode->type=0;
        newProcessNode->next=newHoleNode;
        newProcessNode->prev=NULL;
        newHoleNode->next=NULL;
        newHoleNode->prev=newProcessNode;
        chainsCount++;
        return (void*)newChain->offset;
    }
}

/*
this function print the stats of the MeMS system like
1. How many pages are utilised by using the mems_malloc
2. how much memory is unused i.e. the memory that is in freelist and is not used.
3. It also prints details about each node in the main chain and each segment (PROCESS or HOLE) in the sub-chain.
Parameter: Nothing
Returns: Nothing but should print the necessary information on STDOUT
*/

void mems_print_stats(){
    Chain* currentChain=free_list_head;
    size_t hole_memory=0;
    int pages_used=0;
    int main_length=0;
    for(int i=0;i<chainsCount;i++){
        printf("MAIN[%zu:%zu]->",currentChain->offset,currentChain->offset+currentChain->size);
        Node* currentNode=currentChain->sub_chain;
        size_t start_ptr=currentChain->offset;
        pages_used=pages_used+((currentChain->size+1)/PAGE_SIZE);
        while(currentNode!=NULL){
            if(currentNode->type==1){
                printf("P[%zu:%zu]<->",start_ptr,start_ptr+currentNode->end_addr-currentNode->start_addr);
                start_ptr=start_ptr+(currentNode->end_addr-currentNode->start_addr+1);
            }
            else if(currentNode->type==0){
                printf("H[%zu:%zu]<->",start_ptr,start_ptr+currentNode->end_addr-currentNode->start_addr);
                start_ptr=start_ptr+(currentNode->end_addr-currentNode->start_addr+1);
                hole_memory=hole_memory+(currentNode->end_addr-currentNode->start_addr+1);
            }
            currentNode=currentNode->next;
        }
        printf("NULL");
        printf("\n");
        currentChain=currentChain->next;
        main_length++;
    }
    int sub_chain[main_length];
    currentChain=free_list_head;
    for(int i=0;i<chainsCount;i++){
        Node* currentNode=currentChain->sub_chain;
        int sub_chain_length=0;
        while (currentNode!=NULL){
            sub_chain_length++;
            currentNode=currentNode->next;
        }
        sub_chain[i]=sub_chain_length;
        currentChain=currentChain->next;
    }
    printf("Used Pages:%d\n",pages_used);
    printf("Unused memory:%zu\n",hole_memory);
    printf("Main Chain Length:%d\n",main_length);
    printf("Sub-chain length array:[");
    for(int i=0;i<main_length;i++){
        printf("%d,",sub_chain[i]);
    }
    printf("]\n");
}

/*
Returns the MeMS physical address mapped to ptr ( ptr is MeMS virtual address).
Parameter: MeMS Virtual address (that is created by MeMS)
Returns: MeMS physical address mapped to the passed ptr (MeMS virtual address).
*/

void *mems_get(void*v_ptr){
    
    Chain* CurrentChain=free_list_head;
    while(CurrentChain->next!=NULL){
        if((size_t)v_ptr>=CurrentChain->offset){
            break;
        }
        CurrentChain=CurrentChain->next;
    }
    
    return (void*)CurrentChain->sub_chain->start_addr+(size_t)v_ptr-CurrentChain->offset;
    
}

/*
this function free up the memory pointed by our virtual_address and add it to the free list
Parameter: MeMS Virtual address (that is created by MeMS) 
Returns: nothing
*/

void mems_free(void *v_ptr){
    Chain* CurrentChain=free_list_head;
    while(CurrentChain->next!=NULL){
        if((size_t)v_ptr>=CurrentChain->offset && (size_t)v_ptr<CurrentChain->next->offset){
            break;
        }
        CurrentChain=CurrentChain->next;
    }
    Node* currentNode=CurrentChain->sub_chain;
    while(currentNode!=NULL){
        if((size_t)v_ptr<=CurrentChain->offset+(currentNode->start_addr-CurrentChain->sub_chain->start_addr)){
            break;
        }
        currentNode=currentNode->next;
    }
    currentNode->type=0;
    Node* itrnode=CurrentChain->sub_chain;

    int flag=0;
    while(itrnode!=NULL){
        if(itrnode->next!=NULL){
            if(itrnode->type==0 && itrnode->next->type==0){
                flag=1;
                break;
            }
        }
        itrnode=itrnode->next;
    }
    if(!flag){return;}

    while(itrnode->next!=NULL){
        if(itrnode->next->type!=0){
            break;
        }
        if(itrnode->next->type==0){

            itrnode->end_addr=itrnode->next->end_addr;
            itrnode->next->end_addr=NULL;
            if(itrnode->next->next!=NULL){
                itrnode->next->next->prev=itrnode;
                itrnode->next=itrnode->next->next;  
            }
            else{
                itrnode->next=NULL;
            }
        }
        else{
            break;
        }
    }
}

/*
Internal functions to create new nodes and chains
*/

Node* internal_node_create(){
    Node* ret;
    if((size_t)internal_nodes_ptr+(size_t)sizeof(Node)<(size_t)internal_nodes_head+(size_t)PAGE_SIZE){
        ret=internal_nodes_ptr;
        internal_nodes_ptr++;
    }
    else{
        Node* new_ptr=(Node*)mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        internal_nodes_ptr=new_ptr;
        internal_nodes_head=new_ptr;
        ret=internal_nodes_ptr;
        internal_nodes_ptr++;
    }
    return ret;
}

Chain* internal_chain_create(){
    Chain* ret;
    if((uintptr_t)internal_chains_ptr+sizeof(Chain)<(uintptr_t)internal_chains_head+PAGE_SIZE){
        ret=internal_chains_ptr;
        internal_chains_ptr++;
    }
    else{
        Chain* new_ptr=(Chain*)mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        internal_chains_ptr=new_ptr;
        internal_chains_head=new_ptr;
        ret=internal_chains_ptr;
        internal_chains_ptr++;
    }
    return ret;
}
