#include "service_interface.h"

void _service_locator_init(void)
{
    service_locator_init();
}

struct service_context *_service_locator_query(const char *sn)
{
    return service_locator_query(sn);
}

struct rpc_caller_session *_service_context_open(struct service_context *s)
{
    return service_context_open(s);
}

void _service_context_close(struct service_context *s, struct rpc_caller_session *session_handle)
{
    service_context_close(s, session_handle);
}

void _service_context_relinquish(struct service_context *context)
{
    service_context_relinquish(context);
}