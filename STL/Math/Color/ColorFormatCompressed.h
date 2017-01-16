// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#pragma once

#include "ColorFormats.h"

namespace GX_STL
{
namespace GXMath
{
	
	namespace _color_hidden_
	{
		// COMPRESSED_RED_RGTC1
		struct RGTC1_Red
		{
			union Block
			{
				struct
				{
					uint	red0	: 8,
							red1	: 8,
							bits_0	: 8,
							bits_1	: 8,
							bits_2	: 8,
							bits_3	: 8,
							bits_4	: 8,
							bits_5	: 8;
				}			
							data;
				ulong		compressed;
			};

			typedef Matrix< ubyte, 4, 4 >	unpacked_block_t;

			RGTC1_Red () {}


			/*
			        union {
            struct {
                ulong alpha0 : 8;      // 8
                ulong alpha1 : 8;      // 16
                ulong bits0 : 3;       // 3 - 19
                ulong bits1 : 3;       // 6 - 22
                ulong bits2 : 3;       // 9 - 25
                ulong bits3 : 3;       // 12 - 28
                ulong bits4 : 3;       // 15 - 31
                ulong bits5 : 3;       // 18 - 34
                ulong bits6 : 3;       // 21 - 37
                ulong bits7 : 3;       // 24 - 40
                ulong bits8 : 3;       // 27 - 43
                ulong bits9 : 3;       // 30 - 46
                ulong bitsA : 3;       // 33 - 49
                ulong bitsB : 3;       // 36 - 52
                ulong bitsC : 3;       // 39 - 55
                ulong bitsD : 3;       // 42 - 58
                ulong bitsE : 3;       // 45 - 61
                ulong bitsF : 3;       // 48 - 64
            };
            ulong u;
        };


void BlockATI1::decodeBlock(ColorBlock * block, bool d3d9) const
{
    ubyte alpha_array[8];
    alpha.evaluatePalette(alpha_array, d3d9);

    ubyte index_array[16];
    alpha.indices(index_array);

    for(uint i = 0; i < 16; i++) {
        Color32 & c = block->color(i);
        c.b = c.g = c.r = alpha_array[index_array[i]];
        c.a = 255;
    }
}


void AlphaBlockDXT5::evaluatePalette(ubyte alpha[8], bool d3d9) const
{
    if (alpha0 > alpha1) {
        evaluatePalette8(alpha, d3d9);
    }
    else {
        evaluatePalette6(alpha, d3d9);
    }
}

void AlphaBlockDXT5::evaluatePalette8(ubyte alpha[8], bool d3d9) const
{
    int bias = 0;
    if (d3d9) bias = 3;

    // 8-alpha block:  derive the other six alphas.
    // Bit code 000 = alpha0, 001 = alpha1, others are interpolated.
    alpha[0] = alpha0;
    alpha[1] = alpha1;
    alpha[2] = (6 * alpha[0] + 1 * alpha[1] + bias) / 7;    // bit code 010
    alpha[3] = (5 * alpha[0] + 2 * alpha[1] + bias) / 7;    // bit code 011
    alpha[4] = (4 * alpha[0] + 3 * alpha[1] + bias) / 7;    // bit code 100
    alpha[5] = (3 * alpha[0] + 4 * alpha[1] + bias) / 7;    // bit code 101
    alpha[6] = (2 * alpha[0] + 5 * alpha[1] + bias) / 7;    // bit code 110
    alpha[7] = (1 * alpha[0] + 6 * alpha[1] + bias) / 7;    // bit code 111
}

void AlphaBlockDXT5::evaluatePalette6(ubyte alpha[8], bool d3d9) const
{
    int bias = 0;
    if (d3d9) bias = 2;

    // 6-alpha block.
    // Bit code 000 = alpha0, 001 = alpha1, others are interpolated.
    alpha[0] = alpha0;
    alpha[1] = alpha1;
    alpha[2] = (4 * alpha[0] + 1 * alpha[1] + bias) / 5;    // Bit code 010
    alpha[3] = (3 * alpha[0] + 2 * alpha[1] + bias) / 5;    // Bit code 011
    alpha[4] = (2 * alpha[0] + 3 * alpha[1] + bias) / 5;    // Bit code 100
    alpha[5] = (1 * alpha[0] + 4 * alpha[1] + bias) / 5;    // Bit code 101
    alpha[6] = 0x00;                                        // Bit code 110
    alpha[7] = 0xFF;                                        // Bit code 111
}

void AlphaBlockDXT5::indices(ubyte index_array[16]) const
{
    index_array[0x0] = bits0;
    index_array[0x1] = bits1;
    index_array[0x2] = bits2;
    index_array[0x3] = bits3;
    index_array[0x4] = bits4;
    index_array[0x5] = bits5;
    index_array[0x6] = bits6;
    index_array[0x7] = bits7;
    index_array[0x8] = bits8;
    index_array[0x9] = bits9;
    index_array[0xA] = bitsA;
    index_array[0xB] = bitsB;
    index_array[0xC] = bitsC;
    index_array[0xD] = bitsD;
    index_array[0xE] = bitsE;
    index_array[0xF] = bitsF;
}



			*/

			unpacked_block_t Unpack ()
			{
				Block	b;

				// 48-bit
				ulong bits   = b.data.bits_0 +
								256 * (b.data.bits_1 +
								256 * (b.data.bits_2 +
								256 * (b.data.bits_3 +
								256 * (b.data.bits_4 +
								256 *  b.data.bits_5) ) ) );

				for (uint y = 0; y < 4; ++y)
				{
					for (uint x = 0; x < 4; ++x)
					{
						uint code = 0;

						switch ( code )
						{
							case 0 :	break;
							case 1 :	break;
							case 2 :	break;
							case 3 :	break;
							case 4 :	break;
							case 5 :	break;
							case 6 :	break;
							case 7 :	break;
						}
					}
				}

				/*
				code(x,y) = bits[3*(4*y+x)+2..
								 3*(4*y+x)+0]
				*/
			}
		};

	}	// _color_hidden_


	
	namespace ColorFormat
	{
	}	// ColorFormat

	
}	// GXMath
}	// GX_STL
