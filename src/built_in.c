//
// Created by amindelavar on 9/19/2018.
//
#include <MPL/system.h>

//***********************************
Boolean check_built_in_module_function(String S_params, str_list partypes, uint32 params_len) {
  //convert func_params to std
  str_list func_params = 0;
  uint32 func_params_len = char_split(S_params, '|', &func_params, true);
  for (uint32 i = 0; i < func_params_len; i++) {
    str_list seg = 0;
    uint32 seg_len = char_split(func_params[i], ';', &seg, true);
    if (seg_len > 1)func_params[i] = str_multi_append(seg[0], ";var;", seg[1], 0, 0, 0);
    else func_params[i] = str_multi_append(seg[0], ";var;0", 0, 0, 0, 0);
  }
//		  printf("#####@@:%s,%s,[%i,%i]\n",print_str_list (func_params, func_params_len),print_str_list ( partypes, params_len),func_params_len,params_len);
  if (is_exact_function(func_params, func_params_len, partypes, params_len, true)) {
    //printf("-----------@@@\n");
    return true;
  }
  return false;
}
//***********************************
void convert_built_in_module_vars_to_values(str_list partypes,
                                            str_list params,
                                            uint32 params_len,
                                            str_list *argvs,
                                            long_int *var0_ind) {
  for (uint32 i = 0; i < params_len; i++) {
    str_list p1 = 0;
    char_split(partypes[i], ';', &p1, false);
    //is var
    if (str_ch_equal(p1[2], '1') || str_ch_equal(p1[2], '2')) {
      String var_val = 0;
//      printf("###argv%i:%s(VAR)%s\n", i, params[i], partypes[i]);
      //=>if var not has any index
      if (str_ch_equal(p1[3], '_')) var_val = return_value_var_complete(str_to_long_int(p1[1]));
        //=> else if var has index
      else {
        long_int po_id = get_Mvar(str_to_long_int(p1[1])).pointer_id;
        long_int data_id = get_data_memory_id(po_id, p1[3]);
        var_val = __return_value_var_complete(data_id);
      }
      if (str_ch_equal(p1[3], '_') || str_ch_equal(p1[3], '1') && var_val != 0 && var_val[0] != '{')
        var_val = str_reomve_quotations(var_val, p1[0]);
      //=>if string is utf8
      if (p1[0][0] == 's' && str_indexof(var_val, UTF8_ID_LABEL, 0) == 0) {
        var_val = utf8_to_bytes_string(get_utst_by_label(var_val).utf8_string);
      }
      str_list_append(&(*argvs), var_val, i);
      //if param[0] is var, store its index
      if (i == 0) (*var0_ind) = str_to_long_int(p1[1]);
    }
      //is value
    else {
      String ret1 = 0;
      ret1 = return_value_from_expression(params[i], p1[0]);
      //=>if string is utf8
      if (p1[0][0] == 's' && str_indexof(ret1, UTF8_ID_LABEL, 0) == 0) {
        ret1 = utf8_to_bytes_string(get_utst_by_label(ret1).utf8_string);
      } else ret1 = str_reomve_quotations(ret1, p1[0]);
//      printf("###argv%i:%s(%s)=>%s\n", i, params[i], p1[0], ret1);
      str_list_append(&(*argvs), ret1, i);
    }
  }
}
//***********************************
uint32 call_built_in_funcs(String func_name, str_list params, str_list partypes, uint32 params_len, str_list *returns) {
  //-----------------init vars
  uint32 func_id = 0, returns_len = 0;
  uint8 func_type = 0;
  str_list argvs = 0, argvs_type = 0;
  long_int var0_ind = 0;
  //-----------------find function
  bifs *st = entry_table.bifs_start;
  for (;;) {
    //if func_name is true
    if (str_equal(st->func_name, func_name)) {
      Boolean is_exist = check_built_in_module_function(st->params, partypes, params_len);
      if (is_exist) {
        func_id = st->id;
        func_type = st->type;
        break;
      }
    }
    st = st->next;
    if (st == 0) break;
  }

  //-----------------call function
  if (func_id == 0 || func_type == 0)return 0;
  //convert vars to values
  convert_built_in_module_vars_to_values(partypes, params, params_len, &argvs, &var0_ind);
  //=>get argvs_type
  for (uint32 i = 0; i < params_len; i++) {
    str_list tmp = 0;
    char_split(partypes[i], ';', &tmp, true);
    str_list_append(&argvs_type, tmp[0], i);
  }
//print_struct(PRINT_STRUCT_DES_ST);
//  printf("@BUILT-IN:%s\n%s\n%s\n", print_str_list(params, params_len),print_str_list(partypes, params_len), print_str_list(argvs, params_len));
  //---------------------MPL Function Calls----------------------
  if (func_type == MPL_BUILT_IN_TYPE) {
    if (func_id == _MPL_LEN) {
      String ret = _MPL_TYPE__len(partypes[0]);
      str_list_append(&(*returns), ret, returns_len++);
    } else if (func_id == _MPL_TYPEOF) {
      String ret = _MPL_TYPE__typeof(argvs[0], argvs_type[0]);
      str_list_append(&(*returns), convert_to_string(ret), returns_len++);
      // printf ("Result of \'typeof\':%s[%s]=>%s\n", argvs[0], p[0], ret);
    } else if (func_id == _MPL_PUSH) {
      uint32 ret = _MPL_TYPE__push(var0_ind, argvs[1], argvs_type[1], argvs[2]);
      str_list_append(&(*returns), str_from_long_int((long_int) ret), returns_len++);
      //printf ("Result of \'input\':%s=>%s\n", argvs[0], ret);
    } else if (func_id == _MPL_VAR_TYPE) {
      String ret = argvs_type[0];
      str_list_append(&(*returns), convert_to_string(ret), returns_len++);
      // printf ("Result of \'var_type\':%s[%s]=>%s\n", argvs[0], p[0], ret);
    } else if (func_id == _MPL_ERROR_HANDLE) {
      Boolean ret = _MPL_TYPE__error_handle(str_to_int32(argvs[0]),
                                            argvs[1], argvs[2]);
      str_list_append(&(*returns), convert_to_string(str_from_bool(ret)), returns_len++);
    } else if (func_id == _MPL_CONFIG_ALL) {
      String ret = _MPL_TYPE__mm_all(CONFIG_MAGIC_MACRO_TYPE);
      str_list_append(&(*returns), ret, returns_len++);
    } else if (func_id == _MPL_DEFINE_ALL) {
      String ret = _MPL_TYPE__mm_all(DEFINE_MAGIC_MACRO_TYPE);
      str_list_append(&(*returns), ret, returns_len++);
    } else if (func_id == _MPL_SESSION_ALL) {
      String ret = _MPL_TYPE__mm_all(SESSION_MAGIC_MACRO_TYPE);
      str_list_append(&(*returns), ret, returns_len++);
    } else if (func_id == _MPL_DEFINE_ISSET) {
      Boolean ret = _MPL_TYPE__mm_isset(DEFINE_MAGIC_MACRO_TYPE, argvs[0]);
      str_list_append(&(*returns), str_from_bool(ret), returns_len++);
    } else if (func_id == _MPL_SESSION_ISSET) {
      Boolean ret = _MPL_TYPE__mm_isset(SESSION_MAGIC_MACRO_TYPE, argvs[0]);
      str_list_append(&(*returns), str_from_bool(ret), returns_len++);
    } else if (func_id == _MPL_EXECUTE) {
      Boolean ret = _MPL_TYPE__exec(argvs[0]);
      str_list_append(&(*returns), str_from_bool(ret), returns_len++);
    } else if (func_id == _MPL_CROP) {
      String ret = _MPL_TYPE__crop(var0_ind, str_to_long_int(argvs[1]), str_to_long_int(argvs[2]));
      str_list_append(&(*returns), ret, returns_len++);
    } else if (func_id == _MPL_DEL) {
      Boolean ret = _MPL_TYPE__del(var0_ind);
      str_list_append(&(*returns), str_from_bool(ret), returns_len++);
    }

    //TODO:complete
  }
    //---------------------DATA Function Calls---------------------
  else if (func_type == DATA_BUILT_IN_TYPE) {
    if (func_id == _DATA_AND) {
      String ret = _DATA_TYPE__bit_operations('a', argvs[0], argvs[1]);
      str_list_append(&(*returns), ret, returns_len++);
    } else if (func_id == _DATA_OR) {
      String ret = _DATA_TYPE__bit_operations('o', argvs[0], argvs[1]);
      str_list_append(&(*returns), ret, returns_len++);
    } else if (func_id == _DATA_XOR) {
      String ret = _DATA_TYPE__bit_operations('x', argvs[0], argvs[1]);
      str_list_append(&(*returns), ret, returns_len++);
    } else if (func_id == _DATA_NOT) {
      String ret = _DATA_TYPE__bit_operations('n', argvs[0], 0);
      str_list_append(&(*returns), ret, returns_len++);
    } else if (func_id == _DATA_RSHIFT) {
      String ret = _DATA_TYPE__bit_operations('r', argvs[0], argvs[1]);
      str_list_append(&(*returns), ret, returns_len++);
    } else if (func_id == _DATA_LSHIFT) {
      String ret = _DATA_TYPE__bit_operations('l', argvs[0], argvs[1]);
      str_list_append(&(*returns), ret, returns_len++);
    } else if (func_id == _DATA_AT) {
//      printf("DDD:%s,%s\n", argvs[0], argvs[1]);
      String ret = _DATA_TYPE__at(argvs[0], argvs[1]);
      str_list_append(&(*returns), convert_to_string(ret), returns_len++);
    } else if (func_id == _DATA_INTO) {
      String ret = _DATA_TYPE__into(argvs[0], argvs[1], argvs[2]);
      str_list_append(&(*returns), convert_to_string(ret), returns_len++);
    } else if (func_id == _DATA_TNUM) {
      String ret = _DATA_TYPE__tnum(argvs_type[0], argvs[0]);
      str_list_append(&(*returns), ret, returns_len++);
    } else if (func_id == _DATA_TSTR) {
      String ret = _DATA_TYPE__tstr(argvs_type[0], argvs[0]);
      str_list_append(&(*returns), convert_to_string(ret), returns_len++);
    } else if (func_id == _DATA_TBOOL) {
      Boolean ret = _DATA_TYPE__tbool(argvs_type[0], argvs[0]);
      str_list_append(&(*returns), str_from_bool(ret), returns_len++);
    } else if (func_id == _DATA_TARRAY) {
      String ret = _DATA_TYPE__tarray(argvs_type[0], argvs[0]);
      str_list_append(&(*returns), ret, returns_len++);
    } else if (func_id == _DATA_IBOOL) {
      Boolean ret = _DATA_TYPE__ibool(argvs[0]);
      str_list_append(&(*returns), str_from_bool(ret), returns_len++);
    } else if (func_id == _DATA_INUM) {
      Boolean ret = _DATA_TYPE__inum(argvs[0]);
      str_list_append(&(*returns), str_from_bool(ret), returns_len++);
    } else if (func_id == _DATA_DBSLASH) {
      String ret = _DATA_TYPE__dbslah(argvs[0]);
      str_list_append(&(*returns), convert_to_string(ret), returns_len++);
    }
    //TODO:complete
  }
    //---------------------OS Function Calls-----------------------
  else if (func_type == OS_BUILT_IN_TYPE) {
    if (func_id == _OS_EXIT) {
      _OS_TYPE__exit(str_to_int32(argvs[0]));
    } else if (func_id == _OS_PRINT) {
      Boolean ret = _OS_TYPE__print(argvs, params_len);
      str_list_append(&(*returns), str_from_bool(ret), returns_len++);
    } else if (func_id == _OS_INPUT) {
      String ret = _OS_TYPE__input(argvs[0]);
      str_list_append(&(*returns), convert_to_string(ret), returns_len++);
      //printf ("Result of \'input\':%s=>%s\n", argvs[0], ret);
    } else if (func_id == _OS_SHELL) {
      String ret = _OS_TYPE__shell(argvs[0]);
      str_list_append(&(*returns), convert_to_string(ret), returns_len++);
    } else if (func_id == _OS_TIME) {
      String ret = _OS_TYPE__time();
      str_list_append(&(*returns), ret, returns_len++);
    } else if (func_id == _OS_RAND) {
      String ret = _OS_TYPE__rand(argvs[0], argvs[1]);
//      printf("RAND:%s,%s=>%s\n",argvs[0],argvs[1],ret);
      str_list_append(&(*returns), ret, returns_len++);
    } else if (func_id == _OS_ARGVS) {
      String ret = _OS_TYPE__argvs();
//      printf("ARGVS:%s\n",ret);
      str_list_append(&(*returns), ret, returns_len++);
    } else if (func_id == _OS_PRINTF) {
      Boolean ret = _OS_TYPE__printf(argvs_type, argvs, params_len);
      str_list_append(&(*returns), str_from_bool(ret), returns_len++);
    }else if (func_id == _OS_DATE) {
      String ret = _OS_TYPE__date_english(str_to_long_int(argvs[0]),argvs[1]);
      str_list_append(&(*returns), convert_to_string(ret), returns_len++);
    }
  }
    //---------------------FS Function Calls-----------------------
  else if (func_type == FS_BUILT_IN_TYPE) {
    //=>init _path from argvs[0] is exist and was a 'str' type
    String _path = 0;
    if (params_len > 0 && str_equal(argvs_type[0], "str"))
      _path = convert_mplpath_to_abspath(argvs[0]);
    if (func_id == _FS_ABSPATH) {
      String ret = __syscall_abspath(_path);
//      printf("Abspath:(%s)%s=>%s=>%s\n",argvs_type[0],argvs[0],_path,ret);
      str_list_append(&(*returns), convert_to_string(ret), returns_len++);
    } else if (func_id == _FS_MKDIR) {
      Boolean ret = __syscall_mkdir(_path, false);
//      printf("mkdir:%s=>%s\n",argvs[0],_path);
      str_list_append(&(*returns), str_from_bool(ret), returns_len++);
    } else if (func_id == _FS_FOPEN) {
      int32 ret = _FS_TYPE__fopen(_path, argvs[1]);
      str_list_append(&(*returns), str_from_int32(ret), returns_len++);
    } else if (func_id == _FS_FCLOSE) {
      Boolean ret = _FS_TYPE__fclose(str_to_int32(argvs[0]));
      str_list_append(&(*returns), str_from_bool(ret), returns_len++);
    }else if (func_id == _FS_FWRITE) {
      int32 ret = _FS_TYPE__fwrite(str_to_int32(argvs[0]),argvs[1]);
      str_list_append(&(*returns), str_from_int32(ret), returns_len++);
    }
  }
  //-----------------return

  return returns_len;
}
//***********************************

//***********************************
void init_built_in_funcs() {
  add_to_bifs(0, 0, 0, 0, 0);
  /**
   * a=bool|str|num|struct|var[0] : value
   * aa=bool[?,..]|str[?,..]|num[?,..]|struct[?,..] : var
   * aa..=bool[?,..]|str[?,..]|num[?,..]|struct[?,..] : var,var,..
   */
  //----------------------------------mpl built_in [..]
  add_to_bifs(_MPL_LEN, MPL_BUILT_IN_TYPE, "len", "aa", "num;?");  //=>[OK]
  add_to_bifs(_MPL_VAR_TYPE, MPL_BUILT_IN_TYPE, "type", "aa", "str"); //=>[OK]
  add_to_bifs(_MPL_TYPEOF, MPL_BUILT_IN_TYPE, "typeof", "a", "str"); //=>[OK]
  add_to_bifs(_MPL_ERROR_HANDLE, MPL_BUILT_IN_TYPE, "ErrHandle", "num|str|str", "bool"); //=>[OK]
  add_to_bifs(_MPL_CONFIG_ALL, MPL_BUILT_IN_TYPE, "ConAll", 0, "str;?,2");//=>[OK]
  add_to_bifs(_MPL_DEFINE_ALL, MPL_BUILT_IN_TYPE, "DefAll", 0, "str;?,2");//=>[OK]
  add_to_bifs(_MPL_DEFINE_ISSET, MPL_BUILT_IN_TYPE, "DefIsset", "str", "bool");//=>[OK]
  add_to_bifs(_MPL_EMBED_RUN, MPL_BUILT_IN_TYPE, "EmbedRun", "str;?", "num");
  add_to_bifs(_MPL_SESSION_ALL, MPL_BUILT_IN_TYPE, "SesAll", 0, "str;?,2");//=>[OK]
  add_to_bifs(_MPL_SESSION_ISSET, MPL_BUILT_IN_TYPE, "SesIsset", "str", "bool");//=>[OK]
  add_to_bifs(_MPL_CROP, MPL_BUILT_IN_TYPE, "crop", "aa|num|num", "aa"); //=>[OK]
  add_to_bifs(_MPL_PUSH, MPL_BUILT_IN_TYPE, "push", "aa|a|num", "num");//=>[OK]
  add_to_bifs(_MPL_POP, MPL_BUILT_IN_TYPE, "pop", "aa|num", "num");
  add_to_bifs(_MPL_DEL, MPL_BUILT_IN_TYPE, "del", "aa", "bool"); //=>[..]
  add_to_bifs(_MPL_EXECUTE, MPL_BUILT_IN_TYPE, "exec", "str", "bool"); //=>[OK]
  add_to_bifs(_MPL_ECHO, MPL_BUILT_IN_TYPE, "echo", "aa..", "bool");
  add_to_bifs(_MPL_DEFINE_UNSET, MPL_BUILT_IN_TYPE, "DefUnset", "str", "bool");
  add_to_bifs(_MPL_SESSION_UNSET, MPL_BUILT_IN_TYPE, "SesUnset", "str", "bool");
  //----------------------------------data types built_in [OK]
  add_to_bifs(_DATA_TNUM, DATA_BUILT_IN_TYPE, "tnum", "a", "num"); //=>[OK]
  add_to_bifs(_DATA_TBOOL, DATA_BUILT_IN_TYPE, "tbool", "a", "bool"); //=>[OK]
  add_to_bifs(_DATA_TSTR, DATA_BUILT_IN_TYPE, "tstr", "a", "str"); //=>[OK]
  add_to_bifs(_DATA_TARRAY, DATA_BUILT_IN_TYPE, "tarray", "a", "str;?"); //=>[OK]
  add_to_bifs(_DATA_AT, DATA_BUILT_IN_TYPE, "at", "str|num", "str"); //=>[OK]
  add_to_bifs(_DATA_INTO, DATA_BUILT_IN_TYPE, "into", "str|num|str", "str"); //=>[OK]
  add_to_bifs(_DATA_IBOOL, DATA_BUILT_IN_TYPE, "ibool", "str", "bool"); //=>[OK]
  add_to_bifs(_DATA_INUM, DATA_BUILT_IN_TYPE, "inum", "str", "bool"); //=>[OK]
  add_to_bifs(_DATA_XOR, DATA_BUILT_IN_TYPE, "xor", "num|num", "num");//=>[OK]
  add_to_bifs(_DATA_AND, DATA_BUILT_IN_TYPE, "and", "num|num", "num");//=>[OK]
  add_to_bifs(_DATA_OR, DATA_BUILT_IN_TYPE, "or", "num|num", "num");//=>[OK]
  add_to_bifs(_DATA_NOT, DATA_BUILT_IN_TYPE, "not", "num", "num");//=>[OK]
  add_to_bifs(_DATA_RSHIFT, DATA_BUILT_IN_TYPE, "rshift", "num|num", "num");//=>[OK]
  add_to_bifs(_DATA_LSHIFT, DATA_BUILT_IN_TYPE, "lshift", "num|num", "num");//=>[OK]
  add_to_bifs(_DATA_DBSLASH, DATA_BUILT_IN_TYPE, "dbslah", "str", "str");//=>[OK]
  //----------------------------------os built_in [OK]
  add_to_bifs(_OS_EXIT, OS_BUILT_IN_TYPE, "exit", "num", "bool"); //=>[OK]
  add_to_bifs(_OS_PRINT, OS_BUILT_IN_TYPE, "print", "aa..", "bool"); //=>[OK]
  add_to_bifs(_OS_PRINTF, OS_BUILT_IN_TYPE, "printf", "str|vars", "bool"); //=>[OK]
  add_to_bifs(_OS_INPUT, OS_BUILT_IN_TYPE, "input", "num", "str"); //=>[OK]
  add_to_bifs(_OS_SHELL, OS_BUILT_IN_TYPE, "shell", "str", "str"); //=>[OK]
  add_to_bifs(_OS_TIME, OS_BUILT_IN_TYPE, "time", 0, "num"); //=>[OK]
  add_to_bifs(_OS_RAND, OS_BUILT_IN_TYPE, "rand", "num|num", "num"); //=>[OK]
  add_to_bifs(_OS_ARGVS, OS_BUILT_IN_TYPE, "argvs", 0, "str;?"); //=>[OK]
  add_to_bifs(_OS_DATE, OS_BUILT_IN_TYPE, "date", "num|str", "str"); //=>[..]
  //----------------------------------fs built_in [..]
  add_to_bifs(_FS_ABSPATH, FS_BUILT_IN_TYPE, "abspath", "str", "str"); //=>[OK]
  add_to_bifs(_FS_MKDIR, FS_BUILT_IN_TYPE, "mkdir", "str", "bool"); //=>[OK]
  add_to_bifs(_FS_MKDIRS, FS_BUILT_IN_TYPE, "mkdirs", "str", "bool");
  add_to_bifs(_FS_FOPEN, FS_BUILT_IN_TYPE, "fopen", "str|str", "num"); //=>[OK]
  add_to_bifs(_FS_FCLOSE, FS_BUILT_IN_TYPE, "fclose", "num", "bool"); //=>[OK]
  add_to_bifs(_FS_FWRITE, FS_BUILT_IN_TYPE, "fwrite", "num|str", "num"); //=>[..]
  add_to_bifs(_FS_FCONTENT, FS_BUILT_IN_TYPE, "fcontent", "str", "str");
  add_to_bifs(_FS_FRLINE, FS_BUILT_IN_TYPE, "frline", "num", "str");
  add_to_bifs(_FS_FRUNTIL, FS_BUILT_IN_TYPE, "fruntil", "num|num", "str");
  add_to_bifs(_FS_FINFO, FS_BUILT_IN_TYPE, "finfo", "str", "str;?");
  add_to_bifs(_FS_FCOPY, FS_BUILT_IN_TYPE, "fcopy", "str|str", "bool");
//  add_to_bifs(_FS_RMDIR, FS_BUILT_IN_TYPE, "mkdirs", "str", "bool");
//  add_to_bifs(_FS_RMFILE, FS_BUILT_IN_TYPE, "abspath", "str", "str");
//  add_to_bifs(_FS_DSCAN, FS_BUILT_IN_TYPE, "mkdir", "str", "bool");
//  add_to_bifs(_FS_IDIR, FS_BUILT_IN_TYPE, "mkdirs", "str", "bool");
//  add_to_bifs(_FS_IFILE, FS_BUILT_IN_TYPE, "abspath", "str", "str");
//  add_to_bifs(_FS_PPATH, FS_BUILT_IN_TYPE, "mkdir", "str", "bool");
}

