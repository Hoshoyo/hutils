@echo off

pushd examples
call cl /Zi /nologo main.c ../ho_argument_parse.c
popd