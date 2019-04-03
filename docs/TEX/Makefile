TARGET=tfg
TARGET_BIB = biblio
# make pdf by default
all: ${TARGET}.pdf

%.pdf : %.dvi
	#pdflatex $*
	latex $*
	latex $*
	dvipdfm $i*.dvi

#${TARGET}.bbl: ./aguti.bib
${TARGET}.bbl: ./${TARGET_BIB}.bib
# in case we don't already have a .aux file listing citations
# this should probably be a separate makefile target/dependency instead
# of doing it every time... but *shrug*
	latex ${TARGET}.tex
# get the citations out of the bibliography
	bibtex ${TARGET}
# do it again in case there are out-of-order cross-references
	@latex ${TARGET}.tex

${TARGET}.dvi: ${TARGET}.bbl ${TARGET}.tex
	@latex ${TARGET}.tex

# shortcut, so we can say "make ps"
ps: ${TARGET}.ps

${TARGET}.ps: ${TARGET}.dvi
	@dvips -t a4 ${TARGET}.dvi

clean:
#	rm -f ${TARGET}.{log,aux,ps,dvi,bbl,blg,log,pdf}
	rm -f ${TARGET}.pdf
	rm -f *.dvi
	rm -f *.lof
	rm -f *.lot
	rm -f *.log
	rm -f *.toc
	rm -f *.blg
	rm -f *.bbl
	rm -f *.aux
	rm -f *.out


reallyclean: clean
	rm -f ${TARGET}.{ps,pdf}


PHONY : ps all clean reallyclean
