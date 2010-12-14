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
#include "NyARDoubleMatrix33.h"

namespace NyARToolkitCPP
{
	NyArray<NyARDoubleMatrix33>* NyARDoubleMatrix33::createArray(int i_number)
	{
		return new NyArray<NyARDoubleMatrix33>(i_number);
	}
	 void NyARDoubleMatrix33::setValue(const double i_value[])
	{
		this->m00=i_value[0];
		this->m01=i_value[1];
		this->m02=i_value[2];
		this->m10=i_value[3];
		this->m11=i_value[4];
		this->m12=i_value[5];
		this->m20=i_value[6];
		this->m21=i_value[7];
		this->m22=i_value[8];
		return;
	}
	void NyARDoubleMatrix33::getValue(double o_value[])const
	{
		o_value[0]=this->m00;
		o_value[1]=this->m01;
		o_value[2]=this->m02;
		o_value[3]=this->m10;
		o_value[4]=this->m11;
		o_value[5]=this->m12;
		o_value[6]=this->m20;
		o_value[7]=this->m21;
		o_value[8]=this->m22;
		return;
	}
	void NyARDoubleMatrix33::setValue(const NyARDoubleMatrix33& i_value)
	{
		this->m00=i_value.m00;
		this->m01=i_value.m01;
		this->m02=i_value.m02;
		this->m10=i_value.m10;
		this->m11=i_value.m11;
		this->m12=i_value.m12;
		this->m20=i_value.m20;
		this->m21=i_value.m21;
		this->m22=i_value.m22;
		return;
	}	
	bool NyARDoubleMatrix33::inverse(const NyARDoubleMatrix33& i_src)
	{
		double a11,a12,a13,a21,a22,a23,a31,a32,a33;
		double b11,b12,b13,b21,b22,b23,b31,b32,b33;	
		a11=i_src.m00;a12=i_src.m01;a13=i_src.m02;
		a21=i_src.m10;a22=i_src.m11;a23=i_src.m12;
		a31=i_src.m20;a32=i_src.m21;a33=i_src.m22;
		
		b11=a22*a33-a23*a32;
		b12=a32*a13-a33*a12;
		b13=a12*a23-a13*a22;
		
		b21=a23*a31-a21*a33;
		b22=a33*a11-a31*a13;
		b23=a13*a21-a11*a23;
		
		b31=a21*a32-a22*a31;
		b32=a31*a12-a32*a11;
		b33=a11*a22-a12*a21;
				
		double det_1=a11*b11+a21*b12+a31*b13;
		if(det_1==0){
			return false;
		}
		det_1=1/det_1;

		this->m00=b11*det_1;
		this->m01=b12*det_1;
		this->m02=b13*det_1;
		
		this->m10=b21*det_1;
		this->m11=b22*det_1;
		this->m12=b23*det_1;
		
		this->m20=b31*det_1;
		this->m21=b32*det_1;
		this->m22=b33*det_1;
		
		return true;
	}
	/**
	 * ���̊֐��́A0-PI�̊ԂŒl��Ԃ��܂��B
	 * @param o_out
	 */
	void NyARDoubleMatrix33::getZXYAngle(TNyARDoublePoint3d& o_out)const
	{
		double sina = this->m21;
		if (sina >= 1.0) {
			o_out.x = NyAR_PI / 2;
			o_out.y = 0;
			o_out.z = atan2(-this->m10, this->m00);
		} else if (sina <= -1.0) {
			o_out.x = -NyAR_PI / 2;
			o_out.y = 0;
			o_out.z = atan2(-this->m10, this->m00);
		} else {
			o_out.x = asin(sina);
			o_out.z = atan2(-this->m01, this->m11);
			o_out.y = atan2(-this->m20, this->m22);
		}
	}
	void NyARDoubleMatrix33::setZXYAngle(const TNyARDoublePoint3d& i_angle)
	{
		setZXYAngle(i_angle.x,i_angle.y,i_angle.z);
		return;
	}
	void NyARDoubleMatrix33::setZXYAngle(double i_x, double i_y, double i_z)
	{
		double sina = sin(i_x);
		double cosa = cos(i_x);
		double sinb = sin(i_y);
		double cosb = cos(i_y);
		double sinc = sin(i_z);
		double cosc = cos(i_z);
		this->m00 = cosc * cosb - sinc * sina * sinb;
		this->m01 = -sinc * cosa;
		this->m02 = cosc * sinb + sinc * sina * cosb;
		this->m10 = sinc * cosb + cosc * sina * sinb;
		this->m11 = cosc * cosa;
		this->m12 = sinc * sinb - cosc * sina * cosb;
		this->m20 = -cosa * sinb;
		this->m21 = sina;
		this->m22 = cosb * cosa;
		return;
	}
	/**
	 * ��]�s���K�����č��W�ϊ����܂��B
	 * @param i_angle
	 * @param o_out
	 */
	void NyARDoubleMatrix33::transformVertex(const TNyARDoublePoint3d& i_position,TNyARDoublePoint3d& o_out)
	{
		transformVertex(i_position.x,i_position.y,i_position.z,o_out);
		return;
	}
	
	void NyARDoubleMatrix33::transformVertex(double i_x,double i_y,double i_z,TNyARDoublePoint3d& o_out)
	{
		o_out.x=this->m00*i_x+this->m01*i_y+this->m02*i_z;
		o_out.y=this->m10*i_x+this->m11*i_y+this->m12*i_z;
		o_out.z=this->m20*i_x+this->m21*i_y+this->m22*i_z;
		return;
	}
}
