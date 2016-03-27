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
            arg_list = identifier[identifier.find('(') + 1:-1]
            arg_list = arg_list.split(',')
            arg_to_argnum = {}
            argnum_to_actual = {}
            for i in range(len(arg_list)):
                arg_to_argnum[arg_list[i]] = i
                argnum_to_actual[i] = ""
            function_like_table[identifier[:identifier.find('(') + 1]] = (arg_to_argnum, argnum_to_actual, replacement)
        else:
            #obj-like macro
            object_like_table[identifier] = replacement
    else:
        #line is not a macro definition, check for replacements
        #assumes macro identifiers are not present where they should not be expanded e.g. in string literals
        #assumes no macro identifier is a substring of another
        #assumes formal parameter name is not a substring of replacement token list
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
                    while func_like_macro in line:
                        #get parameters from macro call
                        curr_param_num = 0
                        curr_bracket_count = 1
                        curr_param = ""
                        total_call = func_like_macro
                        i = line.find(func_like_macro) + len(func_like_macro)
                        while (curr_bracket_count > 0):
                            if line[i] == ',' and curr_bracket_count == 1:
                                #param done, add to macro table
                                function_like_table[func_like_macro][1][curr_param_num] = curr_param
                                curr_param = ""
                                curr_param_num += 1
                            elif line[i] == ')' and curr_bracket_count == 1:
                                function_like_table[func_like_macro][1][curr_param_num] = curr_param
                                curr_bracket_count -= 1
                            elif line[i] == '(':
                                curr_bracket_count += 1
                                curr_param += line[i]
                            elif line[i] == ')':
                                curr_bracket_count -= 1
                                curr_param += line[i]
                            else:
                                curr_param += line[i]
                            total_call += line[i]
                            i += 1
                        print("total_call: " + total_call)
                        #construct replacement string
                        replacement_string = function_like_table[func_like_macro][2]
                        for formal_param in function_like_table[func_like_macro][0].keys():
                            replacement_string = replacement_string.replace(formal_param, function_like_table[func_like_macro][1][function_like_table[func_like_macro][0][formal_param]])
                            print("Replaced " + formal_param + " with " + function_like_table[func_like_macro][1][function_like_table[func_like_macro][0][formal_param]])

                        #make substitution
                        line = line.replace(total_call, replacement_string)
                        print("Replaced " + total_call + " with " + replacement_string)
                        print(line)

                    seen.add(func_like_macro)

        output_file.write(line)

input_file.close()
output_file.close()
