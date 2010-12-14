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
#include "NyARCoord2Linear.h"
#include "NyARSquareContourDetector.h"
#include "NyARMatchPattDeviationColorData.h"
#include "NyARTransMat.h"
#include "NyARRectOffset.h"
#include "NyARRasterFilter_ARToolkitThreshold.h"
#include "NyARRasterThresholdAnalyzer_SlidePTile.h"
#include "NyARCode.h"
namespace NyARToolkitCPP
{
	class DetectSquareCB;
	/**
	* ���̃N���X�́A�����ɂP�̃}�[�J���������邱�Ƃ̂ł���A�A�v���P�[�V�����v���Z�b�T�ł��B
	* �}�[�J�̏o���E�ړ��E���ł��A�C�x���g�Œʒm���邱�Ƃ��ł��܂��B
	* �N���X�ɂ͕����̃}�[�J��o�^�ł��܂��B��̃}�[�J��������ƁA�v���Z�b�T�͌p�����ē����}�[�J��
	* �P�����F���������A�������܂ł̊Ԃ͑��̃}�[�J��F�����܂���B
	* 
	* �C�x���g�́A OnEnter��OnUpdate[n]��OnLeave�̏��Ŕ������܂��B
	* �}�[�J��������Ƃ܂�OnEnter���P�x�������āA���Ԃ̃}�[�J���������ꂽ�����킩��܂��B
	* ����OnUpdate�ɂ��A���݂̕ϊ��s�񂪘A�����ēn����܂��B�Ō�Ƀ}�[�J���������ƁAOnLeave
	* �C�x���g���������܂��B
	* 
	*/
	class SingleARMarkerProcesser:public NyARBaseClass
	{
	public:
		/**�I�[�i�[�����R�Ɏg����^�O�ϐ��ł��B
		*/
		void* tag;
	private:
		int _lost_delay_count;
		int _lost_delay;
		NyARSquareContourDetector* _square_detect;
	protected:
		NyARTransMat* _transmat;
	private:
		NyARRectOffset _offset; 
		int _threshold;
		NyARBinRaster* _bin_raster;
		NyARRasterFilter_ARToolkitThreshold* _tobin_filter;
	protected:
		int _current_arcode_index;
	private:
		NyARRasterThresholdAnalyzer_SlidePTile* _threshold_detect;
	private:
		bool _initialized;
		DetectSquareCB* _detectmarker_cb;
	protected:
		SingleARMarkerProcesser();
		~SingleARMarkerProcesser();
		virtual void initInstance(const NyARParam* i_param,int i_raster_type);
	public:
		/**���o����}�[�J�R�[�h�̔z����w�肵�܂��B ���o��Ԃł��̊֐������s����ƁA
		 * �I�u�W�F�N�g��Ԃɋ������Z�b�g��������܂��B
		 */
		void setARCodeTable(NyARCode* const i_ref_code_table[],int i_number_of_code, int i_code_resolution, double i_marker_width);
		void reset(bool i_is_force);
		void detectMarker(const INyARRgbRaster& i_raster);
		void setConfidenceThreshold(double i_new_cf,double i_exist_cf);
	private:
		NyARTransMatResult __NyARSquare_result;
	/**	�I�u�W�F�N�g�̃X�e�[�^�X���X�V���A�K�v�ɉ����ăn���h���֐����쓮���܂��B
	 * 	�߂�l�́A�u���ۂɃ}�[�J�𔭌����鎖���ł������v�ł��B�N���X�̏�ԂƂ͈قȂ�܂��B
	 */
	private:
		bool updateStatus(const NyARSquare& i_square, int i_code_index);
	protected:
		virtual void onEnterHandler(int i_code)=0;
		virtual void onLeaveHandler()=0;
		virtual void onUpdateHandler(const NyARSquare& i_square, const NyARTransMatResult& result)=0;
	};
}
