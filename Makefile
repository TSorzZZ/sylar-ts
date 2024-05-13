# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.22

# Default target executed when no arguments are given to make.
default_target: all
.PHONY : default_target

# Allow only one "make -f Makefile2" at a time, but pass parallelism.
.NOTPARALLEL:

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

# Produce verbose output by default.
VERBOSE = 1

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
CMAKE_SOURCE_DIR = /home/ts/project/sylar-ts

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/ts/project/sylar-ts

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
	/usr/bin/cmake --regenerate-during-build -S$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR)
.PHONY : rebuild_cache

# Special rule for the target rebuild_cache
rebuild_cache/fast: rebuild_cache
.PHONY : rebuild_cache/fast

# The main all target
all: cmake_check_build_system
	$(CMAKE_COMMAND) -E cmake_progress_start /home/ts/project/sylar-ts/CMakeFiles /home/ts/project/sylar-ts//CMakeFiles/progress.marks
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 all
	$(CMAKE_COMMAND) -E cmake_progress_start /home/ts/project/sylar-ts/CMakeFiles 0
.PHONY : all

# The main clean target
clean:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 clean
.PHONY : clean

# The main clean target
clean/fast: clean
.PHONY : clean/fast

# Prepare targets for installation.
preinstall: all
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 preinstall
.PHONY : preinstall

# Prepare targets for installation.
preinstall/fast:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 preinstall
.PHONY : preinstall/fast

# clear depends
depend:
	$(CMAKE_COMMAND) -S$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR) --check-build-system CMakeFiles/Makefile.cmake 1
.PHONY : depend

#=============================================================================
# Target rules for targets named sylar

# Build rule for target.
sylar: cmake_check_build_system
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 sylar
.PHONY : sylar

# fast build rule for target.
sylar/fast:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/sylar.dir/build.make CMakeFiles/sylar.dir/build
.PHONY : sylar/fast

#=============================================================================
# Target rules for targets named test_config

# Build rule for target.
test_config: cmake_check_build_system
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 test_config
.PHONY : test_config

# fast build rule for target.
test_config/fast:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/test_config.dir/build.make CMakeFiles/test_config.dir/build
.PHONY : test_config/fast

#=============================================================================
# Target rules for targets named test_fiber

# Build rule for target.
test_fiber: cmake_check_build_system
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 test_fiber
.PHONY : test_fiber

# fast build rule for target.
test_fiber/fast:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/test_fiber.dir/build.make CMakeFiles/test_fiber.dir/build
.PHONY : test_fiber/fast

#=============================================================================
# Target rules for targets named test_iomanager

# Build rule for target.
test_iomanager: cmake_check_build_system
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 test_iomanager
.PHONY : test_iomanager

# fast build rule for target.
test_iomanager/fast:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/test_iomanager.dir/build.make CMakeFiles/test_iomanager.dir/build
.PHONY : test_iomanager/fast

#=============================================================================
# Target rules for targets named test_log

# Build rule for target.
test_log: cmake_check_build_system
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 test_log
.PHONY : test_log

# fast build rule for target.
test_log/fast:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/test_log.dir/build.make CMakeFiles/test_log.dir/build
.PHONY : test_log/fast

#=============================================================================
# Target rules for targets named test_scheduler

# Build rule for target.
test_scheduler: cmake_check_build_system
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 test_scheduler
.PHONY : test_scheduler

# fast build rule for target.
test_scheduler/fast:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/test_scheduler.dir/build.make CMakeFiles/test_scheduler.dir/build
.PHONY : test_scheduler/fast

#=============================================================================
# Target rules for targets named test_thread

# Build rule for target.
test_thread: cmake_check_build_system
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 test_thread
.PHONY : test_thread

# fast build rule for target.
test_thread/fast:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/test_thread.dir/build.make CMakeFiles/test_thread.dir/build
.PHONY : test_thread/fast

#=============================================================================
# Target rules for targets named test_util

# Build rule for target.
test_util: cmake_check_build_system
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 test_util
.PHONY : test_util

# fast build rule for target.
test_util/fast:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/test_util.dir/build.make CMakeFiles/test_util.dir/build
.PHONY : test_util/fast

sylar/src/config.o: sylar/src/config.cc.o
.PHONY : sylar/src/config.o

# target to build an object file
sylar/src/config.cc.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/sylar.dir/build.make CMakeFiles/sylar.dir/sylar/src/config.cc.o
.PHONY : sylar/src/config.cc.o

sylar/src/config.i: sylar/src/config.cc.i
.PHONY : sylar/src/config.i

# target to preprocess a source file
sylar/src/config.cc.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/sylar.dir/build.make CMakeFiles/sylar.dir/sylar/src/config.cc.i
.PHONY : sylar/src/config.cc.i

sylar/src/config.s: sylar/src/config.cc.s
.PHONY : sylar/src/config.s

# target to generate assembly for a file
sylar/src/config.cc.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/sylar.dir/build.make CMakeFiles/sylar.dir/sylar/src/config.cc.s
.PHONY : sylar/src/config.cc.s

sylar/src/fiber.o: sylar/src/fiber.cc.o
.PHONY : sylar/src/fiber.o

# target to build an object file
sylar/src/fiber.cc.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/sylar.dir/build.make CMakeFiles/sylar.dir/sylar/src/fiber.cc.o
.PHONY : sylar/src/fiber.cc.o

sylar/src/fiber.i: sylar/src/fiber.cc.i
.PHONY : sylar/src/fiber.i

# target to preprocess a source file
sylar/src/fiber.cc.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/sylar.dir/build.make CMakeFiles/sylar.dir/sylar/src/fiber.cc.i
.PHONY : sylar/src/fiber.cc.i

sylar/src/fiber.s: sylar/src/fiber.cc.s
.PHONY : sylar/src/fiber.s

# target to generate assembly for a file
sylar/src/fiber.cc.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/sylar.dir/build.make CMakeFiles/sylar.dir/sylar/src/fiber.cc.s
.PHONY : sylar/src/fiber.cc.s

sylar/src/iomanager.o: sylar/src/iomanager.cc.o
.PHONY : sylar/src/iomanager.o

# target to build an object file
sylar/src/iomanager.cc.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/sylar.dir/build.make CMakeFiles/sylar.dir/sylar/src/iomanager.cc.o
.PHONY : sylar/src/iomanager.cc.o

sylar/src/iomanager.i: sylar/src/iomanager.cc.i
.PHONY : sylar/src/iomanager.i

# target to preprocess a source file
sylar/src/iomanager.cc.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/sylar.dir/build.make CMakeFiles/sylar.dir/sylar/src/iomanager.cc.i
.PHONY : sylar/src/iomanager.cc.i

sylar/src/iomanager.s: sylar/src/iomanager.cc.s
.PHONY : sylar/src/iomanager.s

# target to generate assembly for a file
sylar/src/iomanager.cc.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/sylar.dir/build.make CMakeFiles/sylar.dir/sylar/src/iomanager.cc.s
.PHONY : sylar/src/iomanager.cc.s

sylar/src/log.o: sylar/src/log.cc.o
.PHONY : sylar/src/log.o

# target to build an object file
sylar/src/log.cc.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/sylar.dir/build.make CMakeFiles/sylar.dir/sylar/src/log.cc.o
.PHONY : sylar/src/log.cc.o

sylar/src/log.i: sylar/src/log.cc.i
.PHONY : sylar/src/log.i

# target to preprocess a source file
sylar/src/log.cc.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/sylar.dir/build.make CMakeFiles/sylar.dir/sylar/src/log.cc.i
.PHONY : sylar/src/log.cc.i

sylar/src/log.s: sylar/src/log.cc.s
.PHONY : sylar/src/log.s

# target to generate assembly for a file
sylar/src/log.cc.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/sylar.dir/build.make CMakeFiles/sylar.dir/sylar/src/log.cc.s
.PHONY : sylar/src/log.cc.s

sylar/src/mythread.o: sylar/src/mythread.cc.o
.PHONY : sylar/src/mythread.o

# target to build an object file
sylar/src/mythread.cc.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/sylar.dir/build.make CMakeFiles/sylar.dir/sylar/src/mythread.cc.o
.PHONY : sylar/src/mythread.cc.o

sylar/src/mythread.i: sylar/src/mythread.cc.i
.PHONY : sylar/src/mythread.i

# target to preprocess a source file
sylar/src/mythread.cc.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/sylar.dir/build.make CMakeFiles/sylar.dir/sylar/src/mythread.cc.i
.PHONY : sylar/src/mythread.cc.i

sylar/src/mythread.s: sylar/src/mythread.cc.s
.PHONY : sylar/src/mythread.s

# target to generate assembly for a file
sylar/src/mythread.cc.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/sylar.dir/build.make CMakeFiles/sylar.dir/sylar/src/mythread.cc.s
.PHONY : sylar/src/mythread.cc.s

sylar/src/scheduler.o: sylar/src/scheduler.cc.o
.PHONY : sylar/src/scheduler.o

# target to build an object file
sylar/src/scheduler.cc.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/sylar.dir/build.make CMakeFiles/sylar.dir/sylar/src/scheduler.cc.o
.PHONY : sylar/src/scheduler.cc.o

sylar/src/scheduler.i: sylar/src/scheduler.cc.i
.PHONY : sylar/src/scheduler.i

# target to preprocess a source file
sylar/src/scheduler.cc.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/sylar.dir/build.make CMakeFiles/sylar.dir/sylar/src/scheduler.cc.i
.PHONY : sylar/src/scheduler.cc.i

sylar/src/scheduler.s: sylar/src/scheduler.cc.s
.PHONY : sylar/src/scheduler.s

# target to generate assembly for a file
sylar/src/scheduler.cc.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/sylar.dir/build.make CMakeFiles/sylar.dir/sylar/src/scheduler.cc.s
.PHONY : sylar/src/scheduler.cc.s

sylar/src/timer.o: sylar/src/timer.cc.o
.PHONY : sylar/src/timer.o

# target to build an object file
sylar/src/timer.cc.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/sylar.dir/build.make CMakeFiles/sylar.dir/sylar/src/timer.cc.o
.PHONY : sylar/src/timer.cc.o

sylar/src/timer.i: sylar/src/timer.cc.i
.PHONY : sylar/src/timer.i

# target to preprocess a source file
sylar/src/timer.cc.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/sylar.dir/build.make CMakeFiles/sylar.dir/sylar/src/timer.cc.i
.PHONY : sylar/src/timer.cc.i

sylar/src/timer.s: sylar/src/timer.cc.s
.PHONY : sylar/src/timer.s

# target to generate assembly for a file
sylar/src/timer.cc.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/sylar.dir/build.make CMakeFiles/sylar.dir/sylar/src/timer.cc.s
.PHONY : sylar/src/timer.cc.s

sylar/src/util.o: sylar/src/util.cc.o
.PHONY : sylar/src/util.o

# target to build an object file
sylar/src/util.cc.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/sylar.dir/build.make CMakeFiles/sylar.dir/sylar/src/util.cc.o
.PHONY : sylar/src/util.cc.o

sylar/src/util.i: sylar/src/util.cc.i
.PHONY : sylar/src/util.i

# target to preprocess a source file
sylar/src/util.cc.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/sylar.dir/build.make CMakeFiles/sylar.dir/sylar/src/util.cc.i
.PHONY : sylar/src/util.cc.i

sylar/src/util.s: sylar/src/util.cc.s
.PHONY : sylar/src/util.s

# target to generate assembly for a file
sylar/src/util.cc.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/sylar.dir/build.make CMakeFiles/sylar.dir/sylar/src/util.cc.s
.PHONY : sylar/src/util.cc.s

tests/test_config.o: tests/test_config.cc.o
.PHONY : tests/test_config.o

# target to build an object file
tests/test_config.cc.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/test_config.dir/build.make CMakeFiles/test_config.dir/tests/test_config.cc.o
.PHONY : tests/test_config.cc.o

tests/test_config.i: tests/test_config.cc.i
.PHONY : tests/test_config.i

# target to preprocess a source file
tests/test_config.cc.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/test_config.dir/build.make CMakeFiles/test_config.dir/tests/test_config.cc.i
.PHONY : tests/test_config.cc.i

tests/test_config.s: tests/test_config.cc.s
.PHONY : tests/test_config.s

# target to generate assembly for a file
tests/test_config.cc.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/test_config.dir/build.make CMakeFiles/test_config.dir/tests/test_config.cc.s
.PHONY : tests/test_config.cc.s

tests/test_fiber.o: tests/test_fiber.cc.o
.PHONY : tests/test_fiber.o

# target to build an object file
tests/test_fiber.cc.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/test_fiber.dir/build.make CMakeFiles/test_fiber.dir/tests/test_fiber.cc.o
.PHONY : tests/test_fiber.cc.o

tests/test_fiber.i: tests/test_fiber.cc.i
.PHONY : tests/test_fiber.i

# target to preprocess a source file
tests/test_fiber.cc.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/test_fiber.dir/build.make CMakeFiles/test_fiber.dir/tests/test_fiber.cc.i
.PHONY : tests/test_fiber.cc.i

tests/test_fiber.s: tests/test_fiber.cc.s
.PHONY : tests/test_fiber.s

# target to generate assembly for a file
tests/test_fiber.cc.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/test_fiber.dir/build.make CMakeFiles/test_fiber.dir/tests/test_fiber.cc.s
.PHONY : tests/test_fiber.cc.s

tests/test_iomanager.o: tests/test_iomanager.cc.o
.PHONY : tests/test_iomanager.o

# target to build an object file
tests/test_iomanager.cc.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/test_iomanager.dir/build.make CMakeFiles/test_iomanager.dir/tests/test_iomanager.cc.o
.PHONY : tests/test_iomanager.cc.o

tests/test_iomanager.i: tests/test_iomanager.cc.i
.PHONY : tests/test_iomanager.i

# target to preprocess a source file
tests/test_iomanager.cc.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/test_iomanager.dir/build.make CMakeFiles/test_iomanager.dir/tests/test_iomanager.cc.i
.PHONY : tests/test_iomanager.cc.i

tests/test_iomanager.s: tests/test_iomanager.cc.s
.PHONY : tests/test_iomanager.s

# target to generate assembly for a file
tests/test_iomanager.cc.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/test_iomanager.dir/build.make CMakeFiles/test_iomanager.dir/tests/test_iomanager.cc.s
.PHONY : tests/test_iomanager.cc.s

tests/test_log.o: tests/test_log.cc.o
.PHONY : tests/test_log.o

# target to build an object file
tests/test_log.cc.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/test_log.dir/build.make CMakeFiles/test_log.dir/tests/test_log.cc.o
.PHONY : tests/test_log.cc.o

tests/test_log.i: tests/test_log.cc.i
.PHONY : tests/test_log.i

# target to preprocess a source file
tests/test_log.cc.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/test_log.dir/build.make CMakeFiles/test_log.dir/tests/test_log.cc.i
.PHONY : tests/test_log.cc.i

tests/test_log.s: tests/test_log.cc.s
.PHONY : tests/test_log.s

# target to generate assembly for a file
tests/test_log.cc.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/test_log.dir/build.make CMakeFiles/test_log.dir/tests/test_log.cc.s
.PHONY : tests/test_log.cc.s

tests/test_scheduler.o: tests/test_scheduler.cc.o
.PHONY : tests/test_scheduler.o

# target to build an object file
tests/test_scheduler.cc.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/test_scheduler.dir/build.make CMakeFiles/test_scheduler.dir/tests/test_scheduler.cc.o
.PHONY : tests/test_scheduler.cc.o

tests/test_scheduler.i: tests/test_scheduler.cc.i
.PHONY : tests/test_scheduler.i

# target to preprocess a source file
tests/test_scheduler.cc.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/test_scheduler.dir/build.make CMakeFiles/test_scheduler.dir/tests/test_scheduler.cc.i
.PHONY : tests/test_scheduler.cc.i

tests/test_scheduler.s: tests/test_scheduler.cc.s
.PHONY : tests/test_scheduler.s

# target to generate assembly for a file
tests/test_scheduler.cc.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/test_scheduler.dir/build.make CMakeFiles/test_scheduler.dir/tests/test_scheduler.cc.s
.PHONY : tests/test_scheduler.cc.s

tests/test_thread.o: tests/test_thread.cc.o
.PHONY : tests/test_thread.o

# target to build an object file
tests/test_thread.cc.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/test_thread.dir/build.make CMakeFiles/test_thread.dir/tests/test_thread.cc.o
.PHONY : tests/test_thread.cc.o

tests/test_thread.i: tests/test_thread.cc.i
.PHONY : tests/test_thread.i

# target to preprocess a source file
tests/test_thread.cc.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/test_thread.dir/build.make CMakeFiles/test_thread.dir/tests/test_thread.cc.i
.PHONY : tests/test_thread.cc.i

tests/test_thread.s: tests/test_thread.cc.s
.PHONY : tests/test_thread.s

# target to generate assembly for a file
tests/test_thread.cc.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/test_thread.dir/build.make CMakeFiles/test_thread.dir/tests/test_thread.cc.s
.PHONY : tests/test_thread.cc.s

tests/test_util.o: tests/test_util.cc.o
.PHONY : tests/test_util.o

# target to build an object file
tests/test_util.cc.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/test_util.dir/build.make CMakeFiles/test_util.dir/tests/test_util.cc.o
.PHONY : tests/test_util.cc.o

tests/test_util.i: tests/test_util.cc.i
.PHONY : tests/test_util.i

# target to preprocess a source file
tests/test_util.cc.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/test_util.dir/build.make CMakeFiles/test_util.dir/tests/test_util.cc.i
.PHONY : tests/test_util.cc.i

tests/test_util.s: tests/test_util.cc.s
.PHONY : tests/test_util.s

# target to generate assembly for a file
tests/test_util.cc.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/test_util.dir/build.make CMakeFiles/test_util.dir/tests/test_util.cc.s
.PHONY : tests/test_util.cc.s

# Help Target
help:
	@echo "The following are some of the valid targets for this Makefile:"
	@echo "... all (the default if no target is provided)"
	@echo "... clean"
	@echo "... depend"
	@echo "... edit_cache"
	@echo "... rebuild_cache"
	@echo "... sylar"
	@echo "... test_config"
	@echo "... test_fiber"
	@echo "... test_iomanager"
	@echo "... test_log"
	@echo "... test_scheduler"
	@echo "... test_thread"
	@echo "... test_util"
	@echo "... sylar/src/config.o"
	@echo "... sylar/src/config.i"
	@echo "... sylar/src/config.s"
	@echo "... sylar/src/fiber.o"
	@echo "... sylar/src/fiber.i"
	@echo "... sylar/src/fiber.s"
	@echo "... sylar/src/iomanager.o"
	@echo "... sylar/src/iomanager.i"
	@echo "... sylar/src/iomanager.s"
	@echo "... sylar/src/log.o"
	@echo "... sylar/src/log.i"
	@echo "... sylar/src/log.s"
	@echo "... sylar/src/mythread.o"
	@echo "... sylar/src/mythread.i"
	@echo "... sylar/src/mythread.s"
	@echo "... sylar/src/scheduler.o"
	@echo "... sylar/src/scheduler.i"
	@echo "... sylar/src/scheduler.s"
	@echo "... sylar/src/timer.o"
	@echo "... sylar/src/timer.i"
	@echo "... sylar/src/timer.s"
	@echo "... sylar/src/util.o"
	@echo "... sylar/src/util.i"
	@echo "... sylar/src/util.s"
	@echo "... tests/test_config.o"
	@echo "... tests/test_config.i"
	@echo "... tests/test_config.s"
	@echo "... tests/test_fiber.o"
	@echo "... tests/test_fiber.i"
	@echo "... tests/test_fiber.s"
	@echo "... tests/test_iomanager.o"
	@echo "... tests/test_iomanager.i"
	@echo "... tests/test_iomanager.s"
	@echo "... tests/test_log.o"
	@echo "... tests/test_log.i"
	@echo "... tests/test_log.s"
	@echo "... tests/test_scheduler.o"
	@echo "... tests/test_scheduler.i"
	@echo "... tests/test_scheduler.s"
	@echo "... tests/test_thread.o"
	@echo "... tests/test_thread.i"
	@echo "... tests/test_thread.s"
	@echo "... tests/test_util.o"
	@echo "... tests/test_util.i"
	@echo "... tests/test_util.s"
.PHONY : help



#=============================================================================
# Special targets to cleanup operation of make.

# Special rule to run CMake to check the build system integrity.
# No rule that depends on this can have commands that come from listfiles
# because they might be regenerated.
cmake_check_build_system:
	$(CMAKE_COMMAND) -S$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR) --check-build-system CMakeFiles/Makefile.cmake 0
.PHONY : cmake_check_build_system

