PROJ=imp-proj
PACK=xkotek09-fit
INCLUDE=czechiso.bst

.PHONY: clean pack bibtex pdflatex

.DEFAULT_GOAL := $(PROJ).pdf

$(PROJ).pdf: $(PROJ).ps
	ps2pdf -sPAPERSIZE=a4 $^

$(PROJ).ps: $(PROJ).dvi
	dvips $^

$(PROJ).dvi: $(PROJ).tex $(PROJ).bib
	latex $^
	bibtex $(PROJ)
	latex $^
	latex $^

bibtex: $(PROJ).bib
	bibtex $(PROJ)

pdflatex: $(PROJ).tex $(PROJ).bib
	pdflatex $^
	bibtex $(PROJ)
	pdflatex $^
	pdflatex $^

pack: $(PACK).tar.gz

$(PACK).tar.gz: $(PROJ).tex $(PROJ).bib $(INCLUDE)
	tar -czf $@ $^ Makefile

clean:
	rm -f $(PROJ).aux $(PROJ).dvi $(PROJ).log $(PROJ).ps $(PROJ).synctex.gz $(PROJ).fls $(PROJ).fdb_latexmk $(PROJ).bbl $(PROJ).blg $(PROJ).out $(PROJ).toc $(PROJ).lof $(PROJ).lot $(PROJ).nav $(PROJ).snm $(PROJ).vrb
