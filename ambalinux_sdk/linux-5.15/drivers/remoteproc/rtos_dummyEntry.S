/*
 *
 */

#include <linux/linkage.h>

/*
 * X0: parameter passed from ATF;
 * General purpose registers are used in example.
 * They are 32-bits wide.
 */
SYM_FUNC_START(rtos_dummyEntry)
	MOV	X10, #0
	/* Waiting parameters. */
	MOV     X9, X0
1:
	LDR     W10, [X9]
	CMP     W10, #0
	BEQ     1b
	SEV
	/* Prepare SMC call */
	MOV	X0, #0
	MOV	X1, #0
	MOV	X2, #0
	MOV	X3, #0
	LDR     W0, [X9]
	LDR     W1, [X9, #4]
	LDR     W2, [X9, #8]
	LDR     W3, [X9, #12]
	SMC     #0
	/* Should not be here */
	B       .
SYM_FUNC_END(rtos_dummyEntry)

