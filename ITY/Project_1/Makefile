NAME = proj1

$(NAME).pdf: $(NAME).ps
	ps2pdf $(NAME).ps

$(NAME).ps: $(NAME).dvi
	dvips -sPAPERSIZE=a4 $(NAME).dvi

$(NAME).dvi: $(NAME).tex
	latex $(NAME).tex
	latex $(NAME).tex
