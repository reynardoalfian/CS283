1. Can you think of why we use `fork/execvp` instead of just calling `execvp` directly? What value do you think the `fork` provides?

    > **Answer**:  Because execvp replace the current process, since we want the shell to continue running even after commands are executed. Fork will create a child process that can be replaced by execvp while parent shell continues running.

2. What happens if the fork() system call fails? How does your implementation handle this scenario?

    > **Answer**: If fork() system call fails, my implementation will return an error using perror("fork) and continue the shell loop, making sure it doesn't crash and handle the next subsequent commands.

3. How does execvp() find the command to execute? What system environment variable plays a role in this process?

    > **Answer**:  execvp() searches for commands in the directories listed in the PATH environment variable, allowing it to find executable files anywhere in the system path without needing their full path.

4. What is the purpose of calling wait() in the parent process after forking? What would happen if we didn’t call it?

    > **Answer**:  calling wait() makes the parent processes wait until the child is finish executing. WIthout it, the child would have no parent.

5. In the referenced demo code we used WEXITSTATUS(). What information does this provide, and why is it important?

    > **Answer**:  it extracts the exit status from a child process that would terminate. This allows us to check whether the command ran successfully or failed.

6. Describe how your implementation of build_cmd_buff() handles quoted arguments. Why is this necessary?

    > **Answer**:  build_cmd_buff() preserves spaces within quoted arguments by treating everything between quotes as a single argument. This is necessary for commands that need to handle arguments containing spaces, like echo "hello world".


7. What changes did you make to your parsing logic compared to the previous assignment? Were there any unexpected challenges in refactoring your old code?

    > **Answer**:  I modified the parsing logic to handle command_buff_t instead of command_list_t and added support for quotes. The main challenge was ensuring proper memory management with the new structure.

8. For this quesiton, you need to do some research on Linux signals. You can use [this google search](https://www.google.com/search?q=Linux+signals+overview+site%3Aman7.org+OR+site%3Alinux.die.net+OR+site%3Atldp.org&oq=Linux+signals+overview+site%3Aman7.org+OR+site%3Alinux.die.net+OR+site%3Atldp.org&gs_lcrp=EgZjaHJvbWUyBggAEEUYOdIBBzc2MGowajeoAgCwAgA&sourceid=chrome&ie=UTF-8) to get started.

- What is the purpose of signals in a Linux system, and how do they differ from other forms of interprocess communication (IPC)?

    > **Answer**:  Signals are software interrupts that provide a way for processes to handle asynchronous events. Unlike other IPC methods, signals are meant for asynchronous notifications rather than data transfer.

- Find and describe three commonly used signals (e.g., SIGKILL, SIGTERM, SIGINT). What are their typical use cases?

    > **Answer**:  SIGKILL immediately terminates a process and cannot be ignored, SIGTERM requests graceful process termination, and SIGINT is sent when pressing Ctrl+C to interrupt a program.

- What happens when a process receives SIGSTOP? Can it be caught or ignored like SIGINT? Why or why not?

    > **Answer**:  When a process receives SIGSTOP, it is suspended and cannot be caught or ignored because it's a fundamental signal for process control that must always work for debugging and process management.
