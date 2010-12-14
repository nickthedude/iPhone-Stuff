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
#include "INyARRasterFilter_Rgb2Bin.h" 
#include "NyARCustomSingleDetectMarker.h"
#include "TNyARMatchPattResult.h"
#include "NyARSquareContourDetector.h"
#include "NyARCoord2Linear.h"
#include "INyARColorPatt.h"
#include "INyARTransMat.h"
#include "NyARParam.h"

#include "NyARCode.h"
namespace NyARToolkitCPP
{
	class DetectSquareCB;
	class NyARCustomSingleDetectMarker: public NyARBaseClass
	{
	private:
		bool _is_continue;
		NyARSquareContourDetector* _square_detect;
		INyARTransMat* _transmat;
		//�揈���p
		NyARBinRaster* _bin_raster;
	protected:
		const INyARRasterFilter_Rgb2Bin* _tobin_filter;
	private:
		DetectSquareCB* _detect_cb;
		NyARRectOffset _offset; 

	protected:
		NyARCustomSingleDetectMarker();
		void initInstance(
			INyARColorPatt* i_patt_inst,
			NyARSquareContourDetector* i_sqdetect_inst,
			INyARTransMat* i_transmat_inst,
			const INyARRasterFilter_Rgb2Bin* i_filter,
			const NyARParam& i_ref_param,
			const NyARCode*	i_ref_code,
			double		i_marker_width);
	public:
		~NyARCustomSingleDetectMarker();
		/**
		 * i_image�Ƀ}�[�J�[���o���������s���A���ʂ��L�^���܂��B
		 * 
		 * @param i_raster
		 * �}�[�J�[�����o����C���[�W���w�肵�܂��B�C���[�W�T�C�Y�́A�J�����p�����[�^
		 * �ƈ�v���Ă��Ȃ���΂Ȃ�܂���B
		 * @return �}�[�J�[�����o�ł�������^�U�l�ŕԂ��܂��B
		 * @throws NyARException
		 */
		bool detectMarkerLite(const INyARRgbRaster& i_raster);
		void getTransmationMatrix(NyARTransMatResult &o_result)const;

		/**
		* ��ʏ�̃}�[�J���_����z��ւ̃��t�@�����X��Ԃ��܂��B
		* �Ԃ��ꂽ�I�u�W�F�N�g�̓N���X�ɏ��L���������Ă��܂��B�N���X�̃����o�֐������s����Ɠ��e�������ς��܂��B
		* �O���Ńf�[�^���X�g�b�N����ꍇ�́AgetSquarePosition�ŕ������ĉ������B
		* @return
		*/
		const NyARSquare& refSquare()const;
		/**
		* ���o�����}�[�J�[�̈�v�x��Ԃ��܂��B
		* 
		* @return �}�[�J�[�̈�v�x��Ԃ��܂��B0�`1�܂ł̒l���Ƃ�܂��B ��v�x���Ⴂ�ꍇ�ɂ́A��F���̉\���������Ȃ�܂��B
		* @throws NyARException
		*/
		double getConfidence()const;
		/**
		* getTransmationMatrix�̌v�Z���[�h��ݒ肵�܂��B �����l��TRUE�ł��B
		* 
		* @param i_is_continue
		* TRUE�Ȃ�AtransMatCont�݊��̌v�Z�����܂��B FALSE�Ȃ�AtransMat�݊��̌v�Z�����܂��B
		*/
		void setContinueMode(bool i_is_continue);
	};


}

