NAME = proj1
LOGIN = xherma34

all:
	latex $(NAME).tex
	latex $(NAME).tex
	dvips -t a4 $(NAME).dvi
	ps2pdf $(NAME).ps

zip:
	zip $(LOGIN)-fit.zip *.tex Makefile

clean:
	rm -rf *.dvi *.out *.aux *.log *.ps *.pdf *.zip
