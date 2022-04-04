/* OptimizeHuffmanForRle() function is from Google Zopfli: */
/*
Copyright 2011 Google Inc. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

Author: lode.vandevenne@gmail.com (Lode Vandevenne)
Author: jyrki.alakuijala@gmail.com (Jyrki Alakuijala)
*/


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

static const uint16_t rgui16_len_symbol_table[ 259 ] = {
    0, 0, 0,
    257, 258, 259, 260, 261, 262, 263, 264,
    265, 265, 266, 266, 267, 267, 268, 268,
    269, 269, 269, 269, 270, 270, 270, 270,
    271, 271, 271, 271, 272, 272, 272, 272,
    273, 273, 273, 273, 273, 273, 273, 273,
    274, 274, 274, 274, 274, 274, 274, 274,
    275, 275, 275, 275, 275, 275, 275, 275,
    276, 276, 276, 276, 276, 276, 276, 276,
    277, 277, 277, 277, 277, 277, 277, 277,
    277, 277, 277, 277, 277, 277, 277, 277,
    278, 278, 278, 278, 278, 278, 278, 278,
    278, 278, 278, 278, 278, 278, 278, 278,
    279, 279, 279, 279, 279, 279, 279, 279,
    279, 279, 279, 279, 279, 279, 279, 279,
    280, 280, 280, 280, 280, 280, 280, 280,
    280, 280, 280, 280, 280, 280, 280, 280,
    281, 281, 281, 281, 281, 281, 281, 281,
    281, 281, 281, 281, 281, 281, 281, 281,
    281, 281, 281, 281, 281, 281, 281, 281,
    281, 281, 281, 281, 281, 281, 281, 281,
    282, 282, 282, 282, 282, 282, 282, 282,
    282, 282, 282, 282, 282, 282, 282, 282,
    282, 282, 282, 282, 282, 282, 282, 282,
    282, 282, 282, 282, 282, 282, 282, 282,
    283, 283, 283, 283, 283, 283, 283, 283,
    283, 283, 283, 283, 283, 283, 283, 283,
    283, 283, 283, 283, 283, 283, 283, 283,
    283, 283, 283, 283, 283, 283, 283, 283,
    284, 284, 284, 284, 284, 284, 284, 284,
    284, 284, 284, 284, 284, 284, 284, 284,
    284, 284, 284, 284, 284, 284, 284, 284,
    284, 284, 284, 284, 284, 284, 284, 285
};

static const uint8_t rgui8_len_extra_bits_table[ 29 ] = {
  0, 0, 0, 0, 0, 0, 0, 0,
  1, 1, 1, 1, 2, 2, 2, 2,
  3, 3, 3, 3, 4, 4, 4, 4,
  5, 5, 5, 5, 0
};

static const uint8_t rgui8_len_extra_value_table[ 259 ] = {
  0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 1, 0, 1, 0, 1, 0, 1,
  0, 1, 2, 3, 0, 1, 2, 3,
  0, 1, 2, 3, 0, 1, 2, 3,
  0, 1, 2, 3, 4, 5, 6, 7,
  0, 1, 2, 3, 4, 5, 6, 7,
  0, 1, 2, 3, 4, 5, 6, 7,
  0, 1, 2, 3, 4, 5, 6, 7,
  0, 1, 2, 3, 4, 5, 6, 7,
  8, 9, 10, 11, 12, 13, 14, 15,
  0, 1, 2, 3, 4, 5, 6, 7,
  8, 9, 10, 11, 12, 13, 14, 15,
  0, 1, 2, 3, 4, 5, 6, 7,
  8, 9, 10, 11, 12, 13, 14, 15,
  0, 1, 2, 3, 4, 5, 6, 7,
  8, 9, 10, 11, 12, 13, 14, 15,
  0, 1, 2, 3, 4, 5, 6, 7,
  8, 9, 10, 11, 12, 13, 14, 15,
  16, 17,  18, 19, 20, 21, 22, 23,
  24, 25, 26, 27, 28, 29, 30, 31,
  0, 1, 2, 3, 4, 5, 6, 7,
  8, 9, 10, 11, 12, 13, 14, 15,
  16, 17, 18, 19, 20, 21, 22, 23,
  24, 25, 26, 27, 28, 29, 30, 31,
  0, 1, 2, 3, 4, 5, 6, 7,
  8, 9, 10, 11, 12, 13, 14, 15,
  16, 17, 18, 19, 20, 21, 22, 23,
  24, 25, 26, 27, 28, 29, 30, 31,
  0, 1, 2, 3, 4, 5, 6, 7,
  8, 9, 10, 11, 12, 13, 14, 15,
  16, 17, 18, 19, 20, 21, 22, 23,
  24, 25, 26, 27, 28, 29, 30, 0
};

static const uint8_t rgui8_dist_extra_bits_table[ 32 ] = {
  0, 0, 0, 0, 1, 1, 2, 2,
  3, 3, 4, 4, 5, 5, 6, 6,
  7, 7, 8, 8, 9, 9, 10, 10,
  11, 11, 12, 12, 13, 13,
  0, 0
};

int32_t turtledeflate_get_len_symbol( int32_t i_len )
{
    return ( int32_t ) rgui16_len_symbol_table[ i_len ];
}

int32_t turtledeflate_get_len_extra_bits( int32_t i_lens )
{
    return ( int32_t ) rgui8_len_extra_bits_table[ i_lens - 257 ];
}

int32_t turtledeflate_get_len_extra_value( int32_t i_len )
{
    return ( int32_t ) rgui8_len_extra_value_table[ i_len ];
}

int32_t turtledeflate_get_dist_symbol( int32_t i_dist )
{
    if( i_dist < 193 )
    {
        if( i_dist < 13 )
        {
            if( i_dist < 5 ) return i_dist - 1;
            else if( i_dist < 7 ) return 4;
            else if( i_dist < 9 ) return 5;
            else return 6;
        }
        else
        {
            if( i_dist < 17 ) return 7;
            else if( i_dist < 25 ) return 8;
            else if( i_dist < 33 ) return 9;
            else if( i_dist < 49 ) return 10;
            else if( i_dist < 65 ) return 11;
            else if( i_dist < 97 ) return 12;
            else if( i_dist < 129 ) return 13;
            else return 14;
        }
    }
    else
    {
        if( i_dist < 2049 )
        {
            if( i_dist < 257 ) return 15;
            else if( i_dist < 385 ) return 16;
            else if( i_dist < 513 ) return 17;
            else if( i_dist < 769 ) return 18;
            else if( i_dist < 1025 ) return 19;
            else if( i_dist < 1537 ) return 20;
            else return 21;
        }
        else
        {
            if( i_dist < 3073 ) return 22;
            else if( i_dist < 4097 ) return 23;
            else if( i_dist < 6145 ) return 24;
            else if( i_dist < 8193 ) return 25;
            else if( i_dist < 12289 ) return 26;
            else if( i_dist < 16385 ) return 27;
            else if( i_dist < 24577 ) return 28;
            else return 29;
        }
    }
}

int32_t turtledeflate_get_dist_extra_bits( int32_t i_dists )
{
    return ( int32_t ) rgui8_dist_extra_bits_table[ i_dists ];
}

int32_t turtledeflate_get_dist_extra_value( int32_t i_dist )
{
    if( i_dist < 5 ) return 0;
    else if( i_dist < 9 ) return ( i_dist - 5 ) & 1;
    else if( i_dist < 17 ) return ( i_dist - 9 ) & 3;
    else if( i_dist < 33 ) return ( i_dist - 17 ) & 7;
    else if( i_dist < 65 ) return ( i_dist - 33 ) & 15;
    else if( i_dist < 129 ) return ( i_dist - 65 ) & 31;
    else if( i_dist < 257 ) return ( i_dist - 129 ) & 63;
    else if( i_dist < 513 ) return ( i_dist - 257 ) & 127;
    else if( i_dist < 1025 ) return ( i_dist - 513 ) & 255;
    else if( i_dist < 2049 ) return ( i_dist - 1025 ) & 511;
    else if( i_dist < 4097 ) return ( i_dist - 2049 ) & 1023;
    else if( i_dist < 8193 ) return ( i_dist - 4097 ) & 2047;
    else if( i_dist < 16385 ) return ( i_dist - 8193 ) & 4095;
    else return ( i_dist - 16385 ) & 8191;
}

/* stolen from Zopfli, see start of file */
void OptimizeHuffmanForRle( int length, int32_t *counts )
{

    int i, k, stride;
    int32_t symbol, sum, limit;
    int rgi_good_for_rle[ 512 ];

    /* 1) We don't want to touch the trailing zeros. We may break the
    rules of the format by adding more data in the distance codes. */
    for( ; length >= 0; --length )
    {
        if( length == 0 )
        {
            return;
        }
        if( counts[ length - 1 ] != 0 )
        {
/* Now counts[0..length - 1] does not have trailing zeros. */
            break;
        }
    }
    /* 2) Let's mark all population counts that already can be encoded
    with an rle code.*/
    for( i = 0; i < length; ++i ) rgi_good_for_rle[ i ] = 0;

    /* Let's not spoil any of the existing good rle codes.
    Mark any seq of 0's that is longer than 5 as a rgi_good_for_rle.
    Mark any seq of non-0's that is longer than 7 as a rgi_good_for_rle.*/
    symbol = counts[ 0 ];
    stride = 0;
    for( i = 0; i < length + 1; ++i )
    {
        if( i == length || counts[ i ] != symbol )
        {
            if( ( symbol == 0 && stride >= 5 ) || ( symbol != 0 && stride >= 7 ) )
            {
                for( k = 0; k < stride; ++k )
                {
                    rgi_good_for_rle[ i - k - 1 ] = 1;
                }
            }
            stride = 1;
            if( i != length )
            {
                symbol = counts[ i ];
            }
        }
        else
        {
            ++stride;
        }
    }

    /* 3) Let's replace those population counts that lead to more rle codes. */
    stride = 0;
    limit = counts[ 0 ];
    sum = 0;
    for( i = 0; i < length + 1; ++i )
    {
        if( i == length || rgi_good_for_rle[ i ]
            /* Heuristic for selecting the stride ranges to collapse. */
            || abs( counts[ i ] - limit ) >= 4 )
        {
            if( stride >= 4 || ( stride >= 3 && sum == 0 ) )
            {
/* The stride must end, collapse what we have, if we have enough (4). */
                int count = ( sum + stride / 2 ) / stride;
                if( count < 1 ) count = 1;
                if( sum == 0 )
                {
   /* Don't make an all zeros stride to be upgraded to ones. */
                    count = 0;
                }
                for( k = 0; k < stride; ++k )
                {
/* We don't want to change value at counts[i],
that is already belonging to the next stride. Thus - 1. */
                    counts[ i - k - 1 ] = count;
                }
            }
            stride = 0;
            sum = 0;
            if( i < length - 3 )
            {
/* All interesting strides have a count of at least 4,
at least when non-zeros. */
                limit = ( counts[ i ] + counts[ i + 1 ] +
                    counts[ i + 2 ] + counts[ i + 3 ] + 2 ) / 4;
            }
            else if( i < length )
            {
                limit = counts[ i ];
            }
            else
            {
                limit = 0;
            }
        }
        ++stride;
        if( i != length )
        {
            sum += counts[ i ];
        }
    }
}



void turtledeflate_init_hash( turtledeflate_hash_table_t *ps_hashtable, uint8_t *pui8_data, uint32_t ui_data_size )
{
    uint32_t ui_idx;
    uint32_t ui_num_init = TURTLEDEFLATE_MIN_MATCH_LEN - 1;

    if( ui_num_init > ui_data_size )
    {
        ui_num_init = ui_data_size; /* no matches anyway .. */
    }

    for( ui_idx = 0; ui_idx < ui_num_init; ui_idx++ )
    {
        ps_hashtable->ui16_running_hash = ( ( ps_hashtable->ui16_running_hash << TURTLEDEFLATE_HASH_SHIFT ) ^ pui8_data[ ui_idx ] ) & TURTLEDEFLATE_HASH_MASK;
    }

    for( ui_idx = 0; ui_idx < ( 1 << TURTLEDEFLATE_HASH_BITS ); ui_idx++ )
    {
        ps_hashtable->rgui_hash_head[ ui_idx ] = TURTLEDEFLATE_HASH_END;
    }

}


uint32_t turtledeflate_update_hash( turtledeflate_hash_table_t *ps_hash, uint8_t *pui8_data, uint32_t ui_current, uint32_t ui_current_size )
{
    uint32_t ui_entry;
    uint32_t ui_prev_hash_entry = 0;

    ui_entry = ui_current;
    while( ui_current_size-- )
    {
        ps_hash->ui16_running_hash = ( ( ps_hash->ui16_running_hash << TURTLEDEFLATE_HASH_SHIFT ) ^ pui8_data[ ui_current + TURTLEDEFLATE_MIN_MATCH_LEN - 1 ] ) & TURTLEDEFLATE_HASH_MASK;
        ui_prev_hash_entry = ps_hash->rgui_hash_head[ ps_hash->ui16_running_hash ];
        ps_hash->rgui_prev_entries[ ui_entry % TURTLEDEFLATE_MAX_HASH_ENTRIES ] = ui_prev_hash_entry;
        ps_hash->rgui_hash_head[ ps_hash->ui16_running_hash ] = ui_entry;
        ui_entry++;
        ui_current++;
    }
    return ui_prev_hash_entry;
}



bool turtledeflate_get_longest_match( turtledeflate_hash_table_t *ps_hash, uint8_t *pui8_data, uint32_t ui_hash_entry, uint32_t ui_current, uint32_t ui_block_size, uint16_t *pui16_match_length, uint16_t *pui16_match_offset, uint16_t *pui16_sublen )
{
    int32_t i_search_pos, i_match, i_best_length, i_sublen_fill, i_best_pos, i_idx;
    int32_t i_max_match_length;
    i_best_pos = 0;

    if( ui_current + TURTLEDEFLATE_MAX_MATCH_LEN > ui_block_size )
    {
        i_max_match_length = ui_block_size - ui_current;
    }
    else
    {
        i_max_match_length = TURTLEDEFLATE_MAX_MATCH_LEN;
    }

    i_best_length = TURTLEDEFLATE_MIN_MATCH_LEN - 1;
    i_sublen_fill = 0;

    if( ui_current == 300000 )
    {
        ui_current = ui_current;
    }

    while( ui_hash_entry != TURTLEDEFLATE_HASH_END )
    {
        i_search_pos = ui_hash_entry;
        if( ( ( int32_t ) ui_current ) - i_search_pos - 1 < 0 )
        {
            break;
        }
        if( ( ( int32_t ) ui_current ) - i_search_pos - 1 >= TURTLEDEFLATE_WINDOW_SIZE )
        {
            break;
        }
        ui_hash_entry = ps_hash->rgui_prev_entries[ ( ui_hash_entry + TURTLEDEFLATE_MAX_HASH_ENTRIES ) % TURTLEDEFLATE_MAX_HASH_ENTRIES ];

        if( pui8_data[ ( i_search_pos + i_best_length ) ] != pui8_data[ ui_current + i_best_length ] ||
            pui8_data[ ( i_search_pos + i_best_length - 1 ) ] != pui8_data[ ui_current + i_best_length - 1 ] ||
            pui8_data[ i_search_pos ] != pui8_data[ ui_current ] ||
            pui8_data[ i_search_pos + 1 ] != pui8_data[ ui_current + 1 ] )
        {
            continue;
        }

        i_match = 2;
        while( i_match < i_max_match_length &&
            pui8_data[ ( i_search_pos + i_match ) ] == pui8_data[ ui_current + i_match ] )
        {
            i_match++;
        }

        if( i_match >= TURTLEDEFLATE_MIN_MATCH_LEN && i_match > i_best_length )
        {
            i_best_pos = ( ( int32_t ) ui_current ) - i_search_pos;

            for( i_idx = i_sublen_fill; i_idx <= i_match; i_idx++ )
            {
                pui16_sublen[ i_idx ] = i_best_pos;
            }
            i_sublen_fill = i_match + 1;
            i_best_length = i_match;

            assert( i_search_pos + i_match < ( int32_t ) ui_block_size );
            assert( i_best_pos >= 0 );

            if( i_best_length == i_max_match_length )
            {
                break;
            }
        }

    }
    *pui16_match_offset = 0;
    *pui16_match_length = 0;
    if( i_best_length >= TURTLEDEFLATE_MIN_MATCH_LEN )
    {
        *pui16_match_offset = i_best_pos;
        *pui16_match_length = i_best_length;
        return true;
    }
    return false;
}


void turtledeflate_get_matches( turtledeflate_ctx_t *ps_turtle, uint8_t *pui8_data, int32_t i_data_start, int32_t i_data_end )
{
    uint32_t ui_sublen_idx = 0;
    int32_t i_idx, i_pos;
    int32_t i_hash_start = i_data_start - TURTLEDEFLATE_WINDOW_SIZE;

    if( i_hash_start < 0 )
    {
        i_hash_start = 0;
    }

    turtledeflate_init_hash( &ps_turtle->s_hash, pui8_data - i_hash_start, i_hash_start + i_data_end );

    turtledeflate_update_hash( &ps_turtle->s_hash, pui8_data, i_hash_start, i_data_start - i_hash_start );

    for( i_idx = i_data_start; i_idx < i_data_end; i_idx++ )
    {
        uint32_t ui_hash;
        uint16_t ui16_len, ui16_dist, *pui16_sublen;

        ui_hash = turtledeflate_update_hash( &ps_turtle->s_hash, pui8_data, i_idx, 1 );

        i_pos = i_idx - i_data_start;
        pui16_sublen = &ps_turtle->s_matches.pui16_sublen[ ui_sublen_idx ];

        turtledeflate_get_longest_match( &ps_turtle->s_hash, pui8_data, ui_hash, i_idx, i_data_end, &ui16_len, &ui16_dist, pui16_sublen );

        if( ui16_len > 2 )
        {
            ui16_len = ui16_len;
        }

        ps_turtle->s_matches.pui16_len[ i_pos ] = ui16_len;
        ps_turtle->s_matches.pui16_dist[ i_pos ] = ui16_dist;
        ps_turtle->s_matches.pui_sublen_idx[ i_pos ] = ui_sublen_idx;
        ui_sublen_idx += ( ( uint32_t ) ui16_len ) + 1;
    }
}



void turtledeflate_block_reset( turtledeflate_block_t *ps_block )
{
    memset( ps_block, 0, sizeof( turtledeflate_block_t ) );
}


static void turtledeflate_block_add_litlen_dist( turtledeflate_ctx_t *ps_turtle, turtledeflate_block_t *ps_block, int32_t i_litlen, int32_t i_dist )
{
    turtledeflate_stream_entry_t *ps_entry = &ps_turtle->ps_temp_stream[ ps_block->i_num_entries++ ];

    ps_entry->ui16_litlen = ( uint16_t ) i_litlen;
    ps_entry->ui16_dist = ( uint16_t ) i_dist;

    if( i_dist == 0 )
    {
        ps_block->rgi_count_lenlits[ i_litlen ]++;
    }
    else
    {
        int32_t i_lens = turtledeflate_get_len_symbol( i_litlen );
        int32_t i_dists = turtledeflate_get_dist_symbol( i_dist );

        ps_block->rgi_count_lenlits[ i_lens ]++;
        ps_block->rgi_count_dists[ i_dists ]++;
    }
}

static void turtledeflate_block_temp_stream_to_block_stream( turtledeflate_ctx_t *ps_turtle, turtledeflate_block_t *ps_block )
{
    memcpy( ps_turtle->ps_block_stream, ps_turtle->ps_temp_stream, ps_block->i_num_entries * sizeof( turtledeflate_stream_entry_t ) );
}

void turtledeflate_block_append_block_to_global( turtledeflate_ctx_t *ps_turtle, turtledeflate_block_t *ps_block, int32_t rgi_deflate_block_boundaries[ ] )
{
    turtledeflate_block_t *ps_global = &ps_turtle->s_global_block;
    memcpy( ps_turtle->ps_global_stream + ps_global->i_num_entries, ps_turtle->ps_block_stream, ps_block->i_num_entries * sizeof( turtledeflate_stream_entry_t ) );

    rgi_deflate_block_boundaries[ 0 ] = ps_global->i_num_entries;
    ps_global->i_num_entries += ps_block->i_num_entries;
    rgi_deflate_block_boundaries[ 1 ] = ps_global->i_num_entries;

    /* dont copy stats, they are useless */
}


int32_t turtledeflate_block_deflate_greedy_min_length( int32_t i_dist )
{
    if( i_dist > 1024 )
    {
        return ( TURTLEDEFLATE_MIN_MATCH_LEN + 1 );
    }
    return TURTLEDEFLATE_MIN_MATCH_LEN;
}

void turtledeflate_block_deflate_greedy( turtledeflate_ctx_t *ps_turtle, turtledeflate_block_t *ps_block, uint8_t *pui8_data, int32_t i_data_size, int32_t i_pre )
{
    bool b_used_next;
    int32_t i_idx, i_len, i_dist, i_next_len, i_next_dist;
    turtledeflate_block_t s_block_try;
    turtledeflate_matches_t *ps_match = &ps_turtle->s_matches;

    turtledeflate_get_matches( ps_turtle, pui8_data - i_pre, i_pre, i_pre + i_data_size );

    turtledeflate_block_reset( &s_block_try );

    for( i_idx = 0; i_idx < i_data_size; i_idx += i_len )
    {
        i_len = ps_match->pui16_len[ i_idx ];
        i_dist = ps_match->pui16_dist[ i_idx ];
        if( i_dist != 0 && i_len >= turtledeflate_block_deflate_greedy_min_length( i_dist ) )
        {
            b_used_next = false;
            if( ( i_idx + 1 ) < i_data_size )
            {
                i_next_len = ps_match->pui16_len[ i_idx + 1 ];
                i_next_dist = ps_match->pui16_dist[ i_idx + 1 ];

                if( i_next_dist != 0 && ( i_next_len ) > i_len )
                {
                    turtledeflate_block_add_litlen_dist( ps_turtle, &s_block_try, pui8_data[ i_idx ], 0 );
                    turtledeflate_block_add_litlen_dist( ps_turtle, &s_block_try, i_next_len, i_next_dist );
                    i_len = i_next_len + 1;
                    b_used_next = true;
                }

            }
            if( !b_used_next )
            {
                turtledeflate_block_add_litlen_dist( ps_turtle, &s_block_try, i_len, i_dist );
            }
        }
        else
        {
            i_len = 1;
            turtledeflate_block_add_litlen_dist( ps_turtle, &s_block_try, pui8_data[ i_idx ], 0 );
        }
    }

    *ps_block = s_block_try;
    turtledeflate_block_temp_stream_to_block_stream( ps_turtle, ps_block );
}

#if 0
without rle opt

int32_t turtledeflate_get_estimated_block_bits_internal____( turtledeflate_ctx_t *ps_turtle, int32_t rgi_lenlit_counts[ ], int32_t rgi_dist_counts[ ] )
{
    int32_t i_num_nodes, i_bits, i_flags, i_flag_bits, i_best_flags_bits, i_compressed_size;
    int32_t i_idx, i_root_node; /* unused */
    turtledeflate_tree_node_t rgs_tree_nodes[ TURTLEDEFLATE_NUM_LENLITS * 2 + 1 ]; /* + 1 ? not - 1 ? */
    turtledeflate_block_t s_block;
    int32_t rgi_patched_lenlit_counts[ TURTLEDEFLATE_NUM_LENLITS ];

    memcpy( rgi_patched_lenlit_counts, rgi_lenlit_counts, sizeof( rgi_patched_lenlit_counts ) );
    rgi_patched_lenlit_counts[ 256 ] = 1; /* eob */
    

    turtledeflate_build_tree( ps_turtle, rgi_patched_lenlit_counts, TURTLEDEFLATE_NUM_LENLITS, &rgs_tree_nodes[ 0 ], TURTLEDEFLATE_NUM_LENLITS * 2 + 1, &i_num_nodes, &i_root_node );
    turtledeflate_get_symbol_lengths( ps_turtle, &rgs_tree_nodes[ 0 ], i_num_nodes, &s_block.rgs_lenlit_symbols[ 0 ], TURTLEDEFLATE_NUM_LENLITS, TURTLEDEFLATE_MAX_CODEWORD_BITS );

    turtledeflate_build_tree( ps_turtle, rgi_dist_counts, TURTLEDEFLATE_NUM_DISTS, &rgs_tree_nodes[ 0 ], TURTLEDEFLATE_NUM_DISTS * 2 + 1, &i_num_nodes, &i_root_node );
    turtledeflate_get_symbol_lengths( ps_turtle, &rgs_tree_nodes[ 0 ], i_num_nodes, &s_block.rgs_dist_symbols[ 0 ], TURTLEDEFLATE_NUM_DISTS, TURTLEDEFLATE_MAX_CODEWORD_BITS );

    /* brute force rle flags */
    i_best_flags_bits = -1;
    for( i_flags = 0; i_flags < 8; i_flags++ )
    {
        i_flag_bits = turtledeflate_encode_tree( ps_turtle, &s_block.rgs_lenlit_symbols[ 0 ], &s_block.rgs_dist_symbols[ 0 ], NULL, i_flags );
        if( i_best_flags_bits < 0 || i_flag_bits < i_best_flags_bits )
        {
            i_best_flags_bits = i_flag_bits;
        }
    }

    i_bits = 3; /* header ! */
    i_bits += i_best_flags_bits;

    for( i_idx = 0; i_idx < TURTLEDEFLATE_NUM_LENLITS; i_idx++ )
    {
        int32_t i_code_length = s_block.rgs_lenlit_symbols[ i_idx ].ui_code_length + ( i_idx > 256 ? turtledeflate_get_len_extra_bits( i_idx ) : 0 );
        i_bits += rgi_patched_lenlit_counts[ i_idx ] * i_code_length;

    }
    for( i_idx = 0; i_idx < TURTLEDEFLATE_NUM_DISTS; i_idx++ )
    {
        int32_t i_code_length = s_block.rgs_dist_symbols[ i_idx ].ui_code_length + turtledeflate_get_dist_extra_bits( i_idx );;
        i_bits += rgi_dist_counts[ i_idx ] * i_code_length;
    }

    return i_bits;

}
#endif

int32_t turtledeflate_get_estimated_block_bits_tree( turtledeflate_ctx_t *ps_turtle, turtledeflate_block_t *ps_block, int32_t rgi_lenlit_counts[ ], int32_t rgi_dist_counts[ ] )
{
    turtledeflate_tree_node_t rgs_tree_nodes[ TURTLEDEFLATE_NUM_LENLITS * 2 + 1 ]; /* + 1 ? not - 1 ? */
    int32_t i_best_flags_bits, i_flag_bits, i_flags, i_num_nodes, i_root_node;

    turtledeflate_build_tree( ps_turtle, rgi_lenlit_counts, TURTLEDEFLATE_NUM_LENLITS, &rgs_tree_nodes[ 0 ], TURTLEDEFLATE_NUM_LENLITS * 2 + 1, &i_num_nodes, &i_root_node );
    turtledeflate_get_symbol_lengths( ps_turtle, &rgs_tree_nodes[ 0 ], i_num_nodes, &ps_block->rgs_lenlit_symbols[ 0 ], TURTLEDEFLATE_NUM_LENLITS, TURTLEDEFLATE_MAX_CODEWORD_BITS );

    turtledeflate_build_tree( ps_turtle, rgi_dist_counts, TURTLEDEFLATE_NUM_DISTS, &rgs_tree_nodes[ 0 ], TURTLEDEFLATE_NUM_DISTS * 2 + 1, &i_num_nodes, &i_root_node );
    turtledeflate_get_symbol_lengths( ps_turtle, &rgs_tree_nodes[ 0 ], i_num_nodes, &ps_block->rgs_dist_symbols[ 0 ], TURTLEDEFLATE_NUM_DISTS, TURTLEDEFLATE_MAX_CODEWORD_BITS );

    /* brute force rle flags */
    if( ps_turtle->s_config.i_compression_level > 7 )
    {
        i_best_flags_bits = -1;
        for( i_flags = 0; i_flags < 8; i_flags++ )
        {
            i_flag_bits = turtledeflate_encode_tree( ps_turtle, &ps_block->rgs_lenlit_symbols[ 0 ], &ps_block->rgs_dist_symbols[ 0 ], NULL, i_flags );
            if( i_best_flags_bits < 0 || i_flag_bits < i_best_flags_bits )
            {
                i_best_flags_bits = i_flag_bits;
            }
        }
    }
    else
    {
        i_best_flags_bits = turtledeflate_encode_tree( ps_turtle, &ps_block->rgs_lenlit_symbols[ 0 ], &ps_block->rgs_dist_symbols[ 0 ], NULL, 0x7 );
    }

    return i_best_flags_bits;
}


int32_t turtledeflate_get_estimated_block_bits_data( turtledeflate_ctx_t *ps_turtle, turtledeflate_block_t *ps_block, int32_t rgi_lenlit_counts[ ], int32_t rgi_dist_counts[ ] )
{
    int32_t i_bits = 0, i_idx;

    for( i_idx = 0; i_idx < TURTLEDEFLATE_NUM_LENLITS; i_idx++ )
    {
        int32_t i_code_length = ps_block->rgs_lenlit_symbols[ i_idx ].ui_code_length + ( i_idx > 256 ? turtledeflate_get_len_extra_bits( i_idx ) : 0 );
        i_bits += rgi_lenlit_counts[ i_idx ] * i_code_length;

    }
    for( i_idx = 0; i_idx < TURTLEDEFLATE_NUM_DISTS; i_idx++ )
    {
        int32_t i_code_length = ps_block->rgs_dist_symbols[ i_idx ].ui_code_length + turtledeflate_get_dist_extra_bits( i_idx );
        i_bits += rgi_dist_counts[ i_idx ] * i_code_length;
    }

    return i_bits;
}



int32_t turtledeflate_get_estimated_block_bits( turtledeflate_ctx_t *ps_turtle, int32_t rgi_lenlit_counts[ ], int32_t rgi_dist_counts[ ] )
{
    int32_t i_num_nodes, i_flags, i_flag_bits, i_best_flags_bits, i_compressed_size;
    int32_t i_idx, i_root_node; /* unused */
    turtledeflate_block_t s_block;
    int32_t rgi_patched_lenlit_counts[ TURTLEDEFLATE_NUM_LENLITS ];

    memcpy( rgi_patched_lenlit_counts, rgi_lenlit_counts, sizeof( rgi_patched_lenlit_counts ) );
    rgi_patched_lenlit_counts[ 256 ] = 1; /* eob */

    i_compressed_size = turtledeflate_get_estimated_block_bits_tree( ps_turtle, &s_block, rgi_patched_lenlit_counts, rgi_dist_counts );
    i_compressed_size += turtledeflate_get_estimated_block_bits_data( ps_turtle, &s_block, rgi_patched_lenlit_counts, rgi_dist_counts );

    if( ps_turtle->s_config.i_compression_level > 7 )
    {
        int32_t rgi_try_rle_lenlits[ TURTLEDEFLATE_NUM_LENLITS ];
        int32_t rgi_try_rle_dists[ TURTLEDEFLATE_NUM_DISTS ];
        int32_t i_try_compressed_size;

        memcpy( rgi_try_rle_lenlits, rgi_patched_lenlit_counts, sizeof( rgi_try_rle_lenlits ) );
        memcpy( rgi_try_rle_dists, rgi_dist_counts, sizeof( rgi_try_rle_dists ) );

        OptimizeHuffmanForRle( TURTLEDEFLATE_NUM_LENLITS, rgi_try_rle_lenlits );
        OptimizeHuffmanForRle( TURTLEDEFLATE_NUM_DISTS, rgi_try_rle_dists );

        i_try_compressed_size = turtledeflate_get_estimated_block_bits_tree( ps_turtle, &s_block, rgi_try_rle_lenlits, rgi_try_rle_dists );
        i_try_compressed_size += turtledeflate_get_estimated_block_bits_data( ps_turtle, &s_block, rgi_patched_lenlit_counts, rgi_dist_counts );

        if( i_try_compressed_size < i_compressed_size )
        {
            i_compressed_size = i_try_compressed_size;
        }
    }

    i_compressed_size += 3; /* header */

    return i_compressed_size;
}




void turtledeflate_set_fp_bitcount( int32_t rgi_symbol_counts[], int32_t i_num_symbols, int64_t rgpf_bits[], float64_t f64_fp_scale, float64_t f64_weight_scale )
{
    int32_t i_idx, i_total_sum;
    float64_t f64_log2_t, f64_log2_i, f64_scale;
    int64_t i64_backup_bits;

    f64_log2_i = 1.0 / log( 2.0 );
    f64_scale = f64_fp_scale;
 

    i_total_sum = 0;
    for( i_idx = 0; i_idx < i_num_symbols; i_idx++ )
    {
        i_total_sum += rgi_symbol_counts[ i_idx ];
    }
    i_total_sum += 1; /* end of block ? needed ? */

    f64_log2_t = log( ( float64_t ) i_total_sum ) * f64_log2_i;
    i64_backup_bits = ( int64_t )floor( ( f64_log2_t * f64_scale ) + 0.5 );

    for( i_idx = 0; i_idx < i_num_symbols; i_idx++ )
    {
        int32_t i_symbol_count = rgi_symbol_counts[ i_idx ];

        i_symbol_count = ( int32_t ) ceil( ( i_symbol_count * f64_weight_scale ) + ( ( i_total_sum * ( 1.0 - f64_weight_scale ) ) / i_num_symbols ) - 0.5 );
        if( i_symbol_count != 0 )
        {
            float64_t f64_sum_part = f64_log2_t - ( log( ( float64_t ) i_symbol_count ) * f64_log2_i );
            rgpf_bits[ i_idx ] = ( int64_t )floor( ( f64_sum_part * f64_scale ) + 0.5 );
        }
        else
        {
            rgpf_bits[ i_idx ] = i64_backup_bits;
        }
    }
}

void turtledeflate_set_block_fp_bitcount( turtledeflate_block_t *ps_block, int32_t rgi_lenlit_counts[ ], int32_t rgi_dist_counts[ ], float64_t f64_fp_scale, float64_t f64_weight_scale )
{
    int32_t i_idx;
    turtledeflate_set_fp_bitcount( rgi_lenlit_counts, TURTLEDEFLATE_NUM_LENLITS, ps_block->rgfp64_lenlits_bits, f64_fp_scale, f64_weight_scale );
    turtledeflate_set_fp_bitcount( rgi_dist_counts, TURTLEDEFLATE_NUM_DISTS, ps_block->rgfp64_dists_bits, f64_fp_scale, f64_weight_scale );
    for( i_idx = 0; i_idx < 64; i_idx++ )
    {
        ps_block->rgfp64_extra_bits[ i_idx ] = floor( ( i_idx * f64_fp_scale ) + 0.5 );
    }
}


void turtledeflate_set_block_fp_bitcount_exact( turtledeflate_ctx_t *ps_turtle, turtledeflate_block_t *ps_block, int32_t rgi_lenlit_counts[ ], int32_t rgi_dist_counts[ ] )
{
    int32_t i_num_nodes, i_bits, i_flags, i_flag_bits, i_best_flags_bits, i_max_code_length;
    int32_t i_idx, i_root_node; /* unused */
    turtledeflate_tree_node_t rgs_tree_nodes[ TURTLEDEFLATE_NUM_LENLITS * 2 + 1 ]; /* + 1 ? not - 1 ? */
    turtledeflate_block_t s_block;

    turtledeflate_build_tree( ps_turtle, rgi_lenlit_counts, TURTLEDEFLATE_NUM_LENLITS, &rgs_tree_nodes[ 0 ], TURTLEDEFLATE_NUM_LENLITS * 2 + 1, &i_num_nodes, &i_root_node );
    turtledeflate_get_symbol_lengths( ps_turtle, &rgs_tree_nodes[ 0 ], i_num_nodes, &s_block.rgs_lenlit_symbols[ 0 ], TURTLEDEFLATE_NUM_LENLITS, TURTLEDEFLATE_MAX_CODEWORD_BITS );

    turtledeflate_build_tree( ps_turtle, rgi_dist_counts, TURTLEDEFLATE_NUM_DISTS, &rgs_tree_nodes[ 0 ], TURTLEDEFLATE_NUM_DISTS * 2 + 1, &i_num_nodes, &i_root_node );
    turtledeflate_get_symbol_lengths( ps_turtle, &rgs_tree_nodes[ 0 ], i_num_nodes, &s_block.rgs_dist_symbols[ 0 ], TURTLEDEFLATE_NUM_DISTS, TURTLEDEFLATE_MAX_CODEWORD_BITS );

    for( i_max_code_length = 0, i_idx = 0; i_idx < TURTLEDEFLATE_NUM_LENLITS; i_idx++ )
    {
        if( i_max_code_length < s_block.rgs_lenlit_symbols[ i_idx ].ui_code_length )
        {
            i_max_code_length = s_block.rgs_lenlit_symbols[ i_idx ].ui_code_length;
        }
    }
    for( i_idx = 0; i_idx < TURTLEDEFLATE_NUM_LENLITS; i_idx++ )
    {
        int32_t i_code_length = s_block.rgs_lenlit_symbols[ i_idx ].ui_code_length ? s_block.rgs_lenlit_symbols[ i_idx ].ui_code_length : i_max_code_length;
        ps_block->rgfp64_lenlits_bits[ i_idx ] = i_code_length;
    }


    for( i_max_code_length = 0, i_idx = 0; i_idx < TURTLEDEFLATE_NUM_DISTS; i_idx++ )
    {
        if( i_max_code_length < s_block.rgs_dist_symbols[ i_idx ].ui_code_length )
        {
            i_max_code_length = s_block.rgs_dist_symbols[ i_idx ].ui_code_length;
        }
    }
    for( i_idx = 0; i_idx < TURTLEDEFLATE_NUM_DISTS; i_idx++ )
    {
        int32_t i_code_length = s_block.rgs_dist_symbols[ i_idx ].ui_code_length ? s_block.rgs_dist_symbols[ i_idx ].ui_code_length : i_max_code_length;
        ps_block->rgfp64_dists_bits[ i_idx ] = i_code_length;
    }

    for( i_idx = 0; i_idx < 64; i_idx++ )
    {
        ps_block->rgfp64_extra_bits[ i_idx ] = i_idx;
    }

}


void turtledeflate_reset_squish( turtledeflate_squish_t *ps_squish, int32_t i_data_size )
{
    int32_t i_idx;

    for( i_idx = 0; i_idx <= i_data_size; i_idx++ )
    {
        ps_squish->rgps_trellis[ 0 ][ i_idx ].i64_cost = TURTLEDEFLATE_UNUSED_SQUISH_ENTRY;
        ps_squish->rgps_trellis[ 1 ][ i_idx ].i64_cost = TURTLEDEFLATE_UNUSED_SQUISH_ENTRY;
    }
    ps_squish->rgps_trellis[ 0 ][ 0 ].i64_cost = 0;
    ps_squish->rgps_trellis[ 1 ][ 0 ].i64_cost = 0;
}


int64_t turtledeflate_get_squish_cost( turtledeflate_block_t *ps_block, int32_t i_litlen, int32_t i_dist, float64_t f64_fp_scale )
{
    int64_t i64_cost;

    if( i_dist == 0 )
    {
        i64_cost = ps_block->rgfp64_lenlits_bits[ i_litlen ];
    }
    else
    {
        int32_t i_lens, i_dists, i_len_extra, i_dist_extra;

        i_lens = turtledeflate_get_len_symbol( i_litlen );
        i_len_extra = turtledeflate_get_len_extra_bits( i_lens );
        i_dists = turtledeflate_get_dist_symbol( i_dist );
        i_dist_extra = turtledeflate_get_dist_extra_bits( i_dists );

        i64_cost = ps_block->rgfp64_lenlits_bits[ i_lens ] + ps_block->rgfp64_dists_bits[ i_dists ];
        i64_cost += ps_block->rgfp64_extra_bits[ i_len_extra + i_dist_extra ];
        
    }

    return i64_cost;
}


void turtledeflate_try_add_squish_entry( turtledeflate_block_t *ps_block, turtledeflate_squish_t *ps_squish, int32_t i_idx, int64_t i64_current_cost, int32_t i_litlen, int32_t i_dist, float64_t f64_fp_scale )
{
    int64_t i64_cost, i_trellis;

    i64_cost = i64_current_cost + turtledeflate_get_squish_cost( ps_block, i_litlen, i_dist, f64_fp_scale );

    for( i_trellis = 0; i_trellis < 2; i_trellis++ )
    {
        if( i64_cost < ps_squish->rgps_trellis[ i_trellis ][ i_idx ].i64_cost )
        {
            ps_squish->rgps_trellis[ i_trellis ][ i_idx ].i64_cost = i64_cost;
            ps_squish->rgps_trellis[ i_trellis ][ i_idx ].ui16_litlen = i_litlen;
            ps_squish->rgps_trellis[ i_trellis ][ i_idx ].ui16_dist = i_dist;
            break;
        }
    }
}


int32_t turtledeflate_squish_backtrace( turtledeflate_squish_t *ps_squish, int32_t i_data_size, bool b_alternative_path, int32_t rgi_litlen_counts[], int32_t rgi_dist_counts[] )
{
    int32_t i_idx, i_trace, i_len, i_backtrace, i_alen, i_pos, i_apos, i_epos, i_eapos;

    for( i_idx = i_data_size, i_trace = 0; i_idx > 0; i_idx -= i_len )
    {
        turtledeflate_squish_entry_t *ps_entry = &ps_squish->rgps_trellis[ 0 ][ i_idx ];
        ps_squish->pps_trace[ i_trace++ ] = ps_entry;
        i_len = ps_entry->ui16_dist == 0 ? 1 : ps_entry->ui16_litlen;
    }

    if( b_alternative_path )
    {
        i_backtrace = i_data_size;
        for( i_idx = 0; i_idx < i_trace - 1; i_idx++ )
        {
            turtledeflate_squish_entry_t *ps_entry0;
            turtledeflate_squish_entry_t *ps_entry1;
            turtledeflate_squish_entry_t *ps_aentry0;
            turtledeflate_squish_entry_t *ps_aentry1;

            ps_entry0 = ps_squish->pps_trace[ i_idx ];
            ps_entry1 = ps_squish->pps_trace[ i_idx + 1 ];
            if( ps_entry0->ui16_dist && ps_entry1->ui16_dist )
            {
                i_len = ps_entry0->ui16_litlen + ps_entry1->ui16_litlen;

                ps_aentry0 = &ps_squish->rgps_trellis[ 1 ][ i_backtrace ];
                if( ps_aentry0->i64_cost != TURTLEDEFLATE_UNUSED_SQUISH_ENTRY && ps_aentry0->ui16_dist ) /* no need to check for unused, there is at least a literal there ? */
                {
                    ps_aentry1 = &ps_squish->rgps_trellis[ 0 ][ i_backtrace - ps_aentry0->ui16_litlen ];
                    if( ps_aentry1->ui16_dist )
                    {
                        i_alen = ps_aentry0->ui16_litlen + ps_aentry1->ui16_litlen;

                        if( i_len == i_alen )
                        {
                            int32_t i_lens_0, i_lens_1, i_lens_a0, i_lens_a1, i_ebits, i_aebits, i_epos, i_eapos;

                            i_lens_0 = turtledeflate_get_len_symbol( ps_entry0->ui16_litlen );
                            i_lens_1 = turtledeflate_get_len_symbol( ps_entry1->ui16_litlen );
                            i_lens_a0 = turtledeflate_get_len_symbol( ps_aentry0->ui16_litlen );
                            i_lens_a1 = turtledeflate_get_len_symbol( ps_aentry1->ui16_litlen );

                            i_ebits = turtledeflate_get_len_extra_bits( i_lens_0 ) + turtledeflate_get_len_extra_bits( i_lens_1 );
                            i_aebits = turtledeflate_get_len_extra_bits( i_lens_a0 ) + turtledeflate_get_len_extra_bits( i_lens_a1 );
                            i_epos = i_lens_0 + i_lens_1;
                            i_eapos = i_lens_a0 + i_lens_a1;

                            if( i_epos > i_eapos && i_ebits >= i_aebits )
                            {
                                ps_squish->pps_trace[ i_idx ] = ps_aentry0;
                                ps_squish->pps_trace[ i_idx + 1 ] = ps_aentry1;
                                ps_entry0 = ps_aentry0;
                            }
                        }
                    }
                }
            }
            i_backtrace -= ps_entry0->ui16_dist ? ps_entry0->ui16_litlen : 1;
        }
    }

    memset( rgi_litlen_counts, 0, sizeof( int32_t ) * TURTLEDEFLATE_NUM_LENLITS );
    memset( rgi_dist_counts, 0, sizeof( int32_t ) * TURTLEDEFLATE_NUM_DISTS );

    for( i_idx = 0; i_idx < i_trace; i_idx++ )
    {
        turtledeflate_squish_entry_t *ps_entry = ps_squish->pps_trace[ i_idx ];
        if( ps_entry->ui16_dist )
        {
            int32_t i_lens = turtledeflate_get_len_symbol( ps_entry->ui16_litlen );
            int32_t i_dists = turtledeflate_get_dist_symbol( ps_entry->ui16_dist );

            rgi_litlen_counts[ i_lens ]++;
            rgi_dist_counts[ i_dists ]++;
        }
        else
        {
            rgi_litlen_counts[ ps_entry->ui16_litlen ]++;
        }
    }


        return i_trace;
}



#define TURTLEDEFLATE_SQUISH_FLAGS_EXACT  1
#define TURTLEDEFLATE_SQUISH_FLAGS_COMMIT 2
#define TURTLEDEFLATE_SQUISH_FLAGS_APATH  4

int32_t turtledeflate_block_deflate_squish_iter( turtledeflate_ctx_t *ps_turtle, turtledeflate_block_t *ps_block, turtledeflate_block_t *ps_block_model, float64_t f64_weight_scale, uint8_t *pui8_data, int32_t i_data_size, float64_t f64_fp_scale, int32_t i_flags )
{
    bool b_use_alternative_path;
    int32_t i_idx, i_trace, i_len, i_sublen, i_dist, i_first_path_bits, i_bits, i_alternative_path;
    int64_t i64_current_cost;
    turtledeflate_matches_t *ps_match = &ps_turtle->s_matches;
    turtledeflate_squish_t *ps_squish = &ps_turtle->s_squish;

    turtledeflate_block_reset( ps_block );

    turtledeflate_reset_squish( ps_squish, i_data_size );

    if( !( i_flags & TURTLEDEFLATE_SQUISH_FLAGS_EXACT ) )
    {
        turtledeflate_set_block_fp_bitcount( ps_block, ps_block_model->rgi_count_lenlits, ps_block_model->rgi_count_dists, f64_fp_scale, f64_weight_scale );
    }
    else
    {
        turtledeflate_set_block_fp_bitcount_exact( ps_turtle, ps_block, ps_block_model->rgi_count_lenlits, ps_block_model->rgi_count_dists );
        f64_fp_scale = 1.0;
    }

    for( i_idx = 0; i_idx < i_data_size; i_idx++ )
    {
        i64_current_cost = ps_squish->rgps_trellis[ 0 ][ i_idx ].i64_cost;

        turtledeflate_try_add_squish_entry( ps_block, ps_squish, i_idx + 1, i64_current_cost, pui8_data[ i_idx ], 0, f64_fp_scale );

        i_dist = ps_match->pui16_dist[ i_idx ];
        if( i_dist != 0 )
        {
            uint16_t *pui16_sublen = &ps_match->pui16_sublen[ ps_match->pui_sublen_idx[ i_idx ] ];

            i_len = ps_match->pui16_len[ i_idx ];

            for( i_sublen = i_len; i_sublen >= TURTLEDEFLATE_MIN_MATCH_LEN; i_sublen-- )
            {
                turtledeflate_try_add_squish_entry( ps_block, ps_squish, i_idx + i_sublen, i64_current_cost, i_sublen, pui16_sublen[ i_sublen ], f64_fp_scale );
            }
        }
    }

    for( i_alternative_path = 0; i_alternative_path < 2; i_alternative_path++ )
    {
        bool b_alternative_path = !!i_alternative_path;

        if( b_alternative_path && !( i_flags & TURTLEDEFLATE_SQUISH_FLAGS_APATH ) )
        {
            continue;
        }
        turtledeflate_squish_backtrace( ps_squish, i_data_size, b_alternative_path, ps_block->rgi_count_lenlits, ps_block->rgi_count_dists );

        i_bits = turtledeflate_get_estimated_block_bits( ps_turtle, ps_block->rgi_count_lenlits, ps_block->rgi_count_dists );

        if( !b_alternative_path || i_bits < i_first_path_bits )
        {
            if( i_flags & TURTLEDEFLATE_SQUISH_FLAGS_COMMIT )
            {
                memcpy( ps_block_model->rgi_count_lenlits, ps_block->rgi_count_lenlits, sizeof( ps_block->rgi_count_lenlits ) );
                memcpy( ps_block_model->rgi_count_dists, ps_block->rgi_count_dists, sizeof( ps_block->rgi_count_dists ) );
            }
            b_use_alternative_path = b_alternative_path;
        }
        i_first_path_bits = i_bits;
    }

    if( b_use_alternative_path )
    {
        b_use_alternative_path = b_use_alternative_path;
    }

    i_trace = turtledeflate_squish_backtrace( ps_squish, i_data_size, b_use_alternative_path, ps_block->rgi_count_lenlits, ps_block->rgi_count_dists );

    turtledeflate_block_reset( ps_block );

    for( i_idx = i_trace - 1; i_idx >= 0; i_idx-- )
    {
        turtledeflate_squish_entry_t *ps_entry = ps_squish->pps_trace[ i_idx ];
        turtledeflate_block_add_litlen_dist( ps_turtle, ps_block, ps_entry->ui16_litlen, ps_entry->ui16_dist );
    }

    return turtledeflate_get_estimated_block_bits( ps_turtle, ps_block->rgi_count_lenlits, ps_block->rgi_count_dists );;
}



typedef struct
{
    turtledeflate_block_t s_best_model_block;
    int32_t i_best_model_bits;

    turtledeflate_block_t s_best_exact_block;
    int32_t i_best_exact_bits;

    turtledeflate_block_t s_best_block;
    int32_t i_best_bits;
    int32_t i_best_path_ctx;
} turtldeflate_best_blocks_t;



#define TURTLEDEFLATE_FP_GET_SCALE( x ) ( pow( 2.0, x ) )


int32_t turtledeflate_block_deflate_try_model( turtledeflate_ctx_t *ps_turtle, uint8_t *pui8_data, int32_t i_data_size, turtledeflate_squish_path_ctx_t *ps_fp_ctx, turtldeflate_best_blocks_t *ps_main_ctx, float64_t f64_fp, bool b_exact )
{
    int32_t i_flags, i_exact_bits, i_model_bits, i_return;
    turtledeflate_block_t s_recv_block_mdl, s_recv_block_exact;

    float64_t f64_fp_scale;
    f64_fp_scale = TURTLEDEFLATE_FP_GET_SCALE( f64_fp );

    i_return = 0;

    if( b_exact )
    {
        i_flags = TURTLEDEFLATE_SQUISH_FLAGS_COMMIT | TURTLEDEFLATE_SQUISH_FLAGS_EXACT;
        i_exact_bits = turtledeflate_block_deflate_squish_iter( ps_turtle, &s_recv_block_exact, &ps_fp_ctx->s_block_model, 1.0, pui8_data, i_data_size, f64_fp_scale, i_flags );

        if( i_exact_bits < ps_main_ctx->i_best_bits )
        {
            ps_main_ctx->i_best_bits = i_exact_bits;
            ps_main_ctx->i_best_path_ctx = ps_fp_ctx->i_idx;
            ps_main_ctx->s_best_block = s_recv_block_exact;
            turtledeflate_block_temp_stream_to_block_stream( ps_turtle, &s_recv_block_exact );
            i_return |= 4;
        }

        if( i_exact_bits < ps_main_ctx->i_best_exact_bits )
        {
            ps_main_ctx->i_best_exact_bits = i_exact_bits;
            ps_main_ctx->s_best_exact_block = s_recv_block_exact;
            i_return |= 2;
        }
    }
    else
    {
        i_exact_bits = 0x7fffffff;
    }

    if( !b_exact )
    {
        i_flags = TURTLEDEFLATE_SQUISH_FLAGS_COMMIT | TURTLEDEFLATE_SQUISH_FLAGS_APATH;
        i_model_bits = turtledeflate_block_deflate_squish_iter( ps_turtle, &s_recv_block_mdl, &ps_fp_ctx->s_block_model, 1.0, pui8_data, i_data_size, f64_fp_scale, i_flags );

        if( i_model_bits < ps_main_ctx->i_best_bits )
        {
            ps_main_ctx->i_best_bits = i_model_bits;
            ps_main_ctx->i_best_path_ctx = ps_fp_ctx->i_idx;
            ps_main_ctx->s_best_block = s_recv_block_mdl;
            turtledeflate_block_temp_stream_to_block_stream( ps_turtle, &s_recv_block_mdl );
            i_return |= 4;
        }
        if( i_model_bits < ps_main_ctx->i_best_model_bits )
        {
            ps_main_ctx->i_best_model_bits = i_model_bits;
            ps_main_ctx->s_best_model_block = s_recv_block_mdl;
            i_return |= 1;
        }
    }
    else
    {
        i_model_bits = 0x7fffffff;
    }

    if( i_exact_bits < ps_fp_ctx->i_best_bits )
    {
        ps_fp_ctx->i_best_bits = i_exact_bits;
        i_return |= 4 << 8;
    }
    if( i_model_bits < ps_fp_ctx->i_best_bits )
    {
        ps_fp_ctx->i_best_bits = i_model_bits;
        i_return |= 2 << 8;
    }
    if( i_model_bits < ps_fp_ctx->i_model_bits )
    {
        ps_fp_ctx->f64_last_best_fp = f64_fp;
        ps_fp_ctx->i_model_bits = i_model_bits;
        i_return |= 1 << 8;
    }

    return i_return;
}

int32_t turtledeflate_block_deflate_try_block_model_exact( turtledeflate_ctx_t *ps_turtle, turtldeflate_best_blocks_t *ps_main_ctx, uint8_t *pui8_data, int32_t i_data_size, turtledeflate_block_t *ps_model, bool b_commit )
{
    int32_t i_flags, i_exact_bits, i_model_bits, i_return;
    turtledeflate_block_t s_recv_block_exact;
    float64_t f64_fp_scale;

    i_return = 0;
    f64_fp_scale = TURTLEDEFLATE_FP_GET_SCALE( 16.0 );

    i_flags = TURTLEDEFLATE_SQUISH_FLAGS_EXACT | ( b_commit ? TURTLEDEFLATE_SQUISH_FLAGS_COMMIT : 0 );
    i_exact_bits = turtledeflate_block_deflate_squish_iter( ps_turtle, &s_recv_block_exact, ps_model, 1.0, pui8_data, i_data_size, f64_fp_scale, i_flags );

    if( i_exact_bits < ps_main_ctx->i_best_bits )
    {
        ps_main_ctx->i_best_bits = i_exact_bits;
        ps_main_ctx->i_best_path_ctx = ps_turtle->s_config.i_max_start_fp;
        ps_main_ctx->s_best_block = s_recv_block_exact;
        turtledeflate_block_temp_stream_to_block_stream( ps_turtle, &s_recv_block_exact );
        i_return |= 4;
    }

    if( i_exact_bits < ps_main_ctx->i_best_exact_bits )
    {
        ps_main_ctx->i_best_exact_bits = i_exact_bits;
        ps_main_ctx->s_best_exact_block = s_recv_block_exact;
        i_return |= 2;
    }

    return i_return;
}

int32_t turtledeflate_block_deflate_try_block_model( turtledeflate_ctx_t *ps_turtle, turtldeflate_best_blocks_t *ps_main_ctx, uint8_t *pui8_data, int32_t i_data_size, turtledeflate_block_t *ps_model, float64_t f64_weight_scale, bool b_exact, bool b_commit )
{
    int32_t i_flags, i_bits;
    turtledeflate_block_t s_recv_block;
    float64_t f64_fp_scale;

    f64_fp_scale = TURTLEDEFLATE_FP_GET_SCALE( 16.0 );

    i_flags = ( b_exact ? TURTLEDEFLATE_SQUISH_FLAGS_EXACT : TURTLEDEFLATE_SQUISH_FLAGS_APATH ) | ( b_commit ? TURTLEDEFLATE_SQUISH_FLAGS_COMMIT : 0 );
    i_bits = turtledeflate_block_deflate_squish_iter( ps_turtle, &s_recv_block, ps_model, f64_weight_scale, pui8_data, i_data_size, f64_fp_scale, i_flags );

    if( i_bits < ps_main_ctx->i_best_bits )
    {
        ps_main_ctx->i_best_bits = i_bits;
        ps_main_ctx->i_best_path_ctx = ps_turtle->s_config.i_max_start_fp;
        ps_main_ctx->s_best_block = s_recv_block;
        turtledeflate_block_temp_stream_to_block_stream( ps_turtle, &s_recv_block );
    }

    return i_bits;
}


void turtledeflate_block_deflate_merge_models( turtledeflate_ctx_t *ps_turtleps_turtle, turtledeflate_block_t *ps_dst, turtledeflate_block_t *ps_b0, turtledeflate_block_t *ps_b1, float64_t f64_w0, float64_t f64_w1 )
{
    int32_t i_idx;

    for( i_idx = 0; i_idx < TURTLEDEFLATE_NUM_LENLITS; i_idx++ )
    {
        ps_dst->rgi_count_lenlits[ i_idx ] = floor( ( ps_b0->rgi_count_lenlits[ i_idx ] * f64_w0 ) + ( ps_b1->rgi_count_lenlits[ i_idx ] * f64_w1 ) + 0.5 );
    }
    for( i_idx = 0; i_idx < TURTLEDEFLATE_NUM_DISTS; i_idx++ )
    {
        ps_dst->rgi_count_dists[ i_idx ] = floor( ( ps_b0->rgi_count_dists[ i_idx ] * f64_w0 ) + ( ps_b1->rgi_count_dists[ i_idx ] * f64_w1 ) + 0.5 );
    }
}

void turtledeflate_block_deflate_refine_path_ctx( turtledeflate_ctx_t *ps_turtle, turtldeflate_best_blocks_t *ps_main_ctx, turtledeflate_squish_path_ctx_t *ps_fp_ctx, uint8_t *pui8_data, int32_t i_data_size, bool b_exact, int32_t i_max_iter, int32_t i_break_at )
{
    int32_t i_return, i_iter_count = 0, i_rcount = 0;
    while( 1 )
    {
        i_return = turtledeflate_block_deflate_try_model( ps_turtle, pui8_data, i_data_size, ps_fp_ctx, ps_main_ctx, ps_fp_ctx->f64_current_fp, b_exact );
        i_iter_count++;
        if( !( i_return ) )
        {
            i_rcount++;
        }
        else
        {
            i_rcount = 0;
        }
        if( i_rcount >= i_break_at || i_iter_count >= i_max_iter )
        {
            break;
        }
    }
}


void turtledeflate_block_deflate_reseat_ctx_and_refine( turtledeflate_ctx_t *ps_turtle, bool b_excessive, turtldeflate_best_blocks_t *ps_main_ctx, turtledeflate_squish_path_ctx_t *ps_fp_ctx, uint8_t *pui8_data, int32_t i_data_size, int32_t i_exact_level )
{
    int32_t i_idx, i_flags, i_bits, i_break_bits, i_rcount, i_num_weights;
    turtledeflate_block_t s_dummy;
    turtledeflate_squish_path_ctx_t s_nctx;
    float64_t f64_fp_scale = TURTLEDEFLATE_FP_GET_SCALE( ps_fp_ctx->f64_current_fp );
    float64_t rgf64_weights_a[ 6 ] = { 0.65, 0.72, 0.79, 0.86, 0.93, 1.0 };
    float64_t rgf64_weights_b[ 6 ] = { 0.5, 0.6, 0.7, 0.8, 0.9, 1.0 };
    float64_t *pf64_weightslots;

    if( b_excessive )
    {
        i_num_weights = 6;
        pf64_weightslots = rgf64_weights_b;
    }
    else
    {
        i_num_weights = 6;
        pf64_weightslots = rgf64_weights_a;
    }

    for( i_idx = i_num_weights - 1; i_idx >= 0; i_idx-- )
    {
        float64_t f64_weight_scale = pf64_weightslots[ i_idx ];

        s_nctx = *ps_fp_ctx;

        /* unseat */
        if( f64_weight_scale < 1.0 )
        {
            i_flags = TURTLEDEFLATE_SQUISH_FLAGS_COMMIT | TURTLEDEFLATE_SQUISH_FLAGS_APATH;
            turtledeflate_block_deflate_try_block_model( ps_turtle, ps_main_ctx, pui8_data, i_data_size, &s_nctx.s_block_model, f64_weight_scale, false, true );
        }

        if( i_exact_level > 0 )
        {
            i_flags = TURTLEDEFLATE_SQUISH_FLAGS_EXACT | TURTLEDEFLATE_SQUISH_FLAGS_COMMIT;
            i_bits = turtledeflate_block_deflate_squish_iter( ps_turtle, &s_dummy, &s_nctx.s_block_model, 1.0, pui8_data, i_data_size, f64_fp_scale, i_flags );
            if( i_exact_level > 1 )
            {
                if( i_bits < ps_main_ctx->i_best_bits )
                {
                    ps_main_ctx->i_best_bits = i_bits;
                    ps_main_ctx->i_best_path_ctx = ps_fp_ctx->i_idx;
                    ps_main_ctx->s_best_block = s_dummy;
                    turtledeflate_block_temp_stream_to_block_stream( ps_turtle, &s_dummy );
                }
            }
        }

        turtledeflate_block_deflate_refine_path_ctx( ps_turtle, ps_main_ctx, &s_nctx, pui8_data, i_data_size, i_exact_level > 1, 8, 2 );

        if( s_nctx.i_best_bits < ps_fp_ctx->i_best_bits )
        {
            ps_fp_ctx->s_block_model = s_nctx.s_block_model;
            ps_fp_ctx->i_best_bits = s_nctx.i_best_bits;
            ps_fp_ctx->i_model_bits = s_nctx.i_model_bits;
        }
    }
}


void turtledflate_decimate_models( turtledeflate_ctx_t *ps_turtle )
{
    bool b_equal;
    int32_t i_start_fp_idx, i_following_start_fp_idx, i_idx, i_num_decimated = 0;
    turtledeflate_squish_t *ps_squish = &ps_turtle->s_squish;
    turtledeflate_squish_path_ctx_t *ps_ctx, *ps_next_ctx;

    for( i_start_fp_idx = 0; i_start_fp_idx < ps_turtle->s_config.i_num_start_fp; i_start_fp_idx++ )
    {
        ps_ctx = &ps_squish->rgs_path_ctx[ i_start_fp_idx ];
        if( !ps_ctx->b_active )
        {
            continue;
        }
        for( i_following_start_fp_idx = i_start_fp_idx + 1; i_following_start_fp_idx < ps_turtle->s_config.i_num_start_fp; i_following_start_fp_idx++ )
        {
            ps_next_ctx = &ps_squish->rgs_path_ctx[ i_following_start_fp_idx ];
            if( !ps_next_ctx->b_active )
            {
                continue;
            }
            b_equal = true;
            for( i_idx = 0; i_idx < TURTLEDEFLATE_NUM_LENLITS; i_idx++ )
            {
                if( ps_ctx->s_block_model.rgi_count_lenlits[ i_idx ] != ps_next_ctx->s_block_model.rgi_count_lenlits[ i_idx ] )
                {
                    b_equal = false;
                    break;
                }
            }
            if( b_equal )
            {
                for( i_idx = 0; i_idx < TURTLEDEFLATE_NUM_DISTS; i_idx++ )
                {
                    if( ps_ctx->s_block_model.rgi_count_dists[ i_idx ] != ps_next_ctx->s_block_model.rgi_count_dists[ i_idx ] )
                    {
                        b_equal = false;
                        break;
                    }
                }
            }
            if( b_equal )
            {
                ps_ctx->b_active = false;
                i_num_decimated++;
                break;
            }
        }
    }

    /*fprintf( stderr, "decimated: %d\n", i_num_decimated );*/
}



int32_t turtledeflate_block_deflate_squish( turtledeflate_ctx_t *ps_turtle, turtledeflate_block_t *ps_block_, uint8_t *pui8_data, int32_t i_data_size, int32_t i_pre, int32_t *pi_chosen_start_fp )
{
    turtledeflate_squish_t *ps_squish = &ps_turtle->s_squish;
    turtledeflate_squish_path_ctx_t *ps_ctx;
    turtldeflate_best_blocks_t s_cost_ctx;
    int32_t i_idx, i_len, i_sublen, i_dist, i_stage, i_trace, i_fp_pre, i_start_fp_idx, i_refine_count, i_num_active_paths, i_without_inc, i_num_squish_models;
    int32_t i_bits, i_best_bits, i_worst_bits, i_worst_idx, i_exact_counter;
    turtledeflate_block_t s_recv_block, s_greedy_model;
    float64_t f64_weight_scale, f64_fp, f64_best_fp, f64_fp_scale, f64_start_fp_inc, f64_compressibility;


    turtledeflate_block_deflate_greedy( ps_turtle, ps_block_, pui8_data, i_data_size, i_pre ); /* sets up matches and first result */
    s_greedy_model = *ps_block_;
    i_best_bits = turtledeflate_get_estimated_block_bits( ps_turtle, s_greedy_model.rgi_count_lenlits, s_greedy_model.rgi_count_dists );

#define TURTLEDEFLATE_MIN_PRE_EXIT_ITER        1

#define TURTLEDEFLATE_MIN_PRE_EXIT_FP          ( ps_turtle->s_config.i_max_start_fp )

#define TURTLEDEFLATE_MIN_PRE2_EXIT_STEP       1
#define TURTLEDEFLATE_MIN_PRE2_EXIT_FP2        ( ps_turtle->s_config.i_max_start_fp + 3 )

#define TURTLEDEFLATE_FP_MIN_PRE_START_INC     ( 1.0 )
#define TURTLEDEFLATE_FP_MIN_START_INC         ( 1.0 )
#define TURTLEDEFLATE_FP_MAIN_INC              ( 1.0 )

#define TURTLEDEFLATE_INCDELAY_ONE             2

#define TURTLEDEFLATE_KICK_EM                  0

    * pi_chosen_start_fp = 0;

    s_cost_ctx.i_best_bits = 0x7fffffff;
    s_cost_ctx.i_best_exact_bits = 0x7fffffff;
    s_cost_ctx.i_best_model_bits = 0x7fffffff;

    /* still finding block boundaries ? */
    if( ps_turtle->i_blocksplitter_state < 3 )
    {
        turtledeflate_squish_path_ctx_t *rgps_ctx[ 3 ];
        int32_t i_return;

        for( i_idx = 0; i_idx < 3; i_idx++ )
        {
            rgps_ctx[ i_idx ] = &ps_squish->rgs_path_ctx[ i_idx ];
            rgps_ctx[ i_idx ]->b_active = true;
            rgps_ctx[ i_idx ]->i_idx = i_idx;
            rgps_ctx[ i_idx ]->f64_current_fp = 16.0;
            rgps_ctx[ i_idx ]->f64_last_best_fp = 16.0;
            rgps_ctx[ i_idx ]->i_best_bits = 0x7fffffff;
            rgps_ctx[ i_idx ]->i_model_bits = 0x7fffffff;
            rgps_ctx[ i_idx ]->s_block_model = s_greedy_model; /* greedy */
        }
        
        s_cost_ctx.i_best_bits = 0x7fffffff;
        s_cost_ctx.i_best_exact_bits = 0x7fffffff;
        s_cost_ctx.i_best_model_bits = 0x7fffffff;
        s_cost_ctx.i_best_path_ctx = 0;

        if( ps_turtle->i_blocksplitter_state == 2 )
        {
            turtledeflate_block_deflate_reseat_ctx_and_refine( ps_turtle, false, &s_cost_ctx, rgps_ctx[ 0 ], pui8_data, i_data_size, 1 );
        }
        else if( ps_turtle->i_blocksplitter_state == 1 )
        {
            turtledeflate_block_deflate_reseat_ctx_and_refine( ps_turtle, false, &s_cost_ctx, rgps_ctx[ 0 ], pui8_data, i_data_size, 0 );
        }
        else
        {
            turtledeflate_block_deflate_refine_path_ctx( ps_turtle, &s_cost_ctx, rgps_ctx[ 0 ], pui8_data, i_data_size, false, 8, 2 );
        }

        i_best_bits = s_cost_ctx.i_best_bits;
        *pi_chosen_start_fp = s_cost_ctx.i_best_path_ctx;
        *ps_block_ = s_cost_ctx.s_best_block;

        return i_best_bits;
    }


    /* main randomizing compression path: */

    /* Note if someone wonders: fp is the fixed point bit precision used in path tracing.
      Different precisions lead to different results, so we randomize using this.
      Models dont wander off far most of the time even at negative precisions. */

    for( i_start_fp_idx = 0; i_start_fp_idx < ps_turtle->s_config.i_num_start_fp; i_start_fp_idx++ )
    {
        ps_ctx = &ps_squish->rgs_path_ctx[ i_start_fp_idx ];
        memset( ps_ctx, 0, sizeof( *ps_ctx ) );

        ps_ctx->b_active = true;
        ps_ctx->i_idx = i_start_fp_idx;
        ps_ctx->f64_fp_start = ps_turtle->s_config.i_min_start_fp + ( ( ( ( float64_t ) i_start_fp_idx ) / ( ps_turtle->s_config.i_num_start_fp - 1 ) ) * ( ps_turtle->s_config.i_max_start_fp - ps_turtle->s_config.i_min_start_fp ) );
        ps_ctx->f64_current_fp = ps_ctx->f64_fp_start;
        ps_ctx->f64_last_best_fp = ps_ctx->f64_fp_start;
        ps_ctx->i_best_bits = 0x7fffffff;
        ps_ctx->i_model_bits = 0x7fffffff;
        ps_ctx->s_block_model = s_greedy_model;
    }

    f64_weight_scale = 1.0;

    /* set exit fp for first rough refinement step */
    for( i_idx = 0; i_idx < ps_turtle->s_config.i_num_start_fp; i_idx++ )
    {
        ps_squish->rgs_path_ctx[ i_idx ].f64_min_pre_exit_fp = TURTLEDEFLATE_MIN_PRE_EXIT_FP;
    }

    i_num_squish_models = ps_turtle->s_config.i_num_start_fp;
#if TURTLEDEFLATE_MIN_PRE2_EXIT_STEP
    /* copy current models to safety */
    for( i_idx = 0; i_idx < ps_turtle->s_config.i_num_start_fp; i_idx++ )
    {
        ps_squish->rgs_path_ctx[ i_idx + ps_turtle->s_config.i_num_start_fp ] = ps_squish->rgs_path_ctx[ i_idx ];
    }
    /* set new exit fp */
    for( i_idx = 0; i_idx < ps_turtle->s_config.i_num_start_fp; i_idx++ )
    {
        ps_squish->rgs_path_ctx[ i_idx ].f64_min_pre_exit_fp = TURTLEDEFLATE_MIN_PRE2_EXIT_FP2;
    }
    i_num_squish_models = ps_turtle->s_config.i_num_start_fp * 2;
#endif
    i_num_active_paths = i_num_squish_models;


    /* move to minimum precision TURTLEDEFLATE_MIN_PRE_EXIT_FP, or stay at current if above  */
    for( i_start_fp_idx = 0; i_start_fp_idx < i_num_squish_models; i_start_fp_idx++ )
    {
        int32_t i_model_bits, i_return = 0;
        float64_t f64_delta_fp;
        int32_t i_num_iteration;

        ps_ctx = &ps_squish->rgs_path_ctx[ i_start_fp_idx ];

        if( ps_ctx->b_active )
        {
            /*turtledeflate_block_deflate_refine_path_ctx( ps_turtle, &s_cost_ctx, ps_ctx, pui8_data, i_data_size, true, true, 2 );*/
        }

        f64_delta_fp = ( ps_ctx->f64_min_pre_exit_fp - ps_ctx->f64_current_fp );
        if( f64_delta_fp < 0.001 )
        {
            f64_delta_fp = 0.0; /* well, dont decrease */
        }

        i_num_iteration = floor( ( f64_delta_fp / TURTLEDEFLATE_FP_MIN_PRE_START_INC ) + 0.5 );
        if( i_num_iteration < TURTLEDEFLATE_MIN_PRE_EXIT_ITER )
        {
            i_num_iteration = TURTLEDEFLATE_MIN_PRE_EXIT_ITER;
        }
        f64_start_fp_inc = f64_delta_fp / i_num_iteration;

        i_refine_count = 0;
        for( i_stage = 0; i_stage < i_num_iteration; )
        {
            f64_fp = ps_ctx->f64_current_fp;
            i_return |= turtledeflate_block_deflate_try_model( ps_turtle, pui8_data, i_data_size, ps_ctx, &s_cost_ctx, f64_fp, false );

            i_refine_count++;
            if( i_refine_count >= TURTLEDEFLATE_INCDELAY_ONE )
            {
                i_refine_count = 0;
                i_stage++;
                ps_ctx->f64_current_fp += f64_start_fp_inc;
            }
        }
        if( ps_turtle->s_config.i_verbose >= TURTLEDEFLATE_VERBOSE_STARTFP )
        {
            fprintf( stderr, "f p_iteration %d: %d/%d/%d (%.2f), %s/%s/%s\n", i_start_fp_idx, ps_ctx->i_model_bits, ps_ctx->i_best_bits, s_cost_ctx.i_best_bits, ps_ctx->f64_current_fp,
                i_return & 1 ? "***" : "___", i_return & 2 ? "***" : "___", i_return & 4 ? "***" : "___" );
        }
    }
    if( ps_turtle->s_config.i_verbose >= TURTLEDEFLATE_VERBOSE_STARTFP )
    {
        fprintf( stderr, "-----------\n" );
    }


    turtledflate_decimate_models( ps_turtle );

#if 1
    /* kick some, maybe */
    while( i_num_active_paths > 3 && TURTLEDEFLATE_KICK_EM )
    {
        i_worst_bits = 0;
        i_worst_idx = 0;
        for( i_start_fp_idx = 0; i_start_fp_idx < i_num_squish_models; i_start_fp_idx++ )
        {
            ps_ctx = &ps_squish->rgs_path_ctx[ i_start_fp_idx ];
            if( !ps_ctx->b_active )
            {
                continue;
            }

            i_bits = ps_ctx->i_best_bits;

            if( i_bits > i_worst_bits )
            {
                i_worst_bits = i_bits;
                i_worst_idx = i_start_fp_idx;
            }
        }
        ps_squish->rgs_path_ctx[ i_worst_idx ].b_active = false;
        i_num_active_paths--;
    }
#endif

    /* main refinement, if any */
    for( i_start_fp_idx = 0; i_start_fp_idx < i_num_squish_models; i_start_fp_idx++ )
    {
        ps_ctx = &ps_squish->rgs_path_ctx[ i_start_fp_idx ];
        ps_ctx->f64_current_fp = 16.0;
        if( ps_ctx->b_active )
        {
            turtledeflate_block_deflate_reseat_ctx_and_refine( ps_turtle, true, &s_cost_ctx, ps_ctx, pui8_data, i_data_size, 2 );
        }
    }

    if( 1 )
    {
        int32_t i_return, i_rcount = 0;
        turtledeflate_block_t s_block = s_cost_ctx.s_best_block;
        while( 1 )
        {
            i_return = turtledeflate_block_deflate_try_block_model_exact( ps_turtle, &s_cost_ctx, pui8_data, i_data_size, &s_block, true );
            if( !i_return  )
            {
                i_rcount++;
                if( i_rcount >= 2 )
                {
                    break;
                }
            }
            else
            {
                i_rcount = 0;
            }
        }
    }

    i_best_bits = s_cost_ctx.i_best_bits;
    *pi_chosen_start_fp = s_cost_ctx.i_best_path_ctx;
    *ps_block_ = s_cost_ctx.s_best_block;

    /*fprintf( stderr, "sd: %d -> comp %f\n", *pi_best_from_start_fp, f64_compressibility * 8.0 );*/

    return i_best_bits;
}


void turtledeflate_add_global_entry_to_hist( turtledeflate_ctx_t *ps_turtle, turtledeflate_hist_t *ps_hist, int32_t i_idx )
{
    turtledeflate_stream_entry_t *ps_entry = &ps_turtle->ps_global_stream[ i_idx ];

    if( ps_entry->ui16_dist == 0 )
    {
        ps_hist->rgi_lenlit_hist[ ps_entry->ui16_litlen ]++;
    }
    else
    {
        int32_t i_lens, i_dists;
        i_lens = turtledeflate_get_len_symbol( ps_entry->ui16_litlen );
        i_dists = turtledeflate_get_dist_symbol( ps_entry->ui16_dist );
        ps_hist->rgi_lenlit_hist[ i_lens ]++;
        ps_hist->rgi_dist_hist[ i_dists ]++;
    }
}

void turtledeflate_create_global_histogram( turtledeflate_ctx_t *ps_turtle )
{
    int32_t i_idx, i_hist_idx;
    turtledeflate_hist_t s_rhist, *ps_hist;
    turtledeflate_block_t *ps_gblock = &ps_turtle->s_global_block;

    memset( &s_rhist, 0, sizeof( turtledeflate_hist_t ) );

    i_hist_idx = 0;
    ps_hist = ps_turtle->ps_global_hist;
    for( i_idx = 0; i_idx < ps_gblock->i_num_entries; i_idx++ )
    {
        if( ( i_idx % TURTLEDEFLATE_HIST_INTERVAL ) == 0 )
        {
            ps_hist[ i_hist_idx++ ] = s_rhist;
        }
        
        turtledeflate_add_global_entry_to_hist( ps_turtle, &s_rhist, i_idx );
    }
}


void turtledeflate_get_partial_histogram( turtledeflate_ctx_t *ps_turtle, int32_t i_from, int32_t i_to, turtledeflate_hist_t *ps_hist )
{
    int32_t i_idx, i_ridx, i_rto, i_hist_start, i_hist_end, i_sub_hist = 0;
    turtledeflate_hist_t s_hist, *ps_ghist;

    memset( &s_hist, 0, sizeof( s_hist ) );

    ps_ghist = ps_turtle->ps_global_hist;

    i_hist_start = ( i_from + TURTLEDEFLATE_HIST_INTERVAL - 1 ) / TURTLEDEFLATE_HIST_INTERVAL;
    i_hist_end = i_to / TURTLEDEFLATE_HIST_INTERVAL;
    if( i_hist_end < i_hist_start )
    {
        i_hist_end = i_hist_start;
    }

    i_ridx = i_from;
    i_rto = i_hist_start * TURTLEDEFLATE_HIST_INTERVAL;
    if( i_rto > i_to )
    {
        i_rto = i_to;
    }

    for( ; i_ridx < i_rto; i_ridx++ )
    {
        turtledeflate_add_global_entry_to_hist( ps_turtle, &s_hist, i_ridx );
    }

    for( i_idx = 0; i_idx < TURTLEDEFLATE_NUM_LENLITS; i_idx++ )
    {
        s_hist.rgi_lenlit_hist[ i_idx ] += ps_ghist[ i_hist_end ].rgi_lenlit_hist[ i_idx ] - ps_ghist[ i_hist_start ].rgi_lenlit_hist[ i_idx ];
    }
    for( i_idx = 0; i_idx < TURTLEDEFLATE_NUM_DISTS; i_idx++ )
    {
        s_hist.rgi_dist_hist[ i_idx ] += ps_ghist[ i_hist_end ].rgi_dist_hist[ i_idx ] - ps_ghist[ i_hist_start ].rgi_dist_hist[ i_idx ];
    }
    i_ridx += ( i_hist_end - i_hist_start ) * TURTLEDEFLATE_HIST_INTERVAL;

    /* i_ridx might be at i_to already */
    for( ; i_ridx < i_to; i_ridx++ ) 
    {
        turtledeflate_add_global_entry_to_hist( ps_turtle, &s_hist, i_ridx );
    }

    *ps_hist = s_hist;
}


int32_t turtledeflate_get_histogram_bit_cost( turtledeflate_ctx_t *ps_turtle, turtledeflate_hist_t *ps_hist )
{
    int32_t i_compressed_size;

    i_compressed_size = turtledeflate_get_estimated_block_bits( ps_turtle, ps_hist->rgi_lenlit_hist, ps_hist->rgi_dist_hist );

    return i_compressed_size;
}


int32_t turtledeflate_get_global_sub_block_bit_cost( turtledeflate_ctx_t *ps_turtle, int32_t i_from, int32_t i_to )
{
    turtledeflate_hist_t s_hist;

    turtledeflate_get_partial_histogram( ps_turtle, i_from, i_to, &s_hist );

    s_hist.rgi_lenlit_hist[ 256 ] = 1; /* end of block */

    return turtledeflate_get_histogram_bit_cost( ps_turtle, &s_hist );
}


int32_t turtledeflate_get_min_max( int32_t rgi_costf[], int32_t i_size, int32_t i_idx_, bool b_left, bool b_min )
{
    int32_t i_idx, i_best_costf, i_best_idx, i_dir;

    i_best_costf = rgi_costf[ i_idx_ ];
    i_best_idx = i_idx_;
    i_dir = b_left ? -1 : 1;

    for( i_idx = i_idx_ + i_dir; i_idx >= 0 && i_idx < i_size; i_idx += i_dir )
    {
        if( b_min )
        {
            if( rgi_costf[ i_idx ] < i_best_costf )
            {
                i_best_costf = rgi_costf[ i_idx ];
                i_best_idx = i_idx;
            }
        }
        else
        {
            if( rgi_costf[ i_idx ] > i_best_costf )
            {
                i_best_costf = rgi_costf[ i_idx ];
                i_best_idx = i_idx;
            }
        }
    }
    return i_best_idx;
}


int32_t turtledeflate_best_block_split( turtledeflate_ctx_t *ps_turtle, int32_t i_from, int32_t i_to, int32_t rgi_split_block_bits_[ 2 ], bool b_force_split )
{
    int32_t i_idx, i_split, i_range, i_rfrom, i_rto, i_bits, i_best_bits, i_best_bits_iter, i_best_split_idx, i_best_split, i_filter_range, i_filter_idx;
    int32_t rgi_split_block_bits[ 2 ], rgi_best_split_block_bits[ 2 ];
    int32_t rgi_split_points[ TURTLEDEFLATE_BSPLIT_MAX_NUM_POINTS + 1 ];
    int32_t rgi_split_points_costs[ TURTLEDEFLATE_BSPLIT_MAX_NUM_POINTS + 1 ];
    int32_t rgi_filtered_split_points_costs[ TURTLEDEFLATE_BSPLIT_MAX_NUM_POINTS + 1 ];
    int32_t i_num_pushed_split_point_ranges = 0;
    int32_t rgi_pushed_split_point_ranges[ 64 ][ 2 ];

    i_best_bits = turtledeflate_get_global_sub_block_bit_cost( ps_turtle, i_from, i_to );
    i_best_split = i_to;

    if( b_force_split )
    {
        i_best_bits = 0x7fffffff;
    }

    i_rfrom = i_from + 1;
    i_rto = i_to - 1;
    i_range = ( i_rto - i_rfrom );

    if( i_range >= ps_turtle->s_config.i_block_splitter_min_range_for_points ) /* small blocks are almost random */
    {
        while( 1 )
        {
and_run_the_other_side:;
            for( i_idx = 0; i_idx <= ps_turtle->s_config.i_block_splitter_num_points; i_idx++ )
            {
                i_split = i_rfrom + ( ( i_range * i_idx ) / ps_turtle->s_config.i_block_splitter_num_points );

                if( i_split != i_from )
                {
                    rgi_split_block_bits[ 0 ] = turtledeflate_get_global_sub_block_bit_cost( ps_turtle, i_from, i_split );
                }
                else
                {
                    rgi_split_block_bits[ 0 ] = 0;
                }

                if( i_split != i_to )
                {
                    rgi_split_block_bits[ 1 ] = turtledeflate_get_global_sub_block_bit_cost( ps_turtle, i_split, i_to );
                }
                else
                {
                    rgi_split_block_bits[ 1 ] = 0;
                }
                i_bits = rgi_split_block_bits[ 0 ] + rgi_split_block_bits[ 1 ];

                rgi_split_points[ i_idx ] = i_split;
                rgi_split_points_costs[ i_idx ] = i_bits;
            }


            i_best_split_idx = 0;
            i_best_bits_iter = 0x7fffffff;

            for( i_idx = 0; i_idx <= ps_turtle->s_config.i_block_splitter_num_points; i_idx++ )
            {
                int32_t i_bits, i_shift;
#if 1
                i_filter_range = TURTLE_MAX( ps_turtle->s_config.i_block_splitter_num_points / 10, 1 );

                i_bits = 0;
                for( i_filter_idx = -i_filter_range; i_filter_idx <= i_filter_range; i_filter_idx++ )
                {
                    int32_t i_sample;
                    i_sample = TURTLE_MAX( 0, TURTLE_MIN( ps_turtle->s_config.i_block_splitter_num_points, i_idx + i_filter_idx ) );
                    i_bits += rgi_split_points_costs[ i_sample ];
                }
#else
                i_bits = rgi_split_points_costs[ i_idx ];
#endif

                rgi_filtered_split_points_costs[ i_idx ] = i_bits;

                if( i_bits < i_best_bits_iter )
                {
                    i_best_bits_iter = i_bits;
                    i_best_split_idx = i_idx;
                }
            }

            bool b_normal_advance = true;
            /* does this even work ? */
            if( i_num_pushed_split_point_ranges < 63 && i_range > ps_turtle->s_config.i_block_splitter_num_points )
            {
                int32_t i_min_middle, i_max_left, i_max_right, i_min_right;

                i_min_middle = turtledeflate_get_min_max( rgi_filtered_split_points_costs, ps_turtle->s_config.i_block_splitter_num_points + 1, 0, false, true );
                i_best_split_idx = i_min_middle;

                if( i_min_middle <= ( ps_turtle->s_config.i_block_splitter_num_points / 9 ) )
                {
                    i_max_right = turtledeflate_get_min_max( rgi_filtered_split_points_costs, ps_turtle->s_config.i_block_splitter_num_points + 1, i_min_middle, false, false );
                    if( i_max_right - 1 > i_min_middle && i_max_right < ps_turtle->s_config.i_block_splitter_num_points - ( ps_turtle->s_config.i_block_splitter_center_dist / 3 ) )
                    {
                        rgi_pushed_split_point_ranges[ i_num_pushed_split_point_ranges ][ 0 ] = rgi_split_points[ TURTLE_MAX( 0, i_max_right ) ];
                        rgi_pushed_split_point_ranges[ i_num_pushed_split_point_ranges ][ 1 ] = rgi_split_points[ ps_turtle->s_config.i_block_splitter_num_points ];
                        i_num_pushed_split_point_ranges++;
                    }
                    else
                    {
                        int32_t i_idx;
                        i_idx = 0;
                    }
                }
                else if( ( ps_turtle->s_config.i_block_splitter_num_points - i_min_middle ) <= ( ps_turtle->s_config.i_block_splitter_num_points / 9 ) )
                {
                    i_max_left = turtledeflate_get_min_max( rgi_filtered_split_points_costs, ps_turtle->s_config.i_block_splitter_num_points + 1, i_min_middle, true, false );
                    if( i_max_left + 1 < i_min_middle && i_max_left > 1 + ( ps_turtle->s_config.i_block_splitter_center_dist / 3 ) )
                    {
                        rgi_pushed_split_point_ranges[ i_num_pushed_split_point_ranges ][ 0 ] = rgi_split_points[ 0 ];
                        rgi_pushed_split_point_ranges[ i_num_pushed_split_point_ranges ][ 1 ] = rgi_split_points[ TURTLE_MAX( 0, i_max_left ) ];
                        i_num_pushed_split_point_ranges++;
                    }
                    else
                    {
                        int32_t i_idx;
                        i_idx = 0;
                    }
                }
                else
                {

                }
            }
            /*fprintf( stderr, "nump: %d, rng %d\n", i_num_pushed_split_point_ranges, i_range );*/

            if( i_range <= ps_turtle->s_config.i_block_splitter_min_range_for_points )
            {
                break;
            }
            else
            {
                i_rfrom = rgi_split_points[ TURTLE_MAX( 0, i_best_split_idx - ps_turtle->s_config.i_block_splitter_center_dist ) ];
                i_rto = rgi_split_points[ TURTLE_MIN( ps_turtle->s_config.i_block_splitter_num_points, i_best_split_idx + ps_turtle->s_config.i_block_splitter_center_dist ) ];
            }

            i_range = ( i_rto - i_rfrom );
        }
    }

    for( i_split = i_rfrom; i_split <= i_rto; i_split++ )
    {
        if( i_split != i_from )
        {
            rgi_split_block_bits[ 0 ] = turtledeflate_get_global_sub_block_bit_cost( ps_turtle, i_from, i_split );
        }
        else
        {
            rgi_split_block_bits[ 0 ] = 0;
        }
        if( i_split != i_to )
        {
            rgi_split_block_bits[ 1 ] = turtledeflate_get_global_sub_block_bit_cost( ps_turtle, i_split, i_to );
        }
        else
        {
            rgi_split_block_bits[ 1 ] = 0;
        }
        i_bits = rgi_split_block_bits[ 0 ] + rgi_split_block_bits[ 1 ];

        if( i_bits < i_best_bits )
        {
            i_best_bits = i_bits;
            i_best_split = i_split;
            memcpy( rgi_best_split_block_bits, rgi_split_block_bits, sizeof( rgi_split_block_bits ) );
        }
    }

    if( i_num_pushed_split_point_ranges )
    {
        i_num_pushed_split_point_ranges--;
        i_rfrom = rgi_pushed_split_point_ranges[ i_num_pushed_split_point_ranges ][ 0 ];
        i_rto = rgi_pushed_split_point_ranges[ i_num_pushed_split_point_ranges ][ 1 ];
        i_range = ( i_rto - i_rfrom );
        goto and_run_the_other_side;
    }

    if( i_best_split == i_from || i_best_split == i_to )
    {
        i_best_split = -1;
    }
    else
    {
        memcpy( rgi_split_block_bits_, rgi_best_split_block_bits, sizeof( rgi_best_split_block_bits ) );
    }

    return i_best_split;
}

int32_t turtledeflate_block_splitting_bits( turtledeflate_ctx_t *ps_turtle, int32_t rgi_block_boundaries_deflated[ ], int32_t i_num_sub_blocks )
{
    int32_t i_idx, i_estimated_bits;

    i_estimated_bits = 0;
    for( i_idx = 0; i_idx < i_num_sub_blocks; i_idx++ )
    {
        i_estimated_bits += turtledeflate_get_global_sub_block_bit_cost( ps_turtle, rgi_block_boundaries_deflated[ i_idx ], rgi_block_boundaries_deflated[ i_idx + 1 ] ); /* FIXME: oh oh ! speed for stats ! */
    }

    return i_estimated_bits;
}


typedef struct
{
    int32_t i_num_sub_blocks;
    int32_t rgi_block_boundaries_deflated[ TURTLEDEFLATE_MAX_SUBBLOCKS + 1 ];
    bool rgb_got_moved[ TURTLEDEFLATE_MAX_SUBBLOCKS ];
    bool rgb_can_split[ TURTLEDEFLATE_MAX_SUBBLOCKS ];
    bool rgb_got_split[ TURTLEDEFLATE_MAX_SUBBLOCKS ];
    bool rgb_got_merged[ TURTLEDEFLATE_MAX_SUBBLOCKS ];
    int32_t rgi_block_splits[ TURTLEDEFLATE_MAX_SUBBLOCKS ];
    int32_t rgi_block_splits_bits[ TURTLEDEFLATE_MAX_SUBBLOCKS ][ 2 ];
    int32_t rgi_block_merge_bits[ TURTLEDEFLATE_MAX_SUBBLOCKS ];
    int32_t rgi_sub_block_bits[ TURTLEDEFLATE_MAX_SUBBLOCKS ];
} turtledeflate_block_splitter_state_t;


int32_t turtledeflate_block_splitting_split_bits( turtledeflate_ctx_t *ps_turtle, int32_t i_bits )
{
    return i_bits;
}


bool turtledeflate_block_splitting_reseat( turtledeflate_ctx_t *ps_turtle, turtledeflate_block_splitter_state_t *ps_state )
{
    bool b_reseated, b_reseated_iter;
    int32_t i_start_idx, i_split, i_idx;
    int32_t rgi_block_split_bits[ 2 ];

    b_reseated = false;
    b_reseated_iter = true;
    while( b_reseated_iter )
    {
        b_reseated_iter = false;
        for( i_start_idx = 0; i_start_idx < 2; i_start_idx++ )
        {
            for( i_idx = i_start_idx; i_idx < ps_state->i_num_sub_blocks - 1; i_idx += 2 )
            {
                if( !ps_state->rgb_got_moved[ i_idx ] )
                {
                    i_split = turtledeflate_best_block_split( ps_turtle, ps_state->rgi_block_boundaries_deflated[ i_idx ], ps_state->rgi_block_boundaries_deflated[ i_idx + 2 ], rgi_block_split_bits, false );
                    if( i_split > 0 && i_split != ps_state->rgi_block_boundaries_deflated[ i_idx + 1 ] ) /* merge back if split <= 0 ? */
                    {
                        rgi_block_split_bits[ 0 ] = turtledeflate_block_splitting_split_bits( ps_turtle, rgi_block_split_bits[ 0 ] );
                        rgi_block_split_bits[ 1 ] = turtledeflate_block_splitting_split_bits( ps_turtle, rgi_block_split_bits[ 1 ] );
                        if( rgi_block_split_bits[ 0 ] + rgi_block_split_bits[ 1 ] < ps_state->rgi_sub_block_bits[ i_idx ] + ps_state->rgi_sub_block_bits[ i_idx + 1 ] )
                        {
                            b_reseated = true;
                            b_reseated_iter = true;
                            ps_state->rgi_block_boundaries_deflated[ i_idx + 1 ] = i_split;
                            ps_state->rgb_got_split[ i_idx ] = false;
                            ps_state->rgb_got_split[ i_idx + 1 ] = false;
                            ps_state->rgb_can_split[ i_idx ] = true;
                            ps_state->rgb_can_split[ i_idx + 1 ] = true;
                            if( i_idx > 0 )
                            {
                                ps_state->rgb_got_merged[ i_idx - 1 ] = false;
                                ps_state->rgb_got_moved[ i_idx - 1 ] = false;
                            }
                            ps_state->rgb_got_merged[ i_idx ] = false;
                            ps_state->rgb_got_merged[ i_idx + 1 ] = false;
                            ps_state->rgb_got_moved[ i_idx + 1 ] = false;
                            ps_state->rgi_sub_block_bits[ i_idx ] = rgi_block_split_bits[ 0 ];
                            ps_state->rgi_sub_block_bits[ i_idx + 1 ] = rgi_block_split_bits[ 1 ];
                        }
                    }
                    ps_state->rgb_got_moved[ i_idx ] = true;
                }
            }
        }
    }

    return b_reseated;
}


bool turtledeflate_block_splitting_split( turtledeflate_ctx_t *ps_turtle, turtledeflate_block_splitter_state_t *ps_state, bool b_force )
{
    bool b_found_split = false;
    int32_t i_start_idx, i_split, i_idx, i_best_saved_bits, i_saved_bits, i_best_split_block, i_best_split;
    int32_t rgi_block_split_bits[ 2 ], rgi_best_block_split_bits[ 2 ];

    i_best_saved_bits = b_force ? -0x7fffffff : 0;

    for( i_idx = 0; i_idx < ps_state->i_num_sub_blocks; i_idx++ )
    {
        /* cache split ... */
        if( !b_force )
        {
            if( !ps_state->rgb_got_split[ i_idx ] )
            {
                i_split = turtledeflate_best_block_split( ps_turtle, ps_state->rgi_block_boundaries_deflated[ i_idx ], ps_state->rgi_block_boundaries_deflated[ i_idx + 1 ], rgi_block_split_bits, false );
                rgi_block_split_bits[ 0 ] = turtledeflate_block_splitting_split_bits( ps_turtle, rgi_block_split_bits[ 0 ] );
                rgi_block_split_bits[ 1 ] = turtledeflate_block_splitting_split_bits( ps_turtle, rgi_block_split_bits[ 1 ] );
                ps_state->rgb_got_split[ i_idx ] = true;
                if( i_split > 0 )
                {
                    ps_state->rgb_can_split[ i_idx ] = true;
                    ps_state->rgi_block_splits[ i_idx ] = i_split;
                    memcpy( ps_state->rgi_block_splits_bits[ i_idx ], rgi_block_split_bits, sizeof( rgi_block_split_bits ) );
                }
                else
                {
                    ps_state->rgb_can_split[ i_idx ] = false;
                }
            }
        }
        else
        {
            if( ( ps_state->rgi_block_boundaries_deflated[ i_idx + 1 ] - ps_state->rgi_block_boundaries_deflated[ i_idx ] ) >= 2 )
            {
                i_split = turtledeflate_best_block_split( ps_turtle, ps_state->rgi_block_boundaries_deflated[ i_idx ], ps_state->rgi_block_boundaries_deflated[ i_idx + 1 ], rgi_block_split_bits, true );
                rgi_block_split_bits[ 0 ] = turtledeflate_block_splitting_split_bits( ps_turtle, rgi_block_split_bits[ 0 ] );
                rgi_block_split_bits[ 1 ] = turtledeflate_block_splitting_split_bits( ps_turtle, rgi_block_split_bits[ 1 ] );
                ps_state->rgb_got_split[ i_idx ] = true;
                if( i_split > 0 )
                {
                    ps_state->rgb_can_split[ i_idx ] = true;
                    ps_state->rgi_block_splits[ i_idx ] = i_split;
                    memcpy( ps_state->rgi_block_splits_bits[ i_idx ], rgi_block_split_bits, sizeof( rgi_block_split_bits ) );
                }
                else
                {
                    ps_state->rgb_can_split[ i_idx ] = false;
                }
                ps_state->rgb_got_split[ i_idx ] = false;
            }
            else
            {
                ps_state->rgb_can_split[ i_idx ] = false;
            }
            
        }

        /* check what we save by splitting here */
        if( ps_state->rgb_can_split[ i_idx ] )
        {
            i_split = ps_state->rgi_block_splits[ i_idx ];
            if( i_split > 0 )
            {
                memcpy( rgi_block_split_bits, ps_state->rgi_block_splits_bits[ i_idx ], sizeof( rgi_block_split_bits ) );

                i_saved_bits = ps_state->rgi_sub_block_bits[ i_idx ] - ( rgi_block_split_bits[ 0 ] + rgi_block_split_bits[ 1 ] );

                if( i_saved_bits > i_best_saved_bits )
                {
                    i_best_split_block = i_idx;
                    i_best_split = i_split;
                    memcpy( rgi_best_block_split_bits, rgi_block_split_bits, sizeof( rgi_block_split_bits ) );
                    i_best_saved_bits = i_saved_bits;
                    b_found_split = true;
                }
            }
            else
            {
                ps_state->rgb_can_split[ i_idx ] = false; /* huh ? */
            }
        }
    }
    if( b_found_split )
    {
        ps_state->i_num_sub_blocks++;
        ps_state->rgi_block_boundaries_deflated[ ps_state->i_num_sub_blocks ] = ps_state->rgi_block_boundaries_deflated[ ps_state->i_num_sub_blocks - 1 ];
        for( i_idx = ps_state->i_num_sub_blocks - 1; i_idx >= i_best_split_block + 1; i_idx-- )
        {
            ps_state->rgb_got_split[ i_idx ] = ps_state->rgb_got_split[ i_idx - 1 ];
            ps_state->rgb_can_split[ i_idx ] = ps_state->rgb_can_split[ i_idx - 1 ];
            ps_state->rgb_got_merged[ i_idx ] = ps_state->rgb_got_merged[ i_idx - 1 ];
            ps_state->rgb_got_moved[ i_idx ] = ps_state->rgb_got_moved[ i_idx - 1 ];
            ps_state->rgi_block_splits[ i_idx ] = ps_state->rgi_block_splits[ i_idx - 1 ];
            memcpy( ps_state->rgi_block_splits_bits[ i_idx ], ps_state->rgi_block_splits_bits[ i_idx - 1 ], sizeof( rgi_block_split_bits ) );
            ps_state->rgi_block_merge_bits[ i_idx ] = ps_state->rgi_block_merge_bits[ i_idx - 1 ];
            ps_state->rgi_block_boundaries_deflated[ i_idx + 1 ] = ps_state->rgi_block_boundaries_deflated[ i_idx ];
            ps_state->rgi_sub_block_bits[ i_idx ] = ps_state->rgi_sub_block_bits[ i_idx - 1 ];
        }

        ps_state->rgb_got_split[ i_best_split_block ] = false;
        ps_state->rgb_got_split[ i_best_split_block + 1 ] = false;
        ps_state->rgb_can_split[ i_best_split_block ] = true;
        ps_state->rgb_can_split[ i_best_split_block + 1 ] = true;
        if( i_best_split_block > 0 )
        {
            ps_state->rgb_got_merged[ i_best_split_block - 1 ] = false;
            ps_state->rgb_got_moved[ i_best_split_block - 1 ] = false;
        }
        ps_state->rgb_got_merged[ i_best_split_block ] = false;
        ps_state->rgb_got_merged[ i_best_split_block + 1 ] = false;
        ps_state->rgb_got_moved[ i_best_split_block ] = false; /* well, its true actually as we just did an optimal split here ? */
        ps_state->rgb_got_moved[ i_best_split_block + 1 ] = false;
        if( i_best_split_block + 2 < ps_state->i_num_sub_blocks )
        {
            ps_state->rgb_got_moved[ i_best_split_block + 2 ] = false;
        }
        ps_state->rgi_sub_block_bits[ i_best_split_block ] = rgi_best_block_split_bits[ 0 ];
        ps_state->rgi_sub_block_bits[ i_best_split_block + 1 ] = rgi_best_block_split_bits[ 1 ];
        ps_state->rgi_block_boundaries_deflated[ i_best_split_block + 1 ] = i_best_split;
    }

    return b_found_split;
}


bool turtledeflate_block_splitting_merge( turtledeflate_ctx_t *ps_turtle, turtledeflate_block_splitter_state_t *ps_state )
{
    bool b_found_merge = false;
    int32_t i_idx, i_best_merge_block, i_best_merge_bits, i_saved_bits;
    int32_t i_best_saved_bits = 0;
    int32_t rgi_block_split_bits[ 2 ];

    for( i_idx = 0; i_idx < ps_state->i_num_sub_blocks - 1; i_idx++ )
    {
        int32_t i_merge_bits;
        if( !ps_state->rgb_got_merged[ i_idx ] )
        {
            i_merge_bits = turtledeflate_get_global_sub_block_bit_cost( ps_turtle, ps_state->rgi_block_boundaries_deflated[ i_idx ], ps_state->rgi_block_boundaries_deflated[ i_idx + 2 ] );
            ps_state->rgi_block_merge_bits[ i_idx ] = i_merge_bits;
            ps_state->rgb_got_merged[ i_idx ] = true;
        }
        else
        {
            i_merge_bits = ps_state->rgi_block_merge_bits[ i_idx ];
        }
        i_saved_bits = ( ps_state->rgi_sub_block_bits[ i_idx ] + ps_state->rgi_sub_block_bits[ i_idx + 1 ] ) - i_merge_bits;
        if( i_saved_bits > i_best_saved_bits )
        {
            i_best_merge_block = i_idx;
            i_best_saved_bits = i_saved_bits;
            i_best_merge_bits = i_merge_bits;
            b_found_merge = true;
        }
    }
    if( b_found_merge )
    {
        ps_state->i_num_sub_blocks--;
        for( i_idx = i_best_merge_block + 1; i_idx < ps_state->i_num_sub_blocks; i_idx++ )
        {
            ps_state->rgb_got_split[ i_idx ] = ps_state->rgb_got_split[ i_idx + 1 ];
            ps_state->rgb_can_split[ i_idx ] = ps_state->rgb_can_split[ i_idx + 1 ];
            ps_state->rgi_block_splits[ i_idx ] = ps_state->rgi_block_splits[ i_idx + 1 ];
            ps_state->rgb_got_moved[ i_idx ] = ps_state->rgb_got_moved[ i_idx + 1 ];
            memcpy( ps_state->rgi_block_splits_bits[ i_idx ], ps_state->rgi_block_splits_bits[ i_idx + 1 ], sizeof( rgi_block_split_bits ) );
            ps_state->rgi_block_boundaries_deflated[ i_idx ] = ps_state->rgi_block_boundaries_deflated[ i_idx + 1 ];
            ps_state->rgi_sub_block_bits[ i_idx ] = ps_state->rgi_sub_block_bits[ i_idx + 1 ];
            ps_state->rgi_block_merge_bits[ i_idx ] = ps_state->rgi_block_merge_bits[ i_idx + 1 ];
        }
        ps_state->rgi_block_boundaries_deflated[ i_idx ] = ps_state->rgi_block_boundaries_deflated[ i_idx + 1 ];

        ps_state->rgb_got_split[ i_best_merge_block ] = false;
        ps_state->rgb_can_split[ i_best_merge_block ] = true;
        if( i_best_merge_block > 0 )
        {
            ps_state->rgb_got_merged[ i_best_merge_block - 1 ] = false;
            ps_state->rgb_got_moved[ i_best_merge_block - 1 ] = false;

        }
        ps_state->rgb_got_merged[ i_best_merge_block ] = false;
        ps_state->rgb_got_moved[ i_best_merge_block ] = false;
        if( i_best_merge_block + 1 < ps_state->i_num_sub_blocks )
        {
            ps_state->rgb_got_moved[ i_best_merge_block + 1 ] = false;
        }
        ps_state->rgi_sub_block_bits[ i_best_merge_block ] = i_best_merge_bits;
    }

    return b_found_merge;
}


int32_t turtledeflate_block_splitting_state_bits( turtledeflate_block_splitter_state_t *ps_state )
{
    int32_t i_bits, i_idx;

    i_bits = 0;
    for( i_idx = 0; i_idx < ps_state->i_num_sub_blocks; i_idx++ )
    {
        i_bits += ps_state->rgi_sub_block_bits[ i_idx ];
    }

    return i_bits;
}


int32_t turtledeflate_block_splitting( turtledeflate_ctx_t *ps_turtle, int32_t rgi_block_boundaries_deflated[ ], int32_t i_num_sub_blocks )
{
    
    bool b_found_split, b_found_merge;
    int32_t i_idx, i_best_split, i_best_split_block, i_split, i_saved_bits, i_best_saved_bits, i_iteration;
    turtledeflate_block_splitter_state_t *ps_state, s_state;

    memset( &s_state, 0, sizeof( s_state ) );
    ps_state = &s_state;
    ps_state->i_num_sub_blocks = i_num_sub_blocks;
    memcpy( ps_state->rgi_block_boundaries_deflated, rgi_block_boundaries_deflated, sizeof( ps_state->rgi_block_boundaries_deflated ) );

    for( i_idx = 0; i_idx < ps_state->i_num_sub_blocks; i_idx++ )
    {
        ps_state->rgb_can_split[ i_idx ] = true;
        ps_state->rgb_got_moved[ i_idx ] = false;
        ps_state->rgb_got_split[ i_idx ] = false;
        ps_state->rgb_got_merged[ i_idx ] = false;
        ps_state->rgi_sub_block_bits[ i_idx ] = turtledeflate_get_global_sub_block_bit_cost( ps_turtle, ps_state->rgi_block_boundaries_deflated[ i_idx ], ps_state->rgi_block_boundaries_deflated[ i_idx + 1 ] );
    }

    i_iteration = 0;
    while( i_iteration++ < ps_turtle->s_config.i_max_internal_block_splitter_iterations )
    {
        if( ps_state->i_num_sub_blocks > 1 )
        {
            turtledeflate_block_splitting_reseat( ps_turtle, ps_state );
        }

        b_found_split = false;
        if( ps_state->i_num_sub_blocks < ps_turtle->s_config.i_maximum_subblocks )
        {
            b_found_split = turtledeflate_block_splitting_split( ps_turtle, ps_state, false );
        }


        b_found_merge = false;
        if( ps_state->i_num_sub_blocks > 1 )
        {
            b_found_merge = turtledeflate_block_splitting_merge( ps_turtle, ps_state );
        }

        if( !b_found_split && !b_found_merge )
        {
            break;
        }
    }

    if( ps_turtle->s_config.i_verbose >= TURTLEDEFLATE_VERBOSE_BLOCK )
    {
        fprintf( stderr, "blocksplitter: %d internal iterations\n", i_iteration );
    }

    if( ps_turtle->s_config.b_block_splitter_push_split && ps_turtle->i_blocksplitter_state < 2 )
    {
        int32_t i_best_bits, i_bits, i_pushed = 0;

        turtledeflate_block_splitter_state_t *ps_push_state, s_push_state;

        i_best_bits = turtledeflate_block_splitting_state_bits( ps_state );

        s_push_state = s_state;
        ps_push_state = &s_push_state;

        while( 1 )
        {
            b_found_split = false;
            if( ps_push_state->i_num_sub_blocks < ps_turtle->s_config.i_maximum_subblocks )
            {
                b_found_split = turtledeflate_block_splitting_split( ps_turtle, ps_push_state, true );
            }
            if( b_found_split )
            {
                turtledeflate_block_splitting_reseat( ps_turtle, ps_push_state );

                i_bits = turtledeflate_block_splitting_state_bits( ps_push_state );
                if( i_bits < i_best_bits )
                {
                    if( ps_turtle->s_config.i_verbose >= TURTLEDEFLATE_VERBOSE_BLOCKSPLITTER )
                    {
                        fprintf( stderr, "blocksplitter pushed %d blocks, from %d bits to %d bits\n", ps_push_state->i_num_sub_blocks, i_best_bits, i_bits );
                    }
                    i_best_bits = i_bits;
                    *ps_state = *ps_push_state;
                    i_pushed = 0; /* FIXME: compression ratio says yes but speed says no ? */
                }
                else
                {
                    i_pushed++;
                    if( i_pushed > 4 )
                    {
                        break;
                    }
                }
            }
            else
            {
                break;
            }
        }
    }

    memcpy( rgi_block_boundaries_deflated, ps_state->rgi_block_boundaries_deflated, sizeof( ps_state->rgi_block_boundaries_deflated ) );

    return ps_state->i_num_sub_blocks;
}


void turtledeflate_block_boundaries_inverse( turtledeflate_ctx_t *ps_turtle, int32_t rgi_block_boundaries_deflated[ ], int32_t rgi_block_boundaries[ ], int32_t i_num_blocks )
{
    int32_t i_idx, i_boundary_idx, i_uncompressed_size;
    turtledeflate_block_t *ps_gblock = &ps_turtle->s_global_block;

    i_boundary_idx = i_uncompressed_size = 0;
    for( i_idx = 0; i_idx < ps_gblock->i_num_entries; i_idx++ )
    {
        turtledeflate_stream_entry_t *ps_entry = &ps_turtle->ps_global_stream[ i_idx ];

        if( rgi_block_boundaries_deflated[ i_boundary_idx ] == i_idx )
        {
            rgi_block_boundaries[ i_boundary_idx++ ] = i_uncompressed_size;
        }

        if( ps_entry->ui16_dist == 0 )
        {
            i_uncompressed_size++;
        }
        else
        {
            i_uncompressed_size += ps_entry->ui16_litlen;
        }
    }

    assert( i_boundary_idx == i_num_blocks );
    rgi_block_boundaries[ i_boundary_idx++ ] = i_uncompressed_size;
}





void turtledeflate_block_write( turtledeflate_ctx_t *ps_turtle, int32_t i_global_from, int32_t i_global_to, bool b_last )
{
    int32_t i_idx, i_num_nodes, i_flags, i_best_flags, i_flag_bits, i_best_flags_bits, i_uncompressed_size, i_compressed_size;
    int32_t i_root_node; /* unused */
    turtledeflate_block_t s_block;
    turtledeflate_bitstream_t *ps_bitstream = &ps_turtle->s_bitstream;
    turtledeflate_stream_entry_t *ps_entry = ps_turtle->ps_final_stream;
    turtledeflate_tree_node_t rgs_tree_nodes[ TURTLEDEFLATE_NUM_LENLITS * 2 + 1 ]; /* + 1 ? not - 1 ? */

    /* block header */
    turtledeflate_bitstream_write( ps_bitstream, b_last ? 1 : 0, 1 );
    turtledeflate_bitstream_write( ps_bitstream, 2, 2 );

    turtledeflate_block_reset( &s_block );

    for( i_idx = i_global_from; i_idx < i_global_to; i_idx++ )
    {
        turtledeflate_block_add_litlen_dist( ps_turtle, &s_block, ps_entry[ i_idx ].ui16_litlen, ps_entry[ i_idx ].ui16_dist );
    }
    turtledeflate_block_add_litlen_dist( ps_turtle, &s_block, 256, 0 ); /* end of block */


    i_compressed_size = turtledeflate_get_estimated_block_bits_tree( ps_turtle, &s_block, s_block.rgi_count_lenlits, s_block.rgi_count_dists );
    i_compressed_size += turtledeflate_get_estimated_block_bits_data( ps_turtle, &s_block, s_block.rgi_count_lenlits, s_block.rgi_count_dists );

    if( 1 )
    {
        int32_t rgi_try_rle_lenlits[ TURTLEDEFLATE_NUM_LENLITS ];
        int32_t rgi_try_rle_dists[ TURTLEDEFLATE_NUM_DISTS ];
        int32_t i_try_compressed_size;

        memcpy( rgi_try_rle_lenlits, s_block.rgi_count_lenlits, sizeof( rgi_try_rle_lenlits ) );
        memcpy( rgi_try_rle_dists, s_block.rgi_count_dists, sizeof( rgi_try_rle_dists ) );

        OptimizeHuffmanForRle( TURTLEDEFLATE_NUM_LENLITS, rgi_try_rle_lenlits );
        OptimizeHuffmanForRle( TURTLEDEFLATE_NUM_DISTS, rgi_try_rle_dists );

        i_try_compressed_size = turtledeflate_get_estimated_block_bits_tree( ps_turtle, &s_block, rgi_try_rle_lenlits, rgi_try_rle_dists );
        i_try_compressed_size += turtledeflate_get_estimated_block_bits_data( ps_turtle, &s_block, s_block.rgi_count_lenlits, s_block.rgi_count_dists );

        if( i_try_compressed_size < i_compressed_size )
        {
            memcpy( s_block.rgi_count_lenlits, rgi_try_rle_lenlits, sizeof( rgi_try_rle_lenlits ) );
            memcpy( s_block.rgi_count_dists, rgi_try_rle_dists, sizeof( rgi_try_rle_dists ) );
            i_compressed_size = i_try_compressed_size;
        }
    }

    turtledeflate_build_tree( ps_turtle, &s_block.rgi_count_lenlits[ 0 ], TURTLEDEFLATE_NUM_LENLITS, &rgs_tree_nodes[ 0 ], TURTLEDEFLATE_NUM_LENLITS * 2 + 1, &i_num_nodes, &i_root_node );
    turtledeflate_get_symbol_lengths( ps_turtle, &rgs_tree_nodes[ 0 ], i_num_nodes, &s_block.rgs_lenlit_symbols[ 0 ], TURTLEDEFLATE_NUM_LENLITS, TURTLEDEFLATE_MAX_CODEWORD_BITS );
    turtledeflate_symbol_length_to_code( ps_turtle, &s_block.rgs_lenlit_symbols[ 0 ], TURTLEDEFLATE_NUM_LENLITS, TURTLEDEFLATE_MAX_CODEWORD_BITS );

    turtledeflate_build_tree( ps_turtle, &s_block.rgi_count_dists[ 0 ], TURTLEDEFLATE_NUM_DISTS, &rgs_tree_nodes[ 0 ], TURTLEDEFLATE_NUM_DISTS * 2 + 1, &i_num_nodes, &i_root_node );
    turtledeflate_get_symbol_lengths( ps_turtle, &rgs_tree_nodes[ 0 ], i_num_nodes, &s_block.rgs_dist_symbols[ 0 ], TURTLEDEFLATE_NUM_DISTS, TURTLEDEFLATE_MAX_CODEWORD_BITS );
    turtledeflate_symbol_length_to_code( ps_turtle, &s_block.rgs_dist_symbols[ 0 ], TURTLEDEFLATE_NUM_DISTS, TURTLEDEFLATE_MAX_CODEWORD_BITS );

    /* brute force rle flags */
    i_best_flags_bits = -1;
    for( i_flags = 0; i_flags < 8; i_flags++ )
    {
        i_flag_bits = turtledeflate_encode_tree( ps_turtle, &s_block.rgs_lenlit_symbols[ 0 ], &s_block.rgs_dist_symbols[ 0 ], NULL, i_flags );
        if( i_best_flags_bits < 0 || i_flag_bits < i_best_flags_bits )
        {
            i_best_flags_bits = i_flag_bits;
            i_best_flags = i_flags;
        }
    }
    i_flag_bits = turtledeflate_encode_tree( ps_turtle, &s_block.rgs_lenlit_symbols[ 0 ], &s_block.rgs_dist_symbols[ 0 ], &ps_turtle->s_bitstream, i_best_flags );
    //fprintf( stderr, "huffman tree size: %d bits\n", i_flag_bits );

    i_uncompressed_size = i_compressed_size = 0;
    for( i_idx = i_global_from; i_idx < i_global_to; i_idx++ )
    {
        int32_t i_dist = ps_entry[ i_idx ].ui16_dist;

        if( i_dist == 0 )
        {
            int32_t i_lit = ps_entry[ i_idx ].ui16_litlen;
            turtledeflate_bitstream_writehuff( ps_bitstream, s_block.rgs_lenlit_symbols[ i_lit ].ui_code, s_block.rgs_lenlit_symbols[ i_lit ].ui_code_length );
            i_uncompressed_size += 8;
            i_compressed_size += s_block.rgs_lenlit_symbols[ i_lit ].ui_code_length;
        }
        else
        {
            int32_t i_len = ps_entry[ i_idx ].ui16_litlen;
            int32_t i_lens = turtledeflate_get_len_symbol( i_len );
            int32_t i_dists = turtledeflate_get_dist_symbol( i_dist );
            int32_t i_len_extra = turtledeflate_get_len_extra_bits( i_lens );
            int32_t i_dist_extra = turtledeflate_get_dist_extra_bits( i_dists );

            turtledeflate_bitstream_writehuff( ps_bitstream, s_block.rgs_lenlit_symbols[ i_lens ].ui_code, s_block.rgs_lenlit_symbols[ i_lens ].ui_code_length );
            if( i_len_extra > 0 )
            {
                turtledeflate_bitstream_write( ps_bitstream, turtledeflate_get_len_extra_value( i_len ), i_len_extra );
            }
            turtledeflate_bitstream_writehuff( ps_bitstream, s_block.rgs_dist_symbols[ i_dists ].ui_code, s_block.rgs_dist_symbols[ i_dists ].ui_code_length );
            if( i_dist_extra > 0 )
            {
                turtledeflate_bitstream_write( ps_bitstream, turtledeflate_get_dist_extra_value( i_dist ), i_dist_extra );
            }

            i_uncompressed_size += i_len * 8;
            i_compressed_size += s_block.rgs_lenlit_symbols[ i_lens ].ui_code_length;
            i_compressed_size += s_block.rgs_dist_symbols[ i_dists ].ui_code_length;
            i_compressed_size += i_len_extra;
            i_compressed_size += i_dist_extra;
        }
    }

    turtledeflate_bitstream_writehuff( ps_bitstream, s_block.rgs_lenlit_symbols[ 256 ].ui_code, s_block.rgs_lenlit_symbols[ 256 ].ui_code_length );
    i_compressed_size += s_block.rgs_lenlit_symbols[ 256 ].ui_code_length;
}



