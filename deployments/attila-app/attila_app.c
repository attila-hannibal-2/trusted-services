/*
 * Copyright (c) 2020-2023, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "common/utils/include/util.h"
#include "components/rpc/common/interface/rpc_uuid.h"
#include "components/service/attila/provider/attila_uuid.h"
#include "components/service/locator/interface/service_locator.h"
#include "protocols/rpc/common/packed-c/status.h"

int main(int argc, char *argv[])
{
	rpc_call_handle handle = 0;
	uint8_t *request = NULL;
	uint8_t *response = NULL;
	size_t request_length = 0;
	size_t response_length = 0;
	size_t msg_length = 0;
	struct attila_message *request_desc = NULL;
	const char *msg = "mira-cica";
	rpc_status_t rpc_status = TS_RPC_CALL_ACCEPTED;
	service_status_t service_status;
	struct service_context *context = NULL;
	struct rpc_caller_session *rpc_session = NULL;

	printf("Starting main entry point\n");
	service_locator_init();
	printf("Querying service via service_locator...\n");
	context = service_locator_query("sn:trustedfirmware.org:attila:0");

	if (!context) {
		printf("ERROR Failed to discover service\n");
		return 1;
	}
	printf("Open service context...\n");
	rpc_session = service_context_open(context);
	if (!rpc_session) {
		printf("ERROR Failed to get RPC session\n");
		return 1;
	}

	msg_length = strlen(msg);

	if (ADD_OVERFLOW(msg_length, 1, &msg_length))
		return 2;

	if (ADD_OVERFLOW(sizeof(*request_desc), msg_length, &request_length))
		return 3;

	handle = rpc_caller_session_begin(rpc_session, &request, request_length, 0);
	if (handle) {
		request_desc = (struct attila_message *)request;
		memcpy(&request_desc->msg, msg, msg_length);
		request_desc->msg_length = msg_length;
		printf("Calling RPC invoke...\n");
		rpc_status = rpc_caller_session_invoke(handle, TS_ATTILA_SAY_HELLO, &response,
						       &response_length, &service_status);
		printf("RPC invoke finished rpc_status:%d\n", rpc_status);
		rpc_caller_session_end(handle);
	} else {
		printf("ERROR Failed to get RPC handle\n");
	}

	printf("Clean up resources...\n");
	service_context_close(context, rpc_session);
	rpc_session = NULL;

	service_context_relinquish(context);
	context = NULL;

	printf("App finished\n");

	return 0;
}
