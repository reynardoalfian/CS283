#!/usr/bin/env bats

# File: student_tests.sh
# 
# Create your unit tests suit in this file

setup_server() {
  port="$1"
  # Start server in background (using -s mode) on the specified port.
  ./dsh -s -i 0.0.0.0 -p "$port" &
  SERVER_PID=$!
  # Allow the server time to boot
  sleep 1
}

teardown_server() {
  if kill -0 "$SERVER_PID" 2>/dev/null; then
    kill "$SERVER_PID"
    # Ignore any nonzero exit code from wait by using '|| true'
    wait "$SERVER_PID" 2>/dev/null || true
  fi
  return 0
}



#########################
# Local Mode Tests
#########################

@test "Local mode: ls command runs without error" {
  run ./dsh <<EOF
ls
exit
EOF
  [ "$status" -eq 0 ]
  [[ "$output" =~ "dsh" ]]
}

@test "Local mode: echo command" {
  run ./dsh <<EOF
echo hello
exit
EOF
  [ "$status" -eq 0 ]
  [[ "$output" =~ "hello" ]]
}

@test "Local mode: built-in cd works" {
  run ./dsh <<EOF
cd ..
pwd
exit
EOF
  [ "$status" -eq 0 ]

  [[ "$output" =~ "/" ]]
}

@test "Local mode: dragon command displays art" {
  run ./dsh <<EOF
dragon
exit
EOF
  [ "$status" -eq 0 ]

  [[ "$output" =~ "%%%%" ]]
}

@test "Local mode: pipeline command" {
  run ./dsh <<EOF
ls -l | grep dsh
exit
EOF
  [ "$status" -eq 0 ]
  [[ "$output" =~ "dsh" ]]
}

@test "Local mode: redirection works" {
  run ./dsh <<EOF
echo test > test_output.txt
cat test_output.txt
rm test_output.txt
exit
EOF
  [ "$status" -eq 0 ]
  [[ "$output" =~ "test" ]]
}

#########################
# Remote Mode Tests
#########################

@test "Remote mode: echo command over persistent connection" {
  setup_server 1234
  run ./dsh -c -i 127.0.0.1 -p 1234 <<EOF
hello
exit
EOF
  teardown_server
  [ "$status" -eq 0 ]
  # Expect "hello" echoed back with a newline before the next prompt
  [[ "$output" =~ "hello" ]]
}

@test "Remote mode: multiple commands over persistent connection" {
  setup_server 1235
  run ./dsh -c -i 127.0.0.1 -p 1235 <<EOF
first
second
exit
EOF
  teardown_server
  [ "$status" -eq 0 ]
  [[ "$output" =~ "first" ]]
  [[ "$output" =~ "second" ]]
}

@test "Remote mode: external command 'date' executes successfully" {
  setup_server 1252
  run ./dsh -c -i 127.0.0.1 -p 1252 <<EOF
date
exit
EOF
  teardown_server
  [ "$status" -eq 0 ]
  # Expect the output to contain a 4-digit year (for example, "202")
  [[ "$output" =~ [0-9]{4} ]]
}


@test "Remote mode: built-in exit disconnects client" {
  setup_server 1236
  run ./dsh -c -i 127.0.0.1 -p 1236 <<EOF
exit
EOF
  teardown_server
  [ "$status" -eq 0 ]
}

@test "Remote mode: stop-server shuts down server" {
  setup_server 1237
  run ./dsh -c -i 127.0.0.1 -p 1237 <<EOF
stop-server
EOF
  # When stop-server is issued, the server returns OK_EXIT (which should translate to status 0)
  [ "$status" -eq 0 ]
}

@test "Remote mode: extra spaces and tabs in command are handled" {
  setup_server 1254
  run ./dsh -c -i 127.0.0.1 -p 1254 <<EOF
   echo    spaced    out    command   
exit
EOF
  teardown_server
  [ "$status" -eq 0 ]
  # Expect the output to contain the phrase without extra trimming issues.
  [[ "$output" =~ "spaced out command" ]]
}

@test "Remote mode: external command execution (ls)" {
  setup_server 1238
  run ./dsh -c -i 127.0.0.1 -p 1238 <<EOF
ls
exit
EOF
  teardown_server
  [ "$status" -eq 0 ]
  [[ "$output" =~ "dsh" ]]
}

@test "Remote mode: pipeline command execution" {
  setup_server 1239
  run ./dsh -c -i 127.0.0.1 -p 1239 <<EOF
ls -l | grep dsh
exit
EOF
  teardown_server
  [ "$status" -eq 0 ]
  [[ "$output" =~ "dsh" ]]
}

@test "Remote mode: command with extra spaces is handled correctly" {
  setup_server 1240
  run ./dsh -c -i 127.0.0.1 -p 1240 <<EOF
echo   hello world  
exit
EOF
  teardown_server
  [ "$status" -eq 0 ]
  [[ "$output" =~ "hello world" ]]
}

