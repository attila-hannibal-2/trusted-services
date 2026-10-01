#ifndef SERVICE_INTERFACE_H
#define SERVICE_INTERFACE_H

#include "components/service/locator/interface/service_locator.h"

#ifdef __cplusplus
extern "C" {
#endif

void _service_locator_init(void);
struct service_context *_service_locator_query(const char *sn);
struct rpc_caller_session *_service_context_open(struct service_context *s);
void _service_context_close(struct service_context *s, struct rpc_caller_session *session_handle);
void _service_context_relinquish(struct service_context *context);

#ifdef __cplusplus
}
#endif

#endif /* SERVICE_INTERFACE_H */