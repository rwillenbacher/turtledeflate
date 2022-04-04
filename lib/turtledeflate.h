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
#include <memory.h>
#include <assert.h>
#include <math.h> /* eww */

#include "turtledeflate_api.h"


#define TURTLEDEFLATE_NUM_LENLITS          ( 256 + 1 + 31 )
#define TURTLEDEFLATE_NUM_DISTS            32
#define TURTLEDEFLATE_NUM_LENLITDISTS     (TURTLEDEFLATE_NUM_LENLITS+TURTLEDEFLATE_NUM_DISTS)
#define TURTLEDEFLATE_MAX_UINT     0xffffffff

#define TURTLEDEFLATE_MIN_MATCH_LEN    3
#define TURTLEDEFLATE_MAX_MATCH_LEN  258

#define TURTLEDEFLATE_WINDOW_SIZE ( 1 << 15 )

#define TURTLEDEFLATE_MAX_CODEWORD_BITS       15
#define TURTLEDEFLATE_MAX_TREE_CODEWORD_BITS   7
#define HEVCENCODE_BLMDL_REP36     ( TURTLEDEFLATE_MAX_CODEWORD_BITS + 1 )
#define HEVCENCODE_BLMDL_REPZ310   ( TURTLEDEFLATE_MAX_CODEWORD_BITS + 2 )
#define HEVCENCODE_BLMDL_REPZ11138 ( TURTLEDEFLATE_MAX_CODEWORD_BITS + 3 )
#define HEVCENCODE_BLMDL_ENTRIES   ( TURTLEDEFLATE_MAX_CODEWORD_BITS + 1 + 3 )

#define TURTLE_MIN( x, y ) ( (x) < (y) ? (x) : (y) )
#define TURTLE_MAX( x, y ) ( (x) > (y) ? (x) : (y) )

typedef double float64_t;

typedef struct
{
	uint8_t *pui8_bitstream_start;
	uint8_t *pui8_codeword_ptr;
	uint64_t ui64_codeword;
	int32_t i_codeword_fill;
} turtledeflate_bitstream_t;


typedef struct
{
	uint16_t ui16_parent;
	uint16_t ui16_left;
	uint16_t ui16_right;
	uint16_t ui16_entry;
	uint32_t ui_weight;
	uint8_t ui8_code_length;
	uint32_t ui_order;
} turtledeflate_tree_node_t;

typedef struct
{
	uint32_t ui_code;
	uint32_t ui_code_length;
	uint32_t ui_order;
} turtledeflate_symbol_code_t;


#define TURTLEDEFLATE_HIST_INTERVAL 256
typedef struct
{
	int32_t rgi_lenlit_hist[ TURTLEDEFLATE_NUM_LENLITS ];
	int32_t rgi_dist_hist[ TURTLEDEFLATE_NUM_DISTS ];
} turtledeflate_hist_t;

typedef struct
{
#define TURTLEDEFLATE_MAX_HASH_ENTRIES ( TURTLEDEFLATE_WINDOW_SIZE * 2 )
#define TURTLEDEFLATE_HASH_BITS  ( TURTLEDEFLATE_MIN_MATCH_LEN * 4 )
#define TURTLEDEFLATE_HASH_MASK  ( ( 1 << TURTLEDEFLATE_HASH_BITS ) - 1 )
#define TURTLEDEFLATE_HASH_SHIFT ( ( TURTLEDEFLATE_HASH_BITS + TURTLEDEFLATE_MIN_MATCH_LEN - 1 ) / TURTLEDEFLATE_MIN_MATCH_LEN )
#define TURTLEDEFLATE_HASH_END   0
	uint32_t ui_hash_shift;
	uint16_t ui16_running_hash;
	uint32_t rgui_prev_entries[ TURTLEDEFLATE_MAX_HASH_ENTRIES ];
	uint32_t rgui_hash_head[ 1 << TURTLEDEFLATE_HASH_BITS ];
} turtledeflate_hash_table_t;

typedef struct
{
	uint16_t *pui16_len;
	uint16_t *pui16_dist;
	uint32_t *pui_sublen_idx;
	uint16_t *pui16_sublen;
} turtledeflate_matches_t;


#define TURTLEDEFLATE_UNUSED_SQUISH_ENTRY  ( ( ( int64_t ) 1 ) << 62 )
typedef struct
{
	int64_t i64_cost;
	uint16_t ui16_litlen;
	uint16_t ui16_dist;
} turtledeflate_squish_entry_t;


typedef struct
{
	uint16_t ui16_litlen;
	uint16_t ui16_dist;
} turtledeflate_stream_entry_t;

typedef struct
{
	int32_t i_num_entries;

	int32_t rgi_count_lenlits[ TURTLEDEFLATE_NUM_LENLITS ];
	int32_t rgi_count_dists[ TURTLEDEFLATE_NUM_DISTS ];

	int64_t rgfp64_lenlits_bits[ TURTLEDEFLATE_NUM_LENLITS ];
	int64_t rgfp64_dists_bits[ TURTLEDEFLATE_NUM_DISTS ];
	int64_t rgfp64_extra_bits[ 64 ];

	int32_t rgi_lenlits_bits[ TURTLEDEFLATE_NUM_LENLITS ];
	int32_t rgi_dists_bits[ TURTLEDEFLATE_NUM_DISTS ];

	turtledeflate_symbol_code_t rgs_lenlit_symbols[ TURTLEDEFLATE_NUM_LENLITS ];
	turtledeflate_symbol_code_t rgs_dist_symbols[ TURTLEDEFLATE_NUM_DISTS ];
} turtledeflate_block_t;


typedef struct
{
	bool b_active;
	int32_t i_idx;
	int32_t i_best_bits;
	int32_t i_model_bits;
	float64_t f64_fp_start;
	float64_t f64_current_fp;
	float64_t f64_min_pre_exit_fp;
	float64_t f64_last_best_fp;
	turtledeflate_block_t s_block_model;
} turtledeflate_squish_path_ctx_t;

typedef struct
{
	turtledeflate_squish_entry_t *rgps_trellis[ 2 ];
	turtledeflate_squish_entry_t **pps_trace;

	turtledeflate_squish_path_ctx_t rgs_path_ctx[ TURTLEDEFLATE_MAX_NUM_FP_START ];
} turtledeflate_squish_t;


typedef struct
{
	turtledeflate_config_t s_config;

	int32_t i_blocksplitter_state;
	int32_t i_block_pre_size;
	uint8_t *pui8_block_pre;

	int32_t i_hist_max;
	turtledeflate_hist_t *ps_global_hist;

	turtledeflate_hash_table_t s_hash;
	turtledeflate_matches_t s_matches;

	turtledeflate_block_t s_global_block;
	turtledeflate_stream_entry_t *ps_global_stream;
	turtledeflate_stream_entry_t *ps_block_stream;
	turtledeflate_stream_entry_t *ps_temp_stream;

	int32_t i_final_compressed_bits;
	int32_t i_final_sub_block_count;
	turtledeflate_stream_entry_t *ps_final_stream;
	int32_t rgi_final_block_boundaries_deflated[ TURTLEDEFLATE_MAX_SUBBLOCKS + 1 ];


	turtledeflate_squish_t s_squish;

	turtledeflate_bitstream_t s_bitstream;
	int32_t i_compressed_block_size;
	uint8_t *pui8_compressed_block;
} turtledeflate_ctx_t;



#include "turtledeflate_bitstream.h"
#include "turtledeflate_block.h"
#include "turtledeflate_tree.h"


