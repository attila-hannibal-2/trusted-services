#include "../rpc_interface.h"
#include "protocols/rpc/common/packed-c/status.h"
#include "components/service/attila/provider/attila_uuid.h"
#include "CppUTestExt/MockSupport.h"
#include <cstring>
#include "rpc_interface_mock.h"

uint8_t testRequestBuffer[8192];
attila_message testResponse { .msg_length=21, .msg = "mocked-test-response"};

rpc_call_handle _rpc_caller_session_begin(struct rpc_caller_session *session,
					 uint8_t **request_buffer,
					 size_t request_length,
					 size_t response_max_length)
{
    *request_buffer = testRequestBuffer;

    return static_cast<rpc_call_handle>(mock()
        .actualCall("_rpc_caller_session_begin")
        .withParameter("session", session)
        .withParameter("request_length", request_length)
		.withParameter("response_max_length", response_max_length)
        .returnPointerValueOrDefault(reinterpret_cast<void*>(0x1111)));
   
}

rpc_status_t _rpc_caller_session_invoke(rpc_call_handle handle, uint32_t opcode,
				       uint8_t **response_buffer,
				       size_t *response_length,
				       service_status_t *service_status)
{
    *response_buffer = (uint8_t*)&testResponse;
    *response_length = sizeof(testResponse.msg_length) + strlen(testResponse.msg) + 1;
    return mock()
        .actualCall("_rpc_caller_session_invoke")
        .withParameter("handle", handle)
		.withParameter("opcode", opcode)
        .returnIntValueOrDefault(TS_RPC_CALL_ACCEPTED);

    
}

rpc_status_t _rpc_caller_session_end(rpc_call_handle handle)
{
    return mock()
        .actualCall("_rpc_caller_session_end")
        .withParameter("handle", handle)
        .returnIntValueOrDefault(TS_RPC_CALL_ACCEPTED);
}

void check_request(const char* msg, uint64_t msg_length)
{
    attila_message * msg_ptr = (attila_message *)testRequestBuffer;
    CHECK_EQUAL(msg_length, msg_ptr->msg_length);
    STRCMP_EQUAL(msg, msg_ptr->msg);
}