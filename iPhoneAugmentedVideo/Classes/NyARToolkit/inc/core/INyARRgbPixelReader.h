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
	class INyARRgbPixelReader
	{
	public:
		virtual ~INyARRgbPixelReader(){}
	public:
		/**
		* 1�s�N�Z����int�z��ɂ��ĕԂ��܂��B
		* 
		* @param i_x
		* @param i_y
		* @param i_rgb
		*/
		virtual void getPixel(int i_x, int i_y, int* i_rgb) const=0;
		/**
		* �����̃s�N�Z���l��i_rgb�֕Ԃ��܂��B
		* 
		* @param i_x
		* x�̃C���f�b�N�X�z��
		* @param i_y
		* y�̃C���f�b�N�X�z��
		* @param i_num
		* �Ԃ��s�N�Z���l�̐�
		* @param i_rgb
		* �s�N�Z���l��Ԃ��o�b�t�@
		*/
		virtual void getPixelSet(const int* i_x, const int* i_y, int i_num, int* i_rgb)const =0;
	};
}
