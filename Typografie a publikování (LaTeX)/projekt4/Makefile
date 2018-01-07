# ITY projekty Makefile
# Marek Sipos (xsipos03)

# cislo projektu
PROJECT=4

all:
# doplneni vlnek (nezlomitelne mezery k osamocenym predlozkam)
	vlna -l -m -n -v KkSsVvZzOoUuAaIi proj$(PROJECT).tex
# prvni iterace prekladu pro bibliograficke citace
	latex proj$(PROJECT).tex
	bibtex proj$(PROJECT).aux
# tvorba pomocneho AUX
	latex proj$(PROJECT).tex
# reference krizovych odkazu
	latex proj$(PROJECT).tex
	dvips -t a4 proj$(PROJECT).dvi
	ps2pdf -sPAPERSIZE=a4 proj$(PROJECT).ps

clean clear:
	-rm -f proj$(PROJECT).aux proj$(PROJECT).ps proj$(PROJECT).dvi proj$(PROJECT).log proj$(PROJECT).te~ proj$(PROJECT).bbl proj$(PROJECT).blg

vlna:
	vlna -l -m -n -v KkSsVvZzOoUuAaIi proj$(PROJECT).tex