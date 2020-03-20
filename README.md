# Real-Time-Embedded-Systems
Revision of an existing code that simulates the producers consumers problem using pthreads. The twist is that the queue variable type is a struct that has a function pointer and an argument.Random function pointers are imported in the queue by the producers, and then the consumers grab the pointer and run the function with its argument.The graphs below show the mean time that a number of consumers needs to wait, for a specific amount of producers. The tests took place to an intel core i5-8400 with 6cores and 6 threads.   

### How to run the code
In order to run the code type at the command line gcc -Wall [name-of-the-file].c -o [name-of-the-file] -lm   
It is important to type the -lm to link the math library.


### Results
