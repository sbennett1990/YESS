
YESS?=		~/bin/yess
#YESS?=		../src/obj/yess

YO_ERROR_TESTS=	error1 error2 error3 error4

YO_TEST_PROGS=	test12a test12b test12c test12d test12e test12f test12g test12h

.for y in ${YO_ERROR_TESTS}
REGRESS_TARGETS+=	run-errors-$y
run-errors-$y:
	${YESS} -sf ${.CURDIR}/$y.yo 2>/dev/null | \
		diff -u ${.CURDIR}/$y.dump /dev/stdin
.endfor

.for z in ${YO_TEST_PROGS}
REGRESS_TARGETS+=	run-$z
run-$z:
	${YESS} -f ${.CURDIR}/$z.yo 2>/dev/null | \
		diff -u ${.CURDIR}/$z.dump /dev/stdin
.endfor

.include <bsd.regress.mk>
