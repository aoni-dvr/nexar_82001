/*
 * $ Copyright Open Broadcom Corporation $
 *
 *
 * <<Broadcom-WL-IPTag/Open:>>
 *
 * $Id: epivers.h.in 596126 2015-10-29 19:53:48Z stafford $
 *
*/

#ifndef _epivers_h_
#define _epivers_h_

#define	EPI_MAJOR_VERSION	100

#define	EPI_MINOR_VERSION	10

#define	EPI_RC_NUMBER		96

#define	EPI_INCREMENTAL_NUMBER	0

#define	EPI_BUILD_NUMBER	0

#define	EPI_VERSION		100, 10, 96, 0

#define	EPI_VERSION_NUM		0x640a6000

#define EPI_VERSION_DEV		100.10.96

/* Driver Version String, ASCII, 32 chars max */
#ifdef BCMINTERNAL
#define	EPI_VERSION_STR		"100.10.96 (8c0aa58 BCMINT)"
#else
#ifdef WLTEST
#define	EPI_VERSION_STR		"100.10.96 (8c0aa58 WLTEST)"
#else
#define	EPI_VERSION_STR		"100.10.96 (8c0aa58)"
#endif
#endif /* BCMINTERNAL */

#endif /* _epivers_h_ */
