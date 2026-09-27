/*
 * Copyright (c) 2020-2023, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <app/attila-app/attila_app_component.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "common/utils/include/util.h"
#include "components/rpc/common/interface/rpc_uuid.h"
#include "components/service/attila/provider/attila_uuid.h"
#include "components/service/locator/interface/service_locator.h"
#include "protocols/rpc/common/packed-c/status.h"

struct __attribute__((__packed__)) msg_request {
	uint64_t msg_length;
	char msg[];
};

#define TS_ATTILA_OPCODE_BASE (0x0100)
#define TS_ATTILA_SAY_HELLO   (TS_ATTILA_OPCODE_BASE + 1)

int main(int argc, char *argv[])
{
	printf("Starting component's main entry point\n");
	//rpc_status_t sp_init_status = RPC_ERROR_INTERNAL;
	//const struct rpc_uuid service_uuid = { .uuid = TS_ATTILA_UUID };

	service_locator_init();
	printf("_________1 service_locator_query...\n");
	struct service_context *context = service_locator_query("sn:trustedfirmware.org:attila:0");

	if (!context) {
		printf("Failed to discover service\n");
		return 1;
	}
	printf("_________2 service_context_open...\n");
	struct rpc_caller_session *rpc_session = service_context_open(context);

	rpc_call_handle handle = 0;
	uint8_t *request = NULL;
	uint8_t *response = NULL;
	size_t request_length = 0;
	size_t response_length = 0;
	size_t msg_length = 0;
	struct msg_request *request_desc = NULL;
	const char *msg = "mira-cica";
	rpc_status_t rpc_status = TS_RPC_CALL_ACCEPTED;
	UNUSED_VAR(rpc_status);
	service_status_t service_status;

	msg_length = strlen(msg);

	if (ADD_OVERFLOW(msg_length, 1, &msg_length))
		return 2;

	if (ADD_OVERFLOW(sizeof(*request_desc), msg_length, &request_length))
		return 3;
	printf("_________3 calling rpc_caller_session_begin...\n");
	handle = rpc_caller_session_begin(rpc_session, &request, request_length, 0);
	if (handle) {
		printf("_________4 calling rpc_caller_session_invoke...\n");
		request_desc = (struct msg_request *)request;
		memcpy(&request_desc->msg, msg, msg_length);
		request_desc->msg_length = msg_length;

		rpc_status = rpc_caller_session_invoke(handle, TS_ATTILA_SAY_HELLO, &response,
						       &response_length, &service_status);
		printf("_________5 rpc_status:%d\n", rpc_status);
		rpc_caller_session_end(handle);
	} else {
		printf("_________6 rpc_caller_session_begin returned NULL handle\n");
	}

	printf("_________7 session clean up...\n");
	service_context_close(context, rpc_session);
	rpc_session = NULL;

	service_context_relinquish(context);
	context = NULL;

	printf("Component's main entry point finished\n");

	return 0;
}
