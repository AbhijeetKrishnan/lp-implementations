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
            function_like_table[identifier[:identifier.find('(') + 1]] = (arg_to_argnum, argnum_to_actual)
        else:
            #obj-like macro
            object_like_table[identifier] = replacement
    else:
        #line is not a macro definition, check for replacements
        #assumes macro identifiers are not present where they should not be expanded e.g. in string literals
        #assumes no macro identifier is a substring of another
        no_change = False
        seen = set()
        while not no_change:
            no_change = True
            for obj_like_macro in object_like_table.keys():
                if obj_like_macro in line and obj_like_macro not in seen:
                    no_change = False
                    line = line.replace(obj_like_macro, object_like_table[obj_like_macro])
                    print("Replaced " + obj_like_macro + " with " + object_like_table[obj_like_macro])
                    print(line)
                    seen.add(obj_like_macro)
            for func_like_macro in function_like_table.keys():
                if func_like_macro in line and func_like_macro not in seen:
                    no_change = False
                    #add logic for replacing func-like code
                    seen.add(func_like_macro)

        output_file.write(line)

input_file.close()
output_file.close()
