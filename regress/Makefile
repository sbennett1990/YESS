
YESS?=		~/bin/yess
#YESS?=		../src/obj/yess

YO_TEST_PROGS=	lack-one-space

.for y in ${YO_TEST_PROGS}
REGRESS_TARGETS+=	run-$y
run-$y:
	${YESS} -sf ${.CURDIR}/$y.yo 2>/dev/null | \
		diff -u ${.CURDIR}/$y.dump /dev/stdin
.endfor

.include <bsd.regress.mk>
