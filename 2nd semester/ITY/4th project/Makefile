LOGIN=xherma34
file=proj4

all:
	latex ${file}.tex
	bibtex ${file}
	latex ${file}.tex
	latex ${file}.tex
	dvips ${file}.dvi
	ps2pdf -sPAPERSIZE=a4 ${file}.ps
zip:
	zip $(LOGIN)-fit.zip *.tex *.pdf *.bst *.bib Makefile

clean:
	rm -rf *.dvi *.out *.aux *.log *.ps *.pdf *.zip
