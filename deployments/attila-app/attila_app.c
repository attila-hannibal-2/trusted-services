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
#include "trace.h"

int main(int argc, char *argv[])
{
	rpc_call_handle handle = 0;
	uint8_t *request = NULL;
	uint8_t *response = NULL;
	size_t request_length = 0;
	size_t response_length = 0;
	size_t msg_length = 0;
	struct attila_message *req_message = NULL;
	const char *msg = "Mira and Lola";
	rpc_status_t rpc_status = TS_RPC_CALL_ACCEPTED;
	service_status_t service_status;
	struct service_context *context = NULL;
	struct rpc_caller_session *rpc_session = NULL;

	IMSG("Starting main entry point");
	service_locator_init();
	DMSG("Querying service via service_locator...");
	context = service_locator_query("sn:trustedfirmware.org:attila:0");

	if (!context) {
		EMSG("ERROR Failed to discover service");
		return 1;
	}
	DMSG("Open service context...");
	rpc_session = service_context_open(context);
	if (!rpc_session) {
		EMSG("ERROR Failed to get RPC session");
		return 1;
	}

	msg_length = strlen(msg);

	//closing '\0' character
	if (ADD_OVERFLOW(msg_length, 1, &msg_length))
		return 2;

	if (ADD_OVERFLOW(sizeof(*req_message), msg_length, &request_length))
		return 3;

	handle = rpc_caller_session_begin(rpc_session, &request, request_length, 128);
	if (handle) {
		req_message = (struct attila_message *)request;
		memcpy(&req_message->msg, msg, msg_length);
		req_message->msg_length = msg_length;
		IMSG("Calling RPC invoke...");
		rpc_status = rpc_caller_session_invoke(handle, TS_ATTILA_SAY_HELLO, &response,
						       &response_length, &service_status);
		IMSG("RPC invoke finished rpc_status:%d", rpc_status);
		struct attila_message *resp_message = (struct attila_message *)(response);
		IMSG("RPC response length:%ld content:'%s'", resp_message->msg_length, resp_message->msg);
		rpc_caller_session_end(handle);
	} else {
		EMSG("ERROR Failed to get RPC handle");
	}

	DMSG("Clean up resources...");
	service_context_close(context, rpc_session);
	rpc_session = NULL;

	service_context_relinquish(context);
	context = NULL;

	DMSG("App finished");

	return 0;
}
