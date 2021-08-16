NAME = proj5

$(NAME).pdf: $(NAME).ps
	ps2pdf $(NAME).ps

$(NAME).ps: $(NAME).dvi
	dvips $(NAME).dvi

$(NAME).dvi: $(NAME).tex
	latex $(NAME).tex
	latex $(NAME).tex

clean:
	rm $(NAME).aux $(NAME).dvi $(NAME).log $(NAME).nav $(NAME).out $(NAME).pdf $(NAME).ps $(NAME).snm $(NAME).toc $(NAME).vrb
