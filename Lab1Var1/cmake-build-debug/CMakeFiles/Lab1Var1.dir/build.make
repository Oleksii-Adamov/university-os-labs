# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.15

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
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
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /mnt/d/University/5-sem/university-os-labs/Lab1Var1

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /mnt/d/University/5-sem/university-os-labs/Lab1Var1/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/Lab1Var1.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/Lab1Var1.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/Lab1Var1.dir/flags.make

CMakeFiles/Lab1Var1.dir/main.c.o: CMakeFiles/Lab1Var1.dir/flags.make
CMakeFiles/Lab1Var1.dir/main.c.o: ../main.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/mnt/d/University/5-sem/university-os-labs/Lab1Var1/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/Lab1Var1.dir/main.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/Lab1Var1.dir/main.c.o   -c /mnt/d/University/5-sem/university-os-labs/Lab1Var1/main.c

CMakeFiles/Lab1Var1.dir/main.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/Lab1Var1.dir/main.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /mnt/d/University/5-sem/university-os-labs/Lab1Var1/main.c > CMakeFiles/Lab1Var1.dir/main.c.i

CMakeFiles/Lab1Var1.dir/main.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/Lab1Var1.dir/main.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /mnt/d/University/5-sem/university-os-labs/Lab1Var1/main.c -o CMakeFiles/Lab1Var1.dir/main.c.s

# Object files for target Lab1Var1
Lab1Var1_OBJECTS = \
"CMakeFiles/Lab1Var1.dir/main.c.o"

# External object files for target Lab1Var1
Lab1Var1_EXTERNAL_OBJECTS =

Lab1Var1: CMakeFiles/Lab1Var1.dir/main.c.o
Lab1Var1: CMakeFiles/Lab1Var1.dir/build.make
Lab1Var1: libsupp.so
Lab1Var1: CMakeFiles/Lab1Var1.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/mnt/d/University/5-sem/university-os-labs/Lab1Var1/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable Lab1Var1"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/Lab1Var1.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/Lab1Var1.dir/build: Lab1Var1

.PHONY : CMakeFiles/Lab1Var1.dir/build

CMakeFiles/Lab1Var1.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/Lab1Var1.dir/cmake_clean.cmake
.PHONY : CMakeFiles/Lab1Var1.dir/clean

CMakeFiles/Lab1Var1.dir/depend:
	cd /mnt/d/University/5-sem/university-os-labs/Lab1Var1/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /mnt/d/University/5-sem/university-os-labs/Lab1Var1 /mnt/d/University/5-sem/university-os-labs/Lab1Var1 /mnt/d/University/5-sem/university-os-labs/Lab1Var1/cmake-build-debug /mnt/d/University/5-sem/university-os-labs/Lab1Var1/cmake-build-debug /mnt/d/University/5-sem/university-os-labs/Lab1Var1/cmake-build-debug/CMakeFiles/Lab1Var1.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/Lab1Var1.dir/depend
