#pragma once

#define _STR_CONCAT(expr) #expr
#define STR_CONCAT(expr)  _STR_CONCAT(expr)

#define P_RESET "\033[0m"

#define P_BLACK   "\033[0;30m"
#define P_RED     "\033[0;31m"
#define P_GREEN   "\033[0;32m"
#define P_YELLOW  "\033[0;33m"
#define P_BLUE    "\033[0;34m"
#define P_MAGENTA "\033[0;35m"
#define P_CYAN    "\033[0;36m"
#define P_WHITE   "\033[0;37m"
#define P_DEFAULT "\033[0;39m"

#define P_LGRAY    "\033[0;37m"
#define P_DGRAY    "\033[0;90m"
#define P_LRED     "\033[0;91m"
#define P_LGREEN   "\033[0;92m"
#define P_LYELLOW  "\033[0;93m"
#define P_LBLUE    "\033[0;94m"
#define P_LMAGENTA "\033[0;95m"
#define P_LCYAN    "\033[0;96m"
#define P_LWHITE   "\033[0;97m"

#define BOLD   "\033[1m"
#define NORMAL "\033[21m"

// 256 color sets
#define P_256_COLOR(id) "\u001b[38;5;" STR_CONCAT(id) "m"
#define P_256_VAR_COLOR "\u001b[38;5;%um"
#define P_256_DEFAULT   "\u001b[39m"

#define P_256_BG_COLOR(id) "\u001b[48;5;" STR_CONCAT(id) "m"
#define P_256_BG_DEFAULT   "\u001b[49m"
