# cmake file for project bufr2synop

CMAKE_MINIMUM_REQUIRED (VERSION 2.6)

PROJECT (bufr2synop C)

# check for compiler
find_program(ICC_PROGRAM icc)
IF(ICC_PROGRAM)
  MESSAGE(STATUS "** Using Intel c/c++ compiler **")
  set(CMAKE_C_COMPILER "icc") 
  set(CMAKE_CXX_COMPILER "icc")
  set(CMAKE_C_FLAGS "-O2 -xHOST -Wall -g")
  set(CMAKE_CXX_FLAGS "-O2 -xHOST -Wall -g")
else (ICC_PROGRAM)
  MESSAGE(STATUS "** Using GNU gcc compiler **")
  set(CMAKE_C_FLAGS "-O2 -Wall -Wextra -g")
  set(CMAKE_CXX_FLAGS "-O2 -Wall -Wextra -g")
ENDIF(ICC_PROGRAM)

find_program(IFORT_PROGRAM ifort)
IF(IFORT_PROGRAM)
  MESSAGE(STATUS "** Usando el compilador intel fortran **")
  set(CMAKE_Fortran_COMPILER "ifort") 
  set(CMAKE_Fortran_FLAGS "-O2 -xHOST -Wall -g")
else (IFORT_PROGRAM)
  MESSAGE(STATUS "** Usando el compilador GNU gfortran **")
  set(CMAKE_Fortran_FLAGS "-O2 -Wall -Wextra -g")
ENDIF(IFORT_PROGRAM)

FIND_LIBRARY(BUFR bufr PATHS /usr/lib /usr/local/lib /usr/lib64 /usr/local/lib64)
IF (BUFR)
  MESSAGE("-- ECMWF BUFR library found!")
ELSE (BUFR)
  MESSAGE(FATAL_ERROR "Cannot found ECMWF BUFR library. Please install it")
ENDIF(BUFR)

#add_subdirectory(examples)
add_subdirectory(src)

include (CPack)


