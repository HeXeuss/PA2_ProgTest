# CMAKE generated file: DO NOT EDIT!
# Generated by "MinGW Makefiles" Generator, CMake Version 3.31

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

SHELL = cmd.exe

# The CMake executable.
CMAKE_COMMAND = "C:\Program Files\JetBrains\CLion 2025.1\bin\cmake\win\x64\bin\cmake.exe"

# The command to remove a file.
RM = "C:\Program Files\JetBrains\CLion 2025.1\bin\cmake\win\x64\bin\cmake.exe" -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = C:\Users\patoh\Documents\GitHub\PA2_ProgTest\ProgTest_06

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = C:\Users\patoh\Documents\GitHub\PA2_ProgTest\ProgTest_06\cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/ProgTest_06.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/ProgTest_06.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/ProgTest_06.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/ProgTest_06.dir/flags.make

CMakeFiles/ProgTest_06.dir/codegen:
.PHONY : CMakeFiles/ProgTest_06.dir/codegen

CMakeFiles/ProgTest_06.dir/main.cpp.obj: CMakeFiles/ProgTest_06.dir/flags.make
CMakeFiles/ProgTest_06.dir/main.cpp.obj: C:/Users/patoh/Documents/GitHub/PA2_ProgTest/ProgTest_06/main.cpp
CMakeFiles/ProgTest_06.dir/main.cpp.obj: CMakeFiles/ProgTest_06.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=C:\Users\patoh\Documents\GitHub\PA2_ProgTest\ProgTest_06\cmake-build-debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/ProgTest_06.dir/main.cpp.obj"
	C:\PROGRA~1\JETBRA~1\CLION2~1.1\bin\mingw\bin\G__~1.EXE $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/ProgTest_06.dir/main.cpp.obj -MF CMakeFiles\ProgTest_06.dir\main.cpp.obj.d -o CMakeFiles\ProgTest_06.dir\main.cpp.obj -c C:\Users\patoh\Documents\GitHub\PA2_ProgTest\ProgTest_06\main.cpp

CMakeFiles/ProgTest_06.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/ProgTest_06.dir/main.cpp.i"
	C:\PROGRA~1\JETBRA~1\CLION2~1.1\bin\mingw\bin\G__~1.EXE $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E C:\Users\patoh\Documents\GitHub\PA2_ProgTest\ProgTest_06\main.cpp > CMakeFiles\ProgTest_06.dir\main.cpp.i

CMakeFiles/ProgTest_06.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/ProgTest_06.dir/main.cpp.s"
	C:\PROGRA~1\JETBRA~1\CLION2~1.1\bin\mingw\bin\G__~1.EXE $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S C:\Users\patoh\Documents\GitHub\PA2_ProgTest\ProgTest_06\main.cpp -o CMakeFiles\ProgTest_06.dir\main.cpp.s

# Object files for target ProgTest_06
ProgTest_06_OBJECTS = \
"CMakeFiles/ProgTest_06.dir/main.cpp.obj"

# External object files for target ProgTest_06
ProgTest_06_EXTERNAL_OBJECTS =

ProgTest_06.exe: CMakeFiles/ProgTest_06.dir/main.cpp.obj
ProgTest_06.exe: CMakeFiles/ProgTest_06.dir/build.make
ProgTest_06.exe: CMakeFiles/ProgTest_06.dir/linkLibs.rsp
ProgTest_06.exe: CMakeFiles/ProgTest_06.dir/objects1.rsp
ProgTest_06.exe: CMakeFiles/ProgTest_06.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=C:\Users\patoh\Documents\GitHub\PA2_ProgTest\ProgTest_06\cmake-build-debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable ProgTest_06.exe"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles\ProgTest_06.dir\link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/ProgTest_06.dir/build: ProgTest_06.exe
.PHONY : CMakeFiles/ProgTest_06.dir/build

CMakeFiles/ProgTest_06.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles\ProgTest_06.dir\cmake_clean.cmake
.PHONY : CMakeFiles/ProgTest_06.dir/clean

CMakeFiles/ProgTest_06.dir/depend:
	$(CMAKE_COMMAND) -E cmake_depends "MinGW Makefiles" C:\Users\patoh\Documents\GitHub\PA2_ProgTest\ProgTest_06 C:\Users\patoh\Documents\GitHub\PA2_ProgTest\ProgTest_06 C:\Users\patoh\Documents\GitHub\PA2_ProgTest\ProgTest_06\cmake-build-debug C:\Users\patoh\Documents\GitHub\PA2_ProgTest\ProgTest_06\cmake-build-debug C:\Users\patoh\Documents\GitHub\PA2_ProgTest\ProgTest_06\cmake-build-debug\CMakeFiles\ProgTest_06.dir\DependInfo.cmake "--color=$(COLOR)"
.PHONY : CMakeFiles/ProgTest_06.dir/depend

