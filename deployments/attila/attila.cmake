#-------------------------------------------------------------------------------
# Copyright (c) 2020-2022, Arm Limited and Contributors. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

add_components(TARGET "attila"
	BASE_DIR ${TS_ROOT}
	COMPONENTS
		components/common/tlv
		components/service/common/include
		components/service/common/provider
		components/service/common/client
		components/rpc/common/caller
		protocols/rpc/common/packed-c		
)

target_include_directories(attila PRIVATE
	${TS_ROOT}
	${TS_ROOT}/components
)
