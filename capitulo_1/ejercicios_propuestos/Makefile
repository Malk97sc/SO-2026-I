TARGETS = taller_procesos taller_procesos_2 taller_practico_sumador taller_inventario_biblioteca taller_inventario_procesos
PDFS = $(addsuffix .pdf, $(TARGETS))

all: $(PDFS)

%.pdf: %.tex
	pdflatex $<
	pdflatex $<
	rm -f $*.aux $*.log $*.out

clean:
	rm -f *.pdf *.aux *.log *.out

.PHONY: all clean
