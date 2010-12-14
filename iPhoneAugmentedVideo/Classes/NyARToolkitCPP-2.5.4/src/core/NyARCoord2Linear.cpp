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

#include "NyARCoord2Linear.h"
#include "nyarcore.h"

namespace NyARToolkitCPP
{

	NyARCoord2Linear::NyARCoord2Linear(const TNyARIntSize& i_size,const NyARCameraDistortionFactor& i_distfactor_ref)
	{
		//�c�݌v�Z�e�[�u�������ƁA8*width/height*2�̗̈������܂��B
		//�̈����肽���Ȃ��ꍇ�́Ai_dist_factor_ref�̒l�����̂܂܎g���Ă��������B
		this->_dist_factor = new NyARObserv2IdealMap(i_distfactor_ref,i_size);
		// �֊s�o�b�t�@
		this->_pca=new NyARPca2d_MatrixPCA_O2();
		this->_xpos=new double[i_size.w+i_size.h];//�ő�Ӓ���this._width+this._height
		this->_ypos=new double[i_size.w+i_size.h];//�ő�Ӓ���this._width+this._height
		return;
	}
	NyARCoord2Linear::~NyARCoord2Linear()
	{
		NyAR_SAFE_DELETE(this->_pca);
		NyAR_SAFE_ARRAY_DELETE(this->_xpos);
		NyAR_SAFE_ARRAY_DELETE(this->_ypos);
		NyAR_SAFE_DELETE(this->_dist_factor);
	}


	/**
	* �֊s�_�W������ay+bx+c=0�̒��������v�Z���܂��B
	* @param i_st
	* @param i_ed
	* @param i_xcoord
	* @param i_ycoord
	* @param i_cood_num
	* @param o_line
	* @return
	* @throws NyARException
	*/
	bool NyARCoord2Linear::coord2Line(int i_st,int i_ed,int i_xcoord[], int i_ycoord[],int i_cood_num,TNyARLinear& o_line)
	{
		//���_���擾
		int n,st,ed;
		double w1;

		//�T����Ԃ̌���
		if(i_ed>=i_st){
			//���_[i]���璸�_[i+1]�܂ł̗֊s���A1��Ԃɂ���Ƃ�
			w1 = (double) (i_ed - i_st + 1) * 0.05 + 0.5;
			//�T����Ԃ̌���
			st = (int) (i_st+w1);
			ed = (int) (i_ed - w1);
		}else{
			//���_[i]���璸�_[i+1]�܂ł̗֊s���A2��Ԃɕ�����Ă���Ƃ�
			w1 = (double)((i_ed+i_cood_num-i_st+1)%i_cood_num) * 0.05 + 0.5;
			//�T����Ԃ̌���
			st = ((int)(i_st+w1))%i_cood_num;
			ed = ((int)(i_ed+i_cood_num-w1))%i_cood_num;
		}
		//�T����Ԑ����m�F
		if(st<=ed){
			//�T����Ԃ�1���
			n = ed - st + 1;
			this->_dist_factor->observ2IdealBatch(i_xcoord, i_ycoord, st, n,this->_xpos,this->_ypos,0);
		}else{
			//�T����Ԃ�2���
			n=ed+1+i_cood_num-st;
			this->_dist_factor->observ2IdealBatch(i_xcoord, i_ycoord, st,i_cood_num-st,this->_xpos,this->_ypos,0);
			this->_dist_factor->observ2IdealBatch(i_xcoord, i_ycoord, 0,ed+1,this->_xpos,this->_ypos,i_cood_num-st);
		}
		//�v�f���̊m�F
		if (n < 2) {
			// n��2�ȉ���matrix.PCA���v�Z���邱�Ƃ͂ł��Ȃ��̂ŁA�G���[
			return false;
		}
		//�听�����͂���B
		NyARDoubleMatrix22& evec=this->__getSquareLine_evec;
		double mean[2];
		double ev[2];


		this->_pca->pca(this->_xpos,this->_ypos,n,evec,ev,mean);
		o_line.dy = evec.m01;// line[i][0] = evec->m[1];
		o_line.dx = -evec.m00;// line[i][1] = -evec->m[0];
		o_line.c = -(o_line.dy * mean[0] + o_line.dx * mean[1]);// line[i][2] = -(line[i][0]*mean->v[0] + line[i][1]*mean->v[1]);

		return true;
	}

}

