# MeMS: Memory Management System [CSE231 OS Assignment 3]
[Github Repository Link](https://github.com/pecpo/os-ass-3)
---

## How to run the program


---
### Format of mems_mems_print_stats
* For every Subchain in the free list print the data as follows 
```
MAIN[starting_mems_vitual_address:ending_mems_vitual_address] -> <HOLE or PROCESS>[starting_mems_vitual_address:ending_mems_vitual_address] <-> ..... <-> NULL
```
* After printing the whole freelist print the following stats
```
Page used: <Total Pages used by the user>
Space unused: <Total space mapped but not used by user>
Main Chain Length: <Length of the main chain>
Sub-chain Length array: <Array denoting the length of the subchains>
```
* Analyse the output of the example.c file below to get a better understanding of the output
---
### Example Output
PAGE_SIZE= 4096 Bytes

Starting MeMS Virtual Address= 1000

![Example Output](example_output.jpg)