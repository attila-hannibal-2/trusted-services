#ifndef RPC_INTERFACE_H
#define RPC_INTERFACE_H

#include "components/rpc/common/caller/rpc_caller.h"
#include "components/rpc/common/caller/rpc_caller_session.h"

#ifdef __cplusplus
extern "C" {
#endif

rpc_call_handle _rpc_caller_session_begin(struct rpc_caller_session *session,
					 uint8_t **request_buffer,
					 size_t request_length,
					 size_t response_max_length);

rpc_status_t _rpc_caller_session_invoke(rpc_call_handle handle, uint32_t opcode,
				       uint8_t **response_buffer,
				       size_t *response_length,
				       service_status_t *service_status);

rpc_status_t _rpc_caller_session_end(rpc_call_handle handle);

#ifdef __cplusplus
}
#endif

#endif /* RPC_INTERFACE_H */