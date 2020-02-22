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
	echo "$input => $expected expected, but got $actual"
	exit 1
    fi
}

try 0 0
try 42 42

try 3 1+2
try 6 2+3+5-4

try 17 2+3*5
try 25 '(2+3)*5'

try '1' '-1+2'
try '5' '-10+15'
try '15' '-100+2*50+15'
try '255' '-(3+297)*-1-15*3'

try 1 '1==1'
try 0 '1!=1'
try 0 '(1*1)==(1+1)'
try 1 '(1*1)!=(1+1)'
try 1 '1<2'
try 1 '1<=2'
try 0 '1>2'
try 0 '1>=2'
try 1 '1000>0'
try 0 '1000<0'
echo OK
