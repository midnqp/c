#!/usr/bin/bash

CWD=`pwd`
LD_LIBRARY_PATH=$CWD/build/lib
mkdir -p $LD_LIBRARY_PATH

log_error() {
	err="\033[101m    \033[0m"
	echo -e "$err $@"
}

log_ok() {
	ok="\033[102m    \033[0m"
	echo -e "$ok $@"
}

check_compile() {
	status=$1
	name=$2
	if [ "$status" != "0" ]; then
		log_error "compilation failed for $name"
	else
		log_ok "compiled $name"
	fi
}


# build: deps/linenoise
cd ./deps/linenoise
debugopts="-Wall -Wextra   -fsanitize=address -fno-stack-protector -z execstack -no-pie"
gcc ./linenoise.c  -o ./liblinenoise.so $debugopts -c
check_compile $? linenoise
cp ./liblinenoise.so $LD_LIBRARY_PATH/
cd $CWD


# build: main
gcc ./main.c $debugopts -L $LD_LIBRARY_PATH -l linenoise  -l pthread
check_compile $? main
