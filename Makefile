# Make yess

yess:
	@echo '===> building yess...'
	cd ${.CURDIR}/src && ${MAKE} obj && exec ${MAKE}

install: yess
	@echo '===> installing...'
	cd ${.CURDIR}/src && ${MAKE} install

SUBDIR+= test
#SUBDIR+= contrib

regression-tests:
	@echo '===> running regression tests...'
	@cd ${.CURDIR}/regress && ${MAKE} depend && exec ${MAKE} regress

MAN=	yess.1
README=	README.md

# Generate the README for GitHub
md:
	mandoc -Tmarkdown ${MAN} > ${README}

.include <bsd.subdir.mk>
