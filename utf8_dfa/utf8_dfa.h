// Copyright (c) 2008-2009 Bjoern Hoehrmann <bjoern@hoehrmann.de>
// See http://bjoern.hoehrmann.de/utf-8/decoder/dfa/ for details.

#pragma once
#include <cstdint>

namespace Utf8Dfa
{
	extern const uint8_t table[];

	enum
	{
		ACCEPT = 0,
		REJECT = 1
	};

	inline uint32_t decode(uint32_t* state, uint32_t* codep, uint32_t byte)
	{
		uint32_t type = table[byte];

		*codep = (*state != ACCEPT) ?
			(byte & 0x3fu) | (*codep << 6) :
			(0xff >> type) & (byte);

		*state = table[256 + *state*16 + type];
		return *state;
	}
}
