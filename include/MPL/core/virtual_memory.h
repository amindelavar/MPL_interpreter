//
// Created by amindelavar on 9/22/2018.
//

#ifndef MPL_VIRTUAL_MEMORY_H
#define MPL_VIRTUAL_MEMORY_H

void init_memory();

uint32 return_Mpoint_hash_index(long_int id);

void append_Mvar(Mvar s);

void delete_Mvar(long_int i);

void append_Mpoint(Mpoint s);

void delete_Mpoint(long_int id);

Mvar get_Mvar(long_int i);

Mpoint get_Mpoint(long_int id);
void edit_Mpoint(long_int id, String data, uint8 subtype, Boolean set_data, Boolean set_type);
Boolean append_Mpoint_pointer(long_int id, long_int new_pointer);
void change_Mvar_pointer_id(long_int i, long_int new_po);
Boolean change_Mvar_flag(long_int i, uint8 flag);
Mpoint return_var_memory_value(String var_name);
Mpoint __return_var_memory_value(String var_name);

    long_int get_data_memory_id(long_int pointer_id, String index_var);

long_int
set_memory_var(long_int fin, long_int sid, String name, String value_var, String type_var, Boolean is_create_var);

long_int set_struct_node_Mpoint(vaar_en st);

long_int
set_parent_nodes_Mpoint(int32 max_indexes[], uint8 indexes_len, longint_list pointers_id, uint32 pointers_id_len);

long_int add_to_var_memory(long_int pointer_id, long_int fin, long_int sid, long_int type_var, String name, uint8 flag);

long_int add_to_pointer_memory(String data, uint8 type_data);

void show_memory(uint8 wh);

Boolean delete_full_memory_var(long_int var_ind, Boolean is_del_var);

Boolean delete_pointer_memory(long_int id);

Boolean delete_var_memory(long_int id);

long_int find_index_var_memory(long_int id);

long_int return_var_ind_pointer_id(long_int pointer_id);

uint32 return_array_dimensions(long_int pointer_id, str_list *ret);

long_int copy_memory_var(long_int var_index, String new_name, long_int fin);
uint32 recursive_list_pointer_ids(long_int pointer_id, longint_list *ret);
uint32 delete_array_Mpoints(long_int pointer_id, Boolean is_delete_root);
#endif //MPL_VIRTUAL_MEMORY_H
