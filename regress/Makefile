
YESS?=		~/bin/yess
#YESS=		../src/obj/yess

# programs that test error conditions
YO_ERROR_TESTS=	error1 error2 error3 error4

# programs that test for correct functionality
YO_TEST_PROGS=	test10a test10b test10c test10d test10e test10f test10g test10h \
		test11a test11b test11c test11d test11e test11f test11g test11h \
		test11i test11j \
		test12a test12b test12c test12d test12e test12f test12g test12h

REGRESS_FAIL_EARLY=	yes

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
