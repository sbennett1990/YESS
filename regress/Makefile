
YESS?=		~/bin/yess
#YESS?=		../src/obj/yess

YO_ERROR_TESTS=	error1 error2 error3

YO_TEST_PROGS=

.for y in ${YO_ERROR_TESTS}
REGRESS_TARGETS+=	run-errors-$y
run-errors-$y:
	${YESS} -sf ${.CURDIR}/$y.yo 2>/dev/null | \
		diff -u ${.CURDIR}/$y.dump /dev/stdin
.endfor

.include <bsd.regress.mk>
