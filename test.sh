#!/bin/bash

assert(){
    ./bcc "$2" > temporary.s && gcc temporary.s -o temporary && ./temporary; actual="$?"
    if [ "$actual" = "$1" ]; then
        echo -en "| O | Actual($actual) Expected($1) | \"$2\" |\n"
    else
        echo -en "| X | Actual($actual) Expected($1) | \"$2\" |\n"
    fi
}

assert 42 "42"
assert 3 "1+2*3-4"
assert 8 " 5 + 7 / 2 "
rm -f temporary.s temporary
