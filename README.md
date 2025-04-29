# C++ Producer-Consumer Problem Implementation

This project provides a C++ implementation of the classic Producer-Consumer problem.

## Description
The program simulates multiple "producer" threads creating items and adding them to a shared, fixed-size buffer, and multiple "consumer" threads removing items from the buffer. It uses mutexes and condition variables to ensure thread-safe access to the shared buffer and to coordinate producers and consumers (e.g., producers wait if the buffer is full, consumers wait if it's empty).

The primary goal is to observe the system's performance (measured by overall turnaround time) under different loads by varying the number of producer and consumer threads.

## Dependencies
* A C++ compiler supporting C++11 or later (e.g., g++, clang++)
* `make` (for using the provided Makefile)
* POSIX Threads library (`pthread` - usually linked automatically when using `std::thread` with the `-pthread` flag)

## How to run the code
Compile the code using make,
```bash
make
```

then run the executable provided 2 integer inputs. The first being the producer, and the second being the consumer.
```bash
./producer_consumer 1 1
```
