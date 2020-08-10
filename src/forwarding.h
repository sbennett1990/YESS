/*
 * forwarding.h
 */

#ifndef	FORWARDING_H
#define	FORWARDING_H

#include "instructions.h"
#include "registers.h"

typedef struct {
	icode_t		D_icode;

	rregister	d_srcA;
	rregister	d_srcB;

	icode_t		E_icode;
	rregister	E_dstM;

	unsigned int	e_Cnd;
	rregister	e_dstE;
	unsigned int	e_valE;

	icode_t		M_icode;
	unsigned int	M_Cnd;
	rregister	M_dstE;
	unsigned int	M_valE;
	rregister	M_dstM;
	unsigned int	M_valA;

	stat_t		m_stat;
	unsigned int	m_valM;

	stat_t		W_stat;
	icode_t		W_icode;
	rregister	W_dstE;
	unsigned int	W_valE;
	rregister	W_dstM;
	unsigned int	W_valM;
} forwardType;

#endif	/* FORWARDING_H */
