/*
 * Copyright (c) 2023, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <CppUTest/TestHarness.h>
#include <CppUTestExt/MockSupport.h>
#include <mock_assert.h>
#include <cstdint>
#include <cstring>

#include <app/attila-say-hello/attila_say_hello.h>
#include "components/service/attila/provider/attila_uuid.h"
#include "rpc_interface_mock.h"

TEST_GROUP(AttilaSayHelloTests)
{
    void setup()
    {
    }

    void teardown()
    {
        mock().checkExpectations();
        mock().clear();
    }
};

TEST(AttilaSayHelloTests, OK)
{
    mock().expectOneCall("_service_locator_init");
    mock().expectOneCall("_service_locator_query")
        .withParameter("sn", "sn:trustedfirmware.org:attila:0");
    mock().expectOneCall("_service_context_open")
        .withParameter("s", (void*)0x1234);


    mock().expectOneCall("_rpc_caller_session_begin")
        .withParameter("session", (void*)0x5678)
        .withParameter("request_length", 8+14)
        .withParameter("response_max_length", 128);
    mock().expectOneCall("_rpc_caller_session_invoke")
        .withParameter("handle", (void*)0x1111)
        .withParameter("opcode", TS_ATTILA_SAY_HELLO);
    mock().expectOneCall("_rpc_caller_session_end")
        .withParameter("handle", (void*)0x1111);

    mock().expectOneCall("_service_context_close")
        .withParameter("s", (void*)0x1234)
        .withParameter("session_handle", (void*)0x5678);
    mock().expectOneCall("_service_context_relinquish")
        .withParameter("context", (void*)0x1234);

	int ret = run_attila_say_hello();
    CHECK_EQUAL(0, ret);
    check_request("Mira and Lola", strlen("Mira and Lola") + 1);
}

TEST(AttilaSayHelloTests, NOK)
{
    mock().expectOneCall("_service_locator_init");
    mock().expectOneCall("_service_locator_query")
        .withParameter("sn", "sn:trustedfirmware.org:attila:0");
    mock().expectOneCall("_service_context_open")
        .withParameter("s", (void*)0x1234)
        .andReturnValue((void*)NULL);


	int ret = run_attila_say_hello();
    CHECK_EQUAL(1, ret);
}