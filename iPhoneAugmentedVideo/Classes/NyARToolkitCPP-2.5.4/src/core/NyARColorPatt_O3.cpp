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
#include "NyARColorPatt_O3.h"
#include "nyarcore.h"
#include <cstring>
#include <cstdio>
#define AR_PATT_SAMPLE_NUM 64
#define LT_POS 102.5
#define SQ_SIZE 5.0

namespace NyARToolkitCPP
{



	NyARColorPatt_O3::NyARColorPatt_O3(int i_width, int i_height)
	{
		this->_size.h=i_height;
		this->_size.w=i_width;
		////////////////////
		this->_patdata = new int[i_height*i_width];
		this->_pixelreader=new NyARRgbPixelReader_INT1D_X8R8G8B8_32(&(this->_size),this->_patdata);
		////////////////////
		this->__get_cpara_a=new NyARMat(8, 8);
		this->__get_cpara_b=new NyARMat(8, 1);
		this->__pickFromRaster_cpara = new NyARMat(8, 1);
//		this->__updateExtpat=new T__updateExtpat_struct_t();
		////////////////////
		this->__updateExtpat_rgbset=NULL;
		this->__updateExtpat_xc=NULL;
		this->__updateExtpat_yc=NULL;
		this->__updateExtpat_xw=NULL;
		this->__updateExtpat_yw=NULL;
		this->_last_pix_resolution_x=0;
		this->_last_pix_resolution_y=0;
		return;
	}
	NyARColorPatt_O3::~NyARColorPatt_O3()
	{
		NyAR_SAFE_DELETE(this->_pixelreader);
		NyAR_SAFE_ARRAY_DELETE(this->_patdata);
		////////////////////
		NyAR_SAFE_DELETE(this->__get_cpara_a);
		NyAR_SAFE_DELETE(this->__get_cpara_b);
		NyAR_SAFE_DELETE(this->__pickFromRaster_cpara);
		////////////////////
		NyAR_SAFE_ARRAY_DELETE(this->__updateExtpat_rgbset);
		NyAR_SAFE_ARRAY_DELETE(this->__updateExtpat_xc);
		NyAR_SAFE_ARRAY_DELETE(this->__updateExtpat_yc);
		NyAR_SAFE_ARRAY_DELETE(this->__updateExtpat_xw);
		NyAR_SAFE_ARRAY_DELETE(this->__updateExtpat_yw);
		return;
	}


	int NyARColorPatt_O3::getWidth()const
	{
		return this->_size.w;
	}
	int NyARColorPatt_O3::getHeight()const
	{
		return this->_size.h;
	}
	const TNyARIntSize& NyARColorPatt_O3::getSize()const
	{
		return this->_size;
	}
	void* NyARColorPatt_O3::getBuffer()const
	{
		return this->_patdata;
	}
	int NyARColorPatt_O3::getBufferType()const
	{
		return BUFFER_FORMAT;
	}
	bool NyARColorPatt_O3::isEqualBufferType(int i_type_value)const
	{
		return BUFFER_FORMAT==i_type_value;
	}
	bool NyARColorPatt_O3::hasBuffer()const
	{
		return this->_patdata!=NULL;
	}
	void wrapBuffer(void* i_ref_buf)
	{
		NyARException::notImplement();
	}
	INyARRgbPixelReader& NyARColorPatt_O3::getRgbPixelReader()
	{
		return *(this->_pixelreader);
	}



	/**
	 * @param world
	 * @param vertex
	 * @param o_para
	 * @throws NyARException
	 */
	bool NyARColorPatt_O3::get_cpara(const TNyARIntPoint2d i_vertex[], NyARMat& o_para)
	{
		const static int world[4][2]={// double world[4][2];
			{ 100, 100 }, { 100 + 10, 100 }, { 100 + 10, 100 + 10 }, { 100, 100 + 10 }};
		NyARMat* a = this->__get_cpara_a;// �������Œl��ݒ肷��̂ŁA�������s�v// new NyARMat( 8, 8 );
		NyARMat* b = this->__get_cpara_b;// �������Œl��ݒ肷��̂ŁA�������s�v// new NyARMat( 8, 1 );
		double* a_array = a->getArray();
		double* b_array = b->getArray();

		for (int i = 0; i < 4; i++) {
			double* a_pt0 = a_array+(i * 2)*8;
			double* a_pt1 = a_array+(i * 2 + 1)*8;
			const int* world_pti = world[i];

			a_pt0[0] = (double) world_pti[0];// a->m[i*16+0] = world[i][0];
			a_pt0[1] = (double) world_pti[1];// a->m[i*16+1] = world[i][1];
			a_pt0[2] = 1.0;// a->m[i*16+2] = 1.0;
			a_pt0[3] = 0.0;// a->m[i*16+3] = 0.0;
			a_pt0[4] = 0.0;// a->m[i*16+4] = 0.0;
			a_pt0[5] = 0.0;// a->m[i*16+5] = 0.0;
			a_pt0[6] = (double) (-world_pti[0] * i_vertex[i].x);// a->m[i*16+6]= -world[i][0]*vertex[i][0];
			a_pt0[7] = (double) (-world_pti[1] * i_vertex[i].x);// a->m[i*16+7]=-world[i][1]*vertex[i][0];
			a_pt1[0] = 0.0;// a->m[i*16+8] = 0.0;
			a_pt1[1] = 0.0;// a->m[i*16+9] = 0.0;
			a_pt1[2] = 0.0;// a->m[i*16+10] = 0.0;
			a_pt1[3] = (double) world_pti[0];// a->m[i*16+11] = world[i][0];
			a_pt1[4] = (double) world_pti[1];// a->m[i*16+12] = world[i][1];
			a_pt1[5] = 1.0;// a->m[i*16+13] = 1.0;
			a_pt1[6] = (double) (-world_pti[0] * i_vertex[i].y);// a->m[i*16+14]=-world[i][0]*vertex[i][1];
			a_pt1[7] = (double) (-world_pti[1] * i_vertex[i].y);// a->m[i*16+15]=-world[i][1]*vertex[i][1];
			b_array[(i * 2 + 0)*1+0] = (double) i_vertex[i].x;// b->m[i*2+0] =vertex[i][0];
			b_array[(i * 2 + 1)*1+0] = (double) i_vertex[i].y;// b->m[i*2+1] =vertex[i][1];
		}
		if (!a->matrixSelfInv()) {
			return false;
		}

		o_para.matrixMul(*a,*b);
		return true;
	}

	/**
	 * image����Ai_marker�̈ʒu�ɂ���p�^�[����؂�o���āA�ێ����܂��B Optimize:STEP[769->750]
	 * 
	 * @param image
	 * @param i_marker
	 * @throws Exception
	 */
	bool NyARColorPatt_O3::pickFromRaster(const INyARRgbRaster& image,const TNyARIntPoint2d i_vertexs[])
	{
		NyARMat& cpara = *(this->__pickFromRaster_cpara);
		// xdiv2,ydiv2�̌v�Z
		int xdiv2, ydiv2;
		int l1, l2;
		int w1, w2;
		// x�v�Z
		w1 = i_vertexs[0].x - i_vertexs[1].x;
		w2 = i_vertexs[0].y - i_vertexs[1].y;
		l1 = (w1 * w1 + w2 * w2);
		w1 = i_vertexs[2].x - i_vertexs[3].x;
		w2 = i_vertexs[2].y - i_vertexs[3].y;
		l2 = (w1 * w1 + w2 * w2);
		if (l2 > l1) {
			l1 = l2;
		}
		l1 = l1 / 4;
		xdiv2 = this->_size.w;
		while (xdiv2 * xdiv2 < l1) {
			xdiv2 *= 2;
		}
		if (xdiv2 > AR_PATT_SAMPLE_NUM) {
			xdiv2 = AR_PATT_SAMPLE_NUM;
		}

		// y�v�Z
		w1 = i_vertexs[1].x - i_vertexs[2].x;
		w2 = i_vertexs[1].y - i_vertexs[2].y;
		l1 = (w1 * w1 + w2 * w2);
		w1 = i_vertexs[3].x - i_vertexs[0].x;
		w2 = i_vertexs[3].y - i_vertexs[0].y;
		l2 = (w1 * w1 + w2 * w2);
		if (l2 > l1) {
			l1 = l2;
		}
		ydiv2 = this->_size.h;
		l1 = l1 / 4;
		while (ydiv2 * ydiv2 < l1) {
			ydiv2 *= 2;
		}
		if (ydiv2 > AR_PATT_SAMPLE_NUM) {
			ydiv2 = AR_PATT_SAMPLE_NUM;
		}

		// cpara�̌v�Z
		if (!get_cpara(i_vertexs,cpara)) {
			return false;
		}
		updateExtpat(image,cpara, xdiv2, ydiv2);

		return true;
	}

	void NyARColorPatt_O3::reservWorkBuffers(int i_xdiv,int i_ydiv)
	{
		if(this->_last_pix_resolution_x<i_xdiv || this->_last_pix_resolution_y<i_ydiv){
			NyAR_SAFE_ARRAY_DELETE(this->__updateExtpat_rgbset);
			NyAR_SAFE_ARRAY_DELETE(this->__updateExtpat_xc);
			NyAR_SAFE_ARRAY_DELETE(this->__updateExtpat_yc);
			NyAR_SAFE_ARRAY_DELETE(this->__updateExtpat_xw);
			NyAR_SAFE_ARRAY_DELETE(this->__updateExtpat_yw);
			this->__updateExtpat_xc=new int[i_xdiv*i_ydiv];
			this->__updateExtpat_yc=new int[i_xdiv*i_ydiv];
			this->__updateExtpat_xw=new double[i_xdiv];
			this->__updateExtpat_yw=new double[i_ydiv];
			this->__updateExtpat_rgbset=new int[i_xdiv*i_ydiv*3];
			this->_last_pix_resolution_x=i_xdiv;
			this->_last_pix_resolution_y=i_ydiv;
		}
		return;
	}

	
	//������16�����ɂȂ�Ə����x���Ȃ邩���B
	void NyARColorPatt_O3::updateExtpat(const INyARRgbRaster& image, const NyARMat& i_cpara, int i_xdiv2,int i_ydiv2)
	{
		int i,j;
		int r,g,b;
		//�s�N�Z�����[�_�[���擾
		const int pat_size_w=this->_size.w;
		const int xdiv = i_xdiv2 / pat_size_w;// xdiv = xdiv2/Config.AR_PATT_SIZE_X;
		const int ydiv = i_ydiv2 / this->_size.h;// ydiv = ydiv2/Config.AR_PATT_SIZE_Y;
		const int xdiv_x_ydiv = xdiv * ydiv;
		double reciprocal;
		const double* para=i_cpara.getArray();
		const double para00=para[(0*3+0)*1+0];
		const double para01=para[(0*3+1)*1+0];
		const double para02=para[(0*3+2)*1+0];
		const double para10=para[(1*3+0)*1+0];
		const double para11=para[(1*3+1)*1+0];
		const double para12=para[(1*3+2)*1+0];
		const double para20=para[(2*3+0)*1+0];
		const double para21=para[(2*3+1)*1+0];

		const INyARRgbPixelReader& reader=image.getRgbPixelReader();
		const int img_width=image.getWidth();
		const int img_height=image.getHeight();

		//���[�N�o�b�t�@�̏���
		reservWorkBuffers(xdiv,ydiv);
		double* xw=this->__updateExtpat_xw;
		double* yw=this->__updateExtpat_yw;
		int* xc=this->__updateExtpat_xc;
		int* yc=this->__updateExtpat_yc;
		int* rgb_set = this->__updateExtpat_rgbset;

		
		for(int iy=this->_size.h-1;iy>=0;iy--){
			for(int ix=pat_size_w-1;ix>=0;ix--){
				//xw,yw�}�b�v���쐬
				reciprocal= 1.0 / i_xdiv2;
				for(i=xdiv-1;i>=0;i--){
					xw[i]=LT_POS + SQ_SIZE * (ix*xdiv+i + 0.5) * reciprocal;
				}
				reciprocal= 1.0 / i_ydiv2;
				for(i=ydiv-1;i>=0;i--){
					yw[i]=LT_POS + SQ_SIZE * (iy*ydiv+i + 0.5) * reciprocal;
				}
				//1�s�N�Z�����\������s�N�Z�����W�̏W����xc,yc�z��Ɏ擾
				int number_of_pix=0;
				for(i=ydiv-1;i>=0;i--)
				{
					const double para01_x_yw_para02=para01 * yw[i] + para02;
					const double para11_x_yw_para12=para11 * yw[i] + para12;
					const double para12_x_yw_para22=para21 * yw[i]+ 1.0;
					for(j=xdiv-1;j>=0;j--){
							
						const double d = para20 * xw[j] + para12_x_yw_para22;
						if (d == 0) {
							throw NyARException();
						}
						const int xcw= (int) ((para00 * xw[j] + para01_x_yw_para02) / d);
						const int ycw= (int) ((para10 * xw[j] + para11_x_yw_para12) / d);
						if(xcw<0 || xcw>=img_width || ycw<0 ||ycw>=img_height){
							continue;
						}
						xc[number_of_pix] =xcw;
						yc[number_of_pix] =ycw;
						number_of_pix++;
					}
				}
				//1�s�N�Z�����̔z����擾
				reader.getPixelSet(xc,yc,number_of_pix, rgb_set);
				r=g=b=0;
				for(i=number_of_pix*3-1;i>=0;i-=3){
					r += rgb_set[i-2];// R
					g += rgb_set[i-1];// G
					b += rgb_set[i];// B
				}
				//1�s�N�Z���m��
				this->_patdata[iy*pat_size_w+ix]=(((r / xdiv_x_ydiv)&0xff)<<16)|(((g / xdiv_x_ydiv)&0xff)<<8)|(((b / xdiv_x_ydiv)&0xff));
			}
		}
		return;
	}


}
