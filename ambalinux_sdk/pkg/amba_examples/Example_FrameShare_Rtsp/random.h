#ifndef _RAMDOM_H_
#define _RAMDOM_H_

#ifdef __cplusplus
extern "C" {
#endif

extern void our_srandom(unsigned int x);
extern unsigned int our_random();
extern unsigned int our_random32();

#ifdef __cplusplus
}
#endif

#endif
