# MeMS: Memory Management System [CSE231 OS Assignment 3]
[Github Repository Link](https://github.com/pecpo/os-ass-3)

## Introduction
In this assignment we had to implement a memory mamagement system of our own which allocates memory only in sizes of PAGE_SIZE.This memory can then be used to store data. In a sense we implemented the malloc and free functions present in the stdlib library of c.

## How to run the program
- Go in the same directory as the program files.
- Run make in the terminal.
- After that run "./example" for the example.c file to run correctly.

## Implementation
- First function is mems_init: This initializes the memory management system. In this the we intialize the chains count, the base virtual address and the internal data structures to store the 
yellow main nodes and the blue sub chain nodes.
- Second function is mems_malloc: This function is used to allocate memory to a process. First it checks if the firstTime flag is set if it is it creates the first yellow node with a subchain of blue nodes present. Then the flag is set to 0. For all the next malloc calls one of two things can happen. It first searches if a hole with enough memory is present in any of the subchains. If such a hole is found then that hole becomes a process node. If such a hole is not found. A new yellow main node is created with a subchain of blue nodes containing process or hole nodes.
- Third function is mems_free: This function is used when we want to convert a process node to a hole node. In this case we just set the type of node to hole and after that check for fragmentation in the subchain in which we have freed a memory.If we there are two consecutive hole nodes they are combined into a single one with size equal to sum of both.
- Fourth function is mems_get: For any virtual address it returns a physical address corresponding to that memory location.
- Fifth function is mems_print_stats: This function first prints the 2-D linked list of the memory management system used to keep track of the memory. After that it prints total no of pages used, the total unused memory, no of yellow nodes and no of blue subchain nodes corresponding to each yellow main node.
- Sixth function is mems_finish: This function is called at the end of the program to unmap all the memory which we have allocated. The system call used is munmap to unmap all the memory including that of the internal data structures used to maintain the yellow main nodes and blue subchain nodes.
