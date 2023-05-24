@echo off

gcc main.c -I../../include -L../.. -lparseropt -o short_and_long_option
