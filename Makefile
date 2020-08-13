# Make some manual pages

MAN=	yess.1
README=	README.md

md:
	mandoc -Tmarkdown ${MAN} > ${README}

.include <bsd.prog.mk>
