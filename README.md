# MeMS: Memory Management System [CSE231 OS Assignment 3]
[Github Repository Link](https://github.com/pecpo/os-ass-3)

## Introduction
In this assignment we had to implement a memory mamagement system of our own which allocates memory only in sizes of PAGE_SIZE.This memory can then be used to store data. In a sense we implemented the malloc and free functions present in the stdlib library of c.

## How to run the program
- Go on a wsl terminal and use make in the same directory as the program files.
- After that run "./example" for the example.c file to run correctly.

## Implementation
- First function is mems_init: This initializes the memory management system. In this the we intialize the chains count, the base virtual address and the internal data structures to store the 
yellow main nodes and the blue sub chain nodes.
- Second function is mems_malloc: This function is used to allocate memory to a process. First it checks if the firstTime flag is set if it is it creates the first yellow node with a subchain of blue nodes present. Then the flag is set to 0. For all the next malloc calls one of two things can happen. It first searches if a hole with enough memory is present in any of the subchains. If such a hole is found then that hole becomes a process node. If such a hole is not found. A new yellow main node is created with a subchain of blue nodes containing process or hole nodes.
