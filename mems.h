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
    size_t size;
    int type; // 0 for HOLE, 1 for PROCESS
    struct Node* next;
    struct Node* prev;
} Node;

typedef struct Chain {
    size_t offset;
    struct Node* sub_chain;
    struct Chain* next;
    struct Chain* prev;
    size_t size;
} Chain;

Chain* free_list_head;
int firstTime;

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
}

/*
This function will be called at the end of the MeMS system and its main job is to unmap the 
allocated memory using the munmap system call.
Input Parameter: Nothing
Returns: Nothing
*/
void mems_finish(){
    
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
    // Traverse the free list and find a suitable segment to allocate
    if(firstTime){
        Node* tempNode = (Node*)mmap(NULL, allocation_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        Node* newProcessNode=tempNode;
        Node* newHoleNode=(char*) tempNode +sizeof(Node)+ size;
        newProcessNode->size=size;
        newProcessNode->next=newHoleNode;
        newProcessNode->prev=NULL;
        newHoleNode->size=allocationSize-size;
        newHoleNode->prev=newProcessNode;
        newHoleNode->next=NULL;
        Chain* newChain = (Chain*)mmap(NULL, sizeof(Chain), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        free_list_head=newChain;
        newChain->offset=0;
        newChain->sub_chain=newProcessNode;
        newChain->next=NULL;
        newChain->prev=NULL;
        newChain->size=allocationSize;
        firstTime=0;
        return newChain->offset;
    }
    size_t virtualAddress=0;
    Node* currentNode = free_list_head;
    while (currentNode != NULL){
        Node* currentChain = currentNode->sub_chain;
        virtualAddress=currentNode->offset;
        while (currentChain != NULL) {
            if (currentChain->type == 0 && currentChaun->size >= allocation_size) {
                if (currentChain->size > allocation_size) {
                    Node* newSpace= (Node*)mmap(NULL, sizeof(Node), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
                    newSpace->size = currentChain->size - allocation_size;
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
            currentChain = currentChain->next;
        }
        currentNode = currentNode->next;
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
    newChain->offset=currentNode->size;
    newChain->size=allocationSize;
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

}


/*
Returns the MeMS physical address mapped to ptr ( ptr is MeMS virtual address).
Parameter: MeMS Virtual address (that is created by MeMS)
Returns: MeMS physical address mapped to the passed ptr (MeMS virtual address).
*/
void *mems_get(void*v_ptr){
    
}


/*
this function free up the memory pointed by our virtual_address and add it to the free list
Parameter: MeMS Virtual address (that is created by MeMS) 
Returns: nothing
*/
void mems_free(void *v_ptr){
    
}