# CHANGE THESE TWO TO YOUR OWN DIRECTORY

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/bart/shell

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/bart/shell

# Default target executed when no arguments are given to make.
default_target: shell

.PHONY : default_target

# Allow only one "make -f Makefile2" at a time, but pass parallelism.
.NOTPARALLEL:


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

#=============================================================================
# Targets provided globally by CMake.

# Special rule for the target edit_cache
edit_cache:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "No interactive CMake dialog available..."
	/usr/bin/cmake -E echo No\ interactive\ CMake\ dialog\ available.
.PHONY : edit_cache

# Special rule for the target edit_cache
edit_cache/fast: edit_cache

.PHONY : edit_cache/fast

# Special rule for the target rebuild_cache
rebuild_cache:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "Running CMake to regenerate build system..."
	/usr/bin/cmake -H$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR)
.PHONY : rebuild_cache

# Special rule for the target rebuild_cache
rebuild_cache/fast: rebuild_cache

.PHONY : rebuild_cache/fast

# The main all target
all: cmake_check_build_system
	$(CMAKE_COMMAND) -E cmake_progress_start $(CMAKE_SOURCE_DIR)/CMakeFiles $(CMAKE_SOURCE_DIR)/CMakeFiles/progress.marks
	$(MAKE) -f CMakeFiles/Makefile2 all
	$(CMAKE_COMMAND) -E cmake_progress_start $(CMAKE_SOURCE_DIR)/CMakeFiles 0
.PHONY : all

# The main clean target
clean:
	$(MAKE) -f CMakeFiles/Makefile2 clean
.PHONY : clean

# The main clean target
clean/fast: clean

.PHONY : clean/fast

# Prepare targets for installation.
preinstall: all
	$(MAKE) -f CMakeFiles/Makefile2 preinstall
.PHONY : preinstall

# Prepare targets for installation.
preinstall/fast:
	$(MAKE) -f CMakeFiles/Makefile2 preinstall
.PHONY : preinstall/fast

# clear depends
depend:
	$(CMAKE_COMMAND) -H$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR) --check-build-system CMakeFiles/Makefile.cmake 1
.PHONY : depend

#=============================================================================
# Target rules for targets named shelllib

# Build rule for target.
shelllib: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 shelllib
.PHONY : shelllib

# fast build rule for target.
shelllib/fast:
	$(MAKE) -f CMakeFiles/shelllib.dir/build.make CMakeFiles/shelllib.dir/build
.PHONY : shelllib/fast

#=============================================================================
# Target rules for targets named shell

# Build rule for target.
shell: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 shell
.PHONY : shell

# fast build rule for target.
shell/fast:
	$(MAKE) -f CMakeFiles/shell.dir/build.make CMakeFiles/shell.dir/build
.PHONY : shell/fast

#=============================================================================
# Target rules for targets named shelltest

# Build rule for target.
shelltest: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 shelltest
.PHONY : shelltest

# fast build rule for target.
shelltest/fast:
	$(MAKE) -f CMakeFiles/shelltest.dir/build.make CMakeFiles/shelltest.dir/build
.PHONY : shelltest/fast

#=============================================================================
# Target rules for targets named googletest

# Build rule for target.
googletest: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 googletest
.PHONY : googletest

# fast build rule for target.
googletest/fast:
	$(MAKE) -f ext/gtest/CMakeFiles/googletest.dir/build.make ext/gtest/CMakeFiles/googletest.dir/build
.PHONY : googletest/fast

main.o: main.cpp.o

.PHONY : main.o

# target to build an object file
main.cpp.o:
	$(MAKE) -f CMakeFiles/shell.dir/build.make CMakeFiles/shell.dir/main.cpp.o
.PHONY : main.cpp.o

main.i: main.cpp.i

.PHONY : main.i

# target to preprocess a source file
main.cpp.i:
	$(MAKE) -f CMakeFiles/shell.dir/build.make CMakeFiles/shell.dir/main.cpp.i
.PHONY : main.cpp.i

main.s: main.cpp.s

.PHONY : main.s

# target to generate assembly for a file
main.cpp.s:
	$(MAKE) -f CMakeFiles/shell.dir/build.make CMakeFiles/shell.dir/main.cpp.s
.PHONY : main.cpp.s

shell.o: shell.cpp.o

.PHONY : shell.o

# target to build an object file
shell.cpp.o:
	$(MAKE) -f CMakeFiles/shelllib.dir/build.make CMakeFiles/shelllib.dir/shell.cpp.o
.PHONY : shell.cpp.o

shell.i: shell.cpp.i

.PHONY : shell.i

# target to preprocess a source file
shell.cpp.i:
	$(MAKE) -f CMakeFiles/shelllib.dir/build.make CMakeFiles/shelllib.dir/shell.cpp.i
.PHONY : shell.cpp.i

shell.s: shell.cpp.s

.PHONY : shell.s

# target to generate assembly for a file
shell.cpp.s:
	$(MAKE) -f CMakeFiles/shelllib.dir/build.make CMakeFiles/shelllib.dir/shell.cpp.s
.PHONY : shell.cpp.s

shell.test.o: shell.test.cpp.o

.PHONY : shell.test.o

# target to build an object file
shell.test.cpp.o:
	$(MAKE) -f CMakeFiles/shelltest.dir/build.make CMakeFiles/shelltest.dir/shell.test.cpp.o
.PHONY : shell.test.cpp.o

shell.test.i: shell.test.cpp.i

.PHONY : shell.test.i

# target to preprocess a source file
shell.test.cpp.i:
	$(MAKE) -f CMakeFiles/shelltest.dir/build.make CMakeFiles/shelltest.dir/shell.test.cpp.i
.PHONY : shell.test.cpp.i

shell.test.s: shell.test.cpp.s

.PHONY : shell.test.s

# target to generate assembly for a file
shell.test.cpp.s:
	$(MAKE) -f CMakeFiles/shelltest.dir/build.make CMakeFiles/shelltest.dir/shell.test.cpp.s
.PHONY : shell.test.cpp.s

test.o: test.cpp.o

.PHONY : test.o

# target to build an object file
test.cpp.o:
	$(MAKE) -f CMakeFiles/shelltest.dir/build.make CMakeFiles/shelltest.dir/test.cpp.o
.PHONY : test.cpp.o

test.i: test.cpp.i

.PHONY : test.i

# target to preprocess a source file
test.cpp.i:
	$(MAKE) -f CMakeFiles/shelltest.dir/build.make CMakeFiles/shelltest.dir/test.cpp.i
.PHONY : test.cpp.i

test.s: test.cpp.s

.PHONY : test.s

# target to generate assembly for a file
test.cpp.s:
	$(MAKE) -f CMakeFiles/shelltest.dir/build.make CMakeFiles/shelltest.dir/test.cpp.s
.PHONY : test.cpp.s

# Help Target
help:
	@echo "The following are some of the valid targets for this Makefile:"
	@echo "... all (the default if no target is provided)"
	@echo "... clean"
	@echo "... depend"
	@echo "... edit_cache"
	@echo "... shelllib"
	@echo "... rebuild_cache"
	@echo "... shell"
	@echo "... shelltest"
	@echo "... googletest"
	@echo "... main.o"
	@echo "... main.i"
	@echo "... main.s"
	@echo "... shell.o"
	@echo "... shell.i"
	@echo "... shell.s"
	@echo "... shell.test.o"
	@echo "... shell.test.i"
	@echo "... shell.test.s"
	@echo "... test.o"
	@echo "... test.i"
	@echo "... test.s"
.PHONY : help



#=============================================================================
# Special targets to cleanup operation of make.

# Special rule to run CMake to check the build system integrity.
# No rule that depends on this can have commands that come from listfiles
# because they might be regenerated.
cmake_check_build_system:
	$(CMAKE_COMMAND) -H$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR) --check-build-system CMakeFiles/Makefile.cmake 0
.PHONY : cmake_check_build_system

