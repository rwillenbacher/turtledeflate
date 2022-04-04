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

#include <string.h>

#include "turtledeflate_api.h"


void usage( void )
{
	fprintf( stderr, "usage:\n\n" );
	fprintf( stderr, "turtledeflate [OPTIONS] [FILE]\n" );
	fprintf( stderr, "options:\n" );
	fprintf( stderr, "\t--raw         | do not wrap deflate stream in gzip container\n" );
	fprintf( stderr, "\t--bs <int>    | maximum block size in bytes\n" );
	fprintf( stderr, "\t--nsb <int>   | maximum number of sub blocks\n" );
	fprintf( stderr, "\t--9           | maybe more compression at the cost of speed\n" );
}


static const uint32_t rgui_crc32_table[ 256 ] = {
		   0u, 1996959894u, 3993919788u, 2567524794u,  124634137u, 1886057615u,
  3915621685u, 2657392035u,  249268274u, 2044508324u, 3772115230u, 2547177864u,
   162941995u, 2125561021u, 3887607047u, 2428444049u,  498536548u, 1789927666u,
  4089016648u, 2227061214u,  450548861u, 1843258603u, 4107580753u, 2211677639u,
   325883990u, 1684777152u, 4251122042u, 2321926636u,  335633487u, 1661365465u,
  4195302755u, 2366115317u,  997073096u, 1281953886u, 3579855332u, 2724688242u,
  1006888145u, 1258607687u, 3524101629u, 2768942443u,  901097722u, 1119000684u,
  3686517206u, 2898065728u,  853044451u, 1172266101u, 3705015759u, 2882616665u,
   651767980u, 1373503546u, 3369554304u, 3218104598u,  565507253u, 1454621731u,
  3485111705u, 3099436303u,  671266974u, 1594198024u, 3322730930u, 2970347812u,
   795835527u, 1483230225u, 3244367275u, 3060149565u, 1994146192u,   31158534u,
  2563907772u, 4023717930u, 1907459465u,  112637215u, 2680153253u, 3904427059u,
  2013776290u,  251722036u, 2517215374u, 3775830040u, 2137656763u,  141376813u,
  2439277719u, 3865271297u, 1802195444u,  476864866u, 2238001368u, 4066508878u,
  1812370925u,  453092731u, 2181625025u, 4111451223u, 1706088902u,  314042704u,
  2344532202u, 4240017532u, 1658658271u,  366619977u, 2362670323u, 4224994405u,
  1303535960u,  984961486u, 2747007092u, 3569037538u, 1256170817u, 1037604311u,
  2765210733u, 3554079995u, 1131014506u,  879679996u, 2909243462u, 3663771856u,
  1141124467u,  855842277u, 2852801631u, 3708648649u, 1342533948u,  654459306u,
  3188396048u, 3373015174u, 1466479909u,  544179635u, 3110523913u, 3462522015u,
  1591671054u,  702138776u, 2966460450u, 3352799412u, 1504918807u,  783551873u,
  3082640443u, 3233442989u, 3988292384u, 2596254646u,   62317068u, 1957810842u,
  3939845945u, 2647816111u,   81470997u, 1943803523u, 3814918930u, 2489596804u,
   225274430u, 2053790376u, 3826175755u, 2466906013u,  167816743u, 2097651377u,
  4027552580u, 2265490386u,  503444072u, 1762050814u, 4150417245u, 2154129355u,
   426522225u, 1852507879u, 4275313526u, 2312317920u,  282753626u, 1742555852u,
  4189708143u, 2394877945u,  397917763u, 1622183637u, 3604390888u, 2714866558u,
   953729732u, 1340076626u, 3518719985u, 2797360999u, 1068828381u, 1219638859u,
  3624741850u, 2936675148u,  906185462u, 1090812512u, 3747672003u, 2825379669u,
   829329135u, 1181335161u, 3412177804u, 3160834842u,  628085408u, 1382605366u,
  3423369109u, 3138078467u,  570562233u, 1426400815u, 3317316542u, 2998733608u,
   733239954u, 1555261956u, 3268935591u, 3050360625u,  752459403u, 1541320221u,
  2607071920u, 3965973030u, 1969922972u,   40735498u, 2617837225u, 3943577151u,
  1913087877u,   83908371u, 2512341634u, 3803740692u, 2075208622u,  213261112u,
  2463272603u, 3855990285u, 2094854071u,  198958881u, 2262029012u, 4057260610u,
  1759359992u,  534414190u, 2176718541u, 4139329115u, 1873836001u,  414664567u,
  2282248934u, 4279200368u, 1711684554u,  285281116u, 2405801727u, 4167216745u,
  1634467795u,  376229701u, 2685067896u, 3608007406u, 1308918612u,  956543938u,
  2808555105u, 3495958263u, 1231636301u, 1047427035u, 2932959818u, 3654703836u,
  1088359270u,  936918000u, 2847714899u, 3736837829u, 1202900863u,  817233897u,
  3183342108u, 3401237130u, 1404277552u,  615818150u, 3134207493u, 3453421203u,
  1423857449u,  601450431u, 3009837614u, 3294710456u, 1567103746u,  711928724u,
  3020668471u, 3272380065u, 1510334235u,  755167117u
};


uint32_t turtledeflate_app_update_crc( uint32_t ui_crc32, uint8_t *pui8_data, int32_t i_size )
{
	while( i_size-- )
	{
		ui_crc32 = rgui_crc32_table[ ( ui_crc32 ^ *( pui8_data++ ) ) & 0xff ] ^ ( ui_crc32 >> 8 );
	}
	return ui_crc32;
}




int32_t main( int32_t i_argc, char *rgpc_argv[ ] )
{
	FILE *pf_in, *pf_out;
	char rgc_ofilename[ 1024 ];
	bool b_got_eof, b_gzip = true;
	int32_t i_idx, i_len, i_superblocks = 0, i_count, i_buffer_size, i_buffer_bytes;
	size_t ui_len, ui_input_size, ui_output_size;
	uint8_t *pui8_buffer, *pui8_out_buffer;
	char *rgpc_suffix[ 2 ] = { ".gz", ".deflate" };
	void *p_turtledeflate;
	turtledeflate_config_t s_config;
	uint32_t ui_crc32;
	turtledeflate_superblock_stats_t s_stats, s_global_stats;

	s_config.i_compression_level = 7; /* some semi-worthless stuff kicks in at 8 */
	s_config.i_maximum_block_size = 1000 * 1000;
	s_config.i_maximum_subblocks = TURTLEDEFLATE_MAX_SUBBLOCKS; /* oof */
	s_config.i_max_block_splitter_iterations = 30;
	s_config.i_max_internal_block_splitter_iterations = 100;
	s_config.i_block_splitter_num_points = TURTLEDEFLATE_BSPLIT_NUM_POINTS_DEFAULT;
	s_config.i_block_splitter_center_dist = TURTLEDEFLATE_BSPLIT_CENTER_DIST_DEFAULT;
	s_config.i_block_splitter_min_range_for_points = TURTLEDEFLATE_MIN_RANGE_FOR_POINTS_DEFAULT;
	s_config.b_block_splitter_push_split = true; /* na... */

	/* higher num_start_fp might lead to faster convergence in the 3rd block splitter state */
	s_config.i_min_start_fp = -6;
	s_config.i_max_start_fp = 5;
	s_config.i_num_start_fp = 10;
	s_config.i_verbose = TURTLEDEFLATE_VERBOSE_BLOCK;
	//s_config.i_verbose = TURTLEDEFLATE_VERBOSE_STARTFP;
	//s_config.i_verbose = TURTLEDEFLATE_VERBOSE_SQUISHITER;

	fprintf( stderr, "turtledeflate\n" );

	if( i_argc <= 1 )
	{
		usage( );
		exit( 1 );
	}

	for( i_idx = 1; i_idx < i_argc; i_idx++ )
	{
		if( rgpc_argv[ i_idx ][ 0 ] != '-' ||
			rgpc_argv[ i_idx ][ 1 ] != '-' )
		{
			break;
		}
		else if( strcmp( rgpc_argv[ i_idx ], "--raw" ) == 0 )
		{
			b_gzip = false;
		}
		else if( strcmp( rgpc_argv[ i_idx ], "--bs" ) == 0 )
		{
			s_config.i_maximum_block_size = atoi( rgpc_argv[ ++i_idx ] );
			if( s_config.i_maximum_block_size < TURTLEDEFLATE_MIN_BLOCK_SIZE )
			{
				fprintf( stderr, "block size %d too small, minimum is %d\n", s_config.i_maximum_block_size, TURTLEDEFLATE_MIN_BLOCK_SIZE );
			}
		}
		else if( strcmp( rgpc_argv[ i_idx ], "--nsb" ) == 0 )
		{
			s_config.i_maximum_subblocks = atoi( rgpc_argv[ ++i_idx ] );
			if( s_config.i_maximum_subblocks < TURTLEDEFLATE_MIN_SUBBLOCKS ||
				s_config.i_maximum_subblocks > TURTLEDEFLATE_MAX_SUBBLOCKS )
			{
				fprintf( stderr, "maximum number of sub blocks %d too small or to large, need %d-%d\n", s_config.i_maximum_block_size, TURTLEDEFLATE_MIN_SUBBLOCKS, TURTLEDEFLATE_MAX_SUBBLOCKS );
			}
		}
		else if( strcmp( rgpc_argv[ i_idx ], "--9" ) == 0 )
		{
			s_config.i_compression_level = 9;
		}
		else
		{
			usage( );
			fprintf( stderr, "\n" );
			fprintf( stderr, "unknown option '%s' !\n", rgpc_argv[ i_idx ] );
			exit( 1 );
		}
	}

	if( i_idx == i_argc )
	{
		usage( );
		fprintf( stderr, "\n" );
		fprintf( stderr, "no input file\n" );
		exit( 1 );
	}

	if( i_idx != i_argc - 1 )
	{
		usage( );
		fprintf( stderr, "\n" );
		fprintf( stderr, "file '%s' is not the last program argument\n", rgpc_argv[ i_idx ] );
		exit( 1 );
	}


	pf_in = fopen( rgpc_argv[ i_idx ], "rb" );
	if( !pf_in )
	{
		fprintf( stderr, "unable to open input file '%s'\n", rgpc_argv[ i_idx ] );
		exit( 1 );
	}

	sprintf( rgc_ofilename, "%s%s", rgpc_argv[ i_idx ], rgpc_suffix[ !b_gzip ] );
	pf_out = fopen( rgc_ofilename, "wb" );
	if( !pf_out )
	{
		fprintf( stderr, "unable to open output file '%s'\n", rgc_ofilename );
		exit( 1 );
	}

	fprintf( stderr, "running with config:\n" );
	fprintf( stderr, "compression level: %d\n", s_config.i_compression_level );
	fprintf( stderr, "superblock size  : %d\n", s_config.i_maximum_block_size );
	fprintf( stderr, "max. sub blocks  : %d\n", s_config.i_maximum_subblocks );
	fprintf( stderr, "max split iter   : %d\n", s_config.i_max_block_splitter_iterations );
	fprintf( stderr, "max isplit iter  : %d\n", s_config.i_max_internal_block_splitter_iterations );
	fprintf( stderr, "split points     : %d\n", s_config.i_block_splitter_num_points );
	fprintf( stderr, "split center dist: %d\n", s_config.i_block_splitter_center_dist );
	fprintf( stderr, "split min range  : %d\n", s_config.i_block_splitter_min_range_for_points );
	fprintf( stderr, "push splits      : %s\n", s_config.b_block_splitter_push_split ? "yes" : "no" );
	fprintf( stderr, "start fp min     : %d\n", s_config.i_min_start_fp );
	fprintf( stderr, "start fp max     : %d\n", s_config.i_max_start_fp );
	fprintf( stderr, "num start fp     : %d\n", s_config.i_num_start_fp );

	turtledeflate_create( &p_turtledeflate, &s_config );
	i_buffer_size = s_config.i_maximum_block_size * 2;
	pui8_buffer = malloc( i_buffer_size * sizeof( uint8_t ) );

	if( b_gzip )
	{
		putc( 0x1f, pf_out );
		putc( 0x8b, pf_out );
		putc( 0x08, pf_out );
		putc( 0x00, pf_out );

		putc( 0x00, pf_out );
		putc( 0x00, pf_out );
		putc( 0x00, pf_out );
		putc( 0x00, pf_out );

		putc( 0x02, pf_out );
		putc( 0x03, pf_out );
	}

	memset( &s_global_stats, 0, sizeof( s_global_stats ) );
	s_global_stats.i_min_block_size = s_config.i_maximum_block_size;
	s_global_stats.i_max_block_size = 0;

	i_buffer_bytes = 0;
	ui_output_size = ui_input_size = 0;
	ui_crc32 = 0xffffffffu;
	b_got_eof = false;
	while( 1 )
	{
		int32_t i_readsize, i_commit_size;

		i_readsize = i_buffer_size - i_buffer_bytes;
		if( !b_got_eof )
		{
			ui_len = fread( pui8_buffer + i_buffer_bytes, sizeof( uint8_t ), i_readsize, pf_in );
			if( ui_len == 0 )
			{
				b_got_eof = true;
			}
			ui_crc32 = turtledeflate_app_update_crc( ui_crc32, pui8_buffer + i_buffer_bytes, ( int32_t ) ui_len );

			i_buffer_bytes += ui_len;
			ui_input_size += ui_len;
		}

		fprintf( stderr, ".\n" );

		i_commit_size = i_buffer_bytes > s_config.i_maximum_block_size ? s_config.i_maximum_block_size : i_buffer_bytes;
		if( s_config.i_verbose >= TURTLEDEFLATE_VERBOSE_SUPERBLOCK )
		{
			fprintf( stderr, "passing %d bytes to lib....\n", ( int32_t ) i_commit_size );
		}

		i_len = turtledeflate_block( p_turtledeflate, i_commit_size, pui8_buffer, &pui8_out_buffer, &s_stats, i_commit_size == i_buffer_bytes );

		s_global_stats.i_num_blocks += s_stats.i_num_blocks;
		s_global_stats.i_block_limited += s_stats.i_block_limited;
		s_global_stats.i_average_block_size += s_stats.i_average_block_size;
		s_global_stats.i_min_block_size = s_stats.i_min_block_size < s_global_stats.i_min_block_size ? s_stats.i_min_block_size : s_global_stats.i_min_block_size;
		s_global_stats.i_max_block_size = s_stats.i_max_block_size > s_global_stats.i_max_block_size ? s_stats.i_max_block_size : s_global_stats.i_max_block_size;
		for( i_idx = 0; i_idx < s_config.i_num_start_fp; i_idx++ )
		{
			s_global_stats.rgi_start_fp_chosen[ i_idx ] += s_stats.rgi_start_fp_chosen[ i_idx ];
			s_global_stats.rgi_best_from_start_fp[ i_idx ] += s_stats.rgi_best_from_start_fp[ i_idx ];
		}
		i_superblocks++;

		ui_output_size += i_len;

		if( s_config.i_verbose >= TURTLEDEFLATE_VERBOSE_SUPERBLOCK )
		{
			fprintf( stderr, "got %d bytes from lib. ( current %f %% saved )\n", i_len, ( 100.0 * ( ui_input_size - ui_output_size ) ) / ui_input_size );
		}

		ui_len = fwrite( pui8_out_buffer, sizeof( uint8_t ), i_len, pf_out );

		if( ( int32_t ) ui_len != i_len )
		{
			fprintf( stderr, "error writing to output file\nbroken output\n" );
			exit( 2 );
		}

		i_buffer_bytes -= i_commit_size;
		if( i_buffer_bytes == 0 )
		{
			fprintf( stderr, "eof.\n" );
			break;
		}
		else
		{
			memmove( pui8_buffer, pui8_buffer + i_commit_size, i_buffer_bytes );
		}
	}

	fprintf( stderr, "------------------------\nstats:\n" );
	fprintf( stderr, "superblocks: %d\n", i_superblocks );
	fprintf( stderr, "blocksplit limited: %d\n", s_global_stats.i_block_limited );
	fprintf( stderr, "\n" );
	fprintf( stderr, "total blocks: %d\n", s_global_stats.i_num_blocks );
	fprintf( stderr, "min block size: %d\n", s_global_stats.i_min_block_size );
	fprintf( stderr, "max block size: %d\n", s_global_stats.i_max_block_size );
	fprintf( stderr, "avg block size: %d\n", s_global_stats.i_average_block_size / i_superblocks );
	fprintf( stderr, "start fp:" );
	for( i_idx = 0, i_count = 0; i_idx < s_config.i_num_start_fp; i_idx++ )
	{
		i_count += s_global_stats.rgi_start_fp_chosen[ i_idx ];
	}
	for( i_idx = 0; i_idx < s_config.i_num_start_fp; i_idx++ )
	{
		fprintf( stderr, " (%d) %.2f%%", i_idx, ( s_global_stats.rgi_start_fp_chosen[ i_idx ] * 100.0 ) / i_count );
	}
	fprintf( stderr, "\n" );
	fprintf( stderr, "best from start fp:" );
	for( i_idx = 0, i_count = 0; i_idx < s_config.i_num_start_fp; i_idx++ )
	{
		i_count += s_global_stats.rgi_best_from_start_fp[ i_idx ];
	}
	for( i_idx = 0; i_idx < s_config.i_num_start_fp; i_idx++ )
	{
		fprintf( stderr, " (%d) %.2f%%", i_idx, ( s_global_stats.rgi_best_from_start_fp[ i_idx ] * 100.0 ) / i_count );
	}
	fprintf( stderr, "\n" );


	fprintf( stderr, "input bytes: %d, output bytes %d, removed %f%%\n", ( int32_t ) ui_input_size, ( int32_t ) ui_output_size, ( 100.0 * ( ui_input_size - ui_output_size ) ) / ui_input_size );

	ui_crc32 ^= 0xffffffffu;

	if( b_gzip )
	{
		putc( ( ui_crc32 >> 0 ) & 0xff, pf_out );
		putc( ( ui_crc32 >> 8 ) & 0xff, pf_out );
		putc( ( ui_crc32 >> 16 ) & 0xff, pf_out );
		putc( ( ui_crc32 >> 24 ) & 0xff, pf_out );

		putc( ( ui_input_size >> 0 ) & 0xff, pf_out );
		putc( ( ui_input_size >> 8 ) & 0xff, pf_out );
		putc( ( ui_input_size >> 16 ) & 0xff, pf_out );
		putc( ( ui_input_size >> 24 ) & 0xff, pf_out );
	}

	turtledeflate_destroy( p_turtledeflate );

	fclose( pf_in );
	fclose( pf_out );

	exit( 0 );
}



