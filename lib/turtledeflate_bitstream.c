/*
Copyright (c) 2022, Ralf Willenbacher
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in
   the documentation and/or other materials provided with the
   distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
*/


#include "turtledeflate.h"

static const uint32_t rgui_mask[ 33 ] = {
	0x0, 0x1, 0x3, 0x7, 0xf, 0x1f, 0x3f, 0x7f,
	0xff, 0x1ff, 0x3ff, 0x7ff, 0xfff, 0x1fff, 0x3fff, 0x7fff,
	0xffff, 0x1ffff, 0x3ffff, 0x7ffff, 0xfffff, 0x1fffff, 0x3fffff, 0x7fffff,
	0xffffff, 0x1ffffff, 0x3ffffff, 0x7ffffff, 0xfffffff, 0x1fffffff, 0x3fffffff, 0x7fffffff,
	0xffffffff
};


void turtledeflate_bitstream_reset( turtledeflate_bitstream_t *ps_bitstream, uint8_t *ps_out )
{
	ps_bitstream->pui8_bitstream_start = ps_out;
	ps_bitstream->pui8_codeword_ptr = ps_out;
	ps_bitstream->ui64_codeword = 0;
	ps_bitstream->i_codeword_fill = 0;
}

void turtledeflate_bitstream_advance( turtledeflate_bitstream_t *ps_bitstream )
{
	ps_bitstream->pui8_codeword_ptr = ps_bitstream->pui8_bitstream_start;
}

void turtledeflate_bitstream_write( turtledeflate_bitstream_t *ps_bitstream, uint32_t ui_code, uint32_t ui_length )
{
	ps_bitstream->ui64_codeword |= ( ( uint64_t ) ( ui_code & rgui_mask[ ui_length ] ) ) << ( ps_bitstream->i_codeword_fill );
	ps_bitstream->i_codeword_fill += ui_length;

	while( ps_bitstream->i_codeword_fill > 7 )
	{
		*( ps_bitstream->pui8_codeword_ptr++ ) = ( ps_bitstream->ui64_codeword & 0xff );
		ps_bitstream->ui64_codeword >>= 8;
		ps_bitstream->i_codeword_fill -= 8;
	}
}

void turtledeflate_bitstream_writehuff( turtledeflate_bitstream_t *ps_bitstream, uint32_t ui_code_, uint32_t ui_length )
{
	uint32_t ui_idx;
	uint32_t ui_rcode = 0, ui_code = ui_code_ & rgui_mask[ ui_length ];

	for( ui_idx = 0; ui_idx < ui_length; ui_idx++ )
	{
		ui_rcode |= ( ( ui_code >> ( ui_length - ui_idx - 1 ) ) & 1 ) << ui_idx;
	}

	turtledeflate_bitstream_write( ps_bitstream, ui_rcode, ui_length );
}

void turtledeflate_bitstream_bytealign( turtledeflate_bitstream_t *ps_bitstream )
{
	if( ps_bitstream->i_codeword_fill > 0 )
	{
		*( ps_bitstream->pui8_codeword_ptr++ ) = ( ps_bitstream->ui64_codeword & 0xff );
		ps_bitstream->ui64_codeword = 0;
		ps_bitstream->i_codeword_fill = 0;
	}
}

int32_t turtledeflate_bitstream_bytesize( turtledeflate_bitstream_t *ps_bitstream )
{
	int32_t i_len;
	i_len = ( int32_t ) ( ps_bitstream->pui8_codeword_ptr - ps_bitstream->pui8_bitstream_start );
	return i_len;
}