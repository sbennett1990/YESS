# Make yess

yess:
	@echo '===> building yess...'
	cd ${.CURDIR}/src && ${MAKE} obj && exec ${MAKE}

install: yess
	@echo '===> installing...'
	cd ${.CURDIR}/src && ${MAKE} install

SUBDIR+= test
#SUBDIR+= contrib

MAN=	yess.1
README=	README.md

# Generate the README for GitHub
md:
	mandoc -Tmarkdown ${MAN} > ${README}

.include <bsd.subdir.mk>
