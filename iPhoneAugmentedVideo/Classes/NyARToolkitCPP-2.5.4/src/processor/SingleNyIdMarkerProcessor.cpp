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



#include "SingleNyIdMarkerProcessor.h"
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
		const INyIdMarkerData* marker_data;
		int threshold;


		//�Q��
	private:
		const INyARRgbRaster* _ref_raster;
		//���L�C���X�^���X
		INyIdMarkerData* _current_data;
		INyIdMarkerData* _data_temp;
		NyIdMarkerPickup _id_pickup;
		NyARCoord2Linear* _coordline;
		const INyIdMarkerDataEncoder* _encoder;


		const INyIdMarkerData* _prev_data;
	public:
		DetectSquareCB(const NyARParam& i_param,const INyIdMarkerDataEncoder* i_encoder)
		{
			this->_coordline=new NyARCoord2Linear(i_param.getScreenSize(),i_param.getDistortionFactor());
			this->_data_temp=i_encoder->createDataInstance();
			this->_current_data=i_encoder->createDataInstance();
			this->_encoder=i_encoder;
			return;
		}
		~DetectSquareCB()
		{
			NyAR_SAFE_DELETE(this->_coordline);
			NyAR_SAFE_DELETE(this->_data_temp);
			NyAR_SAFE_DELETE(this->_current_data);
		}

	private:
		//		NyARIntPoint2d[] __tmp_vertex=NyARIntPoint2d.createArray(4);
		/**
		* Initialize call back handler.
		*/
	public:
		void init(const INyARRgbRaster* i_raster,const INyIdMarkerData* i_prev_data)
		{
			this->marker_data=NULL;
			this->_prev_data=i_prev_data;
			this->_ref_raster=i_raster;
		}
		/**
		* ��`�����t���邽�тɌĂяo����܂��B
		* ����������`�̃p�^�[�����������āA���ʂ��l���������_�f�[�^���m�ۂ��܂��B
		*/
	public:
		void onSquareDetect(const NyARSquareContourDetector& i_sender,int i_coordx[],int i_coordy[],int i_coor_num,int i_vertex_index[])
		{
			//���ɔ����ςȂ�I��
			if(this->marker_data!=NULL){
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

			TNyIdMarkerParam param;
			TNyIdMarkerPattern patt_data;
			// �]����ɂȂ�p�^�[�����C���[�W����؂�o��
			if (!this->_id_pickup.pickFromRaster(*this->_ref_raster,vertex, patt_data, param)){
				return;
			}
			//�G���R�[�h
			if(!this->_encoder->encode(patt_data,*this->_data_temp)){
				return;
			}

			//�p���F���v������Ă���H
			if (this->_prev_data==NULL){
				//�p���F���v���Ȃ�
				this->_current_data->copyFrom(*this->_data_temp);
			}else{
				//�p���F���v������
				if(!this->_prev_data->isEqual((*this->_data_temp))){
					return;//�F�������̂�����ID�ƈႤ�B
				}
			}
			//�V�����F���A�܂��͌p���F�����ɍX�V���������Ƃ������ASquare�����X�V����B
			//�R�R�����͂��̏����ł������s����Ȃ��B
			NyARSquare& sq=this->square;
			//direction���l�����āAsquare���X�V����B
			for(int i=0;i<4;i++){
				int idx=(i+4 - param.direction) % 4;
				this->_coordline->coord2Line(i_vertex_index[idx],i_vertex_index[(idx+1)%4],i_coordx,i_coordy,i_coor_num,sq.line[i]);
			}
			for (int i = 0; i < 4; i++) {
				//�������m�̌�_�v�Z
				if(!TNyARLinear::crossPos(sq.line[i],sq.line[(i + 3) % 4],sq.sqvertex[i])){
					throw NyARException();//�����̃G���[���A����Ȃ�_�u���o�b�t�@�ɂ����OK
				}
			}
			this->threshold=param.threshold;
			this->marker_data=this->_current_data;//�݂������B
		}
	};


	SingleNyIdMarkerProcesser::SingleNyIdMarkerProcesser()
	{
		this->_lost_delay_count = 0;
		this->_lost_delay = 5;
		this->_current_threshold=110;

		this->_square_detect =NULL;
		this->_transmat      =NULL;
		this->_callback      =NULL;
		this->_bin_raster      =NULL;
		this->_data_current    =NULL;
		this->_tobin_filter    =NULL;
		this->_threshold_detect=NULL;
		this->_initialized=false;
		return;
	}
	SingleNyIdMarkerProcesser::~SingleNyIdMarkerProcesser()
	{
		NyAR_SAFE_DELETE(this->_square_detect);
		NyAR_SAFE_DELETE(this->_transmat);
		NyAR_SAFE_DELETE(this->_callback);


		// �Q�l�摜�o�b�t�@�����
		NyAR_SAFE_DELETE(this->_bin_raster);
		//���[�N�p�̃f�[�^�I�u�W�F�N�g���Q���
		NyAR_SAFE_DELETE(this->_data_current);
		NyAR_SAFE_DELETE(this->_tobin_filter);
		NyAR_SAFE_DELETE(this->_threshold_detect);
	}

	void SingleNyIdMarkerProcesser::initInstance(const NyARParam* i_param,const INyIdMarkerDataEncoder* i_encoder,double i_marker_width,int i_raster_format)
	{
		//�������ρH
		NyAR_ASSERT(this->_initialized==false);

		const TNyARIntSize& scr_size = i_param->getScreenSize();
		// ��̓I�u�W�F�N�g�����
		this->_square_detect = new NyARSquareContourDetector_Rle(scr_size);
		this->_transmat = new NyARTransMat(i_param);
		this->_callback=new DetectSquareCB(*i_param,i_encoder);

		// �Q�l�摜�o�b�t�@�����
		this->_bin_raster = new NyARBinRaster(scr_size.w, scr_size.h);
		//���[�N�p�̃f�[�^�I�u�W�F�N�g���Q���
		this->_data_current=i_encoder->createDataInstance();
		this->_tobin_filter =new NyARRasterFilter_ARToolkitThreshold(110,i_raster_format);
		this->_threshold_detect=new NyARRasterThresholdAnalyzer_SlidePTile(15,i_raster_format,4);
		this->_initialized=true;
		this->_is_active=false;
		this->_offset.setSquare(i_marker_width);
		return;

	}


	void SingleNyIdMarkerProcesser::setMarkerWidth(int i_width)
	{
		this->_offset.setSquare(i_width);
		return;
	}

	void SingleNyIdMarkerProcesser::reset(bool i_is_force)
	{
		if (i_is_force == false && this->_is_active){
			// �������������łȂ���΃C�x���g�R�[��
			this->onLeaveHandler();
		}
		//�}�[�J����
		this->_is_active=false;
		return;
	}

	void SingleNyIdMarkerProcesser::detectMarker(const INyARRgbRaster& i_raster)
	{
		// �T�C�Y�`�F�b�N
		if (!this->_bin_raster->getSize().isEqualSize(i_raster.getSize().w, i_raster.getSize().h)) {
			throw NyARException();
		}
		// ���X�^���Q�l�C���[�W�ɕϊ�����.
		this->_tobin_filter->setThreshold(this->_current_threshold);
		this->_tobin_filter->doFilter(i_raster,*this->_bin_raster);

		// �X�N�G�A�R�[�h��T��(�������Ɏw�肵���}�[�J�A�������͐V�����}�[�J��T���B)
		//�����Ń|�C���^���Q�Ƃɐ؂�ւ��Ă邯�ǁA�������̂Q�֐��ł����g��Ȃ��̂ŎQ�Ƃ�OK�E�E�EAPI�����邩���˂��B
		this->_callback->init(&i_raster,this->_is_active?this->_data_current:NULL);
		this->_square_detect->detectMarkerCB(*this->_bin_raster,*this->_callback);

		// �F����Ԃ��X�V(�}�[�J�𔭌������Ȃ�Acurrent_data��n������)
		bool is_id_found=updateStatus(this->_callback->square,this->_callback->marker_data);

		//臒l�t�B�[�h�o�b�N(detectExistMarker�ɂ������)
		if(is_id_found){
			//�}�[�J������΁A�}�[�J�̎���臒l�𔽉f
			this->_current_threshold=(this->_current_threshold+this->_callback->threshold)/2;
		}else{
			//�}�[�J���Ȃ���΁A�T��+DualPTail�Ŋ�P�x����
			int th=this->_threshold_detect->analyzeRaster(i_raster);
			this->_current_threshold=(this->_current_threshold+th)/2;
		}		
		return;
	}

	/**�I�u�W�F�N�g�̃X�e�[�^�X���X�V���A�K�v�ɉ����ăn���h���֐����쓮���܂��B
	*/
	bool SingleNyIdMarkerProcesser::updateStatus(const NyARSquare& i_square,const INyIdMarkerData* i_marker_data)
	{
		bool is_id_found=false;
		NyARTransMatResult& result = this->__NyARSquare_result;
		if (!this->_is_active) {// ���F����
			if (i_marker_data==NULL) {// ���F�����疢�F���̑J��
				// �Ȃɂ����Ȃ���[��B
				this->_is_active=false;
			} else {// ���F������F���̑J��
				this->_data_current->copyFrom(*i_marker_data);
				// �C�x���g����
				// OnEnter
				this->onEnterHandler(*this->_data_current);
				// �ϊ��s����쐬
				this->_transmat->transMat(i_square, this->_offset, result);
				// OnUpdate
				this->onUpdateHandler(i_square, result);
				this->_lost_delay_count = 0;
				this->_is_active=true;
				is_id_found=true;
			}
		} else {// �F����
			if (i_marker_data==NULL) {
				// �F�����疢�F���̑J��
				this->_lost_delay_count++;
				if (this->_lost_delay < this->_lost_delay_count) {
					// OnLeave
					this->onLeaveHandler();
					this->_is_active=false;
				}
			} else if(this->_data_current->isEqual(*i_marker_data)) {
				//����id�̍ĔF��
				this->_transmat->transMatContinue(i_square, this->_offset, result);
				// OnUpdate
				this->onUpdateHandler(i_square, result);
				this->_lost_delay_count = 0;
				is_id_found=true;
			} else {// �قȂ�R�[�h�̔F�������̓T�|�[�g���Ȃ��B
				throw NyARException();
			}
		}
		return is_id_found;
	}

}

