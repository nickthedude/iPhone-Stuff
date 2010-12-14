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
#include "NyARTransMat.h"
#include "NyARRotTransOptimize_O2.h"
#include "NyARTransportVectorSolver.h"
#include "NyARPartialDifferentiationOptimize.h"
#include "nyarcore.h"
#include <cstdlib>
#include <stdio.h>
namespace NyARToolkitCPP
{

	NyARTransMat::NyARTransMat()
	{
		//_calculator,_rotmatrix,_mat_optimize���R���X�g���N�^�̏I�����
		//�쐬���Ċ��蓖�ĂĂ��������B
		return;
	}
	NyARTransMat::NyARTransMat(const NyARParam* i_param)
	{
		const NyARCameraDistortionFactor* dist=&i_param->getDistortionFactor();
		const NyARPerspectiveProjectionMatrix* pmat=&i_param->getPerspectiveProjectionMatrix();
		this->_transsolver=new NyARTransportVectorSolver(pmat,4);
		//�݊������d�v�Ȏ��́ANyARRotMatrix_ARToolKit���g�����ƁB
		//������NyARRotMatrix_NyARToolKit��NyARRotMatrix_ARToolKit�����������ǁA���������l�������B
		this->_rotmatrix = new NyARRotMatrix(pmat);
		this->_mat_optimize=new NyARPartialDifferentiationOptimize(pmat);
		this->_ref_dist_factor=dist;
		this->_projection_mat_ref=pmat;
	}
	NyARTransMat::~NyARTransMat()
	{
		NyAR_SAFE_DELETE(this->_transsolver);
		NyAR_SAFE_DELETE(this->_rotmatrix);
		NyAR_SAFE_DELETE(this->_mat_optimize);
		return;
	}

	/**
	 * ���_�������ɁA�G���[臒l���v�Z���܂��B
	 * @param i_vertex
	 */
	double NyARTransMat::makeErrThreshold(const TNyARDoublePoint2d i_vertex[])
	{
		double a,b,l1,l2;
		a=i_vertex[0].x-i_vertex[2].x;
		b=i_vertex[0].y-i_vertex[2].y;
		l1=a*a+b*b;
		a=i_vertex[1].x-i_vertex[3].x;
		b=i_vertex[1].y-i_vertex[3].y;
		l2=a*a+b*b;
		return (sqrt(l1>l2?l1:l2))/200;
	}


	void NyARTransMat::transMat(const NyARSquare& i_square,const NyARRectOffset& i_offset, NyARTransMatResult& o_result)
	{
		TNyARDoublePoint3d trans;
		
		double err_threshold=makeErrThreshold(i_square.sqvertex);
		
		TNyARDoublePoint2d vertex_2d[4];
		TNyARDoublePoint3d vertex_3d[4];

		//���s�ړ��ʌv�Z�@�ɁA2D���W�n���Z�b�g
		this->_ref_dist_factor->ideal2ObservBatch(i_square.sqvertex, vertex_2d,4);		
		this->_transsolver->set2dVertex(vertex_2d,4);

		//��]�s����v�Z
		this->_rotmatrix->initRotBySquare(i_square.line,i_square.sqvertex);

		//��]���3D���W�n����A���s�ړ��ʂ��v�Z
		this->_rotmatrix->getPoint3dBatch(i_offset.vertex,vertex_3d,4);
		this->_transsolver->solveTransportVector(vertex_3d,trans);
		
		//�v�Z���ʂ̍œK��(���s�ړ��ʂƉ�]�s��̍œK��)
		o_result.error=this->optimize(*this->_rotmatrix, trans,*this->_transsolver,i_offset.vertex, vertex_2d,err_threshold);
		
		// �}�g���N�X�̕ۑ�
		this->updateMatrixValue(*this->_rotmatrix,trans,o_result);
		return;
	}


	void NyARTransMat::transMatContinue(const NyARSquare& i_square,const NyARRectOffset& i_offset, NyARTransMatResult& io_result_conv)
	{
		TNyARDoublePoint3d trans;

		// io_result_conv�������l�Ȃ�AtransMat�Ōv�Z����B
		if (!io_result_conv.has_value) {
			this->transMat(i_square,i_offset, io_result_conv);
			return;
		}
		
		//�œK���v�Z��臒l������
		double err_threshold=makeErrThreshold(i_square.sqvertex);
		
		//���s�ړ��ʌv�Z�@�ɁA2D���W�n���Z�b�g
		TNyARDoublePoint2d vertex_2d[4];
		TNyARDoublePoint3d vertex_3d[4];
		this->_ref_dist_factor->ideal2ObservBatch(i_square.sqvertex, vertex_2d,4);		
		this->_transsolver->set2dVertex(vertex_2d,4);

		//��]�s����v�Z
		this->_rotmatrix->initRotBySquare(i_square.line,i_square.sqvertex);

		//��]���3D���W�n����A���s�ړ��ʂ��v�Z
		this->_rotmatrix->getPoint3dBatch(i_offset.vertex,vertex_3d,4);
		this->_transsolver->solveTransportVector(vertex_3d,trans);

		//���݂̃G���[���[�g���v�Z���Ă���
		double min_err=errRate(*this->_rotmatrix,trans, i_offset.vertex, vertex_2d,4,vertex_3d);
		NyARDoubleMatrix33& rot=this->__rot;
		//�G���[���[�g��臒l�����Ă���A�Q�C��
		if(min_err<io_result_conv.error+err_threshold){
			rot.setValue(*this->_rotmatrix);
			//�œK�����Ă݂�B
			for (int i = 0;i<5; i++) {
				//�ϊ��s��̍œK��
				this->_mat_optimize->modifyMatrix(rot, trans, i_offset.vertex, vertex_2d, 4);
				double err=errRate(rot,trans,i_offset.vertex, vertex_2d,4,vertex_3d);
				//System.out.println("E:"+err);
				if(min_err-err<err_threshold/2){
					//System.out.println("BREAK");
					break;
				}
				this->_transsolver->solveTransportVector(vertex_3d, trans);
				this->_rotmatrix->setValue(rot);
				min_err=err;
			}
			this->updateMatrixValue(*this->_rotmatrix,trans,io_result_conv);
		}else{
			
			//��]���3D���W�n����A���s�ړ��ʂ��v�Z
			this->_rotmatrix->getPoint3dBatch(i_offset.vertex,vertex_3d,4);
			this->_transsolver->solveTransportVector(vertex_3d,trans);
			
			//�v�Z���ʂ̍œK��(���s�ړ��ʂƉ�]�s��̍œK��)
			min_err=this->optimize(*this->_rotmatrix, trans,*this->_transsolver,i_offset.vertex, vertex_2d,err_threshold);
			this->updateMatrixValue(*this->_rotmatrix,trans,io_result_conv);
		}
		io_result_conv.error=min_err;
		return;
	}
	double NyARTransMat::optimize(NyARRotMatrix& io_rotmat,TNyARDoublePoint3d& io_transvec,const INyARTransportVectorSolver& i_solver,const TNyARDoublePoint3d i_offset_3d[],TNyARDoublePoint2d i_2d_vertex[],double i_err_threshold)
	{
		//System.out.println("START");
		//printf("[ST]\n");
		TNyARDoublePoint3d vertex_3d[4];
		//�����̃G���[�l���v�Z
		double min_err=errRate(io_rotmat, io_transvec, i_offset_3d, i_2d_vertex,4,vertex_3d);
		NyARDoubleMatrix33& rot=this->__rot;
		rot.setValue(io_rotmat);
		for (int i = 0;i<5; i++) {
			//�ϊ��s��̍œK��
			this->_mat_optimize->modifyMatrix(rot, io_transvec, i_offset_3d, i_2d_vertex, 4);
			double err=errRate(rot,io_transvec, i_offset_3d, i_2d_vertex,4,vertex_3d);
			//System.out.println("E:"+err);
			//printf("[E:]%f\n",err);
			if(min_err-err<i_err_threshold){
				//printf("[BK]\n");
				//System.out.println("BREAK");
				break;
			}
			i_solver.solveTransportVector(vertex_3d, io_transvec);
			io_rotmat.setValue(rot);
			min_err=err;
		}
		//System.out.println("END");
		//printf("[ED]\n");
		return min_err;
	}
	
	//�G���[���[�g�v�Z�@
	double NyARTransMat::errRate(const NyARDoubleMatrix33& io_rot,const TNyARDoublePoint3d& i_trans,const TNyARDoublePoint3d i_vertex3d[], TNyARDoublePoint2d i_vertex2d[],int i_number_of_vertex,TNyARDoublePoint3d o_rot_vertex[])
	{
		const NyARPerspectiveProjectionMatrix& cp = *this->_projection_mat_ref;
		double cp00=cp.m00;
		double cp01=cp.m01;
		double cp02=cp.m02;
		double cp11=cp.m11;
		double cp12=cp.m12;

		double err=0;
		for(int i=0;i<i_number_of_vertex;i++){
			double x3d,y3d,z3d;
			o_rot_vertex[i].x=x3d=io_rot.m00*i_vertex3d[i].x+io_rot.m01*i_vertex3d[i].y+io_rot.m02*i_vertex3d[i].z;
			o_rot_vertex[i].y=y3d=io_rot.m10*i_vertex3d[i].x+io_rot.m11*i_vertex3d[i].y+io_rot.m12*i_vertex3d[i].z;
			o_rot_vertex[i].z=z3d=io_rot.m20*i_vertex3d[i].x+io_rot.m21*i_vertex3d[i].y+io_rot.m22*i_vertex3d[i].z;
			x3d+=i_trans.x;
			y3d+=i_trans.y;
			z3d+=i_trans.z;
			
			//�ˉe�ϊ�
			double x2d=x3d*cp00+y3d*cp01+z3d*cp02;
			double y2d=y3d*cp11+z3d*cp12;
			double h2d=z3d;
			
			//�G���[���[�g�v�Z
			double t1=i_vertex2d[i].x-x2d/h2d;
			double t2=i_vertex2d[i].y-y2d/h2d;
			err+=t1*t1+t2*t2;
			
		}
		return err/i_number_of_vertex;
	}
	void NyARTransMat::updateMatrixValue(const NyARRotMatrix& i_rot,const TNyARDoublePoint3d& i_trans,NyARTransMatResult& o_result)
	{
		o_result.m00=i_rot.m00;
		o_result.m01=i_rot.m01;
		o_result.m02=i_rot.m02;
		o_result.m03=i_trans.x;

		o_result.m10 = i_rot.m10;
		o_result.m11 = i_rot.m11;
		o_result.m12 = i_rot.m12;
		o_result.m13 = i_trans.y;

		o_result.m20 = i_rot.m20;
		o_result.m21 = i_rot.m21;
		o_result.m22 = i_rot.m22;
		o_result.m23 = i_trans.z;
		o_result.has_value = true;
		return;
	}
}

