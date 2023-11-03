// All the main functions with respect to the MeMS are inplemented here
// read the function discription for more details

// NOTE: DO NOT CHANGE THE NAME OR SIGNATURE OF FUNCTIONS ALREADY PROVIDED
// you are only allowed to implement the functions 
// you can also make additional helper functions a you wish

// REFER DOCUMENTATION FOR MORE DETAILS ON FUNSTIONS AND THEIR FUNCTIONALITY
*/
// add other headers as required
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
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
int firstTime;

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
    // free_list_head = (Node*)mmap(NULL, sizeof(Node), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    free_list_head=NULL;
    firstTime=1;
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
    while(CurrentChain!=NULL){
        if(munmap(CurrentChain,CurrentChain->size)==-1){
            perror("munmap");
        }
    }
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
    // Find the correct size to allocate in case of new subchain
    size_t allocationSize=0;
    if(size%PAGE_SIZE==0){
        allocationSize = size;
    }
    else{
        allocationSize = ((size / PAGE_SIZE) + 1) * PAGE_SIZE;
    }
    // Traverse the free list and find a suitable segment to allocate
    if(firstTime){
        Node* newProcessNode=internal_node_create();
        newProcessNode->start_addr=mmap(NULL, allocation_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        newProcessNode->end_addr=newProcessNode->start_addr+(size-1);
        newProcessNode->type=1;
        Node* newHoleNode=internal_node_create();
        newHoleNode->start_addr=newProcessNode->end_addr+1;
        newHoleNode->end_addr=newProcessNode->start_addr+allocationSize;
        newHoleNode->type=0;
        newProcessNode->next=newHoleNode;
        newProcessNode->prev=NULL;
        newHoleNode->next=NULL;
        newHoleNode->prev=newProcessNode;
        Chain* newChain=internal_chain_create();
        free_list_head=newChain;
        newChain->offset=1000;
        newChain->sub_chain=newProcessNode;
        newChain->next=NULL;
        newChain->prev=NULL;
        newChain->size=allocationSize;
        firstTime=0;
        return (void*)newChain->offset;

        // Node* tempNode = (Node*)mmap(NULL, allocation_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        // Node* newProcessNode=tempNode;
        // Node* newHoleNode=(char*) tempNode +sizeof(Node)+ size;
        // newProcessNode->size=size;
        // newProcessNode->next=newHoleNode;
        // newProcessNode->prev=NULL;
        // newHoleNode->size=allocationSize-size;
        // newHoleNode->prev=newProcessNode;
        // newHoleNode->next=NULL;
        // Chain* newChain = (Chain*)mmap(NULL, sizeof(Chain), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        // free_list_head=newChain;
        // newChain->offset=0;
        // newChain->sub_chain=newProcessNode;
        // newChain->next=NULL;
        // newChain->prev=NULL;
        // newChain->size=allocationSize;
        // firstTime=0;
        // return newChain->offset;
    }
    size_t virtualAddress=0;
    Chain* currentChain = free_list_head;
    while (currentChain != NULL){
        Node* currentNode = currentNode->sub_chain;
        virtualAddress=currentNode->offset;
        while (currentNode != NULL) {
            if (currentChain->type == 0 && currentChain->size >= allocation_size) {
                if (currentChain->size > allocation_size) {
                    newSpace->size = currentSegment->size - allocation_size;
                    newSpace->type = 0;
                    newSpace->prev = currentChain;
                    newSpace->next = currentChain->next;
                    currentChain->next = newSpace;
                    currentChain->size = allocation_size;
                    return virtualAddress;
                }
                else{
                    currentChain->type = 1;
                    return virtualAddress;
                }
            }
            else{
                virtualAddress+=currentChain->size;
            }
            currentNode = currentNode->next;
        }
        currentChain = currentChain->next;
    }
    while(currentNode->next!=NULL){
        currentNode=currentNode->next;
    }
    Node* tempNode = (Node*)mmap(NULL, allocation_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    Node* newProcessNode=tempNode;
    Node* newHoleNode=(char*) tempNode +sizeof(Node)+ size;
    newProcessNode->size=size;
    newProcessNode->next=newHoleNode;
    newProcessNode->prev=NULL;
    newHoleNode->size=allocation_size-size;
    newHoleNode->prev=newProcessNode;
    newHoleNode->next=NULL;
    Chain* newChain = (Chain*)mmap(NULL, sizeof(Chain), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    newChain->sub_chain = newProcessNode;
    newChain->prev = currentNode;
    newChain->next = NULL;
    newChain->offset=currentNode->size+currentNode->offset;
    newChain->size=allocationSize;
    currentNode->next=newChain;
    return newChain->offset;
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
    while(currentChain!=NULL){
        printf("MAIN[%zu:%zu]->",currentChain->offset,currentChain->offset+currentChain->size);
        Node* currentNode=currentChain->sub_chain;
        size_t start_ptr=currentChain->offset;
        pages_used=pages_used+(currentChain->size/PAGE_SIZE);
        while(currentNode!=NULL){
            if(currentNode->type==0){
                printf("P[%zu:%zu]<->",start_ptr,start_ptr+currentNode->size-1);
                start_ptr=start_ptr+currentNode->size;
            }
            else if(currentNode->type==1){
                printf("H[%zu:%zu]<->",start_ptr,start_ptr+currentNode->size-1);
                start_ptr=start_ptr+currentNode->size;
                hole_memory=hole_memory+currentNode->size;
            }
            currentNode=currentNode->next;
        }
        printf("NULL");
        printf("\n");
        currentNode=currentNode->next;
        main_length++;
    }
    int sub_chain[main_length];
    int i=0;
    currentChain=free_list_head;
    while(currentChain!=NULL){
        Node* currentNode=currentChain->sub_chain;
        int sub_chain_length=0;
        while (currentNode!=NULL){
            sub_chain_length++;
        }
        sub_chain[i]=sub_chain_length;
        i++;
        currentChain=currentChain->next;
    }
    printf("Used Pages:%d\n",pages_used);
    printf("Unused memory:%zu\n",hole_memory);
    printf("Main Chain Length:%d\n",main_length);
    printf("Sub-chain length array:[")
    for(i=0;i<main_length;i++){
        printf("%d,",sub_chain[i]);
    }
    printf("]");
}


/*
Returns the MeMS physical address mapped to ptr ( ptr is MeMS virtual address).
Parameter: MeMS Virtual address (that is created by MeMS)
Returns: MeMS physical address mapped to the passed ptr (MeMS virtual address).
*/
void *mems_get(void*v_ptr){
    
    Chain* CurrentChain=free_list_head;
    while(CurrentChain!=NULL){
        if((size_t)v_ptr<=CurrentChain->offset+CurrentChain->size){
            break;
        }
        CurrentChain=CurrentChain->next;
    }
    size_t start_ptr=(size_t)v_ptr-CurrentChain->offset;
    Node* CurrentNode=CurrentChain->sub_chain;
    while(CurrentNode!=NULL){
        size_t node_size=CurrentNode->size;
        if(start_ptr>node_size){
            start_ptr=start_ptr-node_size;
        }
        else{
            break;
        }
        CurrentNode=CurrentNode->next;
    }
    return (void*)CurrentNode+a; //TODO figure out return value and type
}


/*
this function free up the memory pointed by our virtual_address and add it to the free list
Parameter: MeMS Virtual address (that is created by MeMS) 
Returns: nothing
*/
void mems_free(void *v_ptr){
    
}


Node* internal_node_create(){
    Node* ret;
    if(internal_nodes_ptr+sizeof(Node)<internal_nodes_head+PAGE_SIZE){
        ret=internal_nodes_ptr;
        internal_nodes_ptr++;
    }
    else{
        Node* new_ptr=(Node*)mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        internal_nodes_ptr=new_ptr;
        internal_nodes_head=new_ptr;
        ret=internal_nodes_ptr;
    }
    return ret;
}

Chain* internal_chain_create(){
    Chain* ret;
    if(internal_chains_ptr+sizeof(Chain)<internal_chains_head+PAGE_SIZE){
        ret=internal_chains_ptr;
        internal_chains_ptr++;
    }
    else{
        Chain* new_ptr=(Chain*)mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        internal_chains_ptr=new_ptr;
        internal_chains_head=new_ptr;
        ret=internal_chains_ptr;
    }
    return ret;
}
