/* 
* PROJECT: NyARToolkitCPP
* --------------------------------------------------------------------------------
*
* The NyARToolkitCPP is C++ version NyARToolkit class library.
* Copyright (C)2008-2009 Ryo Iizuka
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
* 
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
* 
* For further information please contact.
*	http://nyatla.jp/nyatoolkit/
*	<airmail(at)ebony.plala.or.jp> or <nyatla(at)nyatla.jp>
* 
*/
#include "NyARCoord2SquareVertexIndexes.h"
#include "nyarcore.h"	
namespace NyARToolkitCPP
{

#define VERTEX_FACTOR 1.0


	/**
	* get_vertex�֐���؂藣�����߂̃N���X
	* 
	*/
	class NyARVertexCounter
	{
	public:
		int vertex[10];// 6�܂ō���

		int number_of_vertex;

		double thresh;

		const int* x_coord;

		const int* y_coord;

	public:
		bool getVertex(const int i_x_coord[], const int i_y_coord[],int i_coord_len,int st, int ed, double i_thresh)
		{
			this->number_of_vertex = 0;
			this->thresh = i_thresh;
			this->x_coord = i_x_coord;
			this->y_coord = i_y_coord;
			return get_vertex(st, ed,i_coord_len);
		}

		/**
		* 
		* @param x_coord
		* @param y_coord
		* @param st
		* @param ed
		* @param thresh
		* @return
		*/
	private:
		bool get_vertex(int st, int ed,int i_coord_len)
		{
			//����:���W�l��65536�𒴂��Ȃ����int32�ň����đ��v�Ȃ̂ŕύX�B
			//dmax��4��Ȃ̂ł��Ƃ��Ă�int64����Ȃ��ƃ}�Y�C
			int v1 = 0;
			const int* lx_coord = this->x_coord;
			const int* ly_coord = this->y_coord;
			const int a = ly_coord[ed] - ly_coord[st];
			const int b = lx_coord[st] - lx_coord[ed];
			const int c = lx_coord[ed] * ly_coord[st] - ly_coord[ed] * lx_coord[st];
			double dmax = 0;
			if(st<ed){
				//st��ed��1���
				for (int i = st + 1; i < ed; i++) {
					const double d = a * lx_coord[i] + b * ly_coord[i] + c;
					if (d * d > dmax) {
						dmax = d * d;
						v1 = i;
					}
				}
			}else{
				//st��ed��2���
				for (int i = st + 1; i < i_coord_len; i++) {
					const double d = a * lx_coord[i] + b * ly_coord[i] + c;
					if (d * d > dmax) {
						dmax = d * d;
						v1 = i;
					}
				}
				for (int i = 0; i < ed; i++) {
					const double d = a * lx_coord[i] + b * ly_coord[i] + c;
					if (d * d > dmax) {
						dmax = d * d;
						v1 = i;
					}
				}
			}


			if (dmax / (double)(a * a + b * b) > thresh) {
				if (!get_vertex(st, v1,i_coord_len)) {
					return false;
				}
				if (number_of_vertex > 5) {
					return false;
				}
				vertex[number_of_vertex] = v1;// vertex[(*vnum)] = v1;
				number_of_vertex++;// (*vnum)++;

				if (!get_vertex(v1, ed,i_coord_len)) {
					return false;
				}
			}
			return true;
		}
	};


	/**
	* ���W�X�W���i�֊s���j����A�l�p�n�̒��_���_���v�Z���܂��B
	*
	*/
	NyARCoord2SquareVertexIndexes::NyARCoord2SquareVertexIndexes():NyARBaseClass()
	{
		this->__getSquareVertex_wv1 = new NyARVertexCounter();
		this->__getSquareVertex_wv2 = new NyARVertexCounter();
		return;
	}
	NyARCoord2SquareVertexIndexes::~NyARCoord2SquareVertexIndexes()
	{
		NyAR_SAFE_DELETE(this->__getSquareVertex_wv1);
		NyAR_SAFE_DELETE(this->__getSquareVertex_wv2);
		return;
	}

	/**
	* ���W�W������A���_���ɂȂ肻���ȏꏊ���S�ӏ��T���āA���̃C���f�N�X�ԍ���Ԃ��܂��B
	* @param i_x_coord
	* @param i_y_coord
	* @param i_coord_num
	* @param i_area
	* @param o_vertex
	* @return
	*/
	bool NyARCoord2SquareVertexIndexes::getVertexIndexes(int i_x_coord[], int i_y_coord[], int i_coord_num, int i_area, int o_vertex[])
	{
		NyARVertexCounter& wv1 = *this->__getSquareVertex_wv1;
		NyARVertexCounter& wv2 = *this->__getSquareVertex_wv2;
		int vertex1_index=getFarPoint(i_x_coord,i_y_coord,i_coord_num,0);
		int prev_vertex_index=(vertex1_index+i_coord_num)%i_coord_num;
		int v1=getFarPoint(i_x_coord,i_y_coord,i_coord_num,vertex1_index);
		double thresh = (i_area / 0.75) * 0.01 * VERTEX_FACTOR;

		o_vertex[0] = vertex1_index;

		if (!wv1.getVertex(i_x_coord, i_y_coord,i_coord_num, vertex1_index, v1, thresh)) {
			return false;
		}
		if (!wv2.getVertex(i_x_coord, i_y_coord,i_coord_num, v1,prev_vertex_index, thresh)) {
			return false;
		}

		int v2;
		if (wv1.number_of_vertex == 1 && wv2.number_of_vertex == 1) {
			o_vertex[1] = wv1.vertex[0];
			o_vertex[2] = v1;
			o_vertex[3] = wv2.vertex[0];
		} else if (wv1.number_of_vertex > 1 && wv2.number_of_vertex == 0) {
			//���_�ʒu���A�N�_����Ίp�_�̊Ԃ�1/2�ɂ���Ɨ\�z���āA��������B
			if(v1>=vertex1_index){
				v2 = (v1-vertex1_index)/2+vertex1_index;
			}else{
				v2 = ((v1+i_coord_num-vertex1_index)/2+vertex1_index)%i_coord_num;
			}
			if (!wv1.getVertex(i_x_coord, i_y_coord,i_coord_num, vertex1_index, v2, thresh)) {
				return false;
			}
			if (!wv2.getVertex(i_x_coord, i_y_coord,i_coord_num, v2, v1, thresh)) {
				return false;
			}
			if (wv1.number_of_vertex == 1 && wv2.number_of_vertex == 1) {
				o_vertex[1] = wv1.vertex[0];
				o_vertex[2] = wv2.vertex[0];
				o_vertex[3] = v1;
			} else {
				return false;
			}
		} else if (wv1.number_of_vertex == 0 && wv2.number_of_vertex > 1) {
			//v2 = (v1+ end_of_coord)/2;
			if(v1<=prev_vertex_index){
				v2 = (v1+prev_vertex_index)/2;
			}else{
				v2 = ((v1+i_coord_num+prev_vertex_index)/2)%i_coord_num;

			}
			if (!wv1.getVertex(i_x_coord, i_y_coord,i_coord_num, v1, v2, thresh)) {
				return false;
			}
			if (!wv2.getVertex(i_x_coord, i_y_coord,i_coord_num, v2, prev_vertex_index, thresh)) {
				return false;
			}
			if (wv1.number_of_vertex == 1 && wv2.number_of_vertex == 1) {
				o_vertex[1] = v1;
				o_vertex[2] = wv1.vertex[0];
				o_vertex[3] = wv2.vertex[0];
			} else {

				return false;
			}
		} else {
			return false;
		}
		return true;
	}
	/**
	* i_point�̗֊s���W����A�ł������ɂ���֊s���W�̃C���f�N�X��T���܂��B
	* @param i_xcoord
	* @param i_ycoord
	* @param i_coord_num
	* @return
	*/
	int NyARCoord2SquareVertexIndexes::getFarPoint(int i_coord_x[], int i_coord_y[],int i_coord_num,int i_point)
	{
		//
		int sx = i_coord_x[i_point];
		int sy = i_coord_y[i_point];
		int d = 0;
		int w, x, y;
		int ret = 0;
		for (int i = i_point+1; i < i_coord_num; i++) {
			x = i_coord_x[i] - sx;
			y = i_coord_y[i] - sy;
			w = x * x + y * y;
			if (w > d) {
				d = w;
				ret = i;
			}
		}
		for (int i = 0; i < i_point; i++) {
			x = i_coord_x[i] - sx;
			y = i_coord_y[i] - sy;
			w = x * x + y * y;
			if (w > d) {
				d = w;
				ret = i;
			}
		}		
		return ret;
	}	



}
