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
#include "NyARRotTransOptimize_O2.h"
#include <cmath>
#define AR_GET_TRANS_MAT_MAX_LOOP_COUNT 5
#define AR_GET_TRANS_MAT_MAX_FIT_ERROR 1.0
#define AR_GET_TRANS_MAT_MAX_LOOP_COUNT 5
#define AR_GET_TRANS_MAT_MAX_LOOP_COUNT 5
namespace NyARToolkitCPP
{
	NyARRotTransOptimize_O2::NyARRotTransOptimize_O2(const NyARPerspectiveProjectionMatrix* i_projection_mat_ref)
	{
		this->_projection_mat_ref=i_projection_mat_ref;
		return;
	}
	double NyARRotTransOptimize_O2::optimize(NyARRotMatrix& io_rotmat,TNyARDoublePoint3d& io_transvec,const NyARFitVecCalculator& i_calculator)const
	{
		const TNyARDoublePoint2d* fit_vertex=i_calculator.getFitSquare();
		const TNyARDoublePoint3d* offset_square=i_calculator.getOffsetVertex()->vertex;

		double err = -1;
		/*���[�v�𔲂���^�C�~���O��ARToolKit�ƍ��킹�邽�߂ɕςȂ��Ƃ��Ă܂��B*/
		for (int i = 0;; i++) {
			// <arGetTransMat3>
			err = modifyMatrix(io_rotmat,io_transvec,offset_square,fit_vertex);
			i_calculator.calculateTransfer(io_rotmat, io_transvec);
			err = modifyMatrix(io_rotmat,io_transvec,offset_square,fit_vertex);			
			// //</arGetTransMat3>
			if (err < AR_GET_TRANS_MAT_MAX_FIT_ERROR || i == AR_GET_TRANS_MAT_MAX_LOOP_COUNT-1) {
				break;
			}
			i_calculator.calculateTransfer(io_rotmat, io_transvec);
		}		
		return err;
	}
	double NyARRotTransOptimize_O2::modifyMatrix(NyARRotMatrix& io_rot,TNyARDoublePoint3d& trans,const TNyARDoublePoint3d i_vertex3d[], const TNyARDoublePoint2d i_vertex2d[])const
	{
		double factor;
		double a2, b2, c2;
		double h, x, y;
		double err, minerr = 0;
		int t1, t2, t3;
		int best_idx=0;

		factor = 10.0 * NyAR_PI / 180.0;
		double rot0, rot1, rot2;
		double combo00, combo01, combo02, combo03, combo10, combo11, combo12, combo13, combo20, combo21, combo22, combo23;
		double combo02_2, combo02_5, combo02_8, combo02_11;
		double combo22_2, combo22_5, combo22_8, combo22_11;
		double combo12_2, combo12_5, combo12_8, combo12_11;
		// vertex�W�J
		double VX00, VX01, VX02, VX10, VX11, VX12, VX20, VX21, VX22, VX30, VX31, VX32;
		VX00 = i_vertex3d[0].x;
		VX01 = i_vertex3d[0].y;
		VX02 = i_vertex3d[0].z;
		VX10 = i_vertex3d[1].x;
		VX11 = i_vertex3d[1].y;
		VX12 = i_vertex3d[1].z;
		VX20 = i_vertex3d[2].x;
		VX21 = i_vertex3d[2].y;
		VX22 = i_vertex3d[2].z;
		VX30 = i_vertex3d[3].x;
		VX31 = i_vertex3d[3].y;
		VX32 = i_vertex3d[3].z;
		double P2D00, P2D01, P2D10, P2D11, P2D20, P2D21, P2D30, P2D31;
		P2D00 = i_vertex2d[0].x;
		P2D01 = i_vertex2d[0].y;
		P2D10 = i_vertex2d[1].x;
		P2D11 = i_vertex2d[1].y;
		P2D20 = i_vertex2d[2].x;
		P2D21 = i_vertex2d[2].y;
		P2D30 = i_vertex2d[3].x;
		P2D31 = i_vertex2d[3].y;
		const NyARPerspectiveProjectionMatrix* prjmat = this->_projection_mat_ref;
		double CP0 = prjmat->m00,CP1 = prjmat->m01,CP2 = prjmat->m02,CP4 = prjmat->m10,CP5 = prjmat->m11,CP6 = prjmat->m12,CP8 = prjmat->m20,CP9 = prjmat->m21,CP10 = prjmat->m22;
		combo03 = CP0 * trans.x + CP1 * trans.y + CP2 * trans.z + prjmat->m03;
		combo13 = CP4 * trans.x + CP5 * trans.y + CP6 * trans.z + prjmat->m13;
		combo23 = CP8 * trans.x + CP9 * trans.y + CP10 * trans.z + prjmat->m23;
		double CACA, SASA, SACA, CA, SA;
		double CACACB, SACACB, SASACB, CASB, SASB;
		double SACASC, SACACBSC, SACACBCC, SACACC;

		TNyARDoublePoint3d angle;
		double a_factor[3];
		double sinb[3];
		double cosb[3];
		double b_factor[3];
		double sinc[3];
		double cosc[3];
		double c_factor[3];
		double w, w2;
		double wsin, wcos;

		io_rot.getAngle(angle);// arGetAngle( rot, &a, &b, &c );
		a2 = angle.x;
		b2 = angle.y;
		c2 = angle.z;

		// combo��3�s�ڂ��Ɍv�Z
		for (int i = 0; i < 10; i++) {
			minerr = 1000000000.0;
			// sin-cos�e�[�u�����v�Z(���ꂪ�O�ɏo����Ƃ́c�B)
			for (int j = 0; j < 3; j++) {
				w2 = factor * (j - 1);
				w = a2 + w2;
				a_factor[j] = w;
				w = b2 + w2;
				b_factor[j] = w;
				sinb[j] = sin(w);
				cosb[j] = cos(w);
				w = c2 + w2;
				c_factor[j] = w;
				sinc[j] = sin(w);
				cosc[j] = cos(w);
			}
			//
			for (t1 = 0; t1 < 3; t1++) {
				SA = sin(a_factor[t1]);
				CA = cos(a_factor[t1]);
				// Optimize
				CACA = CA * CA;
				SASA = SA * SA;
				SACA = SA * CA;
				for (t2 = 0; t2 < 3; t2++) {
					wsin = sinb[t2];
					wcos = cosb[t2];
					CACACB = CACA * wcos;
					SACACB = SACA * wcos;
					SASACB = SASA * wcos;
					CASB = CA * wsin;
					SASB = SA * wsin;
					// combo�̌v�Z1
					combo02 = CP0 * CASB + CP1 * SASB + CP2 * wcos;
					combo12 = CP4 * CASB + CP5 * SASB + CP6 * wcos;
					combo22 = CP8 * CASB + CP9 * SASB + CP10 * wcos;

					combo02_2 = combo02 * VX02 + combo03;
					combo02_5 = combo02 * VX12 + combo03;
					combo02_8 = combo02 * VX22 + combo03;
					combo02_11 = combo02 * VX32 + combo03;
					combo12_2 = combo12 * VX02 + combo13;
					combo12_5 = combo12 * VX12 + combo13;
					combo12_8 = combo12 * VX22 + combo13;
					combo12_11 = combo12 * VX32 + combo13;
					combo22_2 = combo22 * VX02 + combo23;
					combo22_5 = combo22 * VX12 + combo23;
					combo22_8 = combo22 * VX22 + combo23;
					combo22_11 = combo22 * VX32 + combo23;
					for (t3 = 0; t3 < 3; t3++) {
						wsin = sinc[t3];
						wcos = cosc[t3];
						SACASC = SACA * wsin;
						SACACC = SACA * wcos;
						SACACBSC = SACACB * wsin;
						SACACBCC = SACACB * wcos;

						rot0 = CACACB * wcos + SASA * wcos + SACACBSC - SACASC;
						rot1 = SACACBCC - SACACC + SASACB * wsin + CACA * wsin;
						rot2 = -CASB * wcos - SASB * wsin;
						combo00 = CP0 * rot0 + CP1 * rot1 + CP2 * rot2;
						combo10 = CP4 * rot0 + CP5 * rot1 + CP6 * rot2;
						combo20 = CP8 * rot0 + CP9 * rot1 + CP10 * rot2;

						rot0 = -CACACB * wsin - SASA * wsin + SACACBCC - SACACC;
						rot1 = -SACACBSC + SACASC + SASACB * wcos + CACA * wcos;
						rot2 = CASB * wsin - SASB * wcos;
						combo01 = CP0 * rot0 + CP1 * rot1 + CP2 * rot2;
						combo11 = CP4 * rot0 + CP5 * rot1 + CP6 * rot2;
						combo21 = CP8 * rot0 + CP9 * rot1 + CP10 * rot2;
						//
						err = 0.0;
						h = combo20 * VX00 + combo21 * VX01 + combo22_2;
						x = P2D00 - (combo00 * VX00 + combo01 * VX01 + combo02_2) / h;
						y = P2D01 - (combo10 * VX00 + combo11 * VX01 + combo12_2) / h;
						err += x * x + y * y;
						h = combo20 * VX10 + combo21 * VX11 + combo22_5;
						x = P2D10 - (combo00 * VX10 + combo01 * VX11 + combo02_5) / h;
						y = P2D11 - (combo10 * VX10 + combo11 * VX11 + combo12_5) / h;
						err += x * x + y * y;
						h = combo20 * VX20 + combo21 * VX21 + combo22_8;
						x = P2D20 - (combo00 * VX20 + combo01 * VX21 + combo02_8) / h;
						y = P2D21 - (combo10 * VX20 + combo11 * VX21 + combo12_8) / h;
						err += x * x + y * y;
						h = combo20 * VX30 + combo21 * VX31 + combo22_11;
						x = P2D30 - (combo00 * VX30 + combo01 * VX31 + combo02_11) / h;
						y = P2D31 - (combo10 * VX30 + combo11 * VX31 + combo12_11) / h;
						err += x * x + y * y;
						if (err < minerr) {
							minerr = err;
							a2 = a_factor[t1];
							b2 = b_factor[t2];
							c2 = c_factor[t3];
							best_idx=t1+t2*3+t3*9;
						}
					}
				}
			}
			if (best_idx==(1+3+9)) {
				factor *= 0.5;
			}
		}
		io_rot.setAngle(a2, b2, c2);
		/* printf("factor = %10.5f\n", factor*180.0/MD_PI); */
		return minerr /4;
	}	

}
