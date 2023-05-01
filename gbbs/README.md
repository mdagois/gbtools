# Game Boy Build System

A straightforward build system for programs written in assembly using the [RGBDS](https://github.com/gbdev/rgbds) toolchain.

## Description

Game Boy Build System (GBBS) leverages `make` to simplify building and managing ROMs.
It consists of a single file, `gbbs.mk`, containing a build rule generator.
Building a ROM only requires a makefile that defines a small set of mandatory variables and includes `gbbs.mk`.
Optional variables can be specified in order to have a finer control over the build process.
Additional custom rules can be added to the makefile for tasks such as asset conversion.

### Dependencies

Dependencies are kept to a minimum.

* RGBDS toolchain (`rgbasm`, `rgblink` and `rgbfix`)
* make 4.2 or later
* `mkdir`, `rm` and `touch` Bash commands (available in Git for non-Unix shells)

### Features

* assembly/linkage/fix rules generation
* dependencies management
* compilation/linkage flags change detection
* help rule generation
* out-of-source build
* parallel rule execution support
* GNU Make syntax still fully available
* no recursive makefile
* makefile debugging utilities
* user-friendly error reporting for missing mandatory variables or unexpected values

### Example

Here is a rather minimal example of a makefile using GBBS.
It builds a DMG ROM from a single source file.

	projects = my_game
	configurations = release

	build_directory = build

	my_game_sources = src/main.rgbasm
	my_game_link_options = --dmg --tiny

	include gbbs.mk

For more complex examples, see the tutorial and the samples.

## Terminology

### Solution

A solution is a conceptual structure to organize projects, configurations and build settings.
There is only one solution.
It is implicitly defined by the makefile using GBBS.

### Project

A project is a container for source files and settings used to build a ROM.
There is at least one project in a solution.

### Configuration

A configuration is a collection of properties used when building projects.
Typical configurations include `debug`, `release`, `testing` or `production`.
There is at least one configuration in a solution.

### Target

Targets are generated from projects and configurations.
The most interesting targets produce ROMs.
For example, for a project `game` and a configuration `debug`, GBBS generates a target named `game_debug` which builds a ROM.

## Help

### Tutorial

The `tutorial` directory contains a makefile that presents how to use GBBS.
Open the makefile and read the comments in it to learn the basics of GBBS.

### Samples

The `samples` directory contains a makefile that leverages GBBS to build various ROMs.
It shows a more realistic use case of GBBS.

## Reference

Variables are used to describe the solution, projects and configurations.
These variables must be defined *BEFORE* `gbbs.mk` is included in the solution, as they are used to generate the build rules.

### Solution variables

Here is a list of all the variables defined at the solution level.

#### projects

The list of projects in the solution.
A project's name must be less than 11 characters long, as it becomes the game name in the ROM header.
Mandatory: at least one project must be specified.

#### configurations

The list of configurations in the solution.
Typical values include `debug` or `release`.
Mandatory: at least one configuration must be specified.

#### build_directory

The directory in which all temporary build files are output.
This directory must be dedicated to the build system, as the `clean` type rules delete all or part of it.
Mandatory.

#### compile_options

A list of options passed to `rgbasm` during the compilation of any target in the solution.

#### link_options

A list of options passed to `rgblink` during compilation of any target in the solution.

#### fix_options

A list of options passed to `rgbfix` during compilation of any target in the solution.

#### default_target

The default target executed when running the `make` command without any target specified.
It defaults to `help` when not specified.

#### shell_command_directory

The path to a directory containing shell executables required by the GBBS (e.g. C:\\Program Files\\Git\\usr\\bin\\).
It must end with a directory separator (either `\\` or `/`).
Optional if compatible versions of `mkdir`, `rm` and `touch` are already in the path.

#### emulator_command

The command used to launch the emulator.
Default to `bgb`.

#### rgbds_directory

The path to the RGBDS directory.
It must end with a directory separator (either `\\` or `/`).
Optional if `rgbasm`, `rgblink` and `rgbfix` are already in the path.

### Project variables

#### \<project\>_sources

A list of the source files added to the project.
Source files paths are expressed relative to the source directory.
Mandatory, unless `<project>_<configuration>_sources` is specified.

#### \<project\>_description

The description displayed in the `help` rule for the project.

#### \<project\>_compile_options

A list of options passed to `rgbasm` during the compilation of any target in the project.

#### \<project\>_link_options

A list of options passed to `rgblink` during compilation of any target in the project.

#### \<project\>_fix_options

A list of options passed to `rgbfix` during compilation of any target in the project.

#### \<project\>_launch_options

A list of options passed to the first instance of the `BGB` emulator when launching a target.

#### \<project\>_launch_options2

A list of options passed to the second instance of the `BGB` emulator when launching a target.
The second instance is only started when this variable is not empty.
This is convenient when building and testing multiplayer games, as it is possible to `--listen` on the first instance and `--connect` on the second one.

### Configuration variables

#### \<configuration\>_description

The description displayed in the `help` rule.

#### \<configuration\>_compile_options

A list of options passed to `rgbasm` during the compilation of any target in the configuration.

#### \<configuration\>_link_options

A list of options passed to `rgblink` during compilation of any target in the configuration.

#### \<configuration\>_fix_options

A list of options passed to `rgbfix` during compilation of any target in the configuration.

### Target variables

#### \<project\>_\<configuration\>_sources

A list of the source files added to the target.
Source files paths are expressed relative to the source directory.
Mandatory, unless `<project>_sources` is specified.

#### \<project\>_\<configuration\>_compile_options

A list of options passed to `rgbasm` during the compilation of the target.

#### \<project\>_\<configuration\>_link_options

A list of options passed to `rgblink` during compilation of the target.

#### \<project\>_\<configuration\>_fix_options

A list of options passed to `rgbfix` during compilation of the target.

### Read-only variables

Read-only variables are a convenience that can be used in custom rules.
These variables must be referenced *AFTER* `gbbs.mk` is included in the makefile.
As their name implies, these variables must not be overwritten.

#### \<project\>_\<configuration\>_binary

The absolute path to the ROM produced by the `make <project>_<configuration>` command.

#### \<project\>_\<configuration\>_binary_directory

The absolute path to the directory that contains the ROM produced by the `make <project>_<configuration>` command.

### Notes

The following options are automatically handled by GBBS.

* rgbasm
  * -Imakefile_directory
  * -M dependency_file -MG -MP
* rgblink
  * --map (-m)
  * --sym (-s)
  * --output (-o)
* rgbfix
  * --title (-t)

## Miscellaneous

### Build directory

The build directory contains all binaries and temporary files produced by the build process.

For each configuration, a directory is created to hold the binaries.
These directories are of direct interest to the user of the build system, as they hold the final result of the build process, i.e. the ROMs, map and symbol files.

The build directory also contains a `.temp` directory that holds temporary files used by the build system to perform its duties.
This directory can be safely ignored.

### Debug rules

#### printvars

This rule prints information about most of the variables that are defined by the makefile.

#### print-\<X\>

This rule prints information about the `X` variable.
For example, `make print-foo` would print info about the variable `foo`.

### Parallel execution

Rules execute in parallel when `make` is invoked with the `-j<N>` option.
There is one notable exception though.
`clean` type rules delete directories, so they cannot safely be run at the same time as `build` rules.
So, `make -j8 clean all` will be forcibly executed as `make clean all` (not parallel).
It is recommended to execute both rules separately, i.e. `make clean` first, then `make -j8 all`.

### In case of trouble

If `make` commands keep failing on the same error, even after the error has been seemingly addressed, do `make disable_asserts=1 clean`.
If that does not solve the issue, manually delete the `build_directory` and re-run `make`.

### Acknowledgement

GBBS's design has been inspired by the `make` articles written by John Graham-Cumming.

