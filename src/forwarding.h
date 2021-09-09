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

	icode_t		M_icode;	/* icode Memory Stage operated on */
	unsigned int	M_Cnd;
	rregister	M_dstE;
	unsigned int	M_valE;
	rregister	M_dstM;
	unsigned int	M_valA;

	stat_t		m_stat;		/* status code produced by Memory Stage */
	unsigned int	m_valM;		/* a data value read from memory */

	stat_t		W_stat;		/* status code Writeback Stage received */
	icode_t		W_icode;	/* icode Writeback Stage operated on */
	rregister	W_dstE;		/* program register 'E' written to */
	unsigned int	W_valE;		/* a value stored in program register 'E' */
	rregister	W_dstM;		/* program register 'M' written to */
	unsigned int	W_valM;		/* a value stored in program register 'M',
					   which was read from memory */
} forwardType;

#endif	/* FORWARDING_H */
