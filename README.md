# Real-Time-Embedded-Systems
## Project as part of Aristotle University of Thessaloniki assignment.
Revision of an existing code that simulates the producers consumers problem using pthreads. The twist is that the queue variable type is a struct that has a function pointer and an argument.Random function pointers are imported in the queue by the producers, and then the consumers grab the pointer and run the function with its argument.Consumers have a while 1 loop so in order to exit the program press ctrl + z. The graphs below show the mean time that a number of consumers needs to wait, for a specific amount of producers. The tests took place to an intel core i5-8400 with 6 cores and 6 threads.   

### How to run the code
In order to compile the code type at the command line gcc -O3 -pthread -Wall [name-of-the-file].c -o [name-of-the-file] -lm
   
It is important to type the -lm to link the math library.In order to run it just time ./[name-of-the-file]


### Results
#### with flag -O3
![limited_buffer](https://github.com/doinakis/Real-Time-Embedded-Systems/blob/master/summary_limited_opt.jpg)
![unlimited_buffer](https://github.com/doinakis/Real-Time-Embedded-Systems/blob/master/summary_unlimited_opt.jpg)
#### without flag -O3
![limited_buffer](https://github.com/doinakis/Real-Time-Embedded-Systems/blob/master/summary_limited.jpg)
![unlimited_buffer](https://github.com/doinakis/Real-Time-Embedded-Systems/blob/master/summary_unlimitedd.jpg)
