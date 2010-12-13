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
#include "NyARSingleDetectMarker.h"
#include "NyARParam.h"
#include "NyARMatchPatt_Color_WITHOUT_PCA.h"
#include "INyARSquareDetector.h"
#include "INyARTransMat.h"
#include "NyARTransMat.h"
#include "NyARRasterFilter_ARToolkitThreshold.h"
#include "NyARTransMatResult.h"
#include "NyARColorPatt_O3.h"
#include "NyARSquareDetector.h"
#include "NyARSquareStack.h"
#include "NyAR_types.h"
#define AR_SQUARE_MAX 100
namespace NyARToolkitCPP
{
	NyARSingleDetectMarker::NyARSingleDetectMarker(const NyARParam& i_param, NyARCode* i_code, double i_marker_width)
	{
		const TNyARIntSize* scr_size=i_param.getScreenSize();		
		// ��r�R�[�h��ۑ�
		this->_code = i_code;
		// ��̓I�u�W�F�N�g�����
		this->_square_detect = new NyARSquareDetector(*i_param.getDistortionFactor(),*scr_size);
		this->_transmat = new NyARTransMat(&i_param);
		this->_marker_width = i_marker_width;
		// �]���p�^�[���̃z���_�����
		this->_patt = new NyARColorPatt_O3(i_code->getWidth(), i_code->getHeight());
		// �]��������B
		this->_match_patt = new NyARMatchPatt_Color_WITHOUT_PCA();
		//�Q�l�摜�o�b�t�@�����
		this->_bin_raster=new NyARBinRaster(scr_size->w,scr_size->h);
		//
		this->_square_list =new NyARSquareStack(AR_SQUARE_MAX);
		this->_is_continue =false;
		this->_tobin_filter=new NyARRasterFilter_ARToolkitThreshold(100);
		return;
	}
	NyARSingleDetectMarker::~NyARSingleDetectMarker()
	{
		NyAR_SAFE_DELETE(this->_square_detect);
		NyAR_SAFE_DELETE(this->_transmat);
		NyAR_SAFE_DELETE(this->_patt);
		NyAR_SAFE_DELETE(this->_match_patt);
		NyAR_SAFE_DELETE(this->_bin_raster);
		NyAR_SAFE_DELETE(this->_square_list);
		NyAR_SAFE_DELETE(this->_code);
		NyAR_SAFE_DELETE(this->_tobin_filter);
		return;
	}
	bool NyARSingleDetectMarker::detectMarkerLite(const INyARRgbRaster& i_raster,int i_threshold)
	{
		//�T�C�Y�`�F�b�N
		if(!TNyARIntSize::isEqualSize(this->_bin_raster->getSize(),i_raster.getSize())){
			throw NyARException();
		}

		//���X�^���Q�l�C���[�W�ɕϊ�����.
		this->_tobin_filter->setThreshold(i_threshold);
		this->_tobin_filter->doFilter(i_raster,*this->_bin_raster);


		this->_detected_square = NULL;
		NyARSquareStack& l_square_list = *this->_square_list;
		// �X�N�G�A�R�[�h��T��
		this->_square_detect->detectMarker(*this->_bin_raster, l_square_list);


		int number_of_square = l_square_list.getLength();
		// �R�[�h�͌��������H
		if (number_of_square < 1) {
			return false;
		}

		// �]����ɂȂ�p�^�[�����C���[�W����؂�o��
		if (!this->_patt->pickFromRaster(i_raster,*l_square_list.getItem(0))) {
			// �p�^�[���̐؂�o���Ɏ��s
			return false;
		}
		// �p�^�[����]����ɃZ�b�g
		if (!this->_match_patt->setPatt(*this->_patt)) {
			// �v�Z�Ɏ��s�����B
			throw NyARException();
		}
		// �R�[�h�Ɣ�r����
		this->_match_patt->evaluate(*this->_code);
		int square_index = 0;
		int direction = this->_match_patt->getDirection();
		double confidence = this->_match_patt->getConfidence();
		for (int i = 1; i < number_of_square; i++) {
			// ���̃p�^�[�����擾
			this->_patt->pickFromRaster(i_raster,*l_square_list.getItem(i));
			// �]����ɃZ�b�g����B
			this->_match_patt->setPatt(*this->_patt);
			// �R�[�h�Ɣ�r����
			this->_match_patt->evaluate(*this->_code);
			double c2 = this->_match_patt->getConfidence();
			if (confidence > c2) {
				continue;
			}
			// �����ƈ�v����}�[�J�[���������ۂ�
			square_index = i;
			direction = this->_match_patt->getDirection();
			confidence = c2;
		}
		// �}�[�J�[����ۑ�
		this->_detected_square =l_square_list.getItem(square_index);
		this->_detected_direction = direction;
		this->_detected_confidence = confidence;
		return true;
	}


	void NyARSingleDetectMarker::getTransmationMatrix(NyARTransMatResult& o_result)const
	{
		// ��Ԉ�v�����}�[�J�[�̈ʒu�Ƃ����̕ӂ��v�Z
		if (this->_is_continue) {
			this->_transmat->transMatContinue(*this->_detected_square,this->_detected_direction,this->_marker_width, o_result);
		} else {
			this->_transmat->transMat(*this->_detected_square,this->_detected_direction,this->_marker_width, o_result);
		}
		return;
	}


	double NyARSingleDetectMarker::getConfidence()const
	{
		return this->_detected_confidence;
	}
	int NyARSingleDetectMarker::getDirection()const
	{
		return this->_detected_direction;
	}

	void NyARSingleDetectMarker::setContinueMode(bool i_is_continue)
	{
		this->_is_continue = i_is_continue;
	}

}
