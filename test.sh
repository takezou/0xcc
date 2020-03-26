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

try 0 '0;'
try 42 '42;'

try 3 '1+2;'
try 6 '2+3+5-4;'

try 17 '2+3*5;'
try 25 '(2+3)*5;'

try '1' '-1+2;'
try '5' '-10+15;'
try '15' '-100+2*50+15;'
try '255' '-(3+297)*-1-15*3;'

try 1 '1==1;'
try 0 '1!=1;'
try 0 '(1*1)==(1+1);'
try 1 '(1*1)!=(1+1);'
try 1 '1<2;'
try 1 '1<=2;'
try 0 '1>2;'
try 0 '1>=2;'
try 1 '1000>0;'
try 0 '10000<1;'

try 1 '1;'
try 5 'a=5;'
try 128 'z=1024/8;'
try '3' 'b=-(5-8);'
try '96' 'b=(5-8)*32*-1;'
try '1' 'a=1;a;'
try '2' 'a=1;a+a;'
try '1' 'a=-1;a*a;'
try '1' 'a=1;b=2;a*a;'
try '1' 'a=-1;b=a*a;'
try '86' 'a=10;b=(5-8)*32;c=-(a+b);'
try '3' 'abc=3;'
try '255' 'abc=128;zyx=abc*2;result=zyx-1;'

# this failes before node->offset is implemented properly to accomodate variable names longer than 1
try '5' 'va=5;vb=10;va;'

try '123' '_p=120+3;'
try '222' '_X=111;_Y=2;RESULT_VALUE=_X*_Y;'

try '5' 'return 5;'
try '222' '_X=111;_Y=2;RESULT_VALUE=_X*_Y;return RESULT_VALUE;'

try '1' 'if(1==1) return 1;'
try '2' 'if(1==1) 1; 2;'
try '2' 'if(1!=1) return 1; 2;'
try '2' 'if(1!=1) return 1; else 2;'
try '7' 'var1=10;var2=3;if(var1<var2) return 100; else return var1 - var2;'
echo OK
