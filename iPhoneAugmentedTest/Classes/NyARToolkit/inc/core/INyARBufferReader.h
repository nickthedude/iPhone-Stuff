/* 
 * PROJECT: NyARToolkitCPP
 * --------------------------------------------------------------------------------
 *
 * The NyARToolkitCS is C++ version NyARToolkit class library.
 * 
 * Copyright (C)2008 R.Iizuka
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this framework; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 * 
 * For further information please contact.
 *	http://nyatla.jp/nyatoolkit/
 *	<airmail(at)ebony.plala.or.jp>
 * 
 */
#pragma once
namespace NyARToolkitCPP
{
	class INyARBufferReader
	{
	public:
		virtual ~INyARBufferReader(){}

	public:
		//  ID�K��
		//  00-07(8)�^�ԍ�
		//  08-15(8)�r�b�g�t�H�[�}�b�gID
		//      00:24bit/01:32bit/02:16bit
		//  16-27(8)�^ID
		//      00:����/01:byte[]/02:int[][]/03:short[]
		//  24-31(8)�\��
		//

		/**
		* RGB24�t�H�[�}�b�g�ŁA�S�Ẳ�f��0
		*/
		const static int BUFFERFORMAT_NULL_ALLZERO = 0x00000001;

		/**
		* byte[]�ŁAR8G8B8��24�r�b�g�ŉ�f���i�[����Ă���B
		*/
		const static int BUFFERFORMAT_BYTE1D_R8G8B8_24 = 0x00010001;

		/**
		* byte[]�ŁAB8G8R8��24�r�b�g�ŉ�f���i�[����Ă���B
		*/
		const static int BUFFERFORMAT_BYTE1D_B8G8R8_24 = 0x00010002;

		/**
		* byte[]�ŁAR8G8B8X8��32�r�b�g�ŉ�f���i�[����Ă���B
		*/
		const static int BUFFERFORMAT_BYTE1D_B8G8R8X8_32 = 0x00010101;

		/**
		* byte[]�ŁARGB565��16�r�b�g(little/big endian)�ŉ�f���i�[����Ă���B
		*/
		const static int BUFFERFORMAT_BYTE1D_R5G6B5_16LE = 0x00010201;
		const static int BUFFERFORMAT_BYTE1D_R5G6B5_16BE = 0x00010202;
		/**
		* short[]�ŁARGB565��16�r�b�g(little/big endian)�ŉ�f���i�[����Ă���B
		*/	
		const static int BUFFERFORMAT_WORD1D_R5G6B5_16LE = 0x00030201;
		const static int BUFFERFORMAT_WORD1D_R5G6B5_16BE = 0x00030202;


		/**
		* int[][]�œ��ɒl�͈͂��߂Ȃ�
		*/
		const static int BUFFERFORMAT_INT2D        = 0x00020000;
		const static int BUFFERFORMAT_INT2D_GLAY_8 = 0x00020001;
		const static int BUFFERFORMAT_INT2D_BIN_8  = 0x00020002;

		const static int BUFFERFORMAT_INT1D        = 0x00040000;
		const static int BUFFERFORMAT_INT1D_GLAY_8 = 0x00040001;
		const static int BUFFERFORMAT_INT1D_BIN_8  = 0x00040002;	

		virtual const void* getBuffer() const=0;
		virtual int getBufferType() const=0;
		virtual bool isEqualBufferType(int i_type_value) const=0;
	};
}
