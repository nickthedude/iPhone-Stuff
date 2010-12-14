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
#include "NyARCustomSingleDetectMarker.h"
#include "NyARMatchPatt_Color_WITHOUT_PCA.h"
#include "nyarcore.h"

namespace NyARToolkitCPP
{

	/**
	 * detectMarker�̃R�[���o�b�N�֐�
	 */
	class DetectSquareCB : public NyARSquareContourDetector::IDetectMarkerCallback
	{
		//���J�v���p�e�B
	public:
		double confidence;
		NyARSquare square;
	private:
		//�Q�ƃC���X�^���X
		const INyARRgbRaster* _ref_raster;
		INyARColorPatt* _inst_patt;
	private:
		//���L�C���X�^���X
		NyARMatchPattDeviationColorData* _deviation_data;
		NyARCoord2Linear* _coordline;
		NyARMatchPatt_Color_WITHOUT_PCA* _match_patt;
		
	public:
		DetectSquareCB(INyARColorPatt* i_inst_patt,const NyARCode* i_ref_code,const NyARParam& i_param)
		{
			this->_inst_patt=i_inst_patt;
			this->_deviation_data=new NyARMatchPattDeviationColorData(i_ref_code->getWidth(),i_ref_code->getHeight());
			this->_coordline=new NyARCoord2Linear(i_param.getScreenSize(),i_param.getDistortionFactor());
			this->_match_patt=new NyARMatchPatt_Color_WITHOUT_PCA(i_ref_code);
			return;
		}
		~DetectSquareCB()
		{
			NyAR_SAFE_DELETE(this->_deviation_data);
			NyAR_SAFE_DELETE(this->_coordline);
			NyAR_SAFE_DELETE(this->_match_patt);
			NyAR_SAFE_DELETE(this->_inst_patt);
		}

		/**
		 * ��`�����t���邽�тɌĂяo����܂��B
		 * ����������`�̃p�^�[�����������āA���ʂ��l���������_�f�[�^���m�ۂ��܂��B
		 */
	public:
		void onSquareDetect(const NyARSquareContourDetector& i_sender,int i_coordx[],int i_coordy[],int i_coor_num,int i_vertex_index[])
		{
			TNyARMatchPattResult mr;
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
			if (!this->_inst_patt->pickFromRaster(*(this->_ref_raster),vertex)){
				return;
			}
			//�擾�p�^�[�����J���[�����f�[�^�ɕϊ����ĕ]������B
			this->_deviation_data->setRaster(*this->_inst_patt);
			if(!this->_match_patt->evaluate(*this->_deviation_data,mr)){
				return;
			}
			//���݂̈�v�����Ⴏ��ΏI��
			if (this->confidence > mr.confidence){
				return;
			}
			//��v���̍�����`������΁A���ʂ��l�����Ē��_�����쐬
			NyARSquare& sq=this->square;
			this->confidence = mr.confidence;
			//direction���l�����āAsquare���X�V����B
			for(int i=0;i<4;i++){
				int idx=(i+4 - mr.direction) % 4;
				this->_coordline->coord2Line(i_vertex_index[idx],i_vertex_index[(idx+1)%4],i_coordx,i_coordy,i_coor_num,sq.line[i]);
			}
			for (int i = 0; i < 4; i++) {
				//�������m�̌�_�v�Z
				if(!TNyARLinear::crossPos(sq.line[i],sq.line[(i + 3) % 4],sq.sqvertex[i])){
					throw NyARException();//�����̃G���[���A����Ȃ�_�u���o�b�t�@�ɂ����OK
				}
			}
		}
		void init(const INyARRgbRaster* i_raster)
		{
			this->confidence=0;
			this->_ref_raster=i_raster;
		}
	};


	NyARCustomSingleDetectMarker::~NyARCustomSingleDetectMarker()
	{
		NyAR_SAFE_DELETE(this->_square_detect);
		NyAR_SAFE_DELETE(this->_transmat);

		NyAR_SAFE_DELETE(this->_bin_raster);
		NyAR_SAFE_DELETE(this->_detect_cb);
		return;
	}


	NyARCustomSingleDetectMarker::NyARCustomSingleDetectMarker()
	{
		return;
	}
	void NyARCustomSingleDetectMarker::initInstance(
		INyARColorPatt* i_patt_inst,
		NyARSquareContourDetector* i_sqdetect_inst,
		INyARTransMat* i_transmat_inst,
		const INyARRasterFilter_Rgb2Bin* i_filter,
		const NyARParam&	i_ref_param,
		const NyARCode*	i_ref_code,
		double		i_marker_width)
	{
		this->_is_continue = false;

		const TNyARIntSize& scr_size=i_ref_param.getScreenSize();		
		//�I�u�W�F�N�g���L���̈ڏ�
		this->_square_detect = i_sqdetect_inst;
		this->_transmat = i_transmat_inst;
		this->_tobin_filter=i_filter;
		//�Q�l�摜�o�b�t�@�����
		this->_bin_raster=new NyARBinRaster(scr_size.w,scr_size.h);
		//_detect_cb
		this->_detect_cb=new DetectSquareCB(i_patt_inst,i_ref_code,i_ref_param);
		//�I�t�Z�b�g���Z�b�g
		this->_offset.setSquare(i_marker_width);
		return;
	}

	

	
	/**
	 * i_image�Ƀ}�[�J�[���o���������s���A���ʂ��L�^���܂��B
	 * 
	 * @param i_raster
	 * �}�[�J�[�����o����C���[�W���w�肵�܂��B�C���[�W�T�C�Y�́A�J�����p�����[�^
	 * �ƈ�v���Ă��Ȃ���΂Ȃ�܂���B
	 * @return �}�[�J�[�����o�ł�������^�U�l�ŕԂ��܂��B
	 * @throws NyARException
	 */
	bool NyARCustomSingleDetectMarker::detectMarkerLite(const INyARRgbRaster& i_raster)
	{
		//�T�C�Y�`�F�b�N
		if(!this->_bin_raster->getSize().isEqualSize(i_raster.getSize())){
			throw NyARException();
		}

		//���X�^���Q�l�C���[�W�ɕϊ�����.
		this->_tobin_filter->doFilter(i_raster,*this->_bin_raster);

		//�R�[���o�b�N�n���h���̏���
		this->_detect_cb->init(&i_raster);
		//��`��T��(�߂�l�̓R�[���o�b�N�֐��Ŏ󂯎��B)
		this->_square_detect->detectMarkerCB(*this->_bin_raster,*this->_detect_cb);
		if(this->_detect_cb->confidence==0){
			return false;
		}
		return true;
	}


	void NyARCustomSingleDetectMarker::getTransmationMatrix(NyARTransMatResult &o_result)const
	{
		// ��Ԉ�v�����}�[�J�[�̈ʒu�Ƃ����̕ӂ��v�Z
		if (this->_is_continue) {
			this->_transmat->transMatContinue(this->_detect_cb->square,this->_offset,o_result);
		} else {
			this->_transmat->transMat(this->_detect_cb->square,this->_offset,o_result);
		}
		return;
	}

	/**
	* ��ʏ�̃}�[�J���_����z��ւ̃��t�@�����X��Ԃ��܂��B
	* �Ԃ��ꂽ�I�u�W�F�N�g�̓N���X�ɏ��L���������Ă��܂��B�N���X�̃����o�֐������s����Ɠ��e�������ς��܂��B
	* �O���Ńf�[�^���X�g�b�N����ꍇ�́AgetSquarePosition�ŕ������ĉ������B
	* @return
	*/
	const NyARSquare& NyARCustomSingleDetectMarker::refSquare()const
	{
		return this->_detect_cb->square;
	}


	/**
	* ���o�����}�[�J�[�̈�v�x��Ԃ��܂��B
	* 
	* @return �}�[�J�[�̈�v�x��Ԃ��܂��B0�`1�܂ł̒l���Ƃ�܂��B ��v�x���Ⴂ�ꍇ�ɂ́A��F���̉\���������Ȃ�܂��B
	* @throws NyARException
	*/
	double NyARCustomSingleDetectMarker::getConfidence()const
	{
		return this->_detect_cb->confidence;
	}

	/**
	* getTransmationMatrix�̌v�Z���[�h��ݒ肵�܂��B �����l��TRUE�ł��B
	* 
	* @param i_is_continue
	* TRUE�Ȃ�AtransMatCont�݊��̌v�Z�����܂��B FALSE�Ȃ�AtransMat�݊��̌v�Z�����܂��B
	*/
	void NyARCustomSingleDetectMarker::setContinueMode(bool i_is_continue)
	{
		this->_is_continue = i_is_continue;
	}


}
