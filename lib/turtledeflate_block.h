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


void turtledeflate_block_reset( turtledeflate_block_t *ps_block );
void turtledeflate_block_append_block_to_global( turtledeflate_ctx_t *ps_turtle, turtledeflate_block_t *ps_block, int32_t rgi_deflate_block_boundaries[ ] );

void turtledeflate_block_deflate_greedy( turtledeflate_ctx_t *ps_turtle, turtledeflate_block_t *ps_block, uint8_t *pui8_data, int32_t i_data_size, int32_t i_pre );
int32_t turtledeflate_block_deflate_squish( turtledeflate_ctx_t * ps_turtle, turtledeflate_block_t * ps_block_, uint8_t * pui8_data, int32_t i_data_size, int32_t i_pre, int32_t *pi_chosen_start_fp );

void turtledeflate_create_global_histogram( turtledeflate_ctx_t *ps_turtle );
void turtledeflate_get_partial_histogram( turtledeflate_ctx_t *ps_turtle, int32_t i_from, int32_t i_to, turtledeflate_hist_t *ps_hist );
int32_t turtledeflate_get_histogram_bit_cost( turtledeflate_ctx_t *ps_turtle, turtledeflate_hist_t *ps_hist );
int32_t turtledeflate_get_global_sub_block_bit_cost( turtledeflate_ctx_t *ps_turtle, int32_t i_from, int32_t i_to );
int32_t turtledeflate_block_splitting_bits( turtledeflate_ctx_t *ps_turtle, int32_t rgi_block_boundaries_deflated[ ], int32_t i_num_sub_blocks );

int32_t turtledeflate_block_splitting( turtledeflate_ctx_t *ps_turtle, int32_t rgi_block_boundaries_deflated[ ], int32_t i_num_sub_blocks );
void turtledeflate_block_boundaries_inverse( turtledeflate_ctx_t *ps_turtle, int32_t rgi_block_boundaries_deflated[ ], int32_t rgi_block_boundaries[ ], int32_t i_num_blocks );
void turtledeflate_block_write( turtledeflate_ctx_t *ps_turtle, int32_t i_global_from, int32_t i_global_to, bool b_last );


