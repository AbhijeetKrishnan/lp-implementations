#!/usr/bin/python

import sys

input_file = open(sys.argv[1], 'r')
output_file = open(sys.argv[2], 'w')

#defining data structures
object_like_table = {}
function_like_table = {}

for line in input_file:
    if "#define" in line:
        #line is a macro definition, either obj-like or func-like
        definition = line.split() #assumes no spaces in replacement token list
        identifier = definition[1]
        replacement = definition[2]
        if identifier[-1] == ')':
            #func-like macro
            arg_list = identifier[identifier.find('('):-1]
            arg_list = arg_list.split(',')
            arg_to_argnum = {}
            argnum_to_actual = {}
            for i in range(len(arg_list)):
                arg_to_argnum[arg[i]] = i
                argnum_to_actual[i] = ""
            function_like_table[identifier[:identifier.find('(')]] = (arg_to_argnum, argnum_to_actual)
        else:
            #obj-like macro
            object_like_table[identifier] = replacement
    else:
        #line is not a macro definition, check for replacements
        #assumes macro identifiers are not present where they should not be expanded e.g. in string literals
        no_change = false
        seen = {}
        


input_file.close()
output_file.close()
