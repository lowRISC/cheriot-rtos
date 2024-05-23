// Copyright Microsoft and CHERIoT Contributors.
// SPDX-License-Identifier: MIT

#pragma once

static inline int isprint(int c)
{
	switch (c)
	{
		default:
			return 0;
		case '\x20' ... '\x7e':
			return 1;
	}
}

static inline int isdigit(int c)
{
	return c >= '0' && c <= '9';
}

static inline int isspace(int c)
{
	switch (c)
	{
		default:
			return 0;
		case '\t':
		case '\n':
		case '\v':
		case '\f':
		case '\r':
		case ' ':
			return 1;
	}
}

