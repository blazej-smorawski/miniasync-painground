#!/bin/python
import sys
import subprocess

if len(sys.argv) != 2:
    print("Usage: update-compile-commands.py build_dir")
    exit(1)

args = ['-t', 'compdb', '-x', 'c_COMPILER', 'cpp_COMPILER']
cmd = ['ninja'] + args

with open('compile_commands.json', 'w') as output_f:
    subprocess.Popen(cmd, stdout=output_f, cwd=sys.argv[1])
