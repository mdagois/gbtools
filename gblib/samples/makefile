projects = parallax template
configurations = debug release
default_target = all

build_directory = build

debug_description = build with debug features and extra checks enabled
release_description = deliverable build

compile_options = -v -Werror -Weverything -I../prebuilt
debug_compile_options = -DBUILD_DEBUG
release_compile_options = -DBUILD_RELEASE

padding_value = 199
link_options = --pad $(padding_value) --dmg

fix_options = --pad-value $(padding_value) --validate --mbc-type 0x1B --ram-size 0x02

launch_options = --watch

########################################
# parallax
########################################

parallax_description = a sample to demonstrate a parallax effect
parallax_sources = $(wildcard parallax/*.rgbasm ../prebuilt/gblib.rgbasm)

########################################
# template
########################################

template_description = a template from which to build applications
template_sources = $(wildcard template/*.rgbasm ../prebuilt/gblib.rgbasm)

########################################

include ../../gbbs/gbbs.mk

