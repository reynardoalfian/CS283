#!/usr/bin/env bats

# File: student_tests.sh
# 
# Create your unit tests suit in this file

@test "Example: check ls runs without errors" {
    run ./dsh <<EOF                
ls
EOF
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
    expected_output="helloworlddsh2>dsh2>cmdloopreturned0"

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
    expected_output="${current//[[:space:]]/}dsh2>dsh2>dsh2>cmdloopreturned0"

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
    expected_output="/tmpdsh2>dsh2>dsh2>cmdloopreturned0"

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
    expected_output="test output heredsh2> dsh2> cmd loop returned 0"

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
    expected_output="dsh2>cmdloopreturned0"

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
    expected_output="   test with   spaces   dsh2> dsh2> cmd loop returned 0"

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
    expected_output="testwithspacesdsh2>dsh2>cmdloopreturned0"

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
    expected_output="dsh2> dsh2> cmd loop returned 0"

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
    expected_output="/dsh2>dsh2>dsh2>dsh2>dsh2>cmdloopreturned0"

    [ "$stripped_output" = "$expected_output" ]
}

@test "Echo with mixed quotes" {
    run "./dsh" <<EOF
echo "test'quoted'test"
EOF
    stripped_output=$(echo "$output" | tr -d '\n\r')
    expected_output="test'quoted'testdsh2> dsh2> cmd loop returned 0"

    [ "$stripped_output" = "$expected_output" ]
}

@test "Test multiple sequential commands" {
    run "./dsh" <<EOF
echo first
echo second
echo third
EOF
    stripped_output=$(echo "$output" | tr -d '[:space:]')
    expected_output="firstsecondthirddsh2>dsh2>dsh2>dsh2>cmdloopreturned0"

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
    expected_output="testdsh2>dsh2>cmdloopreturned0"

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