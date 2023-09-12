cmake_minimum_required(VERSION 3.26)
project(EMM)

set(CMAKE_CXX_STANDARD 17)

add_executable(EMM main.cpp
        emm.h
        emm.cpp
        cfg.cpp
        cfg.h
        utils.cpp
)
