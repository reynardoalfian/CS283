#!/usr/bin/env bats

# File: student_tests.sh
# 
# Create your unit tests suit in this file

@test "Example: check ls runs without errors" {
    run ./dsh <<EOF                
ls
EOF

    # Assertions
    [ "$status" -eq 0 ]
}

@test "Empty command shows warning" {
    run "./dsh" <<EOF

EOF
    [[ "$output" == *"warning: no commands provided"* ]]
}

@test "Multiple spaces between commands are handled" {
    run "./dsh" <<EOF
echo     hello         world
EOF
    stripped_output=$(echo "$output" | tr -d '[:space:]')
    expected_output="helloworlddsh3>dsh3>cmdloopreturned0"

    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"

    [ "$stripped_output" = "$expected_output" ]
}

@test "CD without args stays in same directory" {
    current=$(pwd)
    run "./dsh" <<EOF
cd
pwd
EOF
    stripped_output=$(echo "$output" | tr -d '[:space:]')
    expected_output="${current//[[:space:]]/}dsh3>dsh3>dsh3>cmdloopreturned0"

    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"

    [ "$stripped_output" = "$expected_output" ]
}

@test "CD to valid directory works" {
    run "./dsh" <<EOF
cd /tmp
pwd
EOF
    stripped_output=$(echo "$output" | tr -d '[:space:]')
    expected_output="/tmpdsh3>dsh3>dsh3>cmdloopreturned0"

    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"

    [ "$stripped_output" = "$expected_output" ]
}

@test "CD to invalid directory shows error" {
    run "./dsh" <<EOF
cd /nonexistent/directory
EOF
    [[ "$output" == *"No such file or directory"* ]]
}

@test "Execute command with arguments" {
    run "./dsh" <<EOF
echo test output here
EOF
    stripped_output=$(echo "$output" | tr -d '\n\r')
    expected_output="test output heredsh3> dsh3> cmd loop returned 0"

    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"

    [ "$stripped_output" = "$expected_output" ]
}

@test "Exit command works correctly" {
    run "./dsh" <<EOF
exit
EOF
    stripped_output=$(echo "$output" | tr -d '[:space:]')
    expected_output="dsh3>cmdloopreturned0"

    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"

    [ "$stripped_output" = "$expected_output" ]
}

@test "handle quoted strings" {
    run "./dsh" <<EOF
echo "   test with   spaces   "
EOF
    # Strip all whitespace (spaces, tabs, newlines) from the output
    stripped_output=$(echo "$output" | tr -d '\n\r')
    expected_output="   test with   spaces   dsh3> dsh3> cmd loop returned 0"

    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"

    [ "$stripped_output" = "$expected_output" ]
}

@test "Failed command handling" {
    run "./dsh" <<EOF
nonexistentcommand
EOF
    [[ "$output" == *"No such file or directory"* ]]
}

@test "Multiple commands in sequence" {
    run "./dsh" <<EOF
pwd
cd /tmp
pwd
EOF
    [[ "$output" == *"/tmp"* ]]
    [ "$status" -eq 0 ]
}

@test "Handle multiple spaces and tabs mixed" {
    run "./dsh" <<EOF
echo  test   	with	 spaces
EOF
    stripped_output=$(echo "$output" | tr -d '[:space:]')
    expected_output="testwithspacesdsh3>dsh3>cmdloopreturned0"

    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"

    [ "$stripped_output" = "$expected_output" ]
}

@test "Handle empty quoted strings" {
    run "./dsh" <<EOF
echo ""
EOF
    stripped_output=$(echo "$output" | tr -d '\n\r')
    expected_output="dsh3> dsh3> cmd loop returned 0"

    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"

    [ "$stripped_output" = "$expected_output" ]
}

@test "check path command execution" {
    run "./dsh" <<EOF
which ls
EOF
    stripped_output=$(echo "$output" | tr -d '[:space:]')
    [[ "$stripped_output" == *"/bin/ls"* || "$stripped_output" == *"/usr/bin/ls"* ]]
}

@test "Handle path traversal with cd" {
    run "./dsh" <<EOF
cd /
cd usr
cd ..
pwd
EOF
    stripped_output=$(echo "$output" | tr -d '[:space:]')
    expected_output="/dsh3>dsh3>dsh3>dsh3>dsh3>cmdloopreturned0"

    [ "$stripped_output" = "$expected_output" ]
}

@test "Echo with mixed quotes" {
    run "./dsh" <<EOF
echo "test'quoted'test"
EOF
    stripped_output=$(echo "$output" | tr -d '\n\r')
    expected_output="test'quoted'testdsh3> dsh3> cmd loop returned 0"

    [ "$stripped_output" = "$expected_output" ]
}

@test "Test multiple sequential commands" {
    run "./dsh" <<EOF
echo first
echo second
echo third
EOF
    stripped_output=$(echo "$output" | tr -d '[:space:]')
    expected_output="firstsecondthirddsh3>dsh3>dsh3>dsh3>cmdloopreturned0"

    echo "Captured stdout:"
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "Stripped output: '${stripped_output}'"
    echo "Expected output: '${expected_output}'"

    [ "$stripped_output" = "$expected_output" ]
}

@test "Handle leading/trailing whitespace" {
    run "./dsh" <<EOF
    echo test    
EOF
    stripped_output=$(echo "$output" | tr -d '[:space:]')
    expected_output="testdsh3>dsh3>cmdloopreturned0"

    [ "$stripped_output" = "$expected_output" ]
}

@test "Verify CD path correctness" {
    run "./dsh" <<EOF
cd /tmp
pwd
cd -
pwd
EOF
    [[ "$output" == *"/tmp"* ]]
    [ "$status" -eq 0 ]
}

@test "test file commands" {
    run "./dsh" <<EOF
file /bin/ls
EOF
    [[ "$output" == *"executable"* ]]
    [ "$status" -eq 0 ]
}

@test "Simple pipe: ls | grep c" {
    run "./dsh" <<EOF
ls | grep c
EOF
    [[ "$output" == *"c"* ]]
    [ "$status" -eq 0 ]
}

@test "Pipe with multiple words: echo hello world | wc -w" {
    run "./dsh" <<EOF
echo hello world | wc -w
EOF
    stripped_output=$(echo "$output" | tr -d '[:space:]')
    [[ "$stripped_output" == *"2"* ]]
}

@test "Multiple pipes: ls | grep c | wc -l" {
    run "./dsh" <<EOF
ls | grep c | wc -l
EOF
    [ "$status" -eq 0 ]
    [[ "$output" =~ [0-9]+ ]]
}

@test "Pipe with quoted arguments: echo 'hello | world' | wc -c" {
    run "./dsh" <<EOF
echo 'hello | world' | wc -c
EOF
    [ "$status" -eq 0 ]
    [[ "$output" =~ [0-9]+ ]]
}

@test "Empty first command in pipe: | grep test" {
    run "./dsh" <<EOF
| grep test
EOF
    [ "$status" -eq 0 ]
}

@test "Error in first command of pipe: nonexistentcommand | grep test" {
    run "./dsh" <<EOF
nonexistentcommand | grep test
EOF
    [[ "$output" == *"No such file or directory"* ]]
}

@test "Empty second command in pipe: ls |" {
    run "./dsh" <<EOF
ls |
EOF
    [ "$status" -eq 0 ]
}

@test "Pipe with spaces before and after pipe: ls  |  grep c" {
    run "./dsh" <<EOF
ls  |  grep c
EOF
    [ "$status" -eq 0 ]
    [[ "$output" == *"c"* ]]
}

@test "Pipe with multiple spaces between commands: ls   |   grep   c" {
    run "./dsh" <<EOF
ls   |   grep   c
EOF
    [ "$status" -eq 0 ]
    [[ "$output" == *"c"* ]]
}

@test "Three pipes: ls | grep c | sort | head -n 1" {
    run "./dsh" <<EOF
ls | grep c | sort | head -n 1
EOF
    [ "$status" -eq 0 ]
}

@test "Pipe with simple built-in command: echo hello | wc -c" {
    run "./dsh" <<EOF
echo hello | wc -c
EOF
    [ "$status" -eq 0 ]
    [[ "$output" =~ [0-9]+ ]]
}

@test "Pipe output to file command: echo hello | file -" {
    run "./dsh" <<EOF
echo hello | file -
EOF
    [ "$status" -eq 0 ]
    [[ "$output" == *"ASCII text"* ]]
}

@test "Pipe with tab characters: ls\t|\tgrep c" {
    run "./dsh" <<EOF
ls	|	grep c
EOF
    [ "$status" -eq 0 ]
    [[ "$output" == *"c"* ]]
}

@test "Pipe with large output: cat /etc/passwd | wc -l" {
    run "./dsh" <<EOF
cat /etc/passwd | wc -l
EOF
    [ "$status" -eq 0 ]
    [[ "$output" =~ [0-9]+ ]]
}

@test "Pipe preserves standard error: ls nonexistentfile | grep error" {
    run "./dsh" <<EOF
ls nonexistentfile | grep error
EOF
    [[ "$output" == *"No such file or directory"* ]]
}

@test "Pipe with command that produces no output: echo | grep nonexistent" {
    run "./dsh" <<EOF
echo | grep nonexistent
EOF
    [ "$status" -eq 0 ]
    [[ ! "$output" =~ nonexistent ]]
}

@test "Maximum number of pipes: cmd1 | cmd2 | cmd3 | cmd4 | cmd5 | cmd6 | cmd7 | cmd8" {
    run "./dsh" <<EOF
echo a | grep a | grep a | grep a | grep a | grep a | grep a | grep a
EOF
    [[ "$status" -eq 0 || "$output" == *"piping limited"* ]]
}

@test "Pipe with whitespace-only command: echo hello |     | grep hello" {
    run "./dsh" <<EOF
echo hello |     | grep hello
EOF
    [ "$status" -eq 0 ]
}

@test "Verify pipe doesn't affect subsequent commands: ls | grep c && echo success" {
    run "./dsh" <<EOF
ls | grep c
echo success
EOF
    [[ "$output" == *"success"* ]]
    [ "$status" -eq 0 ]
}

@test "File redirection: output" {
    run "./dsh" <<EOF
echo "hello, class" > out.txt
cat out.txt
EOF
    [[ "$output" == *"hello, class"* ]]
    [ "$status" -eq 0 ]
    rm -f out.txt
}