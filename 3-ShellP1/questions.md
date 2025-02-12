1. In this assignment I suggested you use `fgets()` to get user input in the main while loop. Why is `fgets()` a good choice for this application?

    > **Answer**:  f.gets() is better for shell input because it is better at handling command lines when they have a max buffer size and also properly checks for end of file conditions

2. You needed to use `malloc()` to allocte memory for `cmd_buff` in `dsh_cli.c`. Can you explain why you needed to do that, instead of allocating a fixed-size array?

    > **Answer**:  I did not use malloc because the buffer size needed is already known at compile time (ARG_MAX is a constant defined in dshlib.h)

3. In `dshlib.c`, the function `build_cmd_list(`)` must trim leading and trailing spaces from each command before storing it. Why is this necessary? If we didn't trim spaces, what kind of issues might arise when executing commands in our shell?

    > **Answer**:  trimming spaces is important so that when the shell clean command names it can find and execute programs correctly. Without trimming, some commands may fail to run and can't execute.

4. For this question you need to do some research on STDIN, STDOUT, and STDERR in Linux. We've learned this week that shells are "robust brokers of input and output". Google _"linux shell stdin stdout stderr explained"_ to get started.

- One topic you should have found information on is "redirection". Please provide at least 3 redirection examples that we should implement in our custom shell, and explain what challenges we might have implementing them.

    > **Answer**:   Output redirection (>) is essential but requires careful file handling and permissions management
                    Input redirection (<) needs robust error handling for missing files
                    Append redirection (>>) must handle concurrent access safely

- You should have also learned about "pipes". Redirection and piping both involve controlling input and output in the shell, but they serve different purposes. Explain the key differences between redirection and piping.

    > **Answer**:  Pipes and redirection are fundamentally different, pipes connect programs to each other in memory for real-time data flow, while redirection connects programs to files on disk.

- STDERR is often used for error messages, while STDOUT is for regular output. Why is it important to keep these separate in a shell?

    > **Answer**:  Keeping STDERR separate from STDOUT lets users see errors even when output is redirected and helps with debugging since errors don't corrupt the main output stream

- How should our custom shell handle errors from commands that fail? Consider cases where a command outputs both STDOUT and STDERR. Should we provide a way to merge them, and if so, how?

    > **Answer**:  Our shell should display errors immediately and provide options to merge streams (like 2>&1) while maintaining proper exit status handling through the pipeline