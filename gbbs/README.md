# Game Boy Build System

A straightforward build system for programs written in assembly using the [RGBDS](https://github.com/gbdev/rgbds) toolchain.

## Description

Game Boy Build System (GBBS) leverages `make` to simplify building and managing ROMs.
It consists of a single file, `gbbs.mk`, containing a build rule generator.
Building a ROM only requires a makefile that defines a small set of mandatory variables and includes `gbbs.mk`.
Optional variables can be specified to allow a finer control over the build process.
Additional custom rules can be added to the makefile for tasks such as asset conversion.

### Example

Here is a very simple example of a makefile using GBBS.
The user defines several variables.
Then, GBBS generates the `make` targets to build a DMG ROM from a single source file.

	projects = my_game
	configurations = release

	build_directory = build

	fix_options = --validate

	my_game_sources = src/main.rgbasm
	my_game_link_options = --dmg --tiny

	include gbbs.mk

To learn the basics of GBBS, head to the [getting started](#getting-started) section.
Also, check the [reference](#reference) to discover the available variables.

### Requirements

Requirements have been kept to a minimum.
Only the following executables are required.

* [RGBDS](https://github.com/gbdev/rgbds) toolchain (`rgbasm`, `rgblink` and `rgbfix`)
* GNU make 4.2 or later
* `mkdir`, `rm` and `touch` shell commands (like those found in Bash)

On Windows, the shell commands come with Git (by default, installed under `C:\Program Files\Git\usr\bin`).
A copy of `make` is available [here](../third_party) for convenience.
You can grab the RGBDS toolchain binaries from their GitHub repository.

On other operating systems, the shells commands should be already available.
Make sure you have the right version of `make`, and then, install the RGBDS toolchain.

On all systems, GBBS should work without any additional setup if all the required executables are in the PATH.
However, if the executables are not in the PATH, it is possible to define the `shell_command_directory` and `rgbds_directory` variables to point to directories containing the shell command executables and the RGBDS toolchain respectively.
There are three ways to define those variables: on the command line, in the makefile itself and in the special file `user.mk` (more on this one below).

Assuming that Windows is the operating system and that the RGBDS toolchain is in `c:\src\bin`, below is how the `rgbds_directory` variable would be specified on the command line.
Note that `/` is used instead of `\`, even on Windows, as `make` interprets the latter as an escaping character.

	make rgbds_directory=c:/src/bin <target>

When defining the variable in the makefile itself, it just needs to be assigned the right value as below.
Double quotes (`"`) are necessary when the directory path contains spaces.

	rgbds_directory = c:/src/bin

Finally, it is possible to define the variables in a special file, named `user.mk`, placed in the same directory as the makefile.
GBBS always includes `user.mk` if it exists.
This is the preferred method, especially if several people are using the same makefile (e.g. a collaborative project).
It keeps the `make` commands simple (i.e. no need to specify the directory every single time), and it does not clutter the makefile with hardcoded paths.

### Features

Here is a list of the main features of GBBS.

* GNU make rules generation
	* GBBS generates the build rules for the compilation, linkage and fix phases
	* GBBS provides a simple help rule to list available `make` targets
* Dependencies management
	* GBBS tracks dependencies between files and rebuilds only the necessary artifacts
	* GBBS also detects compilation and linkage options changes
* Clean and efficient
	* GBBS is designed for out-of-source builds
	* GBBS has strong support for parallel rule execution (`make -j`)
	* GNU make syntax is still fully available
	* GBBS does not rely on recursive makefiles
* Miscellaneous
	* GBBS exposes some makefile debugging utilities
	* GBBS reports user-friendly error for missing mandatory variables or unexpected values

## Getting started

### Terminology

|Term			|Description
|:---			|:---
|Solution		|A solution is a conceptual structure to organize projects, configurations and build settings. There is only one solution. It is implicitly defined by the makefile using GBBS.
|Project		|A project is a container for source files and settings used to build a ROM. There is at least one project in a solution.
|Configuration	|A configuration is a collection of properties used when building projects. Typical configurations include `debug`, `release`, `testing` or `production`. There is at least one configuration in a solution.
|Target			|Targets are generated from projects and configurations. The most interesting targets produce ROMs. For example, for a project `game` and a configuration `debug`, GBBS generates a target named `game_debug` which builds a ROM.

### Tutorial

The [tutorial](tutorial) directory contains a makefile that introduces the basics of GBBS.
Open that makefile and read the comments in it.

### Samples

The [samples](samples) directory contains a makefile that leverages GBBS to build multiple ROMs.
It shows a more complex use case of GBBS.

## Reference

Variables are used to describe the solution, projects and configurations.
These variables must be defined *BEFORE* `gbbs.mk` is included in the solution, as they are used to generate the build rules.

### Solution variables

Here is a list of the variables available at the solution level.

|Variable							|Description
|:---								|:---
|projects							|The list of projects in the solution. A project's name must be less than 11 characters long, as it becomes the game name in the ROM header. Mandatory: at least one project must be specified.
|configurations						|The list of configurations in the solution. Typical values include `debug` or `release`. Mandatory: at least one configuration must be specified.
|build_directory					|The directory in which all temporary build files are output. This directory must be dedicated to the build system, as the `clean` type rules delete all or part of it. Mandatory.
|compile_options					|A list of options passed to `rgbasm` during the compilation of any target in the solution.
|link_options						|A list of options passed to `rgblink` during compilation of any target in the solution.
|fix_options						|A list of options passed to `rgbfix` during compilation of any target in the solution.
|default_target						|The default target executed when running the `make` command without any target specified. It defaults to `help` when not specified.
|shell_command_directory			|The path to a directory containing the shell command executables required by GBBS (e.g. C:/Program Files/Git/usr/bin). Optional if compatible versions of `mkdir`, `rm` and `touch` are already in the PATH.
|emulator_command					|The command used to launch the emulator. Defaults to `bgb`.
|rgbds_directory					|The path to the RGBDS directory. Optional if `rgbasm`, `rgblink` and `rgbfix` are already in the PATH.

### Project variables

Here is a list of the variables available for each project.

|Variable							|Description
|:---								|:---
|\<project\>_sources				|A list of the source files added to the project. Source files paths are expressed relative to the makefile directory. Mandatory, unless `<project>_<configuration>_sources` is specified.
|\<project\>_description			|The description displayed by the `help` rule for the project.
|\<project\>_compile_options		|A list of options passed to `rgbasm` during the compilation of any target in the project.
|\<project\>_link_options			|A list of options passed to `rgblink` during compilation of any target in the project.
|\<project\>_fix_options			|A list of options passed to `rgbfix` during compilation of any target in the project.
|\<project\>_launch_options			|A list of options passed to the first instance of the emulator when launching a ROM.
|\<project\>_launch_options2		|A list of options passed to the second instance of the emulator when launching a ROM. The second instance is only started when this variable is not empty. It is convenient when building and testing multiplayer games, as it is possible to `--listen` on the first instance and `--connect` on the second one.

### Configuration variables

Here is a list of the variables available for each configuration.

|Variable							|Description
|:---								|:---
|\<configuration\>_description		|The description displayed by the `help` rule.
|\<configuration\>_compile_options	|A list of options passed to `rgbasm` during the compilation of any target in the configuration.
|\<configuration\>_link_options		|A list of options passed to `rgblink` during compilation of any target in the configuration.
|\<configuration\>_fix_options		|A list of options passed to `rgbfix` during compilation of any target in the configuration.

### Target variables

Here is a list of the variables available for each target.

|Variable										|Description
|:---											|:---
|\<project\>_\<configuration\>_sources			|A list of the source files added to the target. Source files paths are expressed relative to the makefile directory. Mandatory, unless `<project>_sources` is specified.
|\<project\>_\<configuration\>_compile_options	|A list of options passed to `rgbasm` during the compilation of the target.
|\<project\>_\<configuration\>_link_options		|A list of options passed to `rgblink` during compilation of the target.
|\<project\>_\<configuration\>_fix_options		|A list of options passed to `rgbfix` during compilation of the target.

### Read-only variables

Read-only variables are a convenience that can be used in custom rules.
These variables must be referenced *AFTER* `gbbs.mk` is included in the makefile.
As their name implies, these variables must not be overwritten.

|Variable										|Description
|:---											|:---
|\<project\>_\<configuration\>_binary			|The absolute path to the ROM produced by the `make <project>_<configuration>` command.
|\<project\>_\<configuration\>_binary_directory	|The absolute path to the directory that contains the ROM produced by the `make <project>_<configuration>` command.

### Notes

The following options are automatically handled by GBBS.
They must not be used as compilation, linkage or fix options.

* rgbasm
  * -M <dependency_file> -MG -MP
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
These directories are of direct interest to the user of the build system, as they hold the final result of the build process, i.e. the ROM, map and symbol files.

The build directory also contains a `.temp` directory that holds temporary files used by the build system to perform its duties.
This directory can be safely ignored.

### Debug rules

There are a few debug rules that can be used to inspect the variables defined by GBBS and the makefile.
They are mostly useful when debugging GBBS itself.

|Rule			|Description
|:---			|:---
|printvars		|This rule prints information about most of the variables that are defined by the makefile.
|print-\<X\>	|This rule prints information about the `X` variable. For example, `make print-foo` would print info about the variable `foo`.

### Parallel execution

Rules execute in parallel when `make` is invoked with the `-j<N>` option.
There is one notable exception though.
Rules of the `clean` type delete directories, so they cannot safely be run at the same time as `build` rules.
So, `make -j8 clean all` will be forcibly executed as `make clean all` (i.e. not parallel).
It is recommended to execute both rules separately, i.e. `make clean` first, then `make -j8 all`.

### In case of trouble

If `make` commands keep failing on the same error, even after the error has been seemingly addressed, do `make disable_asserts=1 clean`.
If the problem persists, manually delete the `build_directory` and re-run `make`.

### Acknowledgement

GBBS's design has been inspired by the `make` articles written by John Graham-Cumming.

