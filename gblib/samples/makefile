projects = parallax
configurations = debug release
default_target = help

build_directory = build

debug_description = build with debug features and extra checks enabled
release_description = deliverable build

compile_options = -v -Werror -Weverything -I../prebuilt
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
parallax_sources = $(wildcard parallax/*.rgbasm ../prebuilt/gblib.rgbasm)
parallax_link_options = --dmg --tiny

########################################

include ../../gbbs/gbbs.mk

