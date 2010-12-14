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
#include "SingleARMarkerProcessor.h"
#include "nyarcore.h"
#include <float.h>
#include "NyARMatchPatt_Color_WITHOUT_PCA.h"
#include "NyARSquareContourDetector_Rle.h"
#include "NyARColorPatt_Perspective_O2.h"
namespace NyARToolkitCPP
{
	/**
	* detectMarker�̃R�[���o�b�N�֐�
	*/
	class DetectSquareCB : public NyARSquareContourDetector::IDetectMarkerCallback
	{
		//���J�v���p�e�B
	public:
		NyARSquare square;
		double confidence;
		int code_index;
		double cf_threshold_new;
		double cf_threshold_exist;

		//�Q��
	private:
		const INyARRgbRaster* _ref_raster;
		//���L�C���X�^���X
		INyARColorPatt* _inst_patt;
		NyARMatchPattDeviationColorData* _deviation_data;
		NyArray<NyARMatchPatt_Color_WITHOUT_PCA*>* _match_patt;
		NyARCoord2Linear* _coordline;

	public:
		DetectSquareCB(const NyARParam& i_param)
		{
			this->_match_patt    =NULL;
			this->_deviation_data=NULL;
			this->_inst_patt     =NULL;
			this->_coordline=new NyARCoord2Linear(i_param.getScreenSize(),i_param.getDistortionFactor());
			return;
		}
		~DetectSquareCB()
		{
			freeARCodeTable();
			NyAR_SAFE_DELETE(this->_coordline);
			return;
		}
		void setARCodeTable(NyARCode* const i_ref_code_table[],int i_number_of_code, int i_code_resolution)
		{
			freeARCodeTable();
			//unmanaged�Ŏ�������Ƃ��́A�����Ń��\�[�X��������邱�ƁB
			this->_deviation_data=new NyARMatchPattDeviationColorData(i_code_resolution,i_code_resolution);
			this->_inst_patt=new NyARColorPatt_Perspective_O2(i_code_resolution,i_code_resolution,4,25);
			this->_match_patt = new NyArray<NyARMatchPatt_Color_WITHOUT_PCA*>(i_number_of_code);
			for(int i=0;i<i_number_of_code;i++){
				this->_match_patt->item[i]=new NyARMatchPatt_Color_WITHOUT_PCA(i_ref_code_table[i]);
			}
		}
	private:
		void freeARCodeTable()
		{
			//�����ς̃I�u�W�F�N�g���폜
			NyAR_SAFE_DELETE(this->_deviation_data);
			NyAR_SAFE_DELETE(this->_inst_patt);
			if(this->_match_patt!=NULL){
				for(int i=0;i<this->_match_patt->length;i++){
					NyAR_SAFE_DELETE(this->_match_patt->item[i]);
				}
				NyAR_SAFE_DELETE(this->_match_patt);
			}
			return;
		}
	private:
		int _target_id;
	public:
		void init(const INyARRgbRaster* i_raster,int i_target_id)
		{
			this->cf_threshold_new = 0.50;
			this->cf_threshold_exist = 0.30;

			this->_ref_raster=i_raster;
			this->_target_id=i_target_id;
			this->code_index=-1;
			this->confidence=DBL_MIN;
		}

		/**
		* ��`�����t���邽�тɌĂяo����܂��B
		* ����������`�̃p�^�[�����������āA���ʂ��l���������_�f�[�^���m�ۂ��܂��B
		*/
	public:
		void onSquareDetect(const NyARSquareContourDetector& i_sender,int i_coordx[],int i_coordy[],int i_coor_num,int i_vertex_index[])
		{
			if (this->_match_patt==NULL) {
				return;
			}
			//�֊s���W���璸�_���X�g�ɕϊ�
			TNyARIntPoint2d vertex[4];
			vertex[0].x=i_coordx[i_vertex_index[0]];
			vertex[0].y=i_coordy[i_vertex_index[0]];
			vertex[1].x=i_coordx[i_vertex_index[1]];
			vertex[1].y=i_coordy[i_vertex_index[1]];
			vertex[2].x=i_coordx[i_vertex_index[2]];
			vertex[2].y=i_coordy[i_vertex_index[2]];
			vertex[3].x=i_coordx[i_vertex_index[3]];
			vertex[3].y=i_coordy[i_vertex_index[3]];

			//�摜���擾
			if (!this->_inst_patt->pickFromRaster(*this->_ref_raster,vertex)){
				return;//�擾���s
			}
			//�擾�p�^�[�����J���[�����f�[�^�ɕϊ����ĕ]������B
			this->_deviation_data->setRaster(*this->_inst_patt);


			//code_index,dir,c1�Ƀf�[�^�𓾂�B
			TNyARMatchPattResult mr;
			int lcode_index = 0;
			int dir = 0;
			double c1 = 0;
			for (int i = 0; i < this->_match_patt->length; i++) {
				this->_match_patt->item[i]->evaluate(*this->_deviation_data,mr);
				double c2 = mr.confidence;
				if (c1 < c2) {
					lcode_index = i;
					c1 = c2;
					dir = mr.direction;
				}
			}

			//�F������
			if (this->_target_id == -1) { // �}�[�J���F��
				//���݂͖��F��
				if (c1 < this->cf_threshold_new) {
					return;
				}
				if (this->confidence > c1) {
					// ��v�x���Ⴂ�B
					return;
				}
				//�F�����Ă���}�[�JID��ۑ�
				this->code_index=lcode_index;
			}else{
				//���݂̓}�[�J�F����				
				// ���݂̃}�[�J��F���������H
				if (lcode_index != this->_target_id) {
					// �F�����̃}�[�J�ł͂Ȃ��̂Ŗ���
					return;
				}
				//�F������臒l���傫�����H
				if (c1 < this->cf_threshold_exist) {
					return;
				}
				//���݂̌�������v�x�͑傫�����H
				if (this->confidence>c1) {
					return;
				}
				this->code_index=this->_target_id;
			}
			//�V�����F���A�܂��͌p���F�����ɍX�V���������Ƃ������ASquare�����X�V����B
			//�R�R�����͂��̏����ł������s����Ȃ��B

			//��v���̍�����`������΁A���ʂ��l�����Ē��_�����쐬
			this->confidence=c1;
			NyARSquare& sq=this->square;
			//direction���l�����āAsquare���X�V����B
			for(int i=0;i<4;i++){
				int idx=(i+4 - dir) % 4;
				this->_coordline->coord2Line(i_vertex_index[idx],i_vertex_index[(idx+1)%4],i_coordx,i_coordy,i_coor_num,sq.line[i]);
			}
			for (int i = 0; i < 4; i++) {
				//�������m�̌�_�v�Z
				if(!TNyARLinear::crossPos(sq.line[i],sq.line[(i + 3) % 4],sq.sqvertex[i])){
					throw NyARException();//�����̃G���[���A����Ȃ�_�u���o�b�t�@�ɂ����OK
				}
			}
		}
	};


	SingleARMarkerProcesser::SingleARMarkerProcesser()
	{
		this->_square_detect   =NULL;
		this->_transmat        =NULL;
		this->_tobin_filter    =NULL;
		this->_bin_raster      =NULL;
		this->_threshold_detect=NULL;
		//�R�[���o�b�N�n���h��
		this->_detectmarker_cb =NULL;
		this->_initialized=false;
	}
	SingleARMarkerProcesser::~SingleARMarkerProcesser()
	{
		NyAR_SAFE_DELETE(this->_square_detect);
		NyAR_SAFE_DELETE(this->_transmat);
		NyAR_SAFE_DELETE(this->_tobin_filter);

		// �Q�l�摜�o�b�t�@�����
		NyAR_SAFE_DELETE(this->_bin_raster);
		NyAR_SAFE_DELETE(this->_threshold_detect);
		//�R�[���o�b�N�n���h��
		NyAR_SAFE_DELETE(this->_detectmarker_cb);
		return;

	}
	void SingleARMarkerProcesser::initInstance(const NyARParam* i_param,int i_raster_type)
	{
		//�������ρH
		NyAR_ASSERT(this->_initialized==false);

		this->_lost_delay_count=0;
		this->_lost_delay=5;
		this->_threshold=110;
		this->_current_arcode_index=-1;

		const TNyARIntSize& scr_size = i_param->getScreenSize();
		// ��̓I�u�W�F�N�g�����
		this->_square_detect = new NyARSquareContourDetector_Rle(scr_size);
		this->_transmat = new NyARTransMat(i_param);
		this->_tobin_filter=new NyARRasterFilter_ARToolkitThreshold(110,i_raster_type);

		// �Q�l�摜�o�b�t�@�����
		this->_bin_raster = new NyARBinRaster(scr_size.w, scr_size.h);
		this->_threshold_detect=new NyARRasterThresholdAnalyzer_SlidePTile(15,i_raster_type,4);
		this->_initialized=true;
		//�R�[���o�b�N�n���h��
		this->_detectmarker_cb=new DetectSquareCB(*i_param);
		return;
	}
	/**���o����}�[�J�R�[�h�̔z����w�肵�܂��B ���o��Ԃł��̊֐������s����ƁA
	* �I�u�W�F�N�g��Ԃɋ������Z�b�g��������܂��B
	*/
	void SingleARMarkerProcesser::setARCodeTable(NyARCode* const i_ref_code_table[],int i_number_of_code, int i_code_resolution, double i_marker_width)
	{
		if (this->_current_arcode_index != -1) {
			// �������Z�b�g
			reset(true);
		}
		//���o����}�[�J�Z�b�g�A���A���o�����蒼���B(1�s�N�Z��4�|�C���g�T���v�����O,�}�[�J�̃p�^�[���̈��50%)
		this->_detectmarker_cb->setARCodeTable(i_ref_code_table,i_number_of_code,i_code_resolution);
		this->_offset.setSquare(i_marker_width);
		return;
	}

	void SingleARMarkerProcesser::reset(bool i_is_force)
	{
		if (this->_current_arcode_index != -1 && i_is_force == false) {
			// �������������łȂ���΃C�x���g�R�[��
			this->onLeaveHandler();
		}
		// �J�����g�}�[�J�����Z�b�g
		this->_current_arcode_index = -1;
		return;
	}
	void SingleARMarkerProcesser::detectMarker(const INyARRgbRaster& i_raster)
	{
		// �T�C�Y�`�F�b�N
		NyAR_ASSERT(this->_bin_raster->getSize().isEqualSize(i_raster.getSize().w, i_raster.getSize().h));

		//BIN�C���[�W�ւ̕ϊ�
		this->_tobin_filter->setThreshold(this->_threshold);
		this->_tobin_filter->doFilter(i_raster,*this->_bin_raster);

		// �X�N�G�A�R�[�h��T��(������i_raster���|�C���^�����Ă邯�ǁA���̊֐����ŎQ�Ƃ����Ă邩��OK�Ƃ���B)
		this->_detectmarker_cb->init(&i_raster,this->_current_arcode_index);
		this->_square_detect->detectMarkerCB(*this->_bin_raster,*this->_detectmarker_cb);

		// �F����Ԃ��X�V
		bool is_id_found=this->updateStatus(this->_detectmarker_cb->square,this->_detectmarker_cb->code_index);
		//臒l�t�B�[�h�o�b�N(detectExistMarker�ɂ������)
		if(!is_id_found){
			//�}�[�J���Ȃ���΁A�T��+DualPTail�Ŋ�P�x����
			int th=this->_threshold_detect->analyzeRaster(i_raster);
			this->_threshold=(this->_threshold+th)/2;
		}
		return;
	}
	/**
	* 
	* @param i_new_detect_cf
	* @param i_exist_detect_cf
	*/
	void SingleARMarkerProcesser::setConfidenceThreshold(double i_new_cf,double i_exist_cf)
	{
		this->_detectmarker_cb->cf_threshold_exist=i_exist_cf;
		this->_detectmarker_cb->cf_threshold_new=i_new_cf;
	}
	/**	�I�u�W�F�N�g�̃X�e�[�^�X���X�V���A�K�v�ɉ����ăn���h���֐����쓮���܂��B
	* 	�߂�l�́A�u���ۂɃ}�[�J�𔭌����鎖���ł������v�ł��B�N���X�̏�ԂƂ͈قȂ�܂��B
	*/
	bool SingleARMarkerProcesser::updateStatus(const NyARSquare& i_square, int i_code_index)
	{
		NyARTransMatResult& result=this->__NyARSquare_result;
		if (this->_current_arcode_index < 0) {// ���F����
			if (i_code_index < 0) {// ���F�����疢�F���̑J��
				// �Ȃɂ����Ȃ���[��B
				return false;
			} else {// ���F������F���̑J��
				this->_current_arcode_index = i_code_index;
				// �C�x���g����
				// OnEnter
				this->onEnterHandler(i_code_index);
				// �ϊ��s����쐬
				this->_transmat->transMat(i_square, this->_offset, result);
				// OnUpdate
				this->onUpdateHandler(i_square, result);
				this->_lost_delay_count = 0;
				return true;
			}
		} else {// �F����
			if (i_code_index < 0) {// �F�����疢�F���̑J��
				this->_lost_delay_count++;
				if (this->_lost_delay < this->_lost_delay_count) {
					// OnLeave
					this->_current_arcode_index = -1;
					this->onLeaveHandler();
				}
				return false;
			} else if (i_code_index == this->_current_arcode_index) {// ����ARCode�̍ĔF��
				// �C�x���g����
				// �ϊ��s����쐬
				this->_transmat->transMatContinue(i_square, this->_offset, result);
				// OnUpdate
				this->onUpdateHandler(i_square, result);
				this->_lost_delay_count = 0;
				return true;
			} else {// �قȂ�R�[�h�̔F�������̓T�|�[�g���Ȃ��B
				throw NyARException();
			}
		}
	}

}

