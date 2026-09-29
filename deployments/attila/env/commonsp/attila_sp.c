/*
 * Copyright (c) 2020-2026, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "common/utils/include/util.h"
#include "components/rpc/common/endpoint/rpc_service_interface.h"
#include "components/rpc/ts_rpc/endpoint/sp/ts_rpc_endpoint_sp.h"
#include "components/service/attila/provider/attila_uuid.h"
#include "components/service/common/provider/service_provider.h"
#include "components/service/log/factory/log_factory.h"
#include "protocols/rpc/common/packed-c/status.h"
#include "sp_api.h"
#include "sp_discovery.h"
#include "sp_messaging.h"
#include "sp_rxtx.h"
#include "trace.h"
#include <string.h>

static uint8_t tx_buffer[4096] __aligned(4096);
static uint8_t rx_buffer[4096] __aligned(4096);

static rpc_status_t say_hello_handler(void *context, struct rpc_request *req);

static const struct service_handler handler_table[] = { { TS_ATTILA_SAY_HELLO,
							  say_hello_handler } };

static rpc_status_t say_hello_handler(void *context, struct rpc_request *req)
{
	DMSG("say_hello_handler called");

	struct rpc_buffer *req_buf = &req->request;
	struct rpc_buffer *resp_buf = &req->response;
	struct attila_message *req_message = (struct attila_message *)(req_buf->data);
	struct attila_message *resp_message = (struct attila_message *)(resp_buf->data);
	size_t request_data_length = 0;
	const char greeting[] = "Hello ";
	const size_t greeting_length = sizeof(greeting) -1; //"compile time" strlen() 

	if(!req_buf || !req_message){
		return TS_RPC_ERROR_INVALID_REQ_BODY;
	}

	/* Checking for overflow */
	if (ADD_OVERFLOW(sizeof(*req_message), req_message->msg_length, &request_data_length)){
		return TS_RPC_ERROR_INVALID_REQ_BODY;
	}

	/* Checking if message and data fits into the request buffer */
	if (req_buf->data_length < request_data_length){
		return TS_RPC_ERROR_INVALID_REQ_BODY;
	}

	DMSG("Incoming message length:%ld, content:'%s'", req_message->msg_length, req_message->msg);
	uint64_t incoming_msg_length = req_message->msg_length;

	// Attila : the two buffers (req->request and req->response) points to the same address: 0x40027000
	// that means when constucting the response (buffer) it overwrites the input (buffer)
	// question: why?
	
	// first move the input to end to have space for the greetings
	memmove(resp_message->msg + greeting_length, req_message->msg, incoming_msg_length);
	// second insert the greating
	memcpy(resp_message->msg, greeting, greeting_length);
	
	uint64_t outgoing_msg_length = greeting_length + incoming_msg_length;
	resp_message->msg_length = outgoing_msg_length;
	resp_buf->data_length = sizeof(resp_message->msg_length) + outgoing_msg_length;

	DMSG("Outgoing attila message length:%ld, content:'%s'", resp_message->msg_length, resp_message->msg);
	DMSG("Outgoing RPC message length:%ld", resp_buf->data_length);
	req->service_status = RPC_SUCCESS;

	return RPC_SUCCESS;
}

void sp_main(union ffa_boot_info *boot_info)
{
	sp_result result = SP_RESULT_INTERNAL_ERROR;
	struct ts_rpc_endpoint_sp rpc_endpoint = { 0 };
	struct sp_msg req_msg = { 0 };
	struct sp_msg resp_msg = { 0 };
	uint16_t own_id = 0;
	const struct rpc_uuid service_uuid = { .uuid = TS_ATTILA_UUID };
	rpc_status_t rpc_status = RPC_ERROR_INTERNAL;
	struct rpc_service_interface *attila_iface = NULL;
	struct service_provider provider;

	/* Boot */
	UNUSED_VAR(boot_info);

	result = sp_rxtx_buffer_map(tx_buffer, rx_buffer, sizeof(rx_buffer));
	if (result != SP_RESULT_OK) {
		EMSG("Failed to map RXTX buffers: %d", result);
		goto fatal_error;
	}

	IMSG("Start discovering logging service");
	log_factory_create();
	DMSG("Attila SP init starts...");
	DMSG("Build date:%s - %s", __DATE__, __TIME__);
	result = sp_discovery_own_id_get(&own_id);
	if (result != SP_RESULT_OK) {
		EMSG("Failed to query own ID: %d", result);
		goto fatal_error;
	}
	DMSG("Own id:%d", own_id);
	// Attila: provider and context are the same???
	service_provider_init(&provider, &provider, &service_uuid, handler_table,
			      ARRAY_SIZE(handler_table));

	attila_iface = service_provider_get_rpc_interface(&provider);

	rpc_status = ts_rpc_endpoint_sp_init(&rpc_endpoint, 1, 16);
	if (rpc_status != RPC_SUCCESS) {
		EMSG("Failed to initialize RPC endpoint: %d", rpc_status);
		goto fatal_error;
	}
	DMSG("ts_rpc_endpoint_sp_init ok status:%d", rpc_status);

	rpc_status = ts_rpc_endpoint_sp_add_service(&rpc_endpoint, attila_iface);
	if (rpc_status != RPC_SUCCESS) {
		EMSG("Failed to add service to RPC endpoint: %d", rpc_status);
		goto fatal_error;
	}
	DMSG("ts_rpc_endpoint_sp_add_service ok status:%d", rpc_status);

	result = sp_msg_wait(&req_msg);
	if (result != SP_RESULT_OK) {
		EMSG("Failed to send message wait %d", result);
		goto fatal_error;
	}
	DMSG("sp_msg_wait ok status:%d", result);

	while (1) {
		ts_rpc_endpoint_sp_receive(&rpc_endpoint, &req_msg, &resp_msg);
		DMSG("ts_rpc_endpoint_sp_receive src:%d, dst:%d", req_msg.source_id,
		     req_msg.destination_id);

		result = sp_msg_send_direct_resp(&resp_msg, &req_msg);
		if (result != SP_RESULT_OK) {
			EMSG("Failed to send direct response %d", result);
			result = sp_msg_wait(&req_msg);
			if (result != SP_RESULT_OK) {
				EMSG("Failed to send message wait %d", result);
				goto fatal_error;
			}
		}
		DMSG("sp_msg_send_direct_resp result:%d", result);
	}

fatal_error:
	/* SP is not viable */
	EMSG("Attila SP error");
	while (1) {
	}
}

void sp_interrupt_handler(uint32_t interrupt_id)
{
	UNUSED_VAR(interrupt_id);
}

ffa_result ffa_vm_created_handler(uint16_t vm_id, uint64_t handle)
{
	UNUSED_VAR(vm_id);
	UNUSED_VAR(handle);

	return FFA_OK;
}

ffa_result ffa_vm_destroyed_handler(uint16_t vm_id, uint64_t handle)
{
	UNUSED_VAR(vm_id);
	UNUSED_VAR(handle);

	return FFA_OK;
}
