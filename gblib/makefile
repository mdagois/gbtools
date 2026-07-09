SHELL := bash

sources = $(wildcard src/*.rgbinc) $(wildcard src/*.rgbasm)

.PHONY: all
all: doc/doc.txt

prebuilt/gblib.rgbinc prebuilt/gblib.rgbasm: $(sources) scripts/build_lib.py
	@python scripts/build_lib.py

doc/doc.txt: prebuilt/gblib.rgbinc scripts/generate_doc.py
	@mkdir -p doc && python scripts/generate_doc.py $< $@

