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
#include "NyARCameraDistortionFactor.h"
#include <cmath>

namespace NyARToolkitCPP
{

	void NyARCameraDistortionFactor::copyFrom(const NyARCameraDistortionFactor& i_ref)
	{
		this->_f0=i_ref._f0;
		this->_f1=i_ref._f1;
		this->_f2=i_ref._f2;
		this->_f3=i_ref._f3;
		return;
	}
	/**
	* �z��̒l���t�@�N�^�l�Ƃ��ăZ�b�g����B
	* @param i_factor
	* 4�v�f�ȏ�̔z��
	*/
	 void NyARCameraDistortionFactor::setValue(const double i_factor[])
	{
		this->_f0=i_factor[0];
		this->_f1=i_factor[1];
		this->_f2=i_factor[2];
		this->_f3=i_factor[3];
		return;
	}
	 void NyARCameraDistortionFactor::getValue(double o_factor[])const
	{
		o_factor[0]=this->_f0;
		o_factor[1]=this->_f1;
		o_factor[2]=this->_f2;
		o_factor[3]=this->_f3;
		return;
	}
	 void NyARCameraDistortionFactor::changeScale(double i_scale)
	{
		this->_f0=this->_f0*i_scale;// newparam->dist_factor[0] =source->dist_factor[0] *scale;
		this->_f1=this->_f1*i_scale;// newparam->dist_factor[1] =source->dist_factor[1] *scale;
		this->_f2=this->_f2/ (i_scale * i_scale);// newparam->dist_factor[2]=source->dist_factor[2]/ (scale*scale);
		//this.f3=this.f3;// newparam->dist_factor[3] =source->dist_factor[3];
		return;
	}
	/**
	* int arParamIdeal2Observ( const double dist_factor[4], const double ix,const double iy,double *ox, double *oy ) �֐��̑�֊֐�
	* 
	* @param i_in
	* @param o_out
	*/
	void NyARCameraDistortionFactor::ideal2Observ(const TNyARDoublePoint2d& i_in, TNyARDoublePoint2d& o_out)const
	{
		const double x = (i_in.x - this->_f0) * this->_f3;
		const double y = (i_in.y - this->_f1) * this->_f3;
		if (x == 0.0 && y == 0.0) {
			o_out.x = this->_f0;
			o_out.y = this->_f1;
		} else {
			const double d = 1.0 - this->_f2 / 100000000.0 * (x * x + y * y);
			o_out.x = x * d + this->_f0;
			o_out.y = y * d + this->_f1;
		}
		return;
	}

	/**
	* ideal2Observ���܂Ƃ߂Ď��s���܂��B
	* @param i_in
	* @param o_out
	*/
	void NyARCameraDistortionFactor::ideal2ObservBatch(const TNyARDoublePoint2d* i_in[], TNyARDoublePoint2d o_out[], int i_size)const
	{
		double x, y;
		const double d0 = this->_f0;
		const double d1 = this->_f1;
		const double d3 = this->_f3;
		const double d2_w = this->_f2 / 100000000.0;
		for (int i = 0; i < i_size; i++) {
			x = (i_in[i]->x - d0) * d3;
			y = (i_in[i]->y - d1) * d3;
			if (x == 0.0 && y == 0.0) {
				o_out[i].x = d0;
				o_out[i].y = d1;
			} else {
				const double d = 1.0 - d2_w * (x * x + y * y);
				o_out[i].x = x * d + d0;
				o_out[i].y = y * d + d1;
			}
		}
		return;
	}

	/**
	* int arParamObserv2Ideal( const double dist_factor[4], const double ox,const double oy,double *ix, double *iy );
	* 
	* @param ix
	* @param iy
	* @param ix
	* @param iy
	* @return
	*/
	void NyARCameraDistortionFactor::observ2Ideal(double ix, double iy, TNyARDoublePoint2d& o_point)const
	{
		double z02, z0, p, q, z, px, py, opttmp_1;
		const double d0 = this->_f0;
		const double d1 = this->_f1;

		px = ix - d0;
		py = iy - d1;
		p = this->_f2 / 100000000.0;
		z02 = px * px + py * py;
		q = z0 = sqrt(z02);// Optimize//q = z0 = Math.sqrt(px*px+ py*py);

		for (int i = 1;; i++) {
			if (z0 != 0.0) {
				// Optimize opttmp_1
				opttmp_1 = p * z02;
				z = z0 - ((1.0 - opttmp_1) * z0 - q) / (1.0 - 3.0 * opttmp_1);
				px = px * z / z0;
				py = py * z / z0;
			} else {
				px = 0.0;
				py = 0.0;
				break;
			}
			if (i == PD_LOOP) {
				break;
			}
			z02 = px * px + py * py;
			z0 = sqrt(z02);// Optimize//z0 = Math.sqrt(px*px+ py*py);
		}
		o_point.x = px / this->_f3 + d0;
		o_point.y = py / this->_f3 + d1;
		return;
	}

	/**
	* �w��͈͂�observ2Ideal���܂Ƃ߂Ď��s���āA���ʂ�o_ideal�Ɋi�[���܂��B
	* @param i_x_coord
	* @param i_y_coord
	* @param i_start
	*            coord�J�n�_
	* @param i_num
	*            �v�Z��
	* @param o_ideal
	*            �o�̓o�b�t�@[i_num][2]�ł��邱�ƁB
	*/
	void NyARCameraDistortionFactor::observ2IdealBatch(const int i_x_coord[], const int i_y_coord[],int i_start, int i_num, double o_x_coord[],double o_y_coord[])const
	{
		double z02, z0, q, z, px, py, opttmp_1;
		const double d0 = this->_f0;
		const double d1 = this->_f1;
		const double d3 = this->_f3;
		const double p = this->_f2 / 100000000.0;
		for (int j = 0; j < i_num; j++) {

			px = i_x_coord[i_start + j] - d0;
			py = i_y_coord[i_start + j] - d1;

			z02 = px * px + py * py;
			q = z0 = sqrt(z02);// Optimize//q = z0 = Math.sqrt(px*px+py*py);

			for (int i = 1;; i++) {
				if (z0 != 0.0) {
					// Optimize opttmp_1
					opttmp_1 = p * z02;
					z = z0 - ((1.0 - opttmp_1) * z0 - q)/ (1.0 - 3.0 * opttmp_1);
					px = px * z / z0;
					py = py * z / z0;
				} else {
					px = 0.0;
					py = 0.0;
					break;
				}
				if (i == PD_LOOP) {
					break;
				}
				z02 = px * px + py * py;
				z0 = sqrt(z02);// Optimize//z0 = Math.sqrt(px*px+ py*py);
			}
			o_x_coord[j] = px / d3 + d0;
			o_y_coord[j] = py / d3 + d1;
		}
		return;
	}

}


