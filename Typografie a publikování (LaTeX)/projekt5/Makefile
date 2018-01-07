# ITY projekty Makefile
# Marek Sipos (xsipos03)

# cislo projektu
PROJECT=5

all:
	latex proj$(PROJECT).tex
	latex proj$(PROJECT).tex
	dvips proj$(PROJECT).dvi
	ps2pdf -sPAPERSIZE=a4 proj$(PROJECT).ps

clean clear:
	-rm -f proj$(PROJECT).aux proj$(PROJECT).ps proj$(PROJECT).dvi proj$(PROJECT).log proj$(PROJECT).te~ proj$(PROJECT).out proj$(PROJECT).snm proj$(PROJECT).toc proj$(PROJECT).nav

vlna:
	vlna -l -m -n -v KkSsVvZzOoUuAaIi proj$(PROJECT).tex