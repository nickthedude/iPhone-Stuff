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
#include "NyARSquareContourDetector_ARToolKit.h"
#include "NyARLabeling_ARToolKit.h"
#include "NyARLabelingImage.h"
#include "NyARObserv2IdealMap.h"
#include "INyARPca2d.h"
#include "NyARPca2d_MatrixPCA_O2.h"
#include "../../inc/utils/NyArray.h"
#include "nyarcore.h"
#include <cstring>
#include <cstdio>
namespace NyARToolkitCPP
{
#define AR_AREA_MAX 100000// #define AR_AREA_MAX 100000
#define AR_AREA_MIN 70// #define AR_AREA_MIN 70



#define VERTEX_FACTOR 1.0// �����o�̃t�@�N�^
#define AR_AREA_MAX 100000// #define AR_AREA_MAX 100000
#define AR_AREA_MIN 70// #define AR_AREA_MIN 70


	NyARSquareContourDetector_ARToolKit::NyARSquareContourDetector_ARToolKit(const TNyARIntSize& i_size)
	{
		this->_width = i_size.w;
		this->_height = i_size.h;

		this->_labeling = new NyARLabeling_ARToolKit();
		this->_limage = new NyARLabelingImage(this->_width, this->_height);
		this->_overlap_checker = new NyARLabelOverlapChecker<NyARLabelingLabel>(32);
		this->_coord2vertex=new NyARCoord2SquareVertexIndexes();


		// �֊s�̍ő咷�͉�ʂɉf�肤��ő�̒����`�T�C�Y�B
		int number_of_coord = (this->_width + this->_height) * 2;
		// �֊s�o�b�t�@�͒��_�ϊ�������̂ŁA�֊s�o�b�t�@�̂Q�{���B
		this->_max_coord = number_of_coord;
		this->_xcoord = new int[number_of_coord * 2];
		this->_ycoord = new int[number_of_coord * 2];
		return;
	}
	NyARSquareContourDetector_ARToolKit::~NyARSquareContourDetector_ARToolKit()
	{
		NyAR_SAFE_DELETE(this->_coord2vertex);
		NyAR_SAFE_DELETE(this->_overlap_checker);
		NyAR_SAFE_DELETE(this->_labeling);
		NyAR_SAFE_DELETE(this->_limage);
		NyAR_SAFE_ARRAY_DELETE(this->_xcoord);
		NyAR_SAFE_ARRAY_DELETE(this->_ycoord);
		return;
	}
	void NyARSquareContourDetector_ARToolKit::detectMarker(const NyARBinRaster& i_raster,IDetectMarkerCallback& i_callback)
	{
		NyARLabelingImage& limage = *this->_limage;

		// ���x�����O
		int label_num =this->_labeling->labeling(i_raster,limage);

		// ���x������0�Ȃ炱���܂�
		if (label_num < 1){
			return;
		}

		NyARLabelingLabelStack& stack = limage.getLabelStack();
		// ���x����傫�����ɐ���
		stack.sortByArea();

		const NyArray<NyARLabelingLabel*>& labels = stack.getArray();



		// �f�J�����x����ǂݔ�΂�
		int i;
		for (i = 0; i < label_num; i++) {
			// �����Ώۓ��̃��x���T�C�Y�ɂȂ�܂Ŗ���
			if (labels.item[i]->area <= AR_AREA_MAX) {
				break;
			}
		}

		const int xsize = this->_width;
		const int ysize = this->_height;
		int* xcoord = this->_xcoord;
		int* ycoord = this->_ycoord;
		const int coord_max = this->_max_coord;
		NyARLabelOverlapChecker<NyARLabelingLabel>* overlap = this->_overlap_checker;

		//�d�Ȃ�`�F�b�J�̍ő吔��ݒ�
		overlap->setMaxLabels(label_num);

		for (; i < label_num; i++) {
			const NyARLabelingLabel& label_pt = *labels.item[i];
			const int label_area = label_pt.area;
			// �����ΏۃT�C�Y�����������Ȃ�����I��
			if (label_area < AR_AREA_MIN) {
				break;
			}
			// �N���b�v�̈悪��ʂ̘g�ɐڂ��Ă���Ώ��O
			if (label_pt.clip_l == 1 || label_pt.clip_r == xsize - 2) {// if(wclip[i*4+0] == 1 || wclip[i*4+1] ==xsize-2){
				continue;
			}
			if (label_pt.clip_t == 1 || label_pt.clip_b == ysize - 2) {// if( wclip[i*4+2] == 1 || wclip[i*4+3] ==ysize-2){
				continue;
			}
			// ���Ɍ��o���ꂽ��`�Ƃ̏d�Ȃ���m�F
			if (!overlap->check(label_pt)) {
				// �d�Ȃ��Ă���悤���B
				continue;
			}

			// �֊s���擾
			const int coord_num = _cpickup->getContour(limage,limage.getTopClipTangentX(label_pt),label_pt.clip_t, coord_max, xcoord, ycoord);
			if (coord_num == coord_max) {
				// �֊s���傫������B
				continue;
			}
			int mkvertex[4];
			//�֊s�����`�F�b�N���āA��`���ǂ����𔻒�B��`�Ȃ��mkvertex�Ɏ擾
			if (!this->_coord2vertex->getVertexIndexes(xcoord, ycoord,coord_num,label_area, mkvertex)) {
				// ���_�̎擾���o���Ȃ�����
				continue;
			}
			//��`�𔭌��������Ƃ��R�[���o�b�N�֐��Œʒm
			i_callback.onSquareDetect(*this,xcoord,ycoord,coord_num,mkvertex);

			// ���o�ς̋�`�̑��������x�����d�Ȃ�`�F�b�N�ɒǉ�����B
			overlap->push(&label_pt);

		}	
		return;
	}
}
