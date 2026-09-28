/*
 * Copyright (c) 2023, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef ATTILA_UUID_H
#define ATTILA_UUID_H
#ifdef __cplusplus
extern "C" {
#endif
#define TS_ATTILA_UUID                                          \
	{                                                       \
		0x11, 0x11, 0x11, 0x11, 0x22, 0x22, 0x33, 0x33, \
		0x44, 0x44, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, \
	}

struct __attribute__((__packed__)) attila_message {
	uint64_t msg_length;
	char msg[];
};

#define TS_ATTILA_OPCODE_BASE (0x0100)
#define TS_ATTILA_SAY_HELLO   (TS_ATTILA_OPCODE_BASE + 1)

#ifdef __cplusplus
}
#endif
#endif /* ATTILA_UUID_H */