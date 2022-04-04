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


#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>



typedef struct
{
	int32_t i_compression_level;                        /* unused */
#define TURTLEDEFLATE_MIN_BLOCK_SIZE 512
	int32_t i_maximum_block_size;                       /* 1024 * 512 ? 1024 * 1024 ? */
#define TURTLEDEFLATE_MIN_SUBBLOCKS    1
#define TURTLEDEFLATE_MAX_SUBBLOCKS  512
	int32_t i_maximum_subblocks;                        /* just set to max ? */

	int32_t i_max_block_splitter_iterations;            /* i suggest at least 15 */
	int32_t i_max_internal_block_splitter_iterations;   /* i suggest at least 50, just to be sure */
#define TURTLEDEFLATE_BSPLIT_MAX_NUM_POINTS      1023
#define TURTLEDEFLATE_BSPLIT_NUM_POINTS_DEFAULT    31    /* hm, ok */
	int32_t i_block_splitter_num_points;
#define TURTLEDEFLATE_BSPLIT_CENTER_DIST_DEFAULT  ( ( TURTLEDEFLATE_BSPLIT_NUM_POINTS_DEFAULT / 4 ) + 1 )
	int32_t i_block_splitter_center_dist;
#define TURTLEDEFLATE_MIN_RANGE_FOR_POINTS_DEFAULT ( 1 * 1024 )
	int32_t i_block_splitter_min_range_for_points;
	bool b_block_splitter_push_split;

	int32_t i_min_start_fp;                             /* -6 is good */
	int32_t i_max_start_fp;                             /*  4 is good */
#define TURTLEDEFLATE_MAX_NUM_FP_START 128
	/* for 'resons' this may not be more than half of TURTLEDEFLATE_MAX_NUM_FP_START, sorry */
	int32_t i_num_start_fp;                             /* need at least 3, might i suggested ( i_max_start_fp - i_min_start_fp + 1 ) ? */

#define TURTLEDEFLATE_VERBOSE_NONE           0
#define TURTLEDEFLATE_VERBOSE_SUPERBLOCK     1
#define TURTLEDEFLATE_VERBOSE_BLOCKSPLITTER  2
#define TURTLEDEFLATE_VERBOSE_BLOCK          3
#define TURTLEDEFLATE_VERBOSE_STARTFP        4
#define TURTLEDEFLATE_VERBOSE_SQUISHITER     5
	int32_t i_verbose;                                  /* watch it do stuff */
} turtledeflate_config_t;


typedef struct
{
	int32_t i_num_blocks;
	int32_t i_min_block_size;
	int32_t i_max_block_size;
	int32_t i_average_block_size;
	int32_t i_block_limited;



	int32_t rgi_start_fp_chosen[ TURTLEDEFLATE_MAX_NUM_FP_START ];
	int32_t rgi_best_from_start_fp[ TURTLEDEFLATE_MAX_NUM_FP_START ];
} turtledeflate_superblock_stats_t;


bool turtledeflate_create( void **p_turtle, turtledeflate_config_t *ps_config );

int32_t turtledeflate_block( void *p_turtle, int32_t i_size, uint8_t *pui8_block_, uint8_t **pui8_out_block, turtledeflate_superblock_stats_t *ps_stats, bool b_last );

void turtledeflate_destroy( void *p_turtle );
