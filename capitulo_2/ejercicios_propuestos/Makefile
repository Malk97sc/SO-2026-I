SUBDIRS = señales pipes shm
TEX_FILES = $(shell find . -name "*.tex")
PDF_FILES = $(TEX_FILES:.tex=.pdf)

all: $(PDF_FILES)

%.pdf: %.tex
	pdflatex -output-directory=$(dir $<) $<
	pdflatex -output-directory=$(dir $<) $<
	rm -f $(dir $<)*.aux $(dir $<)*.log $(dir $<)*.out

clean:
	rm -f $(shell find . -name "*.pdf" -o -name "*.aux" -o -name "*.log" -o -name "*.out")

.PHONY: all clean
