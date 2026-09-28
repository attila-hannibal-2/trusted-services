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


include(${TS_ROOT}/deployments/libts/libts-import.cmake)
target_link_libraries(attila-app PRIVATE libts::ts )


#-------------------------------------------------------------------------------
#  Common main for all deployments
#
#-------------------------------------------------------------------------------
target_sources(attila-app PRIVATE
	"${CMAKE_CURRENT_LIST_DIR}/attila_app.c"
)

#-------------------------------------------------------------------------------
#  Components that are common across all deployments
#
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
#  Define install content.
#
#-------------------------------------------------------------------------------
if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
	set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/install CACHE PATH "location to install build output to." FORCE)
endif()
install(TARGETS attila-app RUNTIME DESTINATION ${TS_ENV}/bin)
