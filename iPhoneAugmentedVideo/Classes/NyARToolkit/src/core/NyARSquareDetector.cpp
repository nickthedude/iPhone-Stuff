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
#include "NyARSquareDetector.h"
#include "NyARLabeling_ARToolKit.h"
#include "NyARLabelingImage.h"
#include "NyARObserv2IdealMap.h"
#include "INyARSquareDetector.h"
#include "NyARVertexCounter.h"
#include "INyARPca2d.h"
#include "NyARPca2d_MatrixPCA_O2.h"
#include "../../inc/utils/NyArray.h"
#include "nyarcore.h"
#include <cstring>
#include <cstdio>
namespace NyARToolkitCPP
{

	/**
	* ���x�����m�̏d�Ȃ�i����֌W�j�𒲂ׂ�N���X�ł��B 
	* ���x�����X�g�ɓ���郉�x����~�ς��A����Ƀ^�[�Q�b�g�̃��x���������Ă��邩 ���m�F���܂��B
	*/
	class OverlapChecker
	{
	private:
		NyArray<const NyARLabelingLabel*> _labels;
		int _length;

		/**
		* �ő�i_max_label�̃��x����~�ςł���悤�ɃI�u�W�F�N�g�����Z�b�g����
		* 
		* @param i_max_label
		*/
	public:
		OverlapChecker()
		{
			return;
		}
		~OverlapChecker()
		{
			return;
		}
		 void reset(int i_max_label)
		{
			if (i_max_label > this->_labels.length){
				this->_labels.renew(i_max_label);
			}
			this->_length = 0;
		}

		/**
		* �`�F�b�N�Ώۂ̃��x����ǉ�����B
		* 
		* @param i_label_ref
		*/
		 void push(const NyARLabelingLabel* i_label_ref)
		{
			this->_labels.item[this->_length] = i_label_ref;
			this->_length++;
		}

		/**
		* ���݃��X�g�ɂ��郉�x���Əd�Ȃ��Ă��邩��Ԃ��B
		* 
		* @param i_label
		* @return ���ꂩ�̃��x���̓����ɂ���Ȃ��false,�Ɨ��������x���ł���\�����������true�ł��D
		*/
		bool check(const NyARLabelingLabel& i_label)
		{
			// �d�Ȃ菈�����ȁH
			const NyARLabelingLabel** label_pt = this->_labels.item;
			const int px1 = (int) i_label.pos_x;
			const int py1 = (int) i_label.pos_y;
			for (int i = this->_length - 1; i >= 0; i--) {
				const int px2 = (int) label_pt[i]->pos_x;
				const int py2 = (int) label_pt[i]->pos_y;
				const int d = (px1 - px2) * (px1 - px2) + (py1 - py2) * (py1 - py2);
				if (d < label_pt[i]->area / 4) {
					// �ΏۊO
					return false;
				}
			}
			// �Ώ�
			return true;
		}
	};



#define VERTEX_FACTOR 1.0// �����o�̃t�@�N�^
#define AR_AREA_MAX 100000// #define AR_AREA_MAX 100000
#define AR_AREA_MIN 70// #define AR_AREA_MIN 70


	NyARSquareDetector::NyARSquareDetector(const NyARCameraDistortionFactor& i_dist_factor_ref,const TNyARIntSize& i_size)
	{
		this->_width = i_size.w;
		this->_height = i_size.h;
		//�c�݌v�Z�e�[�u�������ƁA8*width/height*2�̗̈������܂��B
		//�̈����肽���Ȃ��ꍇ�́Ai_dist_factor_ref�̒l�����̂܂܎g���Ă��������B
		this->_dist_factor = new NyARObserv2IdealMap(i_dist_factor_ref,i_size);
		this->_limage = new NyARLabelingImage(this->_width, this->_height);
		this->_labeling = new NyARLabeling_ARToolKit();
		this->_labeling->attachDestination(this->_limage);
		this->_pca=new NyARPca2d_MatrixPCA_O2();

		// �֊s�̍ő咷�͉�ʂɉf�肤��ő�̒����`�T�C�Y�B
		int number_of_coord = (this->_width + this->_height) * 2;
		// �֊s�o�b�t�@�͒��_�ϊ�������̂ŁA�֊s�o�b�t�@�̂Q�{���B
		this->_max_coord = number_of_coord;
		this->_xcoord = new int[number_of_coord * 2];
		this->_ycoord = new int[number_of_coord * 2];
		this->_xpos=new double[this->_width+this->_height];//�ő�Ӓ���this._width+this._height
		this->_ypos=new double[this->_width+this->_height];//�ő�Ӓ���this._width+this._height
		this->_overlap_checker=new OverlapChecker();
		return;
	}
	NyARSquareDetector::~NyARSquareDetector()
	{
		NyAR_SAFE_DELETE(this->_overlap_checker);
		NyAR_SAFE_DELETE(this->_labeling);
		NyAR_SAFE_DELETE(this->_limage);
		NyAR_SAFE_DELETE(this->_dist_factor);
		NyAR_SAFE_DELETE(this->_pca);
		NyAR_SAFE_DELETE(this->_xcoord);
		NyAR_SAFE_DELETE(this->_ycoord);
		NyAR_SAFE_DELETE(this->_xpos);
		NyAR_SAFE_DELETE(this->_ypos);
		return;
	}
	void NyARSquareDetector::normalizeCoord(int i_coord_x[], int i_coord_y[], int i_index, int i_coord_num)
	{
		// vertex1�����E�ɂ��āA����ɔz���A��
		memcpy(i_coord_x+i_coord_num,i_coord_x+1,i_index*sizeof(int));
		memcpy(i_coord_y+i_coord_num,i_coord_y+1,i_index*sizeof(int));
		return;
	}
	void NyARSquareDetector::detectMarker(const NyARBinRaster& i_raster, NyARSquareStack& o_square_stack)
	{
		NyARLabelingImage& limage = *this->_limage;

		// ������

		// �}�[�J�[�z���_�����Z�b�g
		o_square_stack.clear();

		// ���x�����O
		this->_labeling->labeling(i_raster);

		// ���x������0�Ȃ炱���܂�
		int label_num = limage.getLabelStack()->getLength();
		if (label_num < 1) {
			return;
		}

		NyARLabelingLabelStack* stack = limage.getLabelStack();
		const NyArray<NyARLabelingLabel*>* labels = stack->getArray();


		// ���x����傫�����ɐ���
		stack->sortByArea();

		// �f�J�����x����ǂݔ�΂�
		int i;
		for (i = 0; i < label_num; i++) {
			// �����Ώۓ��̃��x���T�C�Y�ɂȂ�܂Ŗ���
			if (labels->item[i]->area <= AR_AREA_MAX) {
				break;
			}
		}

		const int xsize = this->_width;
		const int ysize = this->_height;
		int* xcoord = this->_xcoord;
		int* ycoord = this->_ycoord;
		const int coord_max = this->_max_coord;
		int mkvertex[5];
		OverlapChecker* overlap = this->_overlap_checker;
		int coord_num;
		int label_area;
		const NyARLabelingLabel* label_pt;

		//�d�Ȃ�`�F�b�J�̍ő吔��ݒ�
		overlap->reset(label_num);

		for (; i < label_num; i++) {
			label_pt = labels->item[i];
			label_area = label_pt->area;
			// �����ΏۃT�C�Y�����������Ȃ�����I��
			if (label_area < AR_AREA_MIN) {
				break;
			}
			// �N���b�v�̈悪��ʂ̘g�ɐڂ��Ă���Ώ��O
			if (label_pt->clip_l == 1 || label_pt->clip_r == xsize - 2) {// if(wclip[i*4+0] == 1 || wclip[i*4+1] ==xsize-2){
				continue;
			}
			if (label_pt->clip_t == 1 || label_pt->clip_b == ysize - 2) {// if( wclip[i*4+2] == 1 || wclip[i*4+3] ==ysize-2){
				continue;
			}
			// ���Ɍ��o���ꂽ��`�Ƃ̏d�Ȃ���m�F
			if (!overlap->check(*label_pt)) {
				// �d�Ȃ��Ă���悤���B
				continue;
			}

			// �֊s���擾
			coord_num = limage.getContour(i, coord_max, xcoord, ycoord);
			if (coord_num == coord_max) {
				// �֊s���傫������B
				continue;
			}
			//���_���̃C���f�N�X���擾
			const int vertex1 = scanVertex(xcoord, ycoord, coord_num);

			// ���_���(vertex1)��擪�ɕ��ׂȂ������z����쐬����B
			normalizeCoord(xcoord, ycoord, vertex1, coord_num);

			// �̈����������B
			NyARSquare* square_ptr = o_square_stack.prePush();

			// ���_�����擾
			if (!getSquareVertex(xcoord, ycoord, vertex1, coord_num, label_area, mkvertex)) {
				o_square_stack.pop();// ���_�̎擾���o���Ȃ������̂Ŕj��
				continue;
			}
			// �}�[�J�[�����o
			if (!getSquareLine(mkvertex, xcoord, ycoord,*square_ptr)) {
				// ��`���������Ȃ������B
				o_square_stack.pop();
				continue;
			}
			// ���o�ς̋�`�̑��������x�����d�Ȃ�`�F�b�N�ɒǉ�����B
			overlap->push(label_pt);
		}	
		return;
	}

	int NyARSquareDetector::scanVertex(const int i_xcoord[], const int i_ycoord[], int i_coord_num)
	{
		const int sx = i_xcoord[0];
		const int sy = i_ycoord[0];
		int d = 0;
		int w, x, y;
		int ret = 0;
		for (int i = 1; i < i_coord_num; i++) {
			x = i_xcoord[i] - sx;
			y = i_ycoord[i] - sy;
			w = x * x + y * y;
			if (w > d) {
				d = w;
				ret = i;
			}
			// �����ł��܂��I������������Ȃ����ȁB
		}
		return ret;
	}
	NyARVertexCounter __getSquareVertex_wv1;
	NyARVertexCounter __getSquareVertex_wv2;

	/**
	* static int arDetectMarker2_check_square( int area, ARMarkerInfo2 *marker_info2, double factor ) �֐��̑�֊֐� OPTIMIZED STEP [450->415] o_square�ɒ��_�����Z�b�g���܂��B
	* 
	* @param i_x_coord
	* @param i_y_coord
	* @param i_vertex1_index
	* @param i_coord_num
	* @param i_area
	* @param o_vertex
	* �v�f����int[4]�ł��鎖
	* @return
	*/
	bool NyARSquareDetector::getSquareVertex(const int i_x_coord[], const int i_y_coord[], int i_vertex1_index, int i_coord_num, int i_area, int o_vertex[])
	{
		NyARVertexCounter& wv1 = this->__getSquareVertex_wv1;
		NyARVertexCounter& wv2 = this->__getSquareVertex_wv2;
		const int end_of_coord = i_vertex1_index + i_coord_num - 1;
		const int sx = i_x_coord[i_vertex1_index];// sx = marker_info2->x_coord[0];
		const int sy = i_y_coord[i_vertex1_index];// sy = marker_info2->y_coord[0];
		int dmax = 0;
		int v1 = i_vertex1_index;
		for (int i = 1 + i_vertex1_index; i < end_of_coord; i++) {// for(i=1;i<marker_info2->coord_num-1;i++)
			// {
			const int d = (i_x_coord[i] - sx) * (i_x_coord[i] - sx) + (i_y_coord[i] - sy) * (i_y_coord[i] - sy);
			if (d > dmax) {
				dmax = d;
				v1 = i;
			}
		}
		const double thresh = (i_area / 0.75) * 0.01 * VERTEX_FACTOR;

		o_vertex[0] = i_vertex1_index;

		if (!wv1.getVertex(i_x_coord, i_y_coord, i_vertex1_index, v1, thresh)) { // if(get_vertex(marker_info2->x_coord,marker_info2->y_coord,0,v1,thresh,wv1,&wvnum1)<
			// 0 ) {
			return false;
		}
		if (!wv2.getVertex(i_x_coord, i_y_coord, v1, end_of_coord, thresh)) {// if(get_vertex(marker_info2->x_coord,marker_info2->y_coord,v1,marker_info2->coord_num-1,thresh,wv2,&wvnum2)
			// < 0) {
			return false;
		}

		int v2;
		if (wv1.number_of_vertex == 1 && wv2.number_of_vertex == 1) {// if(wvnum1 == 1 && wvnum2== 1) {
			o_vertex[1] = wv1.vertex[0];
			o_vertex[2] = v1;
			o_vertex[3] = wv2.vertex[0];
		} else if (wv1.number_of_vertex > 1 && wv2.number_of_vertex == 0) {// }else if( wvnum1 > 1 && wvnum2== 0) {
			//���_�ʒu���A�N�_����Ίp�_�̊Ԃ�1/2�ɂ���Ɨ\�z���āA��������B
			v2 = (v1-i_vertex1_index)/2+i_vertex1_index;
			if (!wv1.getVertex(i_x_coord, i_y_coord, i_vertex1_index, v2, thresh)) {
				return false;
			}
			if (!wv2.getVertex(i_x_coord, i_y_coord, v2, v1, thresh)) {
				return false;
			}
			if (wv1.number_of_vertex == 1 && wv2.number_of_vertex == 1) {
				o_vertex[1] = wv1.vertex[0];
				o_vertex[2] = wv2.vertex[0];
				o_vertex[3] = v1;
			} else {
				return false;
			}
		} else if (wv1.number_of_vertex == 0 && wv2.number_of_vertex > 1) {
			//v2 = (v1-i_vertex1_index+ end_of_coord-i_vertex1_index) / 2+i_vertex1_index;
			v2 = (v1+ end_of_coord)/2;

			if (!wv1.getVertex(i_x_coord, i_y_coord, v1, v2, thresh)) {
				return false;
			}
			if (!wv2.getVertex(i_x_coord, i_y_coord, v2, end_of_coord, thresh)) {
				return false;
			}
			if (wv1.number_of_vertex == 1 && wv2.number_of_vertex == 1) {
				o_vertex[1] = v1;
				o_vertex[2] = wv1.vertex[0];
				o_vertex[3] = wv2.vertex[0];
			} else {
				return false;
			}
		} else {
			return false;
		}
		o_vertex[4] = end_of_coord;
		return true;
	}

	bool NyARSquareDetector::getSquareLine(const int i_mkvertex[], const int i_xcoord[],const int i_ycoord[], NyARSquare& o_square)
	{
		NyARDoubleMatrix22& evec=this->__getSquareLine_evec;
		TNyARDoublePoint2d mean;
		TNyARDoublePoint2d ev;


		for (int i = 0; i < 4; i++) {
			const double w1 = (double) (i_mkvertex[i + 1] - i_mkvertex[i] + 1) * 0.05 + 0.5;
			const int st = (int) (i_mkvertex[i] + w1);
			const int ed = (int) (i_mkvertex[i + 1] - w1);
			const int n = ed - st + 1;
			if (n < 2) {
				// n��2�ȉ���matrix.PCA���v�Z���邱�Ƃ͂ł��Ȃ��̂ŁA�G���[
				return false;
			}
			//�z��쐬
			this->_dist_factor->observ2IdealBatch(i_xcoord, i_ycoord, st, n,this->_xpos,this->_ypos);

			//�听�����͂���B
			this->_pca->pca(this->_xpos,this->_ypos,n,&evec, &ev,&mean);
			TNyARLinear& l_line_i = o_square.line[i];
			l_line_i.run = evec.m01;// line[i][0] = evec->m[1];
			l_line_i.rise = -evec.m00;// line[i][1] = -evec->m[0];
			l_line_i.intercept = -(l_line_i.run * mean.x + l_line_i.rise * mean.y);// line[i][2] = -(line[i][0]*mean->v[0] + line[i][1]*mean->v[1]);
		}

		TNyARDoublePoint2d* l_sqvertex = o_square.sqvertex;
		TNyARIntPoint* l_imvertex = o_square.imvertex;
		for (int i = 0; i < 4; i++) {
			const TNyARLinear& l_line_i = o_square.line[i];
			const TNyARLinear& l_line_2 = o_square.line[(i + 3) % 4];
			const double w1 = l_line_2.run * l_line_i.rise - l_line_i.run * l_line_2.rise;
			if (w1 == 0.0) {
				return false;
			}
			l_sqvertex[i].x = (l_line_2.rise * l_line_i.intercept - l_line_i.rise * l_line_2.intercept) / w1;
			l_sqvertex[i].y = (l_line_i.run * l_line_2.intercept - l_line_2.run * l_line_i.intercept) / w1;
			// ���_�C���f�N�X���璸�_���W�𓾂ĕۑ�
			l_imvertex[i].x = i_xcoord[i_mkvertex[i]];
			l_imvertex[i].y = i_ycoord[i_mkvertex[i]];
		}
		return true;
	}




}
