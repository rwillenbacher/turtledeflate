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


void turtledeflate_build_tree( turtledeflate_ctx_t *ps_turtle, int32_t *pi_symbol_counts, int32_t i_num_symbols, turtledeflate_tree_node_t *ps_tree_nodes, int32_t i_max_tree_nodes, int32_t *pi_num_nodes, int32_t *pi_root_node );
void turtledeflate_get_symbol_lengths( turtledeflate_ctx_t *ps_turtle, turtledeflate_tree_node_t *ps_tree_nodes, int32_t i_num_nodes, turtledeflate_symbol_code_t *ps_symbol_codes, int32_t i_num_symbol_codes, int32_t i_max_codeword_length );
void turtledeflate_symbol_length_to_code( turtledeflate_ctx_t *ps_turtle, turtledeflate_symbol_code_t *ps_symbol_codes, int32_t i_num_symbol_codes, int32_t i_max_codeword_length );
int32_t turtledeflate_encode_tree( turtledeflate_ctx_t *ps_turtle, turtledeflate_symbol_code_t *ps_lenlit_symbols, turtledeflate_symbol_code_t *ps_dist_symbols, turtledeflate_bitstream_t *ps_bitstream, int32_t i_flags );

