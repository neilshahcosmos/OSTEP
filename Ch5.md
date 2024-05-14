# Ch. 5 - Interlude: Process API

- [Ch. 5 - Interlude: Process API](#ch-5---interlude-process-api)
    - [5.1 - The `fork()` System Call](#51---the-fork-system-call)
    - [5.2 - The `wait()` System Call](#52---the-wait-system-call)
    - [5.3 - The `exec()` System Call](#53---the-exec-system-call)
    - [5.4 - Why? Motivating the API](#54---why-motivating-the-api)
    - [5.5 - Process Control and Users](#55---process-control-and-users)

> Aside: Interludes
> Interludes will cover more practical aspects of systems, including a particular focus on OS APIs and how to use them

* Will study three system calls in UNIX used to create and manage new processes:
  * `fork()`: Used to create a new process
  * `exec()`: 
  * `wait()`: Used by a process wishing to wait for one of its child processes to complete

### 5.1 - The `fork()` System Call

* The `fork()` system call in UNIX is used to create child processes. Consider the following code:

```c
// Figure 5.1: Calling fork() (p1.c)
int main(int argc, char* argv[])
{
    printf("hello (pid:%d)\n", (int) getpid());
    int rc = fork();

    if (rc < 0)
    {
        // fork failed
        fprintf(stderr, "fork failed\n");
        exit(1);
    }
    else if (rc == 0)
    {
        // child goes down this path
        printf("child (pid:%d)\n", (int) getpid());
    }
    else
    {
        // parent goes down this path
        printf("parent of %d (pid:%d)\n", rc, (int) getpid());
    }
}
```

* Essentially, when `fork()` is called in the parent process, a child process is created, with its own resources
* Specifically, this child process copied the code but assumes execution is just about to resume at the end of its own `fork()` call, so any code before that in the child process isn't executed
* The `fork()` in the child process returns 0, in the parent process it returns the **Process ID** or PID of the child process
* So we get the output:

```
prompt> ./p1
hello (pid:29146)
parent of 29147 (pid:29146)
child (pid:29147)
prompt>
```

* NOTE: The output here is not deterministic, as the order of whether the child or the parent returns from `fork()` first and finishes executing is dependent on the CPU scheduler

### 5.2 - The `wait()` System Call

* We can change the parent path in the code above as follows:

```c
// Figure 5.2: Calling fork() And wait() (p2.c)
...
else
{
    int rc_wait = wait(NULL);
    printf("parent of %d (rc_wait:%d) (pid:%d)\n", rc, rc_wait, (int) getpid());
}
```

* This will now cause the output to be deterministic, as now even if the scheduler executes the parent process before the child one, the parent will `wait()` for the child call to finish running before finishing its own execution

### 5.3 - The `exec()` System Call

* The final system call we'll look at is `exec()`
* This is useful when you want to call a program that's different from the calling program
* We can modify the child path in the Figure 5.1 code above to the following:

```c
// Figure 5.3: Calling fork(), wait(), And exec() (p3.c)
...
else if (rc == 0)
{
    printf("child (pid:%d)\n", (int) getpid());
    char *myargs[3];
    myargs[0] = strdup("wc"); // program: "wc"
    myargs[1] = strdup("p3.c"); // arg: input file
    myargs[2] = NULL; // mark end of array
    execvp(myargs[0], myargs); // runs word count
}
```

* Here, `execvp()` will run the executable program `wc` on the source file `p3.c`, which will tell us how many lines, words, and bytes are found in that file:

```
prompt> ./p3
hello (pid:29383)
child (pid:29384)
29 107 1030 p3.c
parent of 29384 (rc_wait:29384) (pid:29383)
prompt>
```

* What the `execvp()` code does exactly is that it loads the code and static data from the executable (in this case `wc`), and overwrites the current code segment and static data with it
* The heap and stack and the rest of the memory space of the program are re-initialized as well
* Then the OS simply runs the new program, passing in any arguments as the `argv` of that process
* So to be clear, it **does not** create a new process, but rather transforms the currently running program into a different running program

### 5.4 - Why? Motivating the API

* Why are `fork()` and `exec()` implemented so strangely?
* The reason is that these are perfect for implementing a **shell** program, which is a user program that shows a prompt that you can type commands into (such as `ls -l` and `mkdir /path/`)
* When you run a command on the shell, it `fork()`s the shell program, sets up the parameters and arguments for `exec()` based on your input command and runs it, and then `wait()`s for the child program to finish
* When it's over, it prints out the prompt again and is ready for the next command
* Take an example command here, where the output of `wc p3.c` is stored into a new text file:
`prompt> wc p3.c > newfile.txt`
* A code that could handle this would close the standard output, open another file, and then use `exec()` on the `wc` executable
* An example code that executes this is shown here:

```c
// Figure 5.4: All Of The Above With Redirection (p4.c)
int main(int argc, char* argv[])
{
    int rc = fork();

    if (rc < 0)
    {
        // fork failed
        fprintf(stderr, "fork failed\n");
        exit(1);
    }
    else if (rc == 0)
    {
        // child: redirection standard output to a file
        close(STDOUT_FILENO); // the file number (an integer) of the standard output
        open("./p4.output", O_CREAT|O_WRONLY|O_TRUNC, S_IRWXU);
        // no exec "wc"...
        char *myargs[3];
        myargs[0] = strdup("wc"); // program: "wc"
        myargs[1] = strdup("p4.c"); // arg: input file
        myargs[2] = NULL; // mark end of array
        execvp(myargs[0], myargs); // runs word count
    }
    else
    {
        // parent goes down this path
        int rc_wait = wait(NULL);
    }
}
```

* This would give the output:

```
prompt> ./p4
prompt> cat p4.output
32 109 846 p4.c
prompt>
```

### 5.5 - Process Control and Users

* 


<!--
```c

```
-->
