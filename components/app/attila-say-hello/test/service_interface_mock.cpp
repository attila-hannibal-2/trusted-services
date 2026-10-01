#include "../service_interface.h"

#include "CppUTestExt/MockSupport.h"

void _service_locator_init(void)
{
    mock().actualCall("_service_locator_init");
}
struct service_context *_service_locator_query(const char *sn)
{
   return static_cast<service_context*>(mock()
        .actualCall("_service_locator_query")
        .withParameter("sn", sn)
        .returnPointerValueOrDefault((void*)0x1234));
}

struct rpc_caller_session *_service_context_open(struct service_context *s)
{
     return static_cast<rpc_caller_session*>(mock()
        .actualCall("_service_context_open")
        .withParameter("s", s)
        .returnPointerValueOrDefault((void*)(0x5678)));
}

void _service_context_close(struct service_context *s, struct rpc_caller_session *session_handle)
{
    mock().actualCall("_service_context_close")
        .withParameter("s", s)
        .withParameter("session_handle", session_handle);
}

void _service_context_relinquish(struct service_context *context)
{
     mock().actualCall("_service_context_relinquish").withParameter("context", context);
}