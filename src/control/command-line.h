#ifndef COMMAND_LINE_H
#define COMMAND_LINE_H

#include "../lib/linkedList.h"

LinkedList_t CommandLine_doarg (int argc, char **argv);
static void pre_define_symbol();
static void pre_undefine_symbol();
static void add_include_path();
#endif
