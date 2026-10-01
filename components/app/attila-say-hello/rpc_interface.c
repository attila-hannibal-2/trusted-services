#include "rpc_interface.h"

rpc_call_handle _rpc_caller_session_begin(struct rpc_caller_session *session,
					 uint8_t **request_buffer,
					 size_t request_length,
					 size_t response_max_length)
{
    return rpc_caller_session_begin(session, request_buffer, request_length, response_max_length);
}

rpc_status_t _rpc_caller_session_invoke(rpc_call_handle handle, uint32_t opcode,
				       uint8_t **response_buffer,
				       size_t *response_length,
				       service_status_t *service_status)
{
    return rpc_caller_session_invoke(handle, opcode, response_buffer, response_length, service_status);
}

rpc_status_t _rpc_caller_session_end(rpc_call_handle handle)
{
    return rpc_caller_session_end(handle);
}