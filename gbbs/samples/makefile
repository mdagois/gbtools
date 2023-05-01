projects = parallax sound_test serial printer
configurations = debug release
default_target = help

build_directory = build

debug_description = build with debug features and extra checks enabled
release_description = deliverable build

compile_options = -Werror -Weverything -Hl
debug_compile_options = -DBUILD_DEBUG
release_compile_options = -DBUILD_RELEASE

padding_value = 199
link_options = --pad $(padding_value)

fix_options = --pad-value $(padding_value) --validate

launch_options = --watch

########################################
# parallax
########################################

parallax_description = a sample to demonstrate a parallax effect
parallax_sources = $(wildcard src/parallax_optimized/*.rgbasm)
parallax_link_options = --dmg --tiny

########################################
# sound_test
########################################

sound_test_description = a program to test sound
sound_test_sources = $(wildcard src/sound_test/*.rgbasm)
sound_test_link_options = --dmg --tiny

########################################
# serial
########################################

serial_description = a program that sends multiple bytes between two Game Boy
serial_sources = $(wildcard src/serial_multibytes/*.rgbasm)
serial_link_options = --dmg --tiny
serial_launch_options = --listen 8765
serial_launch_options2 = --connect 127.0.0.1:8765

########################################
# printer
########################################

printer_description = a printer simulator
printer_sources = $(wildcard src/printer/*.rgbasm)
printer_link_options = --tiny
printer_fix_options = --color-only
printer_launch_options = --listen 8765

########################################

include ../gbbs.mk

