NAME=proj4

$(NAME).pdf: $(NAME).ps
	ps2pdf $(NAME).ps

$(NAME).ps: $(NAME).dvi
	dvips -t a4 $(NAME).dvi	

$(NAME).dvi: $(NAME).tex
	latex $(NAME).tex
	bibtex $(NAME).aux
	latex $(NAME).tex
	latex $(NAME).tex

clean:
	rm $(NAME).out $(NAME).aux $(NAME).log $(NAME).pdf $(NAME).dvi $(NAME).ps $(NAME).bbl $(NAME).blg

