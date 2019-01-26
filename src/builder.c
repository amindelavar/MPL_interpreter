//
// Created by amindelavar on 1/20/2019.
//

#include <MPL/system.h>

uint32 build_id = 0;
String main_build_file = 0;
String builder_lib_path = 0;
String res_build_file = 0;
String app_filename = 0;
//*********************************************
void start_builder() {
  long_int startclock = (long_int) clock();
  printf("MPL Program Builder [mbuild]:))\n");
  printf("For more information about commands, type \"mpl -h builder\" in command-line\n");
  //=>generate a builder_id
  build_id = __syscall_rand(1, RAND_MAX);
  //=>set filename of final app
  app_filename = str_append(get_mama(CONFIG_MAGIC_MACRO_TYPE, "AppName").value, APP_EXTENSION);
  printf("***Build '%s' program (build_id:%i)***\n", app_filename, build_id);
  //=>reset global vars
//  is_programmer_debug = 0;
  entry_table.Rline = 0;
  entry_table.Rsrc = 0;
  edit_magic_config("BuildMode", "false", "bool");
  //=>level 1 : check exist builder lib,compiler,linker,resource
  printf("#check for requirements:\n");
  if (!builder_check_exist())return;
  //=>level 2 : generate main code of program in 'main.c'
  printf("#generate program codes:\n");
  if (!builder_generate_code())return;
  //=>level 3 : export program executable
  printf("#export application:\n");
  if (!builder_export_app())return;
  //=>finish building!
  String unit = 0;
  double time_taken = calculate_period_time(startclock, &unit);
  printf("Build finished in %.6f %s\n", time_taken, unit);
}

//*********************************************
Boolean builder_check_exist() {
  //=>check exist compiler
  printf("------------checking compiler:\n");
  //=>check gcc
  if (str_indexof(_OS_TYPE__shell("gcc -v"), "gcc version", 0) == -1) {
    //TODO:error
    printf("B#ERR3679\n");
    return false;
  }
  printf("\t[OK]\n");
  //=>check exist linker
  printf("------------checking linker:\n");
  //=>check ld
  if (str_indexof(_OS_TYPE__shell("ld -v"), "GNU ld", 0) == -1) {
    //TODO:error
    printf("B#ERR3678\n");
    return false;
  }
  printf("\t[OK]\n");
  //=>check exist resource
  printf("------------checking resource:\n");
  //=>check windres
  if (str_indexof(_OS_TYPE__shell("windres -V"), "GNU windres", 0) == -1) {
    //TODO:error
    printf("B#ERR3677\n");
    return false;
  }
  printf("\t[OK]\n");
  printf("------------checking %s:\n", BUILDER_LIB_NAME);
  //=>check builder library
  builder_lib_path = str_multi_append(interpreter_path, os_separator, BUILDER_LIB_NAME, 0,0,0);
  if (fopen(builder_lib_path, "rb") == NULL) {
    //TODO:error
    printf("B#ERR3676\n");
    return false;
  }
  printf("\t[OK]\n");
  return true;
}
//*********************************************
Boolean builder_generate_code() {
  //init vars
  main_build_file = str_multi_append(interpreter_tmp_path, "main_", str_from_long_int(build_id), ".c", 0, 0);
  String ccode = 0, preloader = 0;
  str_init(&ccode, "//Generated by MPL.\n\n");
  //-------------------------------
  //=> create builder include
  ccode = str_append(ccode, __generate_header_codes());
//  ccode = str_multi_append(ccode, "#include \"", interpreter_path, "/builder/builder.h\"\n", 0, 0);
  //=>create prototypes of functions and global vars
  ccode = str_append(ccode, "void pre_loader();\n");
  //=>create main function and its instructions
  ccode = str_append(ccode, "int main(int argc, char **argv){\n"
                            "init_exceptions_list_data();\n"
                            "if (argc > 2)for (int ii = 2; ii < argc; ++ii)\n"
                            " str_list_append(&program_argvs, argv[ii], argvs_len++);\n"
                            "str_init(&stdin_source_path, argv[1]);\n"
                            "stdin_source_path = __syscall_abspath(stdin_source_path);"
                            "init_data_defined();\n"
                            "  init_memory();\n"
                            "  init_built_in_funcs();\n"
                            "  init_magic_define();\n"
                            "  init_magic_config();\n"
                            "is_programmer_debug=0;\n"
                            "pre_loader();\n"
                            "start_runtime();\n"
                            "}\n");
  //=>create pre_loader function
  str_init(&preloader, "void pre_loader(){\n"
                       "//#utst#\n"
                       "//#datas#\n"
                       "//#blst_f#\n"
                       "//#blst_s#\n"
                       "//#instru#\n"
                       "}\n");
  //=>add all utst nodes
  preloader = str_replace(preloader, "//#utst#", __generate_utst_codes(), 1);
  //=>add all datas nodes
  preloader = str_replace(preloader, "//#datas#", __generate_datas_codes(), 1);
  //=>add all blst functions nodes
  preloader = str_replace(preloader, "//#blst_f#", __generate_blst_codes(FUNC_BLOCK_ID), 1);
  //=>add all blst structures nodes
  preloader = str_replace(preloader, "//#blst_s#", __generate_blst_codes(LOOP_STRU_ID), 1);
  //=>add all instru nodes
  preloader = str_replace(preloader, "//#instru#", __generate_instru_codes(), 1);
  //=>append preloader to main code
  ccode = str_append(ccode, preloader);
  //=>create 'main_[build_id].c' in tmp dir and write all codes into it!
  if (!__syscall_write_file(main_build_file, ccode)) {
    //TODO:error
    printf("B#ERR55643\n");
    return false;
  }
  printf("\t[OK]\n");
  return true;
}
//*********************************************
Boolean builder_export_app() {
  //init vars
  String main_object_file = 0;
  String app_file_path = 0;
  String final_app_path = 0;
  //--------------------------
  main_object_file = str_multi_append(interpreter_tmp_path, "main_", str_from_long_int(build_id), ".o", 0, 0);
  app_file_path = str_multi_append(interpreter_tmp_path, "main_", str_from_long_int(build_id), APP_EXTENSION, 0, 0);
  //=>compile main.c file
  printf("------------compile main code:\n");
  String tmp1 = _OS_TYPE__shell(str_multi_append("gcc -std=c99  -fmax-errors=2 -c ",
                                                 main_build_file,
                                                 " -o ",
                                                 main_object_file,
                                                 0,
                                                 0));
  if (is_programmer_debug > 1 && tmp1 != 0)printf(tmp1);
  if (tmp1 == 0)printf("\t[OK]\n");
  //=>create res_[build_id].rc file
  printf("------------generate resource:\n");
  if (!builder_generate_resource()) return false;
  printf("\t[OK]\n");
  //=>link main.o with builder lib
  printf("------------compile resource:\n");
  _OS_TYPE__shell(str_multi_append("gcc ",
                                   char_append(res_build_file, ' '),
                                   char_append(main_object_file, ' '),
                                   builder_lib_path, " -o ",
                                   app_file_path));
  printf("\t[OK]\n");
  //=>export app
  printf("------------export application:\n");
  final_app_path = get_config_mama_value("ExportBuildFile");
  if (final_app_path == 0)final_app_path = str_multi_append(project_root, os_separator, app_filename, 0, 0, 0);
  else final_app_path = str_multi_append(final_app_path, os_separator, app_filename, 0, 0, 0);
  if (!__syscall_binary_copy(app_file_path, final_app_path))return false;
  printf("\t[OK]\n");
  return true;
}
//*********************************************
Boolean builder_generate_resource() {
  String res = 0;
  String res_filename = str_multi_append(interpreter_tmp_path, "res_", str_from_long_int(build_id), ".rc", 0, 0);
  #if WINDOWS_PLATFORM == true
  res_build_file = str_multi_append(interpreter_tmp_path, "win32rc_", str_from_long_int(build_id), ".res", 0, 0);
  //=>generate AppIcon field
  if (get_config_mama_value("AppIcon") != 0)
    res = str_append(res, str_replace("1 ICON \"#\"\n", "#", get_config_mama_value("AppIcon"), 1));
  //=>generate version and others fields
  res = str_append(res, "2 VERSIONINFO\n"
                        "FILEVERSION     #VER#\n"
                        "BEGIN\n"
                        "  BLOCK \"StringFileInfo\"\n"
                        "  BEGIN\n"
                        "    BLOCK \"040904E4\"\n"
                        "    BEGIN\n"
                        "      VALUE \"CompanyName\", \"#creator#\"\n"
                        "      VALUE \"FileDescription\", \"#name#\"\n"
                        "      VALUE \"FileVersion\", \"#ver#\"\n"
                        "      VALUE \"InternalName\", \"#name#\"\n"
                        "      VALUE \"LegalCopyright\", \"#license#\"\n"
                        "      VALUE \"OriginalFilename\", \"#name#.exe\"\n"
                        "      VALUE \"ProductName\", \"#name#\"\n"
                        "      VALUE \"ProductVersion\", \"#ver#\"\n"
                        "    END\n"
                        "  END\n"
                        "  BLOCK \"VarFileInfo\"\n"
                        "  BEGIN\n"
                        "    VALUE \"Translation\", 0x409, 1252\n"
                        "  END\n"
                        "END");
  res = str_replace(res, "#creator#", get_config_mama_value("AppCreator"), 1);
  res = str_replace(res, "#ver#", get_config_mama_value("AppVersion"), -1);
  res = str_replace(res, "#name#", get_config_mama_value("AppName"), -1);
  res = str_replace(res, "#license#", get_config_mama_value("AppLicense"), 1);
  res = str_replace(res, "#VER#", "1,0,0,0", 1);
  //=>create res_[build_id].rc file
  if (!__syscall_write_file(res_filename, res)) {
    //TODO:error
    printf("B#ERR5564378\n");
    return false;
  }
  //=>compile res.rc
  if (_OS_TYPE__shell(
      str_multi_append("windres ", res_filename, " -O coff -o ", res_build_file, 0, 0)) != 0) {
    //TODO:error
    printf("B#ERR556459378\n");
    return false;
  }
  #endif
  return true;
}
//*********************************************
String __generate_blst_codes(uint8 type) {
  String ret = 0;
  long_int node_counter = 0;
  String name = 0, params = 0;
  blst *tmp1;
  if (type == FUNC_BLOCK_ID) {
    str_init(&ret, "//blst func structs:\n");
    tmp1 = entry_table.blst_func_start;
    str_init(&name, "funcnode_");
    str_init(&params, "funcparams_");
  } else {
    str_init(&ret, "//blst stru structs:\n");
    tmp1 = entry_table.blst_stru_start;
    str_init(&name, "strunode");
    str_init(&params, "struparams_");
  }
  if (tmp1 == 0) return ret;
  for (;;) {
    String node_name = str_append(name, str_from_long_int(++node_counter));
    String params_name = str_append(params, str_from_long_int(node_counter));
    if (type == FUNC_BLOCK_ID) {
      ret = str_append(ret, __generate_str_list(tmp1->params, tmp1->params_len, params_name));
      ret = str_multi_append(ret,
                             "blst ", node_name, "={0, 0, 0,",
                             str_from_int32(FUNC_BLOCK_ID),
                             str_append(",", convert_to_string(tmp1->lbl)));
      ret = str_multi_append(ret, ",", params_name, ",", str_from_long_int(tmp1->params_len), ",0,0};\n");
    } else {
      ret = str_append(ret, __generate_str_list(tmp1->params, tmp1->params_len, params_name));
      ret = str_multi_append(ret, "blst ", node_name, "={0,", str_from_long_int(tmp1->func_id), ",");
      ret = str_multi_append(ret, str_from_long_int(tmp1->stru_id), ",", str_from_int32(tmp1->type), ",\"", tmp1->lbl);
      ret = str_multi_append(ret, "\",", params_name, ",", str_from_long_int(tmp1->params_len), ",0,0};\n");
    }

    ret = str_multi_append(ret, "append_blst(", node_name, ");\n", 0, 0);
    tmp1 = tmp1->next;
    if (tmp1 == 0) break;
  }
  return ret;
}
//*********************************************
String __generate_instru_codes() {
  String ret = 0;
  str_init(&ret, "//instru structs:\n");
  long_int node_counter = 0;
  String name = 0;
  str_init(&name, "instrunode_");
  instru *tmp1 = entry_table.instru_start;
  if (tmp1 == 0) return ret;
  for (;;) {
    String node_name = str_append(name, str_from_long_int(++node_counter));
    ret = str_multi_append(ret, "instru ", node_name, "={", str_from_long_int(tmp1->id), ",");
    ret = str_multi_append(ret, str_from_long_int(tmp1->func_id), ",", str_from_long_int(tmp1->stru_id), ",", 0);
    ret = str_multi_append(ret, str_from_int32(tmp1->order), ",\"", str_to_validate(tmp1->code), "\",", 0);
    ret = str_multi_append(ret, str_from_int32(tmp1->type), ",0,0};\n", 0, 0, 0);

    ret = str_multi_append(ret, "append_instru(", node_name, ");\n", 0, 0);
    tmp1 = tmp1->next;
    if (tmp1 == 0) break;
  }
  return ret;
}
//*********************************************
String __generate_utst_codes() {
  String ret = 0;
  str_init(&ret, "//utst structs:\n");
  long_int node_counter = 0;
  String name = 0, utf8_name = 0;
  str_init(&name, "utstnode_");
  str_init(&utf8_name, "utf8str_");
  utst *tmp1 = entry_table.utst_start;
  if (tmp1 == 0) return ret;
  for (;;) {
    String node_name = str_append(name, str_from_long_int(++node_counter));
    String node_utf8_name = str_append(utf8_name, str_from_long_int(node_counter));
    //=>create an str_utf8 var
    ret = str_multi_append(ret, "str_utf8 ",
                           node_utf8_name,
                           "= utf8_encode_bytes(\"",
                           utf8_to_bytes_string(tmp1->utf8_string),
                           "\");\n");
    ret = str_multi_append(ret, "utst ", node_name, "={", str_from_long_int(tmp1->id), ",");
    ret = str_multi_append(ret, str_from_long_int(tmp1->line), ",", node_utf8_name, ",", 0);
    ret = str_multi_append(ret, str_from_int32(tmp1->max_bytes_per_char), "};\n", 0, 0, 0);

    ret = str_multi_append(ret, "append_utst(", node_name, ");\n", 0, 0);
    tmp1 = tmp1->next;
    if (tmp1 == 0) break;
  }
  return ret;
}
//*********************************************
String __generate_datas_codes() {
  String ret = 0;
  str_init(&ret, "//datas structs:\n");
  long_int node_counter = 0;
  String name = 0, params_name = 0;
  str_init(&name, "datasnode_");
  str_init(&params_name, "datasparams_");
  datas *tmp1 = entry_table.datas_start;
  if (tmp1 == 0) return ret;
  for (;;) {
    //=>ignore MAIN_DATA_TYPE structs + exception struct
    if (tmp1->type == MAIN_DATA_TYPE || str_equal(tmp1->name, "exception")) {
      goto IGNORE_LOOP;
    }
    String node_name = str_append(name, str_from_long_int(++node_counter));
    String params_node_name = str_append(params_name, str_from_long_int(node_counter));
    ret = str_append(ret, __generate_str_list(tmp1->params, tmp1->params_len, params_node_name));
    ret = str_multi_append(ret, "datas ", node_name, "={0,", str_from_long_int(tmp1->fid), ",\"");
    ret = str_multi_append(ret, tmp1->name, "\",", str_from_int32(tmp1->type), ",", 0);
    ret = str_multi_append(ret, params_node_name, ",", str_from_int32(tmp1->params_len), "};\n", 0);

    ret = str_multi_append(ret, "append_datas(", node_name, ");\n", 0, 0);
    IGNORE_LOOP:
    tmp1 = tmp1->next;
    if (tmp1 == 0) break;
  }
  return ret;
}
//*********************************************
String __generate_str_list(str_list s, uint32 slen, String name) {
  String nodes = 0;
  if (slen == 0) return str_multi_append("str_list ", name, "=0;\n", 0, 0, 0);
  for (uint32 i = 0; i < slen; i++) {
    nodes = str_multi_append(nodes, "\"", s[i], "\"", 0, 0);
    if (i + 1 < slen)nodes = char_append(nodes, ',');
  }
  return str_multi_append("String ", name, "[]={", nodes, "};\n", 0);
}
//*********************************************
String __generate_header_codes() {
  String ret = 0;
  str_init(&ret, "#define true  1\n"
                 "#define false 0\n");
  //---------------------------------
  ret = str_multi_append(ret, "#define  LINUX_PLATFORM ", str_from_bool(LINUX_PLATFORM), "\n", 0, 0);
  ret = str_multi_append(ret, "#define  WINDOWS_PLATFORM ", str_from_bool(WINDOWS_PLATFORM), "\n", 0, 0);
  ret = str_multi_append(ret, "#define  x86_OS ", str_from_bool(x86_OS), "\n", 0, 0);
  ret = str_multi_append(ret, "#define  x64_OS ", str_from_bool(x64_OS), "\n", 0, 0);
  //---------------------------------
  ret = str_append(ret, "typedef char int8;\n"
                        "typedef unsigned char uint8;\n"
                        "typedef short int16;\n"
                        "typedef unsigned short uint16;\n"
                        "typedef int int32;\n"
                        "typedef unsigned int uint32;\n"
                        "typedef unsigned long long int uint64;\n"
                        "typedef long int num_int;\n"
                        "typedef double num_float;\n"
                        "typedef unsigned char *String;\n"
                        "typedef unsigned char **str_list;\n"
                        "typedef uint32 **utf8_str_list;\n"
                        "typedef uint32 *str_utf8;\n"
                        "typedef uint64 long_int;\n"
                        "typedef long_int *longint_list;\n"
                        "typedef unsigned char Boolean;\n");
  //---------------------------------
  ret = str_append(ret, "String project_root;\n"
                        "str_list program_argvs;\n"
                        "uint32 argvs_len;\n"
                        "String stdin_source_path;\n"
                        "uint8 is_programmer_debug;\n");
  //---------------------------------
  ret = str_append(ret, "typedef struct utf8_strings_struct {\n"
                        "  long_int id;\n"
                        "  uint32 line;\n"
                        "  str_utf8 utf8_string;\n"
                        "  uint8 max_bytes_per_char;\n"
                        "  struct utf8_strings_struct *next;\n"
                        "} utst;\n"
                        "//****************************instructions struct\n"
                        "typedef struct instructions_struct {\n"
                        "  long_int id;\n"
                        "  long_int func_id;\n"
                        "  long_int stru_id;\n"
                        "  uint32 order;\n"
                        "  String code;\n"
                        "  uint8 type;\n"
                        "  uint32 line;\n"
                        "  uint32 source_id;\n"
                        "  struct instructions_struct *next;\n"
                        "} instru;\n"
                        "//****************************block_structures struct\n"
                        "typedef struct block_structures_struct {\n"
                        "  long_int id;\n"
                        "  long_int func_id;\n"
                        "  long_int stru_id;\n"
                        "  uint8 type; //1:loop,2:if,3:elif,4:else,5:manage,6:func\n"
                        "  String lbl;\n"
                        "  str_list params;\n"
                        "  uint32 params_len;\n"
                        "  uint32 line;\n"
                        "  uint32 source_id;\n"
                        "  struct block_structures_struct *next;\n"
                        "} blst;\n"
                        "//****************************data_types struct\n"
                        "typedef struct data_types_struct {\n"
                        "  long_int id;\n"
                        "  long_int fid;\n"
                        "  String name;\n"
                        "  uint8 type; //1:main,2:struct\n"
                        "  str_list params;\n"
                        "  uint32 params_len;\n"
                        "  struct data_types_struct *next;\n"
                        "} datas;");

//  ret=str_append(ret,"")
  return ret;
}