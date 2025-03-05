1. Your shell forks multiple child processes when executing piped commands. How does your implementation ensure that all child processes complete before the shell continues accepting user input? What would happen if you forgot to call waitpid() on all child processes?

In my shell implementation, I make sure that all of the child processes are guaranteed to complete before accepting new user input through waitpid() system calls in both the execute_pipeline() and the single command execution section.
If you forget to call waitpid(), zombie processes would accumulate, the parent would not know when the children finishes, and the system might reach its process limit if many commands are executed without waiting.

2. The dup2() function is used to redirect input and output file descriptors. Explain why it is necessary to close unused pipe ends after calling dup2(). What could go wrong if you leave pipes open?

IT's necessary to close the unused pipe because each open file  descriptor consumes some resources, and open pipe ends prevent processes from receiving EOF signals properly
If pipe ends remain open, processes reading from pipes would hang indefinitely waiting for more data, because the OS signals EOF only when all write ends of a pipe are closed. Since both the parent and other sibling processes would have copies of write ends open, the reading process would never receive EOF.

3. Your shell recognizes built-in commands (cd, exit, dragon). Unlike external commands, built-in commands do not require execvp(). Why is cd implemented as a built-in rather than an external command? What challenges would arise if cd were implemented as an external process?

The cd is implemented as a built in so that it would execute in a separate child process due to the fork/exec model, that child process would change its own working directory, when the child terminates, the parent shell's working directory would remain unchanged
Since the purpose of cd is to change the shell's own working directory for subsequent commands, it must be implemented as a built-in command that executes in the shell process itself

4. Currently, your shell supports a fixed number of piped commands (CMD_MAX). How would you modify your implementation to allow an arbitrary number of piped commands while still handling memory allocation efficiently? What trade-offs would you need to consider?

To support an arbitrary number of piped commands, I would replace the fixed-size arrays with dynamically allocated memory using malloc() based on the actual command count. I'd allocate arrays for pipes and pids at runtime, ensuring to free this memory after use. This approach would require careful error handling for allocation failures though and implementing reasonable limits to prevent resource exhaustion. The main trade-offs would be slightly increased overhead from dynamic memory management versus the greater flexibility of supporting command pipelines of any practical length.