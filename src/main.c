#include <MPL/system.h>

//************************************************prototypes
Boolean start_interpreter();

void init_interpreter();

uint8 interpreter_mode = 0;

//************************************************
int main(int argc, char **argv) {
  //-------------------------time of start program
  AppStartedClock = clock();
  AppStartedTime = str_from_long_int(__syscall_unix_time());
  is_real_mpl = true;
  //-------------------------init exceptions list
  init_exceptions_list_data();
  //--------------------------get Argvs,analyze it
  if (argc < 2) {
    mpl_help_usage();
  } else if (argv[1][0] == '-' && argv[1][1] == 'v') {
    mpl_help_version();
    interpreter_mode = 1;
  } else if (argv[1][0] == '-' && argv[1][1] == 'i') {
    mpl_help_info();
    interpreter_mode = 1;
  } else if (argv[1][0] == '-' && argv[1][1] == 'h') {
    str_list help_argvs = 0;
    uint32 help_argvs_len = 0;
    for (uint32 i = 2; i < argc; i++)
      str_list_append(&help_argvs, str_trim_space(argv[i]), help_argvs_len++);
    mpl_help_starter(help_argvs, help_argvs_len);
    interpreter_mode = 2;
  } else if (argv[1][0] == '-') {
    print_error(0, "unknown_opt", "stdin", argv[1], 0, "main");
    mpl_help_usage();
  } else {
    //store arguments of program
    if (argc > 2)
      for (int ii = 2; ii < argc; ++ii)
        str_list_append(&program_argvs, argv[ii], argvs_len++);
    //printf("argv:%s",argv[4]);
    str_init(&stdin_source_path, argv[1]);
    stdin_source_path = __syscall_abspath(stdin_source_path);
//     printf("####:%s,%s\n",argv[1],stdin_source_path);
    //=>check if source path is exist
    if (stdin_source_path == 0) {
      print_error(0, "not_exist_file", 0, argv[1], 0, "main");
    }
    Boolean ret = start_interpreter();
    interpreter_mode = 4;
    if (!ret)
      print_error(0, "bad_exit", "stdin", 0, 0, "main");

  }

//  String ss = 0;
//  str_init(&ss, "Hello Amin.");
//  printf("encode:%s\n>>>%s\n>>>%s$\n", ss, MPLV1_encode(ss),MPLV1_decode(MPLV1_encode(ss)));
//  console_color_reverse();
//  printf("Hello World:%s\n",argv[1]);
//  console_color_reset();
//  printf("Hello World:%s\n",argv[1]);
  //  print_struct(PRINT_CONDITION_LEVEL_ST);
  //	print_magic_macros(CONFIG_MAGIC_MACRO_TYPE);
//  print_struct(PRINT_UTF8_ST);
  //-------------------------time of end program
  exit_runtime(EXIT_NORMAL);
  return 0;
}
//************************************************
Boolean start_interpreter() {
  //-----------------------init interpreter
  str_init(&interpreter_level, "init");
  init_interpreter();
  //-----------------------parsing source codes
  str_init(&interpreter_level, "parse");
  Boolean ret0 = import_all_files();
  //printf("VVVVVV:%i\n",ret0);
  if (!ret0) return false;
  //-----------------------meaning&running instructions
  str_init(&interpreter_level, "runtime");
  Boolean ret3 = start_runtime();
  if (!ret3) return false;
  //show debug  lists info
  if (is_programmer_debug > 0) {
    show_memory(0);
//    print_magic_macros(2);
//    print_struct(PRINT_INSTRU_ST);
  }
  //-----------------------free memory
  str_init(&interpreter_level, "free");
}

//************************************************
void init_interpreter() {
  //********************
  init_data_defined();
  //********************
  init_memory();
  //********************
  init_built_in_funcs();
  //********************
  init_magic_define();
  //********************
  init_magic_config();
  //********************
  //get_basic_system_info()
}

//************************************************