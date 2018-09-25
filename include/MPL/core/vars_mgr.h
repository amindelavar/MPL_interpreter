//
// Created by amindelavar on 9/19/2018.
//

#ifndef MPL_VARS_MGR_H
#define MPL_VARS_MGR_H

Boolean is_valid_name(String name, Boolean is_array);

void return_name_index_var(String s, Boolean is_empty_index, String *name, String *index);

uint8 define_vars_analyzing(String inst, def_var_s vars_store[]);

uint8 determine_sub_type_var(String *value, String main_type);

long_int return_var_id(String var_name, String var_index);

String return_value_var_complete(long_int var_index);

#endif //MPL_VARS_MGR_H
