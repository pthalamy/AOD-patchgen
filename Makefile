
CC=gcc
CXX=g++
LATEXC=pdflatex
DOCC=doxygen
CFLAGS=-Wall
CXXFLAGS=-O3 -Wall

REFDIR=.
SRCDIR=$(REFDIR)/src
BINDIR=$(REFDIR)/bin
DOCDIR=$(REFDIR)/doc
REPORTDIR=$(REFDIR)/rapport

LATEXSOURCE=$(wildcard $(REPORTDIR)/*.tex)
CSOURCE=$(wildcard $(SRCDIR)/applyPatch.c)
CXXSOURCE=$(wildcard $(SRCDIR)/computePatchOpt.cpp)
PDF=$(LATEXSOURCE:.tex=.pdf)


all: binary report doc


$(BINDIR)/applyPatch: $(CSOURCE)
	$(CC) $(CFLAGS)  $^ -o $@

$(BINDIR)/computePatchOpt: $(CXXSOURCE)
	$(CXX) $(CXXFLAGS) $^ -o $@

%.pdf: $(LATEXSOURCE)
	$(LATEXC) -output-directory $(REPORTDIR) $^

$(DOCDIR)/index.html: $(SRCDIR)/Doxyfile $(CSOURCE)
	$(DOCC) $(SRCDIR)/Doxyfile


binary: $(BINDIR)/applyPatch $(BINDIR)/computePatchOpt

report: $(PDF)

doc: $(DOCDIR)/index.html

clean:
	rm -rf $(DOCDIR) $(BINDIR)/* $(REPORTDIR)/*.aux $(REPORTDIR)/*.log  $(REPORTDIR)/rapport.pdf


.PHONY: all doc binary report
