# CMAKE generated file: DO NOT EDIT!
# Generated by "MinGW Makefiles" Generator, CMake Version 3.25

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
CMAKE_COMMAND = "D:\JetBrains\CLion 2022.2.3\bin\cmake\win\x64\bin\cmake.exe"

# The command to remove a file.
RM = "D:\JetBrains\CLion 2022.2.3\bin\cmake\win\x64\bin\cmake.exe" -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = "D:\JetBrains\CLion 2022.2.3\CLionProjects\fund_algs_2\allocator_1"

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = "D:\JetBrains\CLion 2022.2.3\CLionProjects\fund_algs_2\allocator_1\cmake-build-debug"

# Include any dependencies generated for this target.
include CMakeFiles/allocator_1.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/allocator_1.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/allocator_1.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/allocator_1.dir/flags.make

CMakeFiles/allocator_1.dir/main.cpp.obj: CMakeFiles/allocator_1.dir/flags.make
CMakeFiles/allocator_1.dir/main.cpp.obj: CMakeFiles/allocator_1.dir/includes_CXX.rsp
CMakeFiles/allocator_1.dir/main.cpp.obj: D:/JetBrains/CLion\ 2022.2.3/CLionProjects/fund_algs_2/allocator_1/main.cpp
CMakeFiles/allocator_1.dir/main.cpp.obj: CMakeFiles/allocator_1.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir="D:\JetBrains\CLion 2022.2.3\CLionProjects\fund_algs_2\allocator_1\cmake-build-debug\CMakeFiles" --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/allocator_1.dir/main.cpp.obj"
	"D:\JetBrains\CLion 2022.2.3\bin\mingw\bin\g++.exe" $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/allocator_1.dir/main.cpp.obj -MF CMakeFiles\allocator_1.dir\main.cpp.obj.d -o CMakeFiles\allocator_1.dir\main.cpp.obj -c "D:\JetBrains\CLion 2022.2.3\CLionProjects\fund_algs_2\allocator_1\main.cpp"

CMakeFiles/allocator_1.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/allocator_1.dir/main.cpp.i"
	"D:\JetBrains\CLion 2022.2.3\bin\mingw\bin\g++.exe" $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E "D:\JetBrains\CLion 2022.2.3\CLionProjects\fund_algs_2\allocator_1\main.cpp" > CMakeFiles\allocator_1.dir\main.cpp.i

CMakeFiles/allocator_1.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/allocator_1.dir/main.cpp.s"
	"D:\JetBrains\CLion 2022.2.3\bin\mingw\bin\g++.exe" $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S "D:\JetBrains\CLion 2022.2.3\CLionProjects\fund_algs_2\allocator_1\main.cpp" -o CMakeFiles\allocator_1.dir\main.cpp.s

# Object files for target allocator_1
allocator_1_OBJECTS = \
"CMakeFiles/allocator_1.dir/main.cpp.obj"

# External object files for target allocator_1
allocator_1_EXTERNAL_OBJECTS =

allocator_1.exe: CMakeFiles/allocator_1.dir/main.cpp.obj
allocator_1.exe: CMakeFiles/allocator_1.dir/build.make
allocator_1.exe: CMakeFiles/allocator_1.dir/linkLibs.rsp
allocator_1.exe: CMakeFiles/allocator_1.dir/objects1
allocator_1.exe: CMakeFiles/allocator_1.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir="D:\JetBrains\CLion 2022.2.3\CLionProjects\fund_algs_2\allocator_1\cmake-build-debug\CMakeFiles" --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable allocator_1.exe"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles\allocator_1.dir\link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/allocator_1.dir/build: allocator_1.exe
.PHONY : CMakeFiles/allocator_1.dir/build

CMakeFiles/allocator_1.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles\allocator_1.dir\cmake_clean.cmake
.PHONY : CMakeFiles/allocator_1.dir/clean

CMakeFiles/allocator_1.dir/depend:
	$(CMAKE_COMMAND) -E cmake_depends "MinGW Makefiles" "D:\JetBrains\CLion 2022.2.3\CLionProjects\fund_algs_2\allocator_1" "D:\JetBrains\CLion 2022.2.3\CLionProjects\fund_algs_2\allocator_1" "D:\JetBrains\CLion 2022.2.3\CLionProjects\fund_algs_2\allocator_1\cmake-build-debug" "D:\JetBrains\CLion 2022.2.3\CLionProjects\fund_algs_2\allocator_1\cmake-build-debug" "D:\JetBrains\CLion 2022.2.3\CLionProjects\fund_algs_2\allocator_1\cmake-build-debug\CMakeFiles\allocator_1.dir\DependInfo.cmake" --color=$(COLOR)
.PHONY : CMakeFiles/allocator_1.dir/depend

