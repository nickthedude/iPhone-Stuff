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
#include "NyARLabeling_Rle.h"
#include "nyarcore.h"



#define AR_AREA_MAX 100000// #define AR_AREA_MAX 100000
#define AR_AREA_MIN 70// #define AR_AREA_MIN 70



namespace NyARToolkitCPP
{
	typedef struct TRleInfo_t
	{
		//�p�������o
		int entry_x; // �t���O�����g���x���̈ʒu
		int area;
		int clip_r;
		int clip_l;
		int clip_b;
		int clip_t;
		NyAR_INT64_t pos_x;
		NyAR_INT64_t pos_y;		
	}TRleInfo;

	class RleInfoStack : public NyARObjectStack<TRleInfo>
	{
	public:

	public:
		RleInfoStack(int i_length):NyARObjectStack<TRleInfo>(i_length)
		{
			return;
		}
	};
	/**
	* [strage class]
	*/

	struct TRleElement_t
	{
		int l;
		int r;
		int fid;
	};

	NyARLabeling_Rle::NyARLabeling_Rle(int i_width,int i_height)
	{
		this->_rlestack=new RleInfoStack(i_width*i_height*2048/(320*240)+32);
		this->_rle1 = new TRleElement[i_width/2+1];
		this->_rle2 = new TRleElement[i_width/2+1];
		setAreaRange(AR_AREA_MAX,AR_AREA_MIN);
		return;
	}
	NyARLabeling_Rle::~NyARLabeling_Rle()
	{
		NyAR_SAFE_DELETE(this->_rlestack);
		NyAR_SAFE_ARRAY_DELETE(this->_rle1);
		NyAR_SAFE_ARRAY_DELETE(this->_rle2);
		return;
	}


	void NyARLabeling_Rle::setAreaRange(int i_max,int i_min)
	{
		this->_max_area=i_max;
		this->_min_area=i_min;
		return;
	}


	int NyARLabeling_Rle::toRel(int* i_bin_buf, int i_st, int i_len, TRleElement* i_out)
	{
		int current = 0;
		int r = -1;
		// �s�m��J�n
		int x = i_st;
		const int right_edge = i_st + i_len - 1;
		while (x < right_edge) {
			// �Ó_(0)�X�L����
			if (i_bin_buf[x] != 0) {
				x++;
				continue;
			}
			// �Ó_�������Ó_���𒲂ׂ�
			r = (x - i_st);
			i_out[current].l = r;
			r++;// �Ó_+1
			x++;
			while (x < right_edge) {
				if (i_bin_buf[x] != 0) {
					// ���_(1)���Ó_(0)�z��I��>�o�^
					i_out[current].r = r;
					current++;
					x++;// ���_�̊m�F�B
					r = -1;// �E�[�̈ʒu��0�ɁB
					break;
				} else {
					// �Ó_(0)���ǉ�
					r++;
					x++;
				}
			}
		}
		// �Ō��1�_����������@�������Ⴄ�́B
		if (i_bin_buf[x] != 0) {
			// ���_��r�J�E���g���Ȃ�Ó_�z��I��>�o�^
			if (r >= 0) {
				i_out[current].r = r;
				current++;
			}
		} else {
			// �Ó_���J�E���g���łȂ����l1�Œǉ�
			if (r >= 0) {
				i_out[current].r = (r + 1);
			} else {
				// �Ō��1�_�̏ꍇ
				i_out[current].l = (i_len - 1);
				i_out[current].r = (i_len);
			}
			current++;
		}
		// �s�m��
		return current;
	}

	void NyARLabeling_Rle::addFragment(TRleElement& i_rel_img, int i_nof, int i_row_index,RleInfoStack& o_stack)
	{
		int l=i_rel_img.l;
		const int len=i_rel_img.r - l;
		i_rel_img.fid = i_nof;// REL���̌ŗLID
		TRleInfo* v = o_stack.prePush();
		v->entry_x = l;
		v->area =len;
		v->clip_l=l;
		v->clip_r=i_rel_img.r-1;
		v->clip_t=i_row_index;
		v->clip_b=i_row_index;
		v->pos_x=(len*(2*l+(len-1)))/2;
		v->pos_y=i_row_index*len;

		return;
	}
	int NyARLabeling_Rle::labeling(const NyARBinRaster& i_bin_raster, int i_top, int i_bottom,NyARRleLabelFragmentInfoStack& o_stack)
	{
		NyAR_ASSERT(i_bin_raster.isEqualBufferType(NyARBufferType::INT1D_BIN_8));
		return this->imple_labeling(i_bin_raster,0,i_top,i_bottom,o_stack);
	}
	int NyARLabeling_Rle::labeling(const NyARGrayscaleRaster& i_gs_raster,int i_th, int i_top, int i_bottom,NyARRleLabelFragmentInfoStack& o_stack)
	{
		assert(i_gs_raster.isEqualBufferType(NyARBufferType::INT1D_GRAY_8));
		return this->imple_labeling(i_gs_raster,i_th,i_top,i_bottom,o_stack);
	}
	//
	int NyARLabeling_Rle::imple_labeling(const INyARRaster& i_raster,int i_th,int i_top, int i_bottom,NyARRleLabelFragmentInfoStack& o_stack)
	{
		// ���Z�b�g����
		RleInfoStack& rlestack=*this->_rlestack;
		rlestack.clear();

		//
		TRleElement* rle_prev = this->_rle1;
		TRleElement* rle_current = this->_rle2;
		int len_prev = 0;
		int len_current = 0;
		const int width = i_raster.getWidth();
		int* in_buf = (int*) i_raster.getBuffer();

		int id_max = 0;
		int label_count=0;
		// ���i�o�^

		len_prev = toRel(in_buf, i_top, width, rle_prev);
		for (int i = 0; i < len_prev; i++) {
			// �t���O�����gID=�t���O�����g�����l�APOS=Y�l�AREL�C���f�N�X=�s
			addFragment(rle_prev[i], id_max, i_top, rlestack);
			id_max++;
			// nof�̍ő�l�`�F�b�N
			label_count++;
		}
		TRleInfo** const f_array = rlestack.getArray().item;
		// ���i����
		for (int y = i_top + 1; y < i_bottom; y++) {
			// �J�����g�s�̓Ǎ�
			len_current = toRel(in_buf, y * width, width, rle_current);
			int index_prev = 0;

			for (int i = 0; i < len_current; i++) {
				// index_prev,len_prev�̈ʒu�𒲐�����
				int id = -1;
				// �`�F�b�N���ׂ�prev������Ίm�F
				while (index_prev < len_prev) {
					if (rle_current[i].l - rle_prev[index_prev].r > 0) {// 0�Ȃ�8���ʃ��x�����O
						// prev��cur�̍����ɂ��遨���̃t���O�����g��T��
						index_prev++;
						continue;
					} else if (rle_prev[index_prev].l - rle_current[i].r > 0) {// 0�Ȃ�8���ʃ��x�����O�ɂȂ�
						// prev��cur�E���ɂ��遨�Ɨ��t���O�����g
						addFragment(rle_current[i], id_max, y,rlestack);
						id_max++;
						label_count++;
						// ����index������ׂ�
						goto SCAN_CUR;
					}
					id=rle_prev[index_prev].fid;//���[�g�t���O�����gid
					TRleInfo& id_ptr = *f_array[id];
					//�����Ώ�(����)->prev��ID���R�s�[���āA���[�g�t���O�����g�̏����X�V
					rle_current[i].fid = id;//�t���O�����gID��ۑ�
					//
					const int l= rle_current[i].l;
					const int r= rle_current[i].r;
					const int len=r-l;
					//������t���O�����g�̏����X�V����B
					id_ptr.area += len;
					//t��entry_x�́A������̂��g���̂ōX�V���Ȃ��B
					id_ptr.clip_l=l<id_ptr.clip_l?l:id_ptr.clip_l;
					id_ptr.clip_r=r>id_ptr.clip_r?r-1:id_ptr.clip_r;
					id_ptr.clip_b=y;
					id_ptr.pos_x+=(len*(2*l+(len-1)))/2;
					id_ptr.pos_y+=y*len;
					//���d�����̊m�F�i�Q�ڈȍ~�j
					index_prev++;
					while (index_prev < len_prev) {
						if (rle_current[i].l - rle_prev[index_prev].r > 0) {// 0�Ȃ�8���ʃ��x�����O
							// prev��cur�̍����ɂ��遨prev��cur�ɘA�����Ă��Ȃ��B
							goto SCAN_PREV;
						} else if (rle_prev[index_prev].l - rle_current[i].r > 0) {// 0�Ȃ�8���ʃ��x�����O�ɂȂ�
							// prev��cur�̉E���ɂ��遨prev��cur�ɘA�����Ă��Ȃ��B
							index_prev--;
							goto SCAN_CUR;
						}
						// prev��cur�͘A�����Ă��遨���[�g�t���O�����g�̓���

						//�������郋�[�g�t���O�����g���擾
						const int prev_id =rle_prev[index_prev].fid;
						TRleInfo& prev_ptr = *f_array[prev_id];
						if (id != prev_id){
							label_count--;
							//prev��current�̃t���O�����gid������������B
							for(int i2=index_prev;i2<len_prev;i2++){
								//prev�͌��݂�id����Ō�܂�
								if(rle_prev[i2].fid==prev_id){
									rle_prev[i2].fid=id;
								}
							}
							for(int i2=0;i2<i;i2++){
								//current��0���猻��-1�܂�
								if(rle_current[i2].fid==prev_id){
									rle_current[i2].fid=id;
								}
							}

							//���݂̃��[�g�t���O�����g�ɏ����W��
							id_ptr.area +=prev_ptr.area;
							id_ptr.pos_x+=prev_ptr.pos_x;
							id_ptr.pos_y+=prev_ptr.pos_y;
							//t��entry_x�̌���
							if (id_ptr.clip_t > prev_ptr.clip_t) {
								// ���݂̕������ɂ���B
								id_ptr.clip_t = prev_ptr.clip_t;
								id_ptr.entry_x = prev_ptr.entry_x;
							}else if (id_ptr.clip_t < prev_ptr.clip_t) {
								// ���݂̕�����ɂ���Bprev�Ƀt�B�[�h�o�b�N
							} else {
								// ���������ŏ����������G���g���|�C���g�B
								if (id_ptr.entry_x > prev_ptr.entry_x) {
									id_ptr.entry_x = prev_ptr.entry_x;
								}else{
								}
							}
							//l�̌���
							if (id_ptr.clip_l > prev_ptr.clip_l) {
								id_ptr.clip_l=prev_ptr.clip_l;
							}else{
							}
							//r�̌���
							if (id_ptr.clip_r < prev_ptr.clip_r) {
								id_ptr.clip_r=prev_ptr.clip_r;
							}else{
							}
							//b�̌���

							//�����ς̃��[�g�t���O�����g�𖳌�������B
							prev_ptr.area=0;
						}


						index_prev++;
					}
					index_prev--;
					break;
					SCAN_PREV:;
				}
				// cur��id�����蓖�Ă�ꂽ�����m�F
				// �E�[�Ɨ��t���O�����g��ǉ�
				if (id < 0){
					addFragment(rle_current[i], id_max, y,rlestack);
					id_max++;
					label_count++;
				}
			SCAN_CUR:;
			}
			// prev��rel�̌���
			TRleElement* tmp = rle_prev;
			rle_prev = rle_current;
			len_prev = len_current;
			rle_current = tmp;
		}
		//�Ώۂ̃��x�������]��
		o_stack.init(label_count);
		NyARRleLabelFragmentInfo** o_dest_array=o_stack.getArray().item;
		const int max=this->_max_area;
		const int min=this->_min_area;
		int active_labels=0;
		for(int i=id_max-1;i>=0;i--){
			const int area=f_array[i]->area;
			if(area<min || area>max){//�ΏۊO�̃G���A0�̂�min�ł͂���
				continue;
			}
			//
			const TRleInfo src_info=*f_array[i];
			NyARRleLabelFragmentInfo& dest_info=*o_dest_array[active_labels];
			dest_info.area=area;
			dest_info.clip_b=src_info.clip_b;
			dest_info.clip_r=src_info.clip_r;
			dest_info.clip_t=src_info.clip_t;
			dest_info.clip_l=src_info.clip_l;
			dest_info.entry_x=src_info.entry_x;
			dest_info.pos_x=(double)(src_info.pos_x/src_info.area);
			dest_info.pos_y=(double)(src_info.pos_y/src_info.area);
			active_labels++;
		}
		//���x�������Đݒ�
		o_stack.pops(label_count-active_labels);
		//���x������ԋp
		return active_labels;
	}	


}

