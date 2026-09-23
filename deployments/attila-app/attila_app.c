/*
 * Copyright (c) 2020-2023, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <app/attila-app/attila_app_component.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
	printf("Starting component's main entry point\n");
	run_attila_app_component();

	printf("Component's main entry point finished\n");

	return 0;
}
