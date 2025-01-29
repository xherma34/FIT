NAME = proj3
LOGIN = xherma34

all:
	latex $(NAME).tex
	latex $(NAME).tex
	dvips -t a4 $(NAME).dvi
	ps2pdf -sPAPERSIZE=a4 $(NAME).ps

zip:
	zip $(LOGIN)-fit.zip *.tex Makefile oniisan.eps oniisan2.eps etiopan.eps algorithm2e.sty 

clean:
	rm -rf *.dvi *.out *.aux *.log *.ps *.pdf *.zip
