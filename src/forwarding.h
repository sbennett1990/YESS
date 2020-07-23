/*
 * forwarding.h
 */

#ifndef	FORWARDING_H
#define	FORWARDING_H

#include "registers.h"

typedef struct {
	unsigned int	D_icode;

	rregister	d_srcA;
	rregister	d_srcB;

	unsigned int	E_icode;
	rregister	E_dstM;

	unsigned int	e_Cnd;
	rregister	e_dstE;
	unsigned int	e_valE;

	unsigned int	M_icode;
	unsigned int	M_Cnd;
	rregister	M_dstE;
	unsigned int	M_valE;
	rregister	M_dstM;
	unsigned int	M_valA;

	unsigned int	m_stat;
	unsigned int	m_valM;

	unsigned int	W_stat;
	unsigned int	W_icode;
	rregister	W_dstE;
	unsigned int	W_valE;
	rregister	W_dstM;
	unsigned int	W_valM;
} forwardType;

#endif	/* FORWARDING_H */
