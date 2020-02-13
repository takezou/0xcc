#!/bin/bash
try() {
    expected="$1"
    input="$2"

    ./0xcc "$input" > tmp.s
    gcc -o tmp tmp.s
    ./tmp
    actual="$?"

    if [ "$actual" = "$expected" ]; then
	echo "$input => $actual"
    else
	echo "$input => $expected expected, but goot $actual"
	exit 1
    fi
}

try 0 0
try 42 42

try 3 1+2
try 6 2+3+5-4

echo OK
