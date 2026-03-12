FILENAME=guia_control_flujo

all: $(FILENAME).pdf

$(FILENAME).pdf: $(FILENAME).tex
	pdflatex $(FILENAME).tex
	pdflatex $(FILENAME).tex
	rm -f *.aux *.log *.out

clean:
	rm -f *.pdf *.aux *.log *.out

.PHONY: all clean
