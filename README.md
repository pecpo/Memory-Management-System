# MeMS: Memory Management System [CSE231 OS Assignment 3]
[Github Repository Link](https://github.com/pecpo/os-ass-3)

## Introduction
In this assignment we had to implement a memory mamagement system of our own which allocates memory only in sizes of PAGE_SIZE.This memory can then be used to store data. In a sense we implemented the malloc and free functions present in the stdlib library of c.

## How to run the program
- Go on a wsl terminal and use make in the same directory as the program files.
- After that run "./example" for the example.c file to run correctly.

## Implementation
- FIrst function is mems_init: This initializes the memory management system. In this the we intialize the chains count, the base virtual address and the interal data structures to store the 
yellow main nodes and the blue sub chain nodes.
-  