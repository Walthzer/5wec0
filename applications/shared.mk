COLOR_GREEN="\033[1;32m"
COLOR_RED="\033[1;31m"
COLOR_BLUE="\033[1;34m"
COLOR_YELLOW="\033[1;33m"
COLOR_END="\033[0m"

DEP_PRINT=@echo ${COLOR_BLUE}\(DEP\)${COLOR_END} $@
OBJ_PRINT=@echo ${COLOR_YELLOW}\(OBJ\)${COLOR_END} $@
BUILD_PRINT=@echo ${COLOR_GREEN}\(EXE\)${COLOR_END} $@
CLEAN_PRINT=@echo ${COLOR_RED}Cleaning build files${COLOR_END}

# Can be used to get full output.
VERBOSE?=@

PROJECT_ROOT:=../../

# Allow optimization level to be overwritten?
OPT_LEVEL?=0

# include paths
CFLAGS:= -I. -I${PROJECT_ROOT}/platform/ -I${PROJECT_ROOT}/library/ -I${PROJECT_ROOT}/external/

# for debugging 
CFLAGS+= -ggdb -g3

# C version
CFLAGS+= -std=c99
CFLAGS+=-D_POSIX_C_SOURCE=200809L

# Warnings
CFLAGS+= -Wall -Wextra
CFLAGS+= -O$(OPT_LEVEL)

# Linker flags
LDFLAGS:= -lm -ggdb -g3

