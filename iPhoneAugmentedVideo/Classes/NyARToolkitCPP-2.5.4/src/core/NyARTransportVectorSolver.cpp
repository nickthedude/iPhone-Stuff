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
#include "NyARTransportVectorSolver.h"
#include "nyarcore.h"
namespace NyARToolkitCPP
{
	NyARTransportVectorSolver::NyARTransportVectorSolver(const NyARPerspectiveProjectionMatrix* i_projection_mat_ref,int i_max_vertex)
	{
		this->_projection_mat=i_projection_mat_ref;
		this->_cx=new double[i_max_vertex];
		this->_cy=new double[i_max_vertex];	
		return;
	}
	NyARTransportVectorSolver::~NyARTransportVectorSolver()
	{
		NyAR_SAFE_ARRAY_DELETE(this->_cx);
		NyAR_SAFE_ARRAY_DELETE(this->_cy);
		return;
	}

	void NyARTransportVectorSolver::set2dVertex(const TNyARDoublePoint2d* i_ref_vertex_2d,int i_number_of_vertex)
	{
		//3x2n��2n*3�̍s�񂩂�A�ŏ����@�v�Z���邽�߂�3x3�}�g���N�X�����B		
		//[A]*[A]T
		//�s��[A]��3��ڂ̃L���b�V��
		double* cx=this->_cx;
		double* cy=this->_cy;

		double m22;
		double p00=this->_projection_mat->m00;
		double p01=this->_projection_mat->m01;
		double p11=this->_projection_mat->m11;
		double p12=this->_projection_mat->m12;
		double p02=this->_projection_mat->m02;
		double w1,w2,w3,w4;
		this->_a00=i_number_of_vertex*p00*p00;
		this->_a01_10=i_number_of_vertex*p00*p01;
		this->_a11=i_number_of_vertex*(p01*p01+p11*p11);

		//[A]T*[A]�̌v�Z
		m22=0;
		w1=w2=0;
		for(int i=0;i<i_number_of_vertex;i++){
			//���W��ۑ����Ă����B
			w3=p02-(cx[i]=i_ref_vertex_2d[i].x);
			w4=p12-(cy[i]=i_ref_vertex_2d[i].y);
			w1+=w3;
			w2+=w4;
			m22+=w3*w3+w4*w4;
		}
		this->_a02_20=w1*p00;
		this->_a12_21=p01*w1+p11*w2;
		this->_a22=m22;

		this->_nmber_of_vertex=i_number_of_vertex;
		return;
	}
	void NyARTransportVectorSolver::solveTransportVector(const TNyARDoublePoint3d i_vertex3d[],TNyARDoublePoint3d& o_transfer)const
	{
		int number_of_vertex=this->_nmber_of_vertex;
		double p00=this->_projection_mat->m00;
		double p01=this->_projection_mat->m01;
		double p02=this->_projection_mat->m02;
		double p11=this->_projection_mat->m11;
		double p12=this->_projection_mat->m12;
		//�s��[A]��3��ڂ̃L���b�V��
		double* cx=this->_cx;
		double* cy=this->_cy;			

		//��]�s��������W�̒��_�Q�ɓK��
		//[A]T*[b]���v�Z
		double b1=0,b2=0,b3=0;
		for(int i=0;i<number_of_vertex;i++)
		{
			double w1=i_vertex3d[i].z*cx[i]-p00*i_vertex3d[i].x-p01*i_vertex3d[i].y-p02*i_vertex3d[i].z;
			double w2=i_vertex3d[i].z*cy[i]-p11*i_vertex3d[i].y-p12*i_vertex3d[i].z;
			b1+=w1;
			b2+=w2;
			b3+=cx[i]*w1+cy[i]*w2;
		}
		//[A]T*[b]���v�Z
		b3=p02*b1+p12*b2-b3;//���ԕς�����_����
		b2=p01*b1+p11*b2;
		b1=p00*b1;
		//([A]T*[A])*[T]=[A]T*[b]��������ŉ����B
		//a01��a10��0�Ɖ��肵�Ă��ǂ��񂶂�Ȃ����ȁH
		double a00=this->_a00;
		double a01=this->_a01_10;
		double a02=this->_a02_20;
		double a11=this->_a11;
		double a12=this->_a12_21;
		double a22=this->_a22;
		
		double t1=a22*b2-a12*b3;
		double t2=a12*b2-a11*b3;
		double t3=a01*b3-a02*b2;
		double t4=a12*a12-a11*a22;
		double t5=a02*a12-a01*a22;
		double t6=a02*a11-a01*a12;
		double det=a00*t4-a01*t5 + a02*t6;
		o_transfer.x= (a01*t1 - a02*t2 +b1*t4)/det;
		o_transfer.y=-(a00*t1 + a02*t3 +b1*t5)/det;
		o_transfer.z= (a00*t2 + a01*t3 +b1*t6)/det;
		
		return;
	}
}
