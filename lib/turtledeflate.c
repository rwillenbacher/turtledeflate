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

bool turtledeflate_create( void **p_turtle, turtledeflate_config_t *ps_config )
{
	turtledeflate_ctx_t *ps_turtle;

	ps_turtle = malloc( sizeof( turtledeflate_ctx_t ) );

	memset( ps_turtle, 0, sizeof( turtledeflate_ctx_t ) );

	ps_turtle->s_config = *ps_config;
	ps_turtle->i_block_pre_size = 0;
	ps_turtle->pui8_block_pre = malloc( ( ps_config->i_maximum_block_size + TURTLEDEFLATE_WINDOW_SIZE ) * sizeof( uint8_t ) );
	ps_turtle->pui8_compressed_block = malloc( ( ( ( ps_config->i_maximum_block_size * 3 ) / 2 ) + 512 ) * sizeof( uint8_t ) );

	ps_turtle->ps_global_stream = malloc( sizeof( turtledeflate_stream_entry_t ) * ps_config->i_maximum_block_size );
	ps_turtle->ps_block_stream = malloc( sizeof( turtledeflate_stream_entry_t ) * ps_config->i_maximum_block_size );
	ps_turtle->ps_temp_stream = malloc( sizeof( turtledeflate_stream_entry_t ) * ps_config->i_maximum_block_size );
	ps_turtle->ps_final_stream = malloc( sizeof( turtledeflate_stream_entry_t ) * ps_config->i_maximum_block_size );

	ps_turtle->i_hist_max = ( ps_config->i_maximum_block_size + TURTLEDEFLATE_HIST_INTERVAL ) / TURTLEDEFLATE_HIST_INTERVAL;
	ps_turtle->ps_global_hist = malloc( sizeof( turtledeflate_hist_t ) * ps_turtle->i_hist_max );

	ps_turtle->s_matches.pui16_len = malloc( ( ps_config->i_maximum_block_size + 1 ) * sizeof( uint16_t ) );
	ps_turtle->s_matches.pui16_dist = malloc( ( ps_config->i_maximum_block_size + 1 ) * sizeof( uint16_t ) );
	ps_turtle->s_matches.pui16_sublen = malloc( ( ( ( ps_config->i_maximum_block_size + 1 ) * ( TURTLEDEFLATE_MAX_MATCH_LEN + 1 ) ) ) * sizeof( uint16_t ) );
	ps_turtle->s_matches.pui_sublen_idx = malloc( ( ps_config->i_maximum_block_size + 1 ) * sizeof( uint32_t ) ); 

	ps_turtle->s_squish.rgps_trellis[ 0 ] = malloc( sizeof( turtledeflate_squish_entry_t ) * ( ps_config->i_maximum_block_size + 1 ) );
	ps_turtle->s_squish.rgps_trellis[ 1 ] = malloc( sizeof( turtledeflate_squish_entry_t ) * ( ps_config->i_maximum_block_size + 1 ) );
	ps_turtle->s_squish.pps_trace = malloc( sizeof( turtledeflate_squish_entry_t * ) * ( ps_config->i_maximum_block_size + 1 ) );

	turtledeflate_bitstream_reset( &ps_turtle->s_bitstream, ps_turtle->pui8_compressed_block );

	*p_turtle = ( void * ) ps_turtle;

	return true;
}

static int32_t i_sb_ctr = 0;

#define TDF_THIS_SB_ONLY -1

int32_t turtledeflate_block( void *p_turtle, int32_t i_size, uint8_t *pui8_block_, uint8_t **pui8_out_block, turtledeflate_superblock_stats_t *ps_stats, bool b_last )
{
	bool b_might_do_another_subblock_iter, b_need_final, b_skip_sb, b_different_num_sub_blocks, b_different_boundaries;
	int32_t i_block_split_iter, i_idx, i_block_splitter_estimated_bits, i_squished_estimated_bits;
	turtledeflate_ctx_t *ps_turtle = ( turtledeflate_ctx_t * ) p_turtle;
	uint8_t *pui8_block = ps_turtle->pui8_block_pre + ps_turtle->i_block_pre_size;
	int32_t i_num_sub_blocks, i_num_sub_blocks_new, i_sub_block, i_sub_block_size, i_pre, i_pre_discard, i_compressed_size;
	int32_t rgi_block_boundaries[ TURTLEDEFLATE_MAX_SUBBLOCKS + 1 ];
	int32_t rgi_block_boundaries_new[ TURTLEDEFLATE_MAX_SUBBLOCKS + 1 ];
	int32_t rgi_block_boundaries_deflated[ TURTLEDEFLATE_MAX_SUBBLOCKS + 1 ];
	turtledeflate_block_t s_block;

	int32_t rgi_start_fp[ TURTLEDEFLATE_MAX_NUM_FP_START ];
	int32_t rgi_best_from_start_fp[ TURTLEDEFLATE_MAX_NUM_FP_START ];

	ps_turtle->i_blocksplitter_state = 0;

	
	memcpy( pui8_block, pui8_block_, i_size * sizeof( uint8_t ) ); /* append to pre */

	/* initial block splitting -> no blocks */
	rgi_block_boundaries[ 0 ] = 0;
	rgi_block_boundaries[ 1 ] = i_size;
	i_num_sub_blocks = 1;

	for( i_idx = 0; i_idx < TURTLEDEFLATE_MAX_NUM_FP_START; i_idx++ )
	{
		rgi_start_fp[ i_idx ] = 0;
		rgi_best_from_start_fp[ i_idx ] = 0;
	}

	b_need_final = true;
	b_skip_sb = false;
	ps_turtle->i_final_compressed_bits = i_size * 8; /* set to uncompressed */

	i_sb_ctr++;
	if( TDF_THIS_SB_ONLY >= 0 && i_sb_ctr != TDF_THIS_SB_ONLY )
	{
		b_skip_sb = true;

		ps_turtle->i_final_sub_block_count = 1;
		ps_turtle->rgi_final_block_boundaries_deflated[ 0 ] = 0;
		ps_turtle->rgi_final_block_boundaries_deflated[ 1 ] = 1;
		ps_turtle->ps_final_stream[ 0 ].ui16_litlen = 0;
		ps_turtle->ps_final_stream[ 0 ].ui16_dist = 0;
	}

	if( !b_skip_sb )
	{
		for( i_block_split_iter = 0; i_block_split_iter < ps_turtle->s_config.i_max_block_splitter_iterations; i_block_split_iter++ )
		{
			int32_t i_start_fp;

			/* block deflating */
			turtledeflate_block_reset( &ps_turtle->s_global_block );
			i_squished_estimated_bits = 0;
			for( i_sub_block = 0; i_sub_block < i_num_sub_blocks; i_sub_block++ )
			{
				int32_t i_sub_block_bits;
				i_pre = ps_turtle->i_block_pre_size + rgi_block_boundaries[ i_sub_block ];
				if( i_pre > TURTLEDEFLATE_WINDOW_SIZE )
				{
					i_pre = TURTLEDEFLATE_WINDOW_SIZE;
				}
				i_sub_block_size = rgi_block_boundaries[ i_sub_block + 1 ] - rgi_block_boundaries[ i_sub_block ];
				if( ps_turtle->s_config.i_verbose >= TURTLEDEFLATE_VERBOSE_BLOCK )
				{
					fprintf( stderr, "block %d [ %d - %d ] -> %d bits uncompressed....\n", i_sub_block, rgi_block_boundaries[ i_sub_block ], rgi_block_boundaries[ i_sub_block + 1 ], i_sub_block_size * 8 );
				}
				i_sub_block_bits = turtledeflate_block_deflate_squish( ps_turtle, &s_block, &pui8_block[ rgi_block_boundaries[ i_sub_block ] ], i_sub_block_size, i_pre, &i_start_fp );
				if( ps_turtle->s_config.i_verbose >= TURTLEDEFLATE_VERBOSE_BLOCK )
				{
					fprintf( stderr, "block %d -> %d bits compressed....\n", i_sub_block, i_sub_block_bits );
				}
				i_squished_estimated_bits += i_sub_block_bits;

				if( ps_turtle->i_blocksplitter_state < 2 )
				{
					rgi_best_from_start_fp[ i_start_fp ]++;
				}
				else
				{
					rgi_start_fp[ i_start_fp ]++;
				}
				
				

				turtledeflate_block_append_block_to_global( ps_turtle, &s_block, &rgi_block_boundaries_deflated[ i_sub_block ] );
			}

			/* create histogram */
			turtledeflate_create_global_histogram( ps_turtle );

			/* block splitting */
			if( ps_turtle->s_config.i_verbose >= TURTLEDEFLATE_VERBOSE_BLOCK )
			{
				fprintf( stderr, "sum of compressed block bits: %d\n", i_squished_estimated_bits );
			}

			i_squished_estimated_bits = turtledeflate_block_splitting_bits( ps_turtle, &rgi_block_boundaries_deflated[ 0 ], i_num_sub_blocks );
			if( 1 || ps_turtle->i_blocksplitter_state > 0 )
			{
				i_num_sub_blocks_new = turtledeflate_block_splitting( ps_turtle, &rgi_block_boundaries_deflated[ 0 ], i_num_sub_blocks );
				i_block_splitter_estimated_bits = turtledeflate_block_splitting_bits( ps_turtle, &rgi_block_boundaries_deflated[ 0 ], i_num_sub_blocks_new );
			}
			else
			{
				i_num_sub_blocks_new = i_num_sub_blocks;
				i_block_splitter_estimated_bits = i_squished_estimated_bits;
			}


			if( ps_turtle->s_config.i_verbose >= TURTLEDEFLATE_VERBOSE_BLOCKSPLITTER )
			{
				fprintf( stderr, "block spliter iteration %d (%d): block old: %d, new: %d, %d bits est. in, %d bits est. out. ( %d saved ? )\n", i_block_split_iter, ps_turtle->i_blocksplitter_state, i_num_sub_blocks, i_num_sub_blocks_new,
					i_squished_estimated_bits, i_block_splitter_estimated_bits, i_squished_estimated_bits - i_block_splitter_estimated_bits );
			}

			/* well, no, does not improve things */
			if( 0 && ( i_squished_estimated_bits - i_block_splitter_estimated_bits ) <= 0 && i_num_sub_blocks_new > 1 )
			{

				int32_t i_unstuck_block_splitter_estimated_bits, i_unstuck_num_sub_blocks_new;
				int32_t rgi_unstuck_block_boundaries_deflated[ TURTLEDEFLATE_MAX_SUBBLOCKS + 1 ];

				rgi_unstuck_block_boundaries_deflated[ 0 ] = 0;
				rgi_unstuck_block_boundaries_deflated[ 1 ] = ps_turtle->s_global_block.i_num_entries;
				i_unstuck_num_sub_blocks_new = turtledeflate_block_splitting( ps_turtle, &rgi_unstuck_block_boundaries_deflated[ 0 ], 1 );
				i_unstuck_block_splitter_estimated_bits = turtledeflate_block_splitting_bits( ps_turtle, &rgi_unstuck_block_boundaries_deflated[ 0 ], i_unstuck_num_sub_blocks_new );

				if( i_unstuck_block_splitter_estimated_bits < i_block_splitter_estimated_bits )
				{
					if( ps_turtle->s_config.i_verbose >= TURTLEDEFLATE_VERBOSE_BLOCKSPLITTER )
					{
						fprintf( stderr, "unstuck blocksplitter, %d -> %d, ( %d saved ? )\n", i_block_splitter_estimated_bits, i_unstuck_block_splitter_estimated_bits, i_block_splitter_estimated_bits - i_unstuck_block_splitter_estimated_bits );
					}
					i_block_splitter_estimated_bits = i_unstuck_block_splitter_estimated_bits;
					i_num_sub_blocks_new = i_unstuck_num_sub_blocks_new;
					memcpy( rgi_block_boundaries_deflated, rgi_unstuck_block_boundaries_deflated, sizeof( rgi_block_boundaries_deflated ) );
				}
				else
				{
					if( ps_turtle->s_config.i_verbose >= TURTLEDEFLATE_VERBOSE_BLOCKSPLITTER )
					{
						fprintf( stderr, "unsticking blocksplitter did not help: %d bits, ( %d above ? )\n", i_unstuck_block_splitter_estimated_bits, i_unstuck_block_splitter_estimated_bits - i_block_splitter_estimated_bits );
					}

				}
			}

			b_different_num_sub_blocks = false;
			b_different_boundaries = false;

			if( i_block_splitter_estimated_bits <= i_squished_estimated_bits )
			{
				turtledeflate_block_boundaries_inverse( ps_turtle, &rgi_block_boundaries_deflated[ 0 ], &rgi_block_boundaries_new[ 0 ], i_num_sub_blocks_new );

				/* compare blocks */
				if( i_num_sub_blocks != i_num_sub_blocks_new )
				{
					b_different_num_sub_blocks = true;
					b_different_boundaries = true;
				}
				else
				{
					for( i_idx = 0; i_idx < i_num_sub_blocks; i_idx++ )
					{
						if( rgi_block_boundaries[ i_idx ] != rgi_block_boundaries_new[ i_idx ] )
						{
							b_different_boundaries = true;
						}
					}
				}

				/* set new blocks */
				i_num_sub_blocks = i_num_sub_blocks_new;
				memcpy( rgi_block_boundaries, rgi_block_boundaries_new, sizeof( rgi_block_boundaries_new ) );
			}

			b_might_do_another_subblock_iter = true;
			if( b_need_final || i_block_splitter_estimated_bits < ps_turtle->i_final_compressed_bits )
			{
				int32_t i_last_final_bits = ps_turtle->i_final_compressed_bits;
				ps_turtle->i_final_compressed_bits = i_block_splitter_estimated_bits;
				ps_turtle->i_final_sub_block_count = i_num_sub_blocks_new;
				memcpy( ps_turtle->ps_final_stream, ps_turtle->ps_global_stream, ps_turtle->s_global_block.i_num_entries * sizeof( turtledeflate_stream_entry_t ) );
				memcpy( ps_turtle->rgi_final_block_boundaries_deflated, rgi_block_boundaries_deflated, sizeof( rgi_block_boundaries_deflated ) );
				if( ps_turtle->s_config.i_verbose >= TURTLEDEFLATE_VERBOSE_SUPERBLOCK )
				{
					fprintf( stderr, "New Final: %d compressed size ( %d saved ? )\n", ps_turtle->i_final_compressed_bits, i_last_final_bits - ps_turtle->i_final_compressed_bits );
				}
				b_need_final = false;

				if( ps_turtle->i_blocksplitter_state < 2 )
				{
					if( !b_different_num_sub_blocks )
					{
						ps_turtle->i_blocksplitter_state++;
					}
				}
				else if( ps_turtle->i_blocksplitter_state == 2 )
				{
					if( !b_different_boundaries )
					{
						ps_turtle->i_blocksplitter_state++;
					}
				}
				else
				{
					if( !b_different_boundaries )
					{
						b_might_do_another_subblock_iter = false;
					}
				}
			}
			else /* results got worse, copy back best previous result and increase state */
			{
				if( ps_turtle->s_config.i_verbose >= TURTLEDEFLATE_VERBOSE_SUPERBLOCK )
				{
					fprintf( stderr, "Iteration did not yield a better result, %d bits above previous\n", i_block_splitter_estimated_bits - ps_turtle->i_final_compressed_bits );
				}
				i_num_sub_blocks = ps_turtle->i_final_sub_block_count;
				ps_turtle->s_global_block.i_num_entries = ps_turtle->rgi_final_block_boundaries_deflated[ i_num_sub_blocks ];
				memcpy( ps_turtle->ps_global_stream, ps_turtle->ps_final_stream, ps_turtle->s_global_block.i_num_entries * sizeof( turtledeflate_stream_entry_t ) );
				memcpy( rgi_block_boundaries_deflated, ps_turtle->rgi_final_block_boundaries_deflated, sizeof( rgi_block_boundaries_deflated ) );
				turtledeflate_block_boundaries_inverse( ps_turtle, &rgi_block_boundaries_deflated[ 0 ], &rgi_block_boundaries[ 0 ], i_num_sub_blocks );

				ps_turtle->i_blocksplitter_state++; /* better luck in the future */

				b_might_do_another_subblock_iter = ps_turtle->i_blocksplitter_state < 4;
			}

			if( !b_might_do_another_subblock_iter )
			{
				break;
			}
		}
	}

	/* move sliding window */
	ps_turtle->i_block_pre_size += i_size;
	i_pre_discard = ps_turtle->i_block_pre_size - TURTLEDEFLATE_WINDOW_SIZE;
	if( i_pre_discard > 0 )
	{
		memmove( ps_turtle->pui8_block_pre, ps_turtle->pui8_block_pre + i_pre_discard, TURTLEDEFLATE_WINDOW_SIZE * sizeof( uint8_t ) );
		ps_turtle->i_block_pre_size -= i_pre_discard;
	}

	/* write block(s) */
	turtledeflate_bitstream_advance( &ps_turtle->s_bitstream ); /* resets current bitstream pointer */
	for( i_sub_block = 0; i_sub_block < ps_turtle->i_final_sub_block_count; i_sub_block++ )
	{
		bool b_final_block = b_last && ( ( i_sub_block + 1 ) == ps_turtle->i_final_sub_block_count );
		turtledeflate_block_write( ps_turtle, ps_turtle->rgi_final_block_boundaries_deflated[ i_sub_block ], ps_turtle->rgi_final_block_boundaries_deflated[ i_sub_block + 1 ], b_final_block );
	}

	if( b_last )
	{
		turtledeflate_bitstream_bytealign( &ps_turtle->s_bitstream );
	}
	*pui8_out_block = ps_turtle->s_bitstream.pui8_bitstream_start;
	i_compressed_size = turtledeflate_bitstream_bytesize( &ps_turtle->s_bitstream );


	if( ps_stats )
	{
		int32_t i_block_size, i_average_block_size;
		ps_stats->i_num_blocks = i_num_sub_blocks;
		ps_stats->i_block_limited = i_num_sub_blocks == ps_turtle->s_config.i_maximum_subblocks ? 1 : 0;

		ps_stats->i_min_block_size = i_size;
		ps_stats->i_max_block_size = 0;

		i_average_block_size = 0;
		for( i_sub_block = 0; i_sub_block < i_num_sub_blocks; i_sub_block++ )
		{
			i_block_size = rgi_block_boundaries[ i_sub_block + 1 ] - rgi_block_boundaries[ i_sub_block ];
			if( i_block_size < ps_stats->i_min_block_size )
			{
				ps_stats->i_min_block_size = i_block_size;
			}
			if( i_block_size > ps_stats->i_max_block_size )
			{
				ps_stats->i_max_block_size = i_block_size;
			}

			i_average_block_size += i_block_size;
		}

		i_average_block_size /= i_num_sub_blocks;
		ps_stats->i_average_block_size = i_average_block_size;

		for( i_idx = 0; i_idx < ps_turtle->s_config.i_num_start_fp; i_idx++ )
		{
			ps_stats->rgi_start_fp_chosen[ i_idx ] = rgi_start_fp[ i_idx ];
			ps_stats->rgi_best_from_start_fp[ i_idx ] = rgi_best_from_start_fp[ i_idx ];
		}
	}


	return i_compressed_size;
}


void turtledeflate_destroy( void *p_turtle )
{
	/* free em ! */
}
