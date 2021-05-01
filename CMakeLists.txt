cmake_minimum_required(VERSION 3.19)
project(KU_MMU)

set(CMAKE_CXX_STANDARD 14)

add_executable(KU_MMU.out ku_mmu.h ku_cpu.c ku_trav.o)