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
#include "NyARBaseClass.h"
namespace NyARToolkitCPP
{
	class NyARVertexCounter;
	/**
	* ���W�X�W���i�֊s���j����A�l�p�n�̒��_���_���v�Z���܂��B
	*
	*/
	class NyARCoord2SquareVertexIndexes:public NyARBaseClass
	{
	private:
		NyARVertexCounter* __getSquareVertex_wv1;
		NyARVertexCounter* __getSquareVertex_wv2;
	public:
		NyARCoord2SquareVertexIndexes();
		~NyARCoord2SquareVertexIndexes();
		/**
		* ���W�W������A���_���ɂȂ肻���ȏꏊ���S�ӏ��T���āA���̃C���f�N�X�ԍ���Ԃ��܂��B
		* @param i_x_coord
		* @param i_y_coord
		* @param i_coord_num
		* @param i_area
		* @param o_vertex
		* @return
		*/
		bool getVertexIndexes(int i_x_coord[], int i_y_coord[], int i_coord_num, int i_area, int o_vertex[]);
		/**
		* i_point�̗֊s���W����A�ł������ɂ���֊s���W�̃C���f�N�X��T���܂��B
		* @param i_xcoord
		* @param i_ycoord
		* @param i_coord_num
		* @return
		*/
	private:
		static int getFarPoint(int i_coord_x[], int i_coord_y[],int i_coord_num,int i_point);
	};
}
