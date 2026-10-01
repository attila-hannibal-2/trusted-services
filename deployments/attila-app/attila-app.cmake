#-------------------------------------------------------------------------------
# Copyright (c) 2020-2023, Arm Limited and Contributors. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
#  The base build file shared between deployments of 'attila-app' for
#  different environments.  Demonstrates use of trusted services by a
#  client application.
#-------------------------------------------------------------------------------

# Attila: either use the libts library and link to our app to have the 
# main components / .c files compile
# and in this case the IMSG() / EMSG() macroes uses the pre-compiled
# ts_trace_printf() and TRACE_PREFIX="LIBTS"
#include(${TS_ROOT}/deployments/libts/libts-import.cmake)
#target_link_libraries(attila-app PRIVATE libts::ts )


#-------------------------------------------------------------------------------
#  Common main for all deployments
#
#-------------------------------------------------------------------------------
target_sources(attila-app PRIVATE
	"${CMAKE_CURRENT_LIST_DIR}/attila_app.c"
	"${CMAKE_CURRENT_LIST_DIR}/posix_trace.c"
)


# Attila: or compile the components selectively and thus use custom TRACE_PREFIX
set(TRACE_PREFIX "Attila-App" CACHE STRING "Trace prefix")
set(TRACE_LEVEL "TRACE_LEVEL_DEBUG" CACHE STRING "Trace level")
target_compile_definitions(attila-app PRIVATE
	TRACE_LEVEL=${TRACE_LEVEL}
	TRACE_PREFIX="${TRACE_PREFIX}"
)
#-------------------------------------------------------------------------------
#  Components that are common across all deployments
#
#-------------------------------------------------------------------------------
# Setting the MM communication buffer parameters
set(MM_COMM_BUFFER_ADDRESS "0x881000000" CACHE STRING "Address of MM communicte buffer")
set(MM_COMM_BUFFER_SIZE "8*4*1024" CACHE STRING "Size of the MM communicate buffer in bytes")
add_components(
	TARGET "attila-app"
	BASE_DIR ${TS_ROOT}
	COMPONENTS
		"components/app/attila-say-hello"
		"components/common/utils"
		"components/common/trace"
		"components/rpc/common/caller"
		"components/rpc/common/interface"
		"components/rpc/ts_rpc/caller/linux"
		"components/rpc/mm_communicate/caller/linux/"
		"components/service/locator"
		"components/service/locator/linux"
		"components/service/locator/interface"
		"components/service/locator/linux/ffa"
		"components/service/locator/linux/mm_communicate"
)

#-------------------------------------------------------------------------------
#  Define install content.
#
#-------------------------------------------------------------------------------
if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
	set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/install CACHE PATH "location to install build output to." FORCE)
endif()
install(TARGETS attila-app RUNTIME DESTINATION ${TS_ENV}/bin)
