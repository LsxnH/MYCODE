# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.11

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
CMAKE_COMMAND = /cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase/x86_64/Cmake/3.11.0/Linux-x86_64/bin/cmake

# The command to remove a file.
RM = /cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase/x86_64/Cmake/3.11.0/Linux-x86_64/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /hepustc/home/hengli/testarea/MyDerivation/source

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /hepustc/home/hengli/testarea/MyDerivation/build

# Utility rule file for ContinuousCoverage.

# Include the progress variables for this target.
include CMakeFiles/ContinuousCoverage.dir/progress.make

CMakeFiles/ContinuousCoverage:
	/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase/x86_64/Cmake/3.11.0/Linux-x86_64/bin/ctest -D ContinuousCoverage

ContinuousCoverage: CMakeFiles/ContinuousCoverage
ContinuousCoverage: CMakeFiles/ContinuousCoverage.dir/build.make

.PHONY : ContinuousCoverage

# Rule to build all files generated by this target.
CMakeFiles/ContinuousCoverage.dir/build: ContinuousCoverage

.PHONY : CMakeFiles/ContinuousCoverage.dir/build

CMakeFiles/ContinuousCoverage.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/ContinuousCoverage.dir/cmake_clean.cmake
.PHONY : CMakeFiles/ContinuousCoverage.dir/clean

CMakeFiles/ContinuousCoverage.dir/depend:
	cd /hepustc/home/hengli/testarea/MyDerivation/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /hepustc/home/hengli/testarea/MyDerivation/source /hepustc/home/hengli/testarea/MyDerivation/source /hepustc/home/hengli/testarea/MyDerivation/build /hepustc/home/hengli/testarea/MyDerivation/build /hepustc/home/hengli/testarea/MyDerivation/build/CMakeFiles/ContinuousCoverage.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/ContinuousCoverage.dir/depend

