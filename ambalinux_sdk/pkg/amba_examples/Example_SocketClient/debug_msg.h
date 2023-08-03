/*
 * debug_msg.h
 *
 * Copyright 2022 Ambarella Inc.
 *
 */

#ifndef _DEBUG_MSG_H
#define _DEBUG_MSG_H

#ifdef __cplusplus
extern "C" {
#endif

extern uint32_t g_debug_en;
#define DBG_MSG(f_, ...) { if (g_debug_en) printf((f_),##__VA_ARGS__);} //show debug message

#ifdef __cplusplus
}
#endif

#endif	/* _DEBUG_MSG_H */
