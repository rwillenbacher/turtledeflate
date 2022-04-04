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


typedef struct turtledeflate_tree_node_helper_s
{
	struct turtledeflate_tree_node_helper_s * ps_next;
	int32_t i_node;
	int32_t i_weight;
} turtledeflate_tree_node_helper_t;


void turtledeflate_build_tree_insert_node( turtledeflate_tree_node_helper_t **pps_list, turtledeflate_tree_node_helper_t *ps_node )
{
	turtledeflate_tree_node_helper_t *ps_lnode;

	while( *pps_list )
	{
		ps_lnode = *pps_list;

		if( ps_node->i_weight < ps_lnode->i_weight )
		{
			ps_node->ps_next = ps_lnode;
			*pps_list = ps_node;
			return;
		}
		pps_list = &( ps_lnode->ps_next );
	}
	ps_node->ps_next = NULL;
	*pps_list = ps_node;
}


void turtledeflate_build_tree( turtledeflate_ctx_t *ps_turtle, int32_t *pi_symbol_counts, int32_t i_num_symbols, turtledeflate_tree_node_t *ps_tree_nodes, int32_t i_max_tree_nodes, int32_t *pi_num_nodes, int32_t *pi_root_node )
{
	int32_t i_idx, i_unlinked_nodes, i_tree_root, i_num_nodes, i_num_helper_nodes;
	uint32_t ui_order = 1;
	turtledeflate_tree_node_helper_t rgs_helper_nodes[ 1024 ];
	turtledeflate_tree_node_helper_t *ps_sorted_leaflist = NULL;

	assert( 1024 >= i_max_tree_nodes );

	i_num_nodes = 0;
	i_num_helper_nodes = 0;
	for( i_idx = 0; i_idx < i_num_symbols; i_idx++ )
	{
		if( pi_symbol_counts[ i_idx ] > 0 )
		{
			rgs_helper_nodes[ i_num_helper_nodes ].i_weight = pi_symbol_counts[ i_idx ];
			rgs_helper_nodes[ i_num_helper_nodes ].i_node = i_num_nodes;
			turtledeflate_build_tree_insert_node( &ps_sorted_leaflist, &rgs_helper_nodes[ i_num_helper_nodes ] );

			ps_tree_nodes[ i_num_nodes ].ui16_parent = 0xffff;
			ps_tree_nodes[ i_num_nodes ].ui16_left = 0xffff;
			ps_tree_nodes[ i_num_nodes ].ui16_right = 0xffff;
			ps_tree_nodes[ i_num_nodes ].ui_weight = pi_symbol_counts[ i_idx ];
			ps_tree_nodes[ i_num_nodes ].ui16_entry = i_idx;
			ps_tree_nodes[ i_num_nodes ].ui_order = TURTLEDEFLATE_MAX_UINT;
			i_num_nodes++;
			i_num_helper_nodes++;
			assert( i_num_nodes <= i_max_tree_nodes );

		}
	}

	if( i_num_nodes == 0 )
	{
		*pi_num_nodes = 0;
		*pi_root_node = TURTLEDEFLATE_MAX_UINT;
		return;
	}

	i_unlinked_nodes = i_num_nodes;

	while( i_unlinked_nodes > 1 )
	{
		uint32_t ui_lowest, ui_second_lowest;
#if 0
		ui_lowest = ui_second_lowest = 0xffff;
		for( i_idx = 0; i_idx < i_num_nodes; i_idx++ )
		{
			if( ps_tree_nodes[ i_idx ].ui16_parent == 0xffff && ( ui_lowest == 0xffff || ps_tree_nodes[ i_idx ].ui_weight < ps_tree_nodes[ ui_lowest ].ui_weight ) )
			{
				ui_second_lowest = ui_lowest;
				ui_lowest = i_idx;
			}
			else if( ps_tree_nodes[ i_idx ].ui16_parent == 0xffff && ( ui_second_lowest == 0xffff || ps_tree_nodes[ i_idx ].ui_weight < ps_tree_nodes[ ui_second_lowest ].ui_weight ) )
			{
				ui_second_lowest = i_idx;
			}
		}
#else
		ui_lowest = ps_sorted_leaflist->i_node;
		ps_sorted_leaflist = ps_sorted_leaflist->ps_next;
		ui_second_lowest = ps_sorted_leaflist->i_node;
		ps_sorted_leaflist = ps_sorted_leaflist->ps_next;

		rgs_helper_nodes[ i_num_helper_nodes ].i_weight = ps_tree_nodes[ ui_lowest ].ui_weight + ps_tree_nodes[ ui_second_lowest ].ui_weight;
		rgs_helper_nodes[ i_num_helper_nodes ].i_node = i_num_nodes;
		turtledeflate_build_tree_insert_node( &ps_sorted_leaflist, &rgs_helper_nodes[ i_num_helper_nodes ] );
		i_num_helper_nodes++;

#endif

		assert( ui_lowest != 0xffff && ui_second_lowest != 0xffff );



		ps_tree_nodes[ i_num_nodes ].ui16_parent = 0xffff;
		ps_tree_nodes[ i_num_nodes ].ui16_left = ui_lowest;
		ps_tree_nodes[ i_num_nodes ].ui16_right = ui_second_lowest;
		ps_tree_nodes[ i_num_nodes ].ui_weight = ps_tree_nodes[ ui_lowest ].ui_weight + ps_tree_nodes[ ui_second_lowest ].ui_weight;
		ps_tree_nodes[ i_num_nodes ].ui16_entry = 0xffff;

		ps_tree_nodes[ ui_lowest ].ui16_parent = i_num_nodes;
		ps_tree_nodes[ ui_lowest ].ui_order = ui_order++;
		ps_tree_nodes[ ui_second_lowest ].ui16_parent = i_num_nodes;
		ps_tree_nodes[ ui_second_lowest ].ui_order = ui_order++;

		( i_num_nodes )++;
		i_unlinked_nodes--;
		assert( i_num_nodes <= i_max_tree_nodes );
	}

	i_unlinked_nodes = 0;
	i_tree_root = TURTLEDEFLATE_MAX_UINT - 1;
	for( i_idx = 0; i_idx < i_num_nodes; i_idx++ )
	{
		if( ps_tree_nodes[ i_idx ].ui16_parent == 0xffff )
		{
			i_tree_root = i_idx;
			i_unlinked_nodes++;
		}
	}

	*pi_num_nodes = i_num_nodes;
	*pi_root_node = i_tree_root;

	assert( i_unlinked_nodes == 1 );
}


void turtledeflate_get_symbol_lengths( turtledeflate_ctx_t *ps_turtle, turtledeflate_tree_node_t *ps_tree_nodes, int32_t i_num_nodes, turtledeflate_symbol_code_t *ps_symbol_codes, int32_t i_num_symbol_codes, int32_t i_max_codeword_length )
{
	int32_t i_num_max_length_violations;
	int32_t i_assigned_1st_pass, i_assigned_fixup_pass, i_stack_idx;
	int32_t i_idx, i_symbol_idx, ui_node_idx, i_bits, i_next_symbol_idx, i_last_assigned;
	uint32_t i_lowest_order, i_highest_next_order;
	int32_t rgi_cwbit_counts[ TURTLEDEFLATE_MAX_CODEWORD_BITS + 1 ] = { 0 };
	uint16_t rgui16_node_right_stack[ TURTLEDEFLATE_MAX_CODEWORD_BITS + 1 + TURTLEDEFLATE_NUM_LENLITS ] = { 0 }; /* + TURTLEDEFLATE_NUM_LENLITS because there is no real limit on tree depth */
	uint32_t ui_root, ui_currentent_node;
	
	i_num_max_length_violations = 0;
	i_assigned_1st_pass = 0;
	for( i_idx = 0; i_idx < i_num_symbol_codes; i_idx++ )
	{
		ps_symbol_codes[ i_idx ].ui_code_length = 0;
		ps_symbol_codes[ i_idx ].ui_code = TURTLEDEFLATE_MAX_UINT;
		ps_symbol_codes[ i_idx ].ui_order = TURTLEDEFLATE_MAX_UINT;
	}
	ui_root = TURTLEDEFLATE_MAX_UINT;

	for( ui_node_idx = 0; ui_node_idx < i_num_nodes; ui_node_idx++ )
	{
		if( ps_tree_nodes[ ui_node_idx ].ui16_parent == 0xffff )
		{
			ui_root = ui_node_idx;
			break;
		}
	}

	ui_currentent_node = ui_root;
	i_stack_idx = 0;
	i_last_assigned = 0;

	while( ui_currentent_node != TURTLEDEFLATE_MAX_UINT )
	{
		if( ps_tree_nodes[ ui_currentent_node ].ui16_parent != 0xffff )
		{
			ps_tree_nodes[ ui_currentent_node ].ui8_code_length = ps_tree_nodes[ ps_tree_nodes[ ui_currentent_node ].ui16_parent ].ui8_code_length + 1;
		}
		else
		{
			ps_tree_nodes[ ui_currentent_node ].ui8_code_length = 0;
		}
		if( ps_tree_nodes[ ui_currentent_node ].ui8_code_length > i_max_codeword_length )
		{
			i_num_max_length_violations++;
			ps_tree_nodes[ ui_currentent_node ].ui8_code_length = i_max_codeword_length;
		}

		if( ps_tree_nodes[ ui_currentent_node ].ui16_entry == 0xffff )
		{
			rgui16_node_right_stack[ i_stack_idx++ ] = ps_tree_nodes[ ui_currentent_node ].ui16_right;
			ui_currentent_node = ps_tree_nodes[ ui_currentent_node ].ui16_left;
		}
		else
		{
			i_assigned_1st_pass++;


			int32_t i_entry = ps_tree_nodes[ ui_currentent_node ].ui16_entry;
			i_last_assigned = i_entry;

			ps_symbol_codes[ i_entry ].ui_order = ps_tree_nodes[ ui_currentent_node ].ui_order;
			ps_symbol_codes[ i_entry ].ui_code_length = ps_tree_nodes[ ui_currentent_node ].ui8_code_length;

			rgi_cwbit_counts[ ps_symbol_codes[ i_entry ].ui_code_length ]++;

			if( i_stack_idx == 0 )
			{
				break;
			}
			else
			{
				ui_currentent_node = rgui16_node_right_stack[ i_stack_idx - 1 ];
				i_stack_idx--;
			}
		}
	}


	if( i_assigned_1st_pass < 2 )
	{
		/* force at least two i_code_length_symbol codes with length 1 */
		ps_symbol_codes[ i_last_assigned ].ui_code_length = 1;
		ps_symbol_codes[ !i_last_assigned ].ui_code_length = 1;
	}

	if( i_num_max_length_violations == 0 )
	{
		return;
	}

	while( i_num_max_length_violations > 0 )
	{
		int32_t i_length_class = i_max_codeword_length - 1;

		while( rgi_cwbit_counts[ i_length_class ] == 0 )
		{
			i_length_class--;
		}
		rgi_cwbit_counts[ i_length_class ]--;
		rgi_cwbit_counts[ i_length_class + 1 ] += 2;
		rgi_cwbit_counts[ i_max_codeword_length ]--;
		i_num_max_length_violations -= 2;
	}

	i_assigned_fixup_pass = 0;
	i_lowest_order = 0;
	for( i_bits = i_max_codeword_length; i_bits > 0; i_bits-- )
	{
		int32_t i_cwbit_counts_in_class = rgi_cwbit_counts[ i_bits ];
		for( i_idx = 0; i_idx < i_cwbit_counts_in_class; i_idx++ )
		{
			i_next_symbol_idx = 0;
			i_highest_next_order = TURTLEDEFLATE_MAX_UINT;
			for( i_symbol_idx = 0; i_symbol_idx < i_num_symbol_codes; i_symbol_idx++ )
			{
				if( ps_symbol_codes[ i_symbol_idx ].ui_order > i_lowest_order && i_highest_next_order > ps_symbol_codes[ i_symbol_idx ].ui_order )
				{
					i_next_symbol_idx = i_symbol_idx;
					i_highest_next_order = ps_symbol_codes[ i_symbol_idx ].ui_order;
				}
			}
			ps_symbol_codes[ i_next_symbol_idx ].ui_code_length = i_bits;
			i_lowest_order = ps_symbol_codes[ i_next_symbol_idx ].ui_order;
			i_assigned_fixup_pass++;
		}
	}

	assert( i_assigned_fixup_pass == i_assigned_1st_pass );
}

void turtledeflate_symbol_length_to_code( turtledeflate_ctx_t *ps_turtle, turtledeflate_symbol_code_t *ps_symbol_codes, int32_t i_num_symbol_codes, int32_t i_max_codeword_length )
{
	int32_t rgi_bit_length_count[ TURTLEDEFLATE_MAX_CODEWORD_BITS + 1 ];
	int32_t rgi_next_code[ TURTLEDEFLATE_MAX_CODEWORD_BITS + 1 ];
	int32_t i_bits, i_idx;
	uint32_t ui_code;

	for( i_idx = 0; i_idx < i_num_symbol_codes; i_idx++ )
	{
		ps_symbol_codes[ i_idx ].ui_code = 0;
	}
	for( i_bits = 0; i_bits <= i_max_codeword_length; i_bits++ )
	{
		rgi_bit_length_count[ i_bits ] = 0;
	}
	for( i_idx = 0; i_idx < i_num_symbol_codes; i_idx++ )
	{
		assert( ps_symbol_codes[ i_idx ].ui_code_length <= ( uint32_t ) i_max_codeword_length );
		rgi_bit_length_count[ ps_symbol_codes[ i_idx ].ui_code_length ]++;
	}

	ui_code = 0;
	rgi_bit_length_count[ 0 ] = 0;
	for( i_bits = 1; i_bits <= i_max_codeword_length; i_bits++ )
	{
		ui_code = ( ui_code + rgi_bit_length_count[ i_bits - 1 ] ) << 1;
		rgi_next_code[ i_bits ] = ui_code;
	}

	for( i_idx = 0; i_idx < i_num_symbol_codes; i_idx++ )
	{
		uint32_t ui_len = ps_symbol_codes[ i_idx ].ui_code_length;
		if( ui_len != 0 )
		{
			ps_symbol_codes[ i_idx ].ui_code = rgi_next_code[ ui_len ];
			rgi_next_code[ ui_len ]++;
		}
	}

}


#define TURTLEDEFLATE_ENCODE_TREE_FLAGS_USE_16   1
#define TURTLEDEFLATE_ENCODE_TREE_FLAGS_USE_17   2
#define TURTLEDEFLATE_ENCODE_TREE_FLAGS_USE_18   4

int32_t turtledeflate_encode_tree( turtledeflate_ctx_t *ps_turtle, turtledeflate_symbol_code_t *ps_lenlit_symbols, turtledeflate_symbol_code_t *ps_dist_symbols, turtledeflate_bitstream_t *ps_bitstream, int32_t i_flags )
{
	int32_t i_total_litlendists;
	int32_t i_num_tree_symbols = 0;
	int32_t rgi_tree_symbols[ TURTLEDEFLATE_NUM_LENLITDISTS + 1 ]; /* + 1 ? */
	int32_t rgi_tree_symbol_rep[ TURTLEDEFLATE_NUM_LENLITDISTS + 1 ];
	int32_t i_highest_lenlits2, i_highest_litlens = 29, i_highest_dists = 29, i_highest_codes, i_tree_size_bits = 0;
	int32_t i_idx, i_j, i_rle_repetition;
	int32_t rgi_code_counts[ 19 ];
	int32_t i_code_num_nodes;
	int32_t i_code_root_node; /* unused */
	turtledeflate_symbol_code_t rgs_code_symbols[ 19 ];
	turtledeflate_tree_node_t rgs_code_nodes[ 19 * 2 + 1 ]; /* + 1 ? not - 1 ? */
	int32_t i_size_in_bits;

	static const int32_t rgi_order[ 19 ] = {
	  16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15
	};

	for( i_idx = 0; i_idx < 19; i_idx++ )
	{
		rgi_code_counts[ i_idx ] = 0;
	}

	while( i_highest_litlens > 0 )
	{
		if( ps_lenlit_symbols[ 257 + i_highest_litlens - 1 ].ui_code_length != 0 )
		{
			break;
		}
		i_highest_litlens--;
	}
	while( i_highest_dists > 0 )
	{
		if( ps_dist_symbols[ 1 + i_highest_dists - 1 ].ui_code_length != 0 )
		{
			break;
		}
		i_highest_dists--;
	}
	i_highest_lenlits2 = i_highest_litlens + 257;

	i_total_litlendists = i_highest_lenlits2 + i_highest_dists + 1;

	for( i_idx = 0; i_idx < i_total_litlendists; i_idx++ )
	{
		int32_t i_code_length_symbol = i_idx < i_highest_lenlits2 ? ps_lenlit_symbols[ i_idx ].ui_code_length : ps_dist_symbols[ i_idx - i_highest_lenlits2 ].ui_code_length;
		int32_t i_repetition = 1;
		if( ( i_flags & TURTLEDEFLATE_ENCODE_TREE_FLAGS_USE_16 ) ||
			( i_code_length_symbol == 0 && ( i_flags & ( TURTLEDEFLATE_ENCODE_TREE_FLAGS_USE_17 | TURTLEDEFLATE_ENCODE_TREE_FLAGS_USE_18 ) ) ) )
		{
			for( i_j = i_idx + 1; i_j < i_total_litlendists && i_code_length_symbol == ( i_j < i_highest_lenlits2 ? ps_lenlit_symbols[ i_j ].ui_code_length : ps_dist_symbols[ i_j - i_highest_lenlits2 ].ui_code_length ); i_j++ )
			{
				i_repetition++;
			}
		}
		i_idx += i_repetition - 1;

		if( i_code_length_symbol == 0 && i_repetition >= 3 )
		{
			if( i_flags & TURTLEDEFLATE_ENCODE_TREE_FLAGS_USE_18 )
			{
				while( i_repetition >= 11 )
				{
					i_rle_repetition = i_repetition > 138 ? 138 : i_repetition;
					rgi_tree_symbols[ i_num_tree_symbols ] = 18;
					rgi_tree_symbol_rep[ i_num_tree_symbols++ ] = i_rle_repetition - 11;
					rgi_code_counts[ 18 ]++;
					i_repetition -= i_rle_repetition;
				}
			}
			if( i_flags & TURTLEDEFLATE_ENCODE_TREE_FLAGS_USE_17 )
			{
				while( i_repetition >= 3 )
				{
					i_rle_repetition = i_repetition > 10 ? 10 : i_repetition;
					rgi_tree_symbols[ i_num_tree_symbols ] = 17;
					rgi_tree_symbol_rep[ i_num_tree_symbols++ ] = i_rle_repetition - 3;
					rgi_code_counts[ 17 ]++;
					i_repetition -= i_rle_repetition;
				}
			}
		}
		if( ( i_flags & TURTLEDEFLATE_ENCODE_TREE_FLAGS_USE_16 ) && i_repetition >= 4 )
		{
			i_repetition--;
			rgi_code_counts[ i_code_length_symbol ]++;
			rgi_tree_symbols[ i_num_tree_symbols ] = i_code_length_symbol;
			rgi_tree_symbol_rep[ i_num_tree_symbols++ ] = 0;
			while( i_repetition >= 3 )
			{
				i_rle_repetition = i_repetition > 6 ? 6 : i_repetition;
				rgi_tree_symbols[ i_num_tree_symbols ] = 16;
				rgi_tree_symbol_rep[ i_num_tree_symbols++ ] = i_rle_repetition - 3;
				rgi_code_counts[ 16 ]++;
				i_repetition -= i_rle_repetition;
			}
		}
		rgi_code_counts[ i_code_length_symbol ] += i_repetition;
		while( i_repetition > 0 )
		{
			rgi_tree_symbols[ i_num_tree_symbols ] = i_code_length_symbol;
			rgi_tree_symbol_rep[ i_num_tree_symbols++ ] = 0;
			i_repetition--;
		}
	}

	turtledeflate_build_tree( ps_turtle, rgi_code_counts, 19, rgs_code_nodes, 19 * 2 + 1, &i_code_num_nodes, &i_code_root_node );
	turtledeflate_get_symbol_lengths( ps_turtle, &rgs_code_nodes[ 0 ], i_code_num_nodes, rgs_code_symbols, 19, TURTLEDEFLATE_MAX_TREE_CODEWORD_BITS );
	turtledeflate_symbol_length_to_code( ps_turtle, rgs_code_symbols, 19, TURTLEDEFLATE_MAX_TREE_CODEWORD_BITS );

	i_highest_codes = 15;
	while( i_highest_codes > 0 )
	{
		if( rgi_code_counts[ rgi_order[ i_highest_codes + 4 - 1 ] ] != 0 )
		{
			break;
		}
		i_highest_codes--;
	}

	if( ps_bitstream )
	{
		turtledeflate_bitstream_write( ps_bitstream, i_highest_litlens, 5 );
		turtledeflate_bitstream_write( ps_bitstream, i_highest_dists, 5 );
		turtledeflate_bitstream_write( ps_bitstream, i_highest_codes, 4 );

		for( i_idx = 0; i_idx < i_highest_codes + 4; i_idx++ )
		{
			turtledeflate_bitstream_write( ps_bitstream, rgs_code_symbols[ rgi_order[ i_idx ] ].ui_code_length, 3 );
		}

		for( i_idx = 0; i_idx < i_num_tree_symbols; i_idx++ )
		{
			uint32_t ui_symbol = rgs_code_symbols[ rgi_tree_symbols[ i_idx ] ].ui_code;
			turtledeflate_bitstream_writehuff( ps_bitstream, ui_symbol, rgs_code_symbols[ rgi_tree_symbols[ i_idx ] ].ui_code_length );
			
			if( rgi_tree_symbols[ i_idx ] == 16 )
			{
				turtledeflate_bitstream_write( ps_bitstream, rgi_tree_symbol_rep[ i_idx ], 2 );
			}
			else if( rgi_tree_symbols[ i_idx ] == 17 )
			{
				turtledeflate_bitstream_write( ps_bitstream, rgi_tree_symbol_rep[ i_idx ], 3 );
			}
			else if( rgi_tree_symbols[ i_idx ] == 18 )
			{
				turtledeflate_bitstream_write( ps_bitstream, rgi_tree_symbol_rep[ i_idx ], 7 );
			}
		}
	}

	i_size_in_bits = 14;
	i_size_in_bits += ( i_highest_codes + 4 ) * 3;
	for( i_idx = 0; i_idx < 19; i_idx++ )
	{
		i_size_in_bits += rgs_code_symbols[ i_idx ].ui_code_length * rgi_code_counts[ i_idx ];
	}
	i_size_in_bits += rgi_code_counts[ 16 ] * 2;
	i_size_in_bits += rgi_code_counts[ 17 ] * 3;
	i_size_in_bits += rgi_code_counts[ 18 ] * 7;

	return i_size_in_bits;
}


