!#/bin/sh

clear;
make re && valgrind ./ircserv 6667 1234
