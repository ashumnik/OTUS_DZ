# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.18

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Disable VCS-based implicit rules.
% : %,v


# Disable VCS-based implicit rules.
% : RCS/%


# Disable VCS-based implicit rules.
% : RCS/%,v


# Disable VCS-based implicit rules.
% : SCCS/s.%


# Disable VCS-based implicit rules.
% : s.%


.SUFFIXES: .hpux_make_needs_suffix_list


# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/aleksey/OTUS/DZ4/json-c

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/aleksey/OTUS/DZ4/json-c-build

# Include any dependencies generated for this target.
include tests/CMakeFiles/test_int_add.dir/depend.make

# Include the progress variables for this target.
include tests/CMakeFiles/test_int_add.dir/progress.make

# Include the compile flags for this target's objects.
include tests/CMakeFiles/test_int_add.dir/flags.make

tests/CMakeFiles/test_int_add.dir/test_int_add.c.o: tests/CMakeFiles/test_int_add.dir/flags.make
tests/CMakeFiles/test_int_add.dir/test_int_add.c.o: /home/aleksey/OTUS/DZ4/json-c/tests/test_int_add.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/aleksey/OTUS/DZ4/json-c-build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object tests/CMakeFiles/test_int_add.dir/test_int_add.c.o"
	cd /home/aleksey/OTUS/DZ4/json-c-build/tests && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/test_int_add.dir/test_int_add.c.o -c /home/aleksey/OTUS/DZ4/json-c/tests/test_int_add.c

tests/CMakeFiles/test_int_add.dir/test_int_add.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/test_int_add.dir/test_int_add.c.i"
	cd /home/aleksey/OTUS/DZ4/json-c-build/tests && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/aleksey/OTUS/DZ4/json-c/tests/test_int_add.c > CMakeFiles/test_int_add.dir/test_int_add.c.i

tests/CMakeFiles/test_int_add.dir/test_int_add.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/test_int_add.dir/test_int_add.c.s"
	cd /home/aleksey/OTUS/DZ4/json-c-build/tests && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/aleksey/OTUS/DZ4/json-c/tests/test_int_add.c -o CMakeFiles/test_int_add.dir/test_int_add.c.s

# Object files for target test_int_add
test_int_add_OBJECTS = \
"CMakeFiles/test_int_add.dir/test_int_add.c.o"

# External object files for target test_int_add
test_int_add_EXTERNAL_OBJECTS =

tests/test_int_add: tests/CMakeFiles/test_int_add.dir/test_int_add.c.o
tests/test_int_add: tests/CMakeFiles/test_int_add.dir/build.make
tests/test_int_add: libjson-c.so.5.2.0
tests/test_int_add: tests/CMakeFiles/test_int_add.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/aleksey/OTUS/DZ4/json-c-build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable test_int_add"
	cd /home/aleksey/OTUS/DZ4/json-c-build/tests && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/test_int_add.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
tests/CMakeFiles/test_int_add.dir/build: tests/test_int_add

.PHONY : tests/CMakeFiles/test_int_add.dir/build

tests/CMakeFiles/test_int_add.dir/clean:
	cd /home/aleksey/OTUS/DZ4/json-c-build/tests && $(CMAKE_COMMAND) -P CMakeFiles/test_int_add.dir/cmake_clean.cmake
.PHONY : tests/CMakeFiles/test_int_add.dir/clean

tests/CMakeFiles/test_int_add.dir/depend:
	cd /home/aleksey/OTUS/DZ4/json-c-build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/aleksey/OTUS/DZ4/json-c /home/aleksey/OTUS/DZ4/json-c/tests /home/aleksey/OTUS/DZ4/json-c-build /home/aleksey/OTUS/DZ4/json-c-build/tests /home/aleksey/OTUS/DZ4/json-c-build/tests/CMakeFiles/test_int_add.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : tests/CMakeFiles/test_int_add.dir/depend

