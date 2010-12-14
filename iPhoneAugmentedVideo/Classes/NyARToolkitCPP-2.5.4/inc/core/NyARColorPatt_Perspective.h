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
#pragma once
#include "INyARColorPatt.h"
#include "NyAR_types.h"
#include "NyARPerspectiveParamGenerator_O1.h"
#include "NyARRgbPixelReader_INT1D_X8R8G8B8_32.h"
#include "NyARColorPatt_Perspective.h"
#include "NyARBufferType.h"


namespace NyARToolkitCPP
{
	/**
	* ���ߖ@���g�����p�[�X�y�N�e�B�u�␳�������āA���X�^��̎l�p�`����
	* �C�Ӊ𑜓x�̋�`�p�^�[�����쐬���܂��B
	*
	*/


	class NyARColorPatt_Perspective :public INyARColorPatt
	{
	protected:
		int* _patdata;
		TNyARIntPoint2d _pickup_lt;
		int _resolution;
		TNyARIntSize _size;
		NyARPerspectiveParamGenerator_O1* _perspective_gen;
	private:
		const static int BUFFER_FORMAT=NyARBufferType::INT1D_X8R8G8B8_32;
		NyARRgbPixelReader_INT1D_X8R8G8B8_32* _pixelreader;

	private:
		void initializeInstance(int i_width, int i_height,int i_point_per_pix);
		/**
		* �Ⴆ�΁A64
		* @param i_width
		* �擾�摜�̉𑜓x��
		* @param i_height
		* �擾�摜�̉𑜓x����
		* @param i_point_per_pix
		* 1�s�N�Z��������̏c���T���v�����O���B2�Ȃ�2x2=4�|�C���g���T���v�����O����B
		*/
	public:
		NyARColorPatt_Perspective(int i_width, int i_height,int i_point_per_pix);
		~NyARColorPatt_Perspective();
		/**
		* �Ⴆ�΁A64
		* @param i_width
		* �擾�摜�̉𑜓x��
		* @param i_height
		* �擾�摜�̉𑜓x����
		* @param i_edge_percentage
		* �G�b�W���̊���(ARToolKit�W���Ɠ����Ȃ�A25)
		*/
		NyARColorPatt_Perspective(int i_width, int i_height,int i_resolution,int i_edge_percentage);
		/**
		* ��`�̈�̃G�b�W�T�C�Y���w�肵�܂��B
		* �G�b�W�̌v�Z���@�͈ȉ��̒ʂ�ł��B
		* 1.�}�[�J�S�̂�(i_x_edge*2+width)x(i_y_edge*2+height)�̉𑜓x�Ńp�����^���v�Z���܂��B
		* 2.�s�N�Z���̎擾�J�n�ʒu��(i_x_edge/2,i_y_edge/2)�ֈړ����܂��B
		* 3.�J�n�ʒu����Awidth x height�̃s�N�Z�����擾���܂��B
		* 
		* ARToolKit�W���}�[�J�̏ꍇ�́Awidth/2,height/2���w�肵�Ă��������B
		* @param i_x_edge
		* @param i_y_edge
		*/
		void setEdgeSize(int i_x_edge,int i_y_edge,int i_resolution);
		void setEdgeSizeByPercent(int i_x_percent,int i_y_percent,int i_resolution);
		int getWidth()const;
		int getHeight()const;
		const TNyARIntSize& getSize()const;
		INyARRgbPixelReader& getRgbPixelReader()const;


		bool pickFromRaster(const INyARRgbRaster& image,const TNyARIntPoint2d i_vertexs[]);

		void* getBuffer()const;
		int getBufferType()const;
		bool isEqualBufferType(int i_type_value)const;
		bool hasBuffer()const;
		void wrapBuffer(void* i_ref_buf);





	};
}

