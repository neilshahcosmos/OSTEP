# Ch. 2 - Introduction

- [Ch. 2 - Introduction](#ch-2---introduction)
    - [2.1 - Virtualizaing the CPU](#21---virtualizaing-the-cpu)
    - [2.2 - Virtualizing Memory](#22---virtualizing-memory)
    - [2.3 - Concurrency](#23---concurrency)
    - [2.4 - Persistence](#24---persistence)
    - [2.5 - Design Goals](#25---design-goals)
    - [2.6 - Some History](#26---some-history)
    - [Homework](#homework)

* What happens when a program runs?
* The simple Von Neumann model tells us that a program executes instructions sequentially, taking the following steps:
  * It **fetches** an instruction from memory
  * It **decodes** the instruction, and figures out what this instruction is doing
  * It **executes** the instruction
  * It moves on to the next instruction in sequential memory (in the PC)
* However the real picture involves a lot more complexity, that all have the goal of making the system **easy to use**
* We need a body of software to make it easy to run many programs (seemingly) at the same time, to allow programs to share memory, and interact with devices easily
* This is the Operating Software, or the OS
> **The Crux of the Problem**: How to Virtualize Resources 
> One central question we want to answer is, *how* does the OS virtualize resources? Our question isn't *why*, but *how*, since the why is obvious: to make the system easier to use
> These shaded boxes are "crux of the problem"s, which will highlight specific problems we're trying to solve, and the solution will be presented in the chapter
* The primary way the OS does this is through **virtualization**, where the OS takes a physical resource and transforms it into a more general, powerful, and easy-to-use virtual form of itself
* The OS also provides APIs through many hundreds of **system calls** that are available to programs (these APIs are also called a **standard library**)
* The OS can also be thought of as a **resource manager**, where each resource can be the CPU, memory, or an external device

### 2.1 - Virtualizaing the CPU

* The first major theme of this course is Virtualization
* Let's look at our first piece of code:

```c
// Figure 2.1: Simple Example: Code That Loops And Prints (cpu.c)
int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "usage: cpu <string>\n");
        exit(1);
    }
    char *str = argv[1];
    while (1) {
        Spin(1);
        printf("%s", str);
    }
    return 0;
}
```

* All it does is call a function `spin()`, which checks the time and returns once 1 second has passed since its call. It then prints the input string
* If we run it once, we'll get a trivial output:

```
prompt> gcc -o cpu cpu.c -Wall
prompt> ./cpu "A" 
A A A A ^C
prompt>
```

* Here we get the expected output, where we interrupt the infinite loop using a keyboard interrupt with `^C`
* Now what if we do the same thing, but run many different versions of this?

```
prompt> ./cpu A & ./cpu B & ./cpu C & ./cpu D &
A B D C A B D C A ... 
```

* Even though we only have one CPU, somehow all of these programs are running at the same time!
* The OS does this by **virtualizing** the CPU, so that each programs is given the illusion of having access to the CPU. We'll see how this is done in the next chapters

### 2.2 - Virtualizing Memory

* Memory is a physical array of bytes, with each byte having an address
* Instructions access memory, and are themselves stored in memory
* Let's look at another program that allocates some memory:

```c
// Figure 2.3: A Program that accesses memory (mem.c)
int main(int argc, char *argv[])
{
    int *p = malloc(sizeof(int));
    assert(p != NULL);
    printf("(%d) address pointed to by p: %p\n", getpid(), p);
    *p = 0;
    while (1)
    {
        Spin(1);
        *p = *p + 1;
        printf("(%d) address pointed to by p: %p\n", getpid(), p);
    }
    return 0;
}
```

* This program allocates memory for an integer variable, prints its address as well as the Process ID (through `getpid()`), and has the output:

```
prompt>./mem
(2134) address pointed to by p: 0x200000
(2134) p: 1
(2134) p: 2
(2134) p: 3
...
```

* Now what if, as we did before, we ran two instances of this code at the same time? We get:

```
prompt>./mem & ./mem
(24113) address pointed to by p: 0x200000
(24114) address pointed to by p: 0x200000
(24113) p: 1
(24114) p: 1
(24113) p: 2
(24114) p: 2
...
```

* NOTE: I actually didn't get this, the two addresses were different, not sure why
* We get that both processes are using the same memory address, without overwriting each other! How?
* This is happening because the OS is virtualizing the memory, by giving each process its own private **virtual address space**, which the OS somehow maps onto physical memory

### 2.3 - Concurrency

* The second major theme of this course is Concurrency
* Both the OS, and the CPU with multi-threaded programs, have to juggle **concurrency**
* Say we have a program that takes a function which iterates a variable, and creates two **threads** of it (where a thread is a function running within the same memory space as other functions, with multiple threads being able to run at the same time)
* So if we have two threads in this program, we expect that `./threads N` should give us $2N$ as a result. However, if $N$ is large enough, we actually get inconcsistent and often incorrect results
> **The Crux of the Problem**: How to Build Correct Concurrent Programs
> When there are many concurrently executing threads within the same memory space, how can we build a correctly working program?

### 2.4 - Persistence

* The third major theme of this course is Persistence
* In memory, data can often be lost, when power goes away or the system crashes for example
* The software in the OS that manages the disk is called the **file system**
* So as we saw for the CPU and memory, we might assume the disk is virtualized as well. However this is usually not the case, as programs will often want to talk to each other and share info that is on disk
* Let's take a look at some disk I/O code:

```c
int main(int argc, char* argv[])
{
    int fd = open("/tmp/file", O_WRONLY|O_CREAT|O_TRUNC, S_IRWXU);
    assert(fd > -1);
    int rc = write(fd, "Hello, World!\n", 13);
    assert(rc == 13);
    close(fd);
    return 0;
}
```

* This makes 3 system calls: `open()`, `write()`, and `close()`, which are then routed to the file system software inside the OS
> **The Crux of the Problem**: How to Store Data Persistently
> How does the file system correctly manage persistent data? How can it do so with high performance?

### 2.5 - Design Goals

* So let's review! The OS:
  * Virtualizes physical resources, such as the CPU and memory (and sometimes the disk)
  * Manages Concurrency of simultaneous programs
  * Stores files persistently in a safe way
* We have some basic goals for an OS that can accomplish all this:
  * Set up **abstractions**, that can help simplify complex problems
  * Provide **high performance**, by minimizing the **overhead** of the OS (overheads are extra costs, such as extra time and extra memory, incurred not by the program but just by the OS)
  * Protect applications from each other, and the OS from applications
  * Must run non-stop reliably, as if the OS fails all the programs fail
  * Energy-efficiency, security, and mobility can be goals as well

### 2.6 - Some History

* OSs were initially just libraries, and the order of program processing was handled by a human operator. This was called **batch processing**, where programs ran in batches
* It was soon realized that OS calls should have a special level of privilege, so that not just any program can access memory whenever it wants. So now the idea of the **system call** is invented, where a system call raises the hardware privilege level from User mode to Supervisor (or Kernel) mode before executing
* As computers became accessible to singular people instead of large companies, the idea of **multiprogramming** became commonplace, where the OS would load a number of jobs and rapidly switch between them and executing. This is very helpful, since the CPU can now work on one job while the OS is doing I/O (which is very slow) for another job

### Homework

* There are two types of homeworks:
  * Simulation: These are simple programs that pretend to do some interesting parts of what real systems do, then report some output metrics to show how the system behaves. They are great for exploring systems without the trouble of working with a real system
  * Unfortunately simulations are approximations, and their results should be taken with a grain of salt
  * Real-world code: These are small forays into real-world code with real systems


<!--
```c

```
-->
