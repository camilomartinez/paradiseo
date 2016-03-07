######################################################################################
### Check the CMake version
######################################################################################

cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

######################################################################################
### 0) Include the install configuration file where are defined the main variables
######################################################################################

INCLUDE(${CMAKE_SOURCE_DIR}/install.cmake)

######################################################################################


######################################################################################
### 1) Set your application properties
######################################################################################

# Define the project name
PROJECT(flow-shop)

SET(PACKAGE_NAME "flow-shop" CACHE STRING "Package name" FORCE)
SET(PACKAGE_VERSION "1.0" CACHE STRING "Package version" FORCE)

######################################################################################


######################################################################################
### 2) Find the required modukes
######################################################################################

# Need to find Doxygen to generate the documentation
INCLUDE(FindDoxygen)
# Need to find MPI to use EO-MPI
find_package(MPI REQUIRED)
add_definitions(-DWITH_MPI)
SET(CMAKE_CXX_COMPILE_FLAGS ${CMAKE_CXX_COMPILE_FLAGS} ${MPI_COMPILE_FLAGS})
SET(CMAKE_CXX_LINK_FLAGS ${CMAKE_CXX_LINK_FLAGS} ${MPI_LINK_FLAGS})
INCLUDE_DIRECTORIES(${MPI_INCLUDE_PATH})
#LINK_DIRECTORIES(${MPI_DIR}/lib)

######################################################################################


######################################################################################
### 3) Link the librairies for your executable
######################################################################################

SUBDIRS(application doc src)

######################################################################################
