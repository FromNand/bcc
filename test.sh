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
assert 15 "5*(9-6)"
assert 4 "(3+6)/2"
assert 10 "-10+20"
assert 10 "- -10"
assert 10 "- - +10"
rm -f temporary.s temporary
