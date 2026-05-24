TARGETS = pre_parcial_1 pre_parcial_2
PDFS = $(addsuffix .pdf, $(TARGETS))

all: $(PDFS)

%.pdf: %.tex
	pdflatex $<
	pdflatex $<
	rm -f *.aux *.log *.out

clean:
	rm -f *.pdf *.aux *.log *.out

.PHONY: all clean
