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

#-------------------------------------------------------------------------------
#  Use libpsats for locating PSA services. An appropriate version of
#  libpsats will be imported for the environment.
#-------------------------------------------------------------------------------
if (COVERAGE)
	set(LIBPSATS_BUILD_TYPE "DEBUGCOVERAGE" CACHE STRING "Libpsats build type" FORCE)
	set(LIBTS_BUILD_TYPE "DEBUGCOVERAGE" CACHE STRING "Libts build type" FORCE)
endif()

include(${TS_ROOT}/deployments/libpsats/libpsats-import.cmake)
target_link_libraries(attila-app PRIVATE libpsats::psats)

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
add_components(
	TARGET "attila-app"
	BASE_DIR ${TS_ROOT}
	COMPONENTS
		"components/app/attila-app"
)

#-------------------------------------------------------------------------------
#  Define install content.
#
#-------------------------------------------------------------------------------
if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
	set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/install CACHE PATH "location to install build output to." FORCE)
endif()
install(TARGETS attila-app RUNTIME DESTINATION ${TS_ENV}/bin)
