#ifndef _DDRC_TRAINING_ASM_H_
#define _DDRC_TRAINING_ASM_H_

#ifndef _STDDEF_H
/* To obtain macro NULL */
/* Could also be stdio.h, stdlib.h and others in hosted environments */
#include <stddef.h>
#endif

void InitStack(unsigned long addr);

#endif
