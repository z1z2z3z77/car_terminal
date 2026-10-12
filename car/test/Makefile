CXX := /home/book/100ask_imx6ull-sdk/ToolChain/arm-buildroot-linux-gnueabihf_sdk-buildroot/bin/arm-buildroot-linux-gnueabihf-g++

CXXFLAGS = -g -Wall -std=c++11 -pthread -I.

TARGET_PATH = /home/book/Desktop/car/test_makefile

SRC =$(wildcard *.cpp)

EXES =$(patsubst %.cpp,%,$(SRC))

all:$(EXES)

$(EXES):%:%.cpp
	$(CXX) $(CXXFLAGS) $< -o $@ &&\
	mv $@ $(TARGET_PATH)

clean:
	rm -f $(TARGET_PATH)/*
.PHONY: clean