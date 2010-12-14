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
#include "NyIdMarkerPickup.h"
#include "NyARPerspectiveParamGenerator_O1.h"

#include "INyARRgbRaster.h"
#include "nyarcore.h"
namespace NyARToolkitCPP
{

	class MarkerPattDecoder
	{
	public:
		void decode(int model,int domain,int mask)
		{

		}
	};


	const static int _bit_table_3[]={
		25,	26,	27,	28,	29,	30,	31,
		48,	9,	10,	11,	12,	13,	32,
		47,	24,	1,	2,	3,	14,	33,
		46,	23,	8,	0,	4,	15,	34,
		45,	22,	7,	6,	5,	16,	35,
		44,	21,	20,	19,	18,	17,	36,
		43,	42,	41,	40,	39,	38,	37
	};	
	const static int _bit_table_2[]={
		9,	10,	11,	12,	13,
		24,	1,	2,	3,	14,
		23,	8,	0,	4,	15,
		22,	7,	6,	5,	16,
		21,	20,	19,	18,	17
	};
	const static int* _bit_tables[]={
		_bit_table_2,_bit_table_3,NULL,NULL,NULL,NULL,NULL
	};

	/**
	* �}�[�J�p�^�[���̃G���R�[�_�ł��B
	*
	*/
	class MarkerPattEncoder
	{
	private:

		/**
		* RECT(0):[0]=(0)
		* RECT(1):[1]=(1-8)
		* RECT(2):[2]=(9-16),[3]=(17-24)
		* RECT(3):[4]=(25-32),[5]=(33-40),[6]=(41-48)
		*/
		const int* _bit_table;
		int _bits[16];
		int _model;
	public:
		void setBitByBitIndex(int i_index_no,int i_value)
		{
			NyAR_ASSERT(i_value==0 || i_value==1);
			const int bit_no=this->_bit_table[i_index_no];
			if(bit_no==0){
				this->_bits[0]=i_value;
			}else{
				int bidx=(bit_no-1)/8+1;
				int sidx=(bit_no-1)%8;
				this->_bits[bidx]=(this->_bits[bidx]&(~(0x01<<sidx)))|(i_value<<sidx);
			}
			return;
		}

		void setBit(int i_bit_no,int i_value)
		{
			NyAR_ASSERT(i_value==0 || i_value==1);
			if(i_bit_no==0){
				this->_bits[0]=i_value;
			}else{
				int bidx=(i_bit_no-1)/8+1;
				int sidx=(i_bit_no-1)%8;
				this->_bits[bidx]=(this->_bits[bidx]&(~(0x01<<sidx)))|(i_value<<sidx);
			}
			return;
		}
		int getBit(int i_bit_no)
		{
			if(i_bit_no==0){
				return this->_bits[0];
			}else{
				int bidx=(i_bit_no-1)/8+1;
				int sidx=(i_bit_no-1)%8;
				return (this->_bits[bidx]>>(sidx))&(0x01);
			}
		}
		int getModel()
		{
			return this->_model;
		}
	private:
		static int getControlValue(int i_model,const int i_data[])
		{
			int v;
			switch(i_model){
	case 2:
		v=(i_data[2] & 0x0e)>>1;
		return v>=5?v-1:v;
	case 3:
		v=(i_data[4] & 0x3e)>>1;
		return v>=21?v-1:v;
	case 4:
	case 5:
	case 6:
	case 7:
	default:
		break;
			}
			return -1;
		}
	public:
		static int getCheckValue(int i_model,const int i_data[])
		{
			int v;
			switch(i_model){
	case 2:
		v=(i_data[2] & 0xe0)>>5;
		return v>5?v-1:v;
	case 3:
		v=((i_data[4] & 0x80)>>7) |((i_data[5] & 0x0f)<<1);
		return v>21?v-1:v;
	case 4:
	case 5:
	case 6:
	case 7:
	default:
		break;
			}
			return -1;
		}
		bool initEncoder(int i_model)
		{
			if(i_model>3 || i_model<2){
				//Lv4�ȍ~�ɑΉ����鎞�́A���̐�����ς���B
				return false;
			}
			this->_bit_table=_bit_tables[i_model-2];
			this->_model=i_model;
			return true;
		}
	private:
		int getDirection()
		{
			int l,t,r,b;
			int timing_pat;
			switch(this->_model){
	case 2:
		//�g���b�L���O�Z���𓾂�
		t=this->_bits[2] & 0x1f;
		r=((this->_bits[2] & 0xf0)>>4)|((this->_bits[3]&0x01)<<4);
		b=this->_bits[3] & 0x1f;
		l=((this->_bits[3] & 0xf0)>>4)|((this->_bits[2]&0x01)<<4);
		timing_pat=0x0a;
		break;
	case 3:
		t=this->_bits[4] & 0x7f;
		r=((this->_bits[4] & 0xc0)>>6)|((this->_bits[5] & 0x1f)<<2);
		b=((this->_bits[5] & 0xf0)>>4)|((this->_bits[6] & 0x07)<<4);
		l=((this->_bits[6] & 0xfc)>>2)|((this->_bits[4] & 0x01)<<6);
		timing_pat=0x2a;
		break;
	default:
		return -3;
			}
			//�^�C�~���O�p�^�[���̔�r
			if(t==timing_pat){
				if(r==timing_pat){
					return (b!=timing_pat && l!=timing_pat)?2:-2;
				}else if(l==timing_pat){
					return (b!=timing_pat && r!=timing_pat)?3:-2;
				}
			}else if(b==timing_pat){
				if(r==timing_pat){
					return (t!=timing_pat && l!=timing_pat)?1:-2;
				}else if(l==timing_pat){
					return (t!=timing_pat && r!=timing_pat)?0:-2;
				}
			}
			return -1;
		}
		/**
		* �i�[���Ă���}�[�J�p�^�[�����G���R�[�h���āA�}�[�J�f�[�^��Ԃ��܂��B
		* @param o_out
		* @return
		* ��������΃}�[�J�̕��ʂ�ԋp���܂��B���s�����-1��Ԃ��܂��B
		*/

	public:
		int encode(TNyIdMarkerPattern& o_out)
		{
			const int d=getDirection();
			if(d<0){
				return -1;
			}
			//��]�r�b�g�̎擾
			getRotatedBits(d,o_out.data);
			const int model=this->_model;
			//���Ӄr�b�g�̎擾
			o_out.model=model;
			int control_bits=getControlValue(model,o_out.data);
			o_out.check=getCheckValue(model,o_out.data);
			o_out.ctrl_mask=control_bits%5;
			o_out.ctrl_domain=control_bits/5;
			if(o_out.ctrl_domain!=0 || o_out.ctrl_mask!=0){
				return -1;
			}
			//�}�X�N�����������������邱��
			return d;
		}
	private:
		void getRotatedBits(int i_direction,int o_out[])
		{
			int sl=i_direction*2;
			int sr=8-sl;

			int w1;
			o_out[0]=this->_bits[0];
			//RECT1
			w1=this->_bits[1];
			o_out[1]=((w1<<sl)|(w1>>sr))& 0xff;

			//RECT2
			sl=i_direction*4;
			sr=16-sl;
			w1=this->_bits[2]|(this->_bits[3]<<8);
			w1=(w1<<sl)|(w1>>sr);
			o_out[2]=w1 & 0xff;
			o_out[3]=(w1>>8) & 0xff;

			if(this->_model<2){
				return;
			}

			//RECT3
			sl=i_direction*6;
			sr=24-sl;			
			w1=this->_bits[4]|(this->_bits[5]<<8)|(this->_bits[6]<<16);
			w1=(w1<<sl)|(w1>>sr);
			o_out[4]=w1 & 0xff;
			o_out[5]=(w1>>8) & 0xff;
			o_out[6]=(w1>>16) & 0xff;

			if(this->_model<3){
				return;
			}
			//RECT4(Lv4�ȍ~�͂����̐�����ς���)
			//		shiftLeft(this._bits,7,3,i_direction*8);
			//		if(this._model<4){
			//			return;
			//		}
			return;
		}
	public:
		void shiftLeft(int i_pack[],int i_start,int i_length,int i_ls)
		{
			int work[16];
			//�[���V�t�g
			const int mod_shift=i_ls%8;
			for(int i=i_length-1;i>=1;i--){
				work[i]=(i_pack[i+i_start]<<mod_shift)|(0xff&(i_pack[i+i_start-1]>>(8-mod_shift)));
			}
			work[0]=(i_pack[i_start]<<mod_shift)|(0xff&(i_pack[i_start+i_length-1]>>(8-mod_shift)));
			//�o�C�g�V�t�g
			const int byte_shift=(i_ls/8)%i_length;
			for(int i=i_length-1;i>=0;i--){
				i_pack[(byte_shift+i)%i_length+i_start]=0xff & work[i];
			}
			return;
		}	
	};


	/**
	* NyARColorPatt_NyIdMarker�����X�^����Perspective�ϊ����ēǂ݂������߂̃N���X
	*
	*/
	class PerspectivePixelReader
	{
	private:
		NyARPerspectiveParamGenerator* _param_gen;
		double _cparam[8];

	private:


	public:
		PerspectivePixelReader()
		{
			this->_param_gen=new NyARPerspectiveParamGenerator_O1(1,1,100,100);
			return;
		}
		virtual ~PerspectivePixelReader()
		{
			NyAR_SAFE_DELETE(this->_param_gen);
			return;
		}
/*		void setSourceRaster(const INyARRgbRaster* i_raster)
		{
			this->_raster=i_raster;
			this->_raster_size=i_raster->getSize();
			return;
		}*/
		bool setSourceSquare(const TNyARIntPoint2d i_vertex[])
		{
			return this->_param_gen->getParam(i_vertex, this->_cparam);
		}

		/**
		* ��`����s�N�Z����؂�o���܂�
		* @param i_lt_x
		* @param i_lt_y
		* @param i_step_x
		* @param i_step_y
		* @param i_width
		* @param i_height
		* @param i_out_st
		* o_pixel�ւ̊i�[�ꏊ�̐擪�C���f�N�X
		* @param o_pixel
		* @throws NyARException
		*/
	private:
		bool rectPixels(const INyARRgbPixelReader &i_reader,const TNyARIntSize &i_raster_size,int i_lt_x,int i_lt_y,int i_step_x,int i_step_y,int i_width,int i_height,int i_out_st,int o_pixel[])
		{
			const double* cpara=this->_cparam;
			int ref_x[REF_ARRAY_LENGTH];
			int ref_y[REF_ARRAY_LENGTH];
			int pixcel_temp[REF_ARRAY_LENGTH*3];
			const int raster_width=i_raster_size.w;
			const int raster_height=i_raster_size.h;

			int out_index=i_out_st;
			const double cpara_6=cpara[6];
			const double cpara_0=cpara[0];
			const double cpara_3=cpara[3];

			for(int i=0;i<i_height;i++){
				//1�񕪂̃s�N�Z���̃C���f�b�N�X�l���v�Z����B
				int cy0=1+i*i_step_y+i_lt_y;
				double cpy0_12=cpara[1]*cy0+cpara[2];
				double cpy0_45=cpara[4]*cy0+cpara[5];
				double cpy0_7=cpara[7]*cy0+1.0;			
				int pt=0;
				for(int i2=0;i2<i_width;i2++)
				{
					const int cx0=1+i2*i_step_x+i_lt_x;				
					const double d=cpara_6*cx0+cpy0_7;
					const int x=(int)((cpara_0*cx0+cpy0_12)/d);
					const int y=(int)((cpara_3*cx0+cpy0_45)/d);
					if(x<0||y<0||x>=raster_width||y>=raster_height)
					{
						return false;
					}
					ref_x[pt]=x;
					ref_y[pt]=y;
					pt++;
				}
				//1�s���̃s�N�Z�����擾(�ꍇ�ɂ���Ă͐�p�A�N�Z�T����������������)
				i_reader.getPixelSet(ref_x,ref_y,i_width,pixcel_temp);
				//�O���[�X�P�[���ɂ��Ȃ���Aline��map�ւ̓]��
				for(int i2=0;i2<i_width;i2++){
					int index=i2*3;
					o_pixel[out_index]=(pixcel_temp[index+0]+pixcel_temp[index+1]+pixcel_temp[index+2])/3;
					out_index++;
				}			
			}
			return true;
		}
		/**
		* i_freq�ɂ���[���N���X�_�̎������A���Ԋu�����ׂ܂��B
		* ���i���������A�O�i��80%���傫���A120%�����ł�����̂��A���Ԋu�����ł���Ƃ݂Ȃ��܂��B
		* @param i_freq
		* @param i_width
		*/
		static bool checkFreqWidth(const int i_freq[],int i_width)
		{
			int c=i_freq[1]-i_freq[0];
			const int count=i_width*2-1;
			for(int i=1;i<count;i++){
				const int n=i_freq[i+1]-i_freq[i];
				const int v=n*100/c;
				if(v>150 || v<50){
					return false;
				}
				c=n;
			}
			return true;
		}
		/**
		* i_freq_count_table��i_freq_table�̓��e�𒲍����A�ł��傫�Ȏ��g��������Ԃ��܂��B
		* @param i_freq_count_table
		* @param i_freq_table
		* @param o_freq_table
		* @return
		* �������0�ȏ�A���h�����0����
		*/
		static int getMaxFreq(const int i_freq_count_table[],const int i_freq_table[],int o_freq_table[])
		{
			//��Ԑ����̑傫�����̂𓾂�
			int index=-1;
			int max=0;
			for(int i=0;i<MAX_FREQ;i++){
				if(max<i_freq_count_table[i]){
					index=i;
					max=i_freq_count_table[i];
				}
			}		
			if(index==-1){
				return -1;
			}
			/*���g���C���f�N�X���v�Z*/
			const int st=(index-1)*index;
			for(int i=0;i<index*2;i++)
			{
				o_freq_table[i]=i_freq_table[st+i]*FRQ_STEP/max;
			}
			return index;
		}


		//�^�C�~���O�p�^�[���p�̃p�����^(FRQ_POINTS*FRQ_STEP��100�𒴂��Ȃ��悤�ɂ��邱��)
	private:
		const static int FRQ_EDGE=5;
		const static int FRQ_STEP=2;
		const static int FRQ_POINTS=(100-(FRQ_EDGE*2))/FRQ_STEP;


		const static int MIN_FREQ=3;
		const static int MAX_FREQ=10;
		const static int FREQ_SAMPLE_NUM=4;
		const static int MAX_DATA_BITS=MAX_FREQ+MAX_FREQ-1;
	private:
		const static int REF_ARRAY_LENGTH =108;
		//		int _ref_x[REF_ARRAY_LENGTH];
		//		int _ref_y[REF_ARRAY_LENGTH];
		//(model+1)*4*3��THRESHOLD_PIXEL*3�̂ǂ��炩�傫����
		//		int _pixcel_temp[REF_ARRAY_COUNT*3];

		//		int _freq_count_table[MAX_FREQ];
		//		int _freq_table[(MAX_FREQ*2-1)*MAX_FREQ*2/2];

		/**
		* i_y1�s�ڂ�i_y2�s�ڂ𕽋ς��āA�^�C�~���O�p�^�[���̎��g���𓾂܂��B
		* LHL��1�����Ƃ��āA���Ƃ���LHLHL�̏ꍇ��2��Ԃ��܂��BLHLH��HLHL���̎n�[�ƏI�[�̃��x�����قȂ�p�^�[����
		* ���o�����ꍇ�A�֐��͎��s���܂��B
		* 
		* @param i_y1
		* @param i_y2
		* @param i_th_h
		* @param i_th_l
		* @param o_edge_index
		* ���o�����G�b�W�ʒu(H->L,L->H)�̃C���f�N�X���󂯎��z��ł��B
		* [FRQ_POINTS]�ȏ�̔z����w�肵�Ă��������B
		* @return
		* @throws NyARException
		*/
	public:
		int getRowFrequency(const INyARRgbPixelReader &i_reader,const TNyARIntSize &i_raster_size,int i_y1,int i_th_h,int i_th_l,int o_edge_index[])
		{
			//3,4,5,6,7,8,9,10
			int freq_count_table[MAX_FREQ];
			//0,2,4,6,8,10,12,14,16,18,20�̗v�f�����z��
			int freq_table[(MAX_FREQ*2-1)*MAX_FREQ*2/2];
			//������
			const double *cpara=this->_cparam;
			int ref_x[REF_ARRAY_LENGTH];
			int ref_y[REF_ARRAY_LENGTH];
			int pixcel_temp[REF_ARRAY_LENGTH*3];
			for(int i=0;i<10;i++){
				freq_count_table[i]=0;
			}
			for(int i=0;i<110;i++){
				freq_table[i]=0;
			}
			const int raster_width=i_raster_size.w;
			const int raster_height=i_raster_size.h;

			const double cpara_0=cpara[0];
			const double cpara_3=cpara[3];
			const double cpara_6=cpara[6];		

			//10-20�s�N�Z���ڂ���^�C�~���O�p�^�[�������o
			for(int i=0;i<FREQ_SAMPLE_NUM;i++){
				//2�s���̃s�N�Z���C���f�b�N�X���v�Z
				const double cy0=1+i_y1+i;
				const double cpy0_12=cpara[1]*cy0+cpara[2];
				const double cpy0_45=cpara[4]*cy0+cpara[5];
				const double cpy0_7=cpara[7]*cy0+1.0;

				int pt=0;
				for(int i2=0;i2<FRQ_POINTS;i2++)
				{
					const double cx0=1+i2*FRQ_STEP+FRQ_EDGE;			
					const double d=(cpara_6*cx0)+cpy0_7;
					const int x=(int)((cpara_0*cx0+cpy0_12)/d);
					const int y=(int)((cpara_3*cx0+cpy0_45)/d);
					if(x<0||y<0||x>=raster_width||y>=raster_height)
					{
						return -1;
					}
					ref_x[pt]=x;
					ref_y[pt]=y;
					pt++;
				}

				//�s�N�Z�����擾(���͉摜�𑽗l������Ȃ炱�������𒲐����邱��)
				i_reader.getPixelSet(ref_x,ref_y,FRQ_POINTS,pixcel_temp);

				//o_edge_index���ꎞ�I�ɔj�󂵂Ē�������
				const int freq_t=getFreqInfo(pixcel_temp,i_th_h,i_th_l,o_edge_index);			

				//������3-10�ł��邱��
				if(freq_t<MIN_FREQ || freq_t>MAX_FREQ){
					continue;
				}
				//�����͓��Ԋu�ł��邱��
				if(!checkFreqWidth(o_edge_index,freq_t)){
					continue;
				}
				//���o�J�E���^��ǉ�
				freq_count_table[freq_t]++;
				const int table_st=(freq_t-1)*freq_t;
				for(int i2=0;i2<freq_t*2;i2++){
					freq_table[table_st+i2]+=o_edge_index[i2];
				}
			}
			return getMaxFreq(freq_count_table,freq_table,o_edge_index);
		}

		int getColFrequency(const INyARRgbPixelReader &i_reader,const TNyARIntSize &i_raster_size,int i_x1,int i_th_h,int i_th_l,int o_edge_index[])
		{
			const double* cpara=this->_cparam;
			int ref_x[REF_ARRAY_LENGTH];
			int ref_y[REF_ARRAY_LENGTH];
			int pixcel_temp[REF_ARRAY_LENGTH*3];
			//0,2,4,6,8,10,12,14,16,18,20=(11*20)/2=110
			//������
			int freq_count_table[MAX_FREQ];
			for(int i=0;i<10;i++){
				freq_count_table[i]=0;
			}
			int freq_table[(MAX_FREQ*2-1)*MAX_FREQ*2/2];
			for(int i=0;i<110;i++){
				freq_table[i]=0;
			}
			const int raster_width=i_raster_size.w;
			const int raster_height=i_raster_size.h;


			const double cpara7=cpara[7];
			const double cpara4=cpara[4];
			const double cpara1=cpara[1];
			//��_����4�s�N�Z�����Q�ƃp�^�[���Ƃ��Ē��o
			for(int i=0;i<FREQ_SAMPLE_NUM;i++){

				int cx0=1+i+i_x1;
				const double cp6_0=cpara[6]*cx0;
				const double cpx0_0=cpara[0]*cx0+cpara[2];
				const double cpx3_0=cpara[3]*cx0+cpara[5];

				int pt=0;
				for(int i2=0;i2<FRQ_POINTS;i2++)
				{
					int cy=1+i2*FRQ_STEP+FRQ_EDGE;

					const double d=cp6_0+cpara7*cy+1.0;
					const int x=(int)((cpx0_0+cpara1*cy)/d);
					const int y=(int)((cpx3_0+cpara4*cy)/d);
					if(x<0||y<0||x>=raster_width||y>=raster_height)
					{
						return -1;
					}
					ref_x[pt]=x;
					ref_y[pt]=y;				
					pt++;
				}		

				//�s�N�Z�����擾(���͉摜�𑽗l������Ȃ炱���𒲐����邱��)
				i_reader.getPixelSet(ref_x,ref_y,FRQ_POINTS,pixcel_temp);

				const int freq_t=getFreqInfo(pixcel_temp,i_th_h,i_th_l,o_edge_index);
				//������3-10�ł��邱��
				if(freq_t<MIN_FREQ || freq_t>MAX_FREQ){
					continue;
				}
				//�����͓��Ԋu�ł��邱��
				if(!checkFreqWidth(o_edge_index,freq_t)){
					continue;
				}
				//���o�J�E���^��ǉ�
				freq_count_table[freq_t]++;
				const int table_st=(freq_t-1)*freq_t;
				for(int i2=0;i2<freq_t*2;i2++){
					freq_table[table_st+i2]+=o_edge_index[i2];
				}
			}
			return getMaxFreq(freq_count_table,freq_table,o_edge_index);		
		}

		/**
		* �f�o�b�N���񂾂�static�ɂ��Ă���
		* @param i_pixcels
		* @param i_th_h
		* @param i_th_l
		* @param o_edge_index
		* @return
		*/
	private:
		static int getFreqInfo(const int i_pixcels[],int i_th_h,int i_th_l,int o_edge_index[])
		{
			//�g�[�N������͂��āA���g�����v�Z
			int i=0;
			int frq_l2h=0;
			int frq_h2l=0;
			while(i<FRQ_POINTS){
				//L->H�g�[�N�������o����
				while(i<FRQ_POINTS){
					const int index=i*3;
					const int pix=(i_pixcels[index+0]+i_pixcels[index+1]+i_pixcels[index+2])/3;
					if(pix>i_th_h){
						//�g�[�N������
						o_edge_index[frq_l2h+frq_h2l]=i;
						frq_l2h++;
						break;
					}
					i++;
				}
				i++;
				//L->H�g�[�N�������o����
				while(i<FRQ_POINTS){
					const int index=i*3;
					const int pix=(i_pixcels[index+0]+i_pixcels[index+1]+i_pixcels[index+2])/3;
					if(pix<=i_th_l){
						//�g�[�N������
						o_edge_index[frq_l2h+frq_h2l]=i;
						frq_h2l++;
						break;
					}
					i++;
				}
				i++;
			}
			return frq_l2h==frq_h2l?frq_l2h:-1;			
		}

	private:
		const static int THRESHOLD_EDGE=10;
		const static int THRESHOLD_STEP=2;
		const static int THRESHOLD_WIDTH=10;
		const static int THRESHOLD_PIXEL=THRESHOLD_WIDTH/THRESHOLD_STEP;
		const static int THRESHOLD_SAMPLE=THRESHOLD_PIXEL*THRESHOLD_PIXEL;
		const static int THRESHOLD_SAMPLE_LT=THRESHOLD_EDGE;
		const static int THRESHOLD_SAMPLE_RB=100-THRESHOLD_WIDTH-THRESHOLD_EDGE;

	public:
		struct TThreshold_t
		{
			int th_h;
			int th_l;
			int th;
			int lt_x;
			int lt_y;
			int rb_x;
			int rb_y;
		};


		struct THighAndLow_t
		{
			int h;
			int l;
		};
		/**
		* �s�N�Z���z��̏�ʁA���ʂ�4�s�N�Z���̃s�N�Z���l���ς����߂܂��B
		* ���̊֐��́A(4/i_pixcel.length)�̗̈���L����Ptail�@�őo������臒l�����߂邱�ƂɂȂ�܂��B
		* @param i_pixcel
		* @param i_initial
		* @param i_out
		*/
	private:
		void getPtailHighAndLow(const int i_pixel[],int i_pixel_length,struct THighAndLow_t &i_out)
		{
			int h3,h2,h1,h0,l3,l2,l1,l0;
			h3=h2=h1=h0=l3=l2=l1=l0=i_pixel[0];

			for(int i=i_pixel_length-1;i>=1;i--){
				const int pix=i_pixel[i];
				if(h0<pix){
					if(h1<pix){
						if(h2<pix){
							if(h3<pix){
								h0=h1;
								h1=h2;
								h2=h3;
								h3=pix;
							}else{
								h0=h1;
								h1=h2;
								h2=pix;
							}
						}else{
							h0=h1;
							h1=pix;
						}
					}else{
						h0=pix;
					}
				}
				if(l0>pix){
					if(l1>pix){
						if(l2>pix){
							if(l3>pix){
								l0=l1;
								l1=l2;
								l2=l3;
								l3=pix;
							}else{
								l0=l1;
								l1=l2;
								l2=pix;
							}
						}else{
							l0=l1;
							l1=pix;
						}
					}else{
						l0=pix;
					}
				}
			}
			i_out.l=(l0+l1+l2+l3)/4;
			i_out.h=(h0+h1+h2+h3)/4;
			return;
		}
	private:
		/**
		* �w�肵���ꏊ�̃s�N�Z���l�𒲍����āA臒l���v�Z���ĕԂ��܂��B
		* @param i_reader
		* @param i_x
		* @param i_y
		* @return
		* @throws NyARException
		*/
	public:
		void detectThresholdValue(const INyARRgbPixelReader &i_reader,const TNyARIntSize &i_raster_size,TThreshold_t& o_threshold)
		{
			int th_pixels[THRESHOLD_SAMPLE*4];

			//����̃s�b�N�A�b�v�̈悩��s�N�Z���𓾂�(00-24)
			rectPixels(i_reader,i_raster_size,THRESHOLD_SAMPLE_LT,THRESHOLD_SAMPLE_LT,THRESHOLD_STEP,THRESHOLD_STEP,THRESHOLD_PIXEL,THRESHOLD_PIXEL,0,th_pixels);

			//�����̃s�b�N�A�b�v�̈悩��s�N�Z���𓾂�(25-49)
			rectPixels(i_reader,i_raster_size,THRESHOLD_SAMPLE_LT,THRESHOLD_SAMPLE_RB,THRESHOLD_STEP,THRESHOLD_STEP,THRESHOLD_PIXEL,THRESHOLD_PIXEL,THRESHOLD_SAMPLE,th_pixels);

			//�E��̃s�b�N�A�b�v�̈悩��s�N�Z���𓾂�(50-74)
			rectPixels(i_reader,i_raster_size,THRESHOLD_SAMPLE_RB,THRESHOLD_SAMPLE_LT,THRESHOLD_STEP,THRESHOLD_STEP,THRESHOLD_PIXEL,THRESHOLD_PIXEL,THRESHOLD_SAMPLE*2,th_pixels);

			//�E���̃s�b�N�A�b�v�̈悩��s�N�Z���𓾂�(75-99)
			rectPixels(i_reader,i_raster_size,THRESHOLD_SAMPLE_RB,THRESHOLD_SAMPLE_RB,THRESHOLD_STEP,THRESHOLD_STEP,THRESHOLD_PIXEL,THRESHOLD_PIXEL,THRESHOLD_SAMPLE*3,th_pixels);

			struct THighAndLow_t hl;
			//Ptail�ŋ��߂��s�N�Z������
			getPtailHighAndLow(th_pixels,THRESHOLD_SAMPLE*4,hl);



			//臒l���S
			int th=(hl.h+hl.l)/2;
			//�q�X�e���V�X(������20%)
			int th_sub=(hl.h-hl.l)/5;

			o_threshold.th=th;
			o_threshold.th_h=th+th_sub;//�q�X�e���V�X�t��臒l
			o_threshold.th_l=th-th_sub;//�q�X�e���V�X�t��臒l

			//�G�b�W���v�Z(���_�d�S)
			int lt_x,lt_y,lb_x,lb_y,rt_x,rt_y,rb_x,rb_y;
			TNyARIntPoint2d tpt;
			//LT
			if(getHighPixelCenter(0,th_pixels,THRESHOLD_PIXEL,THRESHOLD_PIXEL,th,tpt)){
				lt_x=tpt.x*THRESHOLD_STEP;
				lt_y=tpt.y*THRESHOLD_STEP;
			}else{
				lt_x=11;
				lt_y=11;
			}
			//LB
			if(getHighPixelCenter(THRESHOLD_SAMPLE*1,th_pixels,THRESHOLD_PIXEL,THRESHOLD_PIXEL,th,tpt)){
				lb_x=tpt.x*THRESHOLD_STEP;
				lb_y=tpt.y*THRESHOLD_STEP;
			}else{
				lb_x=11;
				lb_y=-1;
			}
			//RT
			if(getHighPixelCenter(THRESHOLD_SAMPLE*2,th_pixels,THRESHOLD_PIXEL,THRESHOLD_PIXEL,th,tpt)){
				rt_x=tpt.x*THRESHOLD_STEP;
				rt_y=tpt.y*THRESHOLD_STEP;
			}else{
				rt_x=-1;
				rt_y=11;
			}
			//RB
			if(getHighPixelCenter(THRESHOLD_SAMPLE*3,th_pixels,THRESHOLD_PIXEL,THRESHOLD_PIXEL,th,tpt)){
				rb_x=tpt.x*THRESHOLD_STEP;
				rb_y=tpt.y*THRESHOLD_STEP;
			}else{
				rb_x=-1;
				rb_y=-1;
			}
			//�g���b�L���O�J�n�ʒu�̌���
			o_threshold.lt_x=(lt_x+lb_x)/2+THRESHOLD_SAMPLE_LT-1;
			o_threshold.rb_x=(rt_x+rb_x)/2+THRESHOLD_SAMPLE_RB+1;
			o_threshold.lt_y=(lt_y+rt_y)/2+THRESHOLD_SAMPLE_LT-1;
			o_threshold.rb_y=(lb_y+rb_y)/2+THRESHOLD_SAMPLE_RB+1;
			return;
		}

	private:
		bool getHighPixelCenter(int i_st,const int i_pixels[],int i_width,int i_height,int i_th,TNyARIntPoint2d& o_point)
		{
			int rp=i_st;
			int pos_x=0;
			int pos_y=0;
			int number_of_pos=0;
			for(int i=0;i<i_height;i++){
				for(int i2=0;i2<i_width;i2++){
					if(i_pixels[rp++]>i_th){
						pos_x+=i2;
						pos_y+=i;
						number_of_pos++;
					}
				}
			}
			if(number_of_pos>0){
				pos_x/=number_of_pos;
				pos_y/=number_of_pos;
			}else{
				return false;
			}
			o_point.x=pos_x;
			o_point.y=pos_y;
			return true;
		}
	private:
		int detectDataBitsIndex(const INyARRgbPixelReader &i_reader,const TNyARIntSize &i_raster_size,const TThreshold_t& i_th,double o_index_row[],double o_index_col[])
		{
			//���g���𑪒�
			int freq_index1[FRQ_POINTS];
			int freq_index2[FRQ_POINTS];


			int frq_t=getRowFrequency(i_reader,i_raster_size,i_th.lt_y,i_th.th_h,i_th.th_l,freq_index1);
			int frq_b=getRowFrequency(i_reader,i_raster_size,i_th.rb_y,i_th.th_h,i_th.th_l,freq_index2);
			//���g���͂܂Ƃ��H
			if((frq_t<0 && frq_b<0) || frq_t==frq_b){
				return -1;
			}
			//�^�C�~���O�p�^�[������C���f�N�X���쐬
			int freq_h,freq_v;
			const int* index;
			if(frq_t>frq_b){
				freq_h=frq_t;
				index=freq_index1;
			}else{
				freq_h=frq_b;
				index=freq_index2;
			}
			for(int i=0;i<freq_h+freq_h-1;i++){
				o_index_row[i*2]=((index[i+1]-index[i])*2/5+index[i])+FRQ_EDGE;
				o_index_row[i*2+1]=((index[i+1]-index[i])*3/5+index[i])+FRQ_EDGE;
			}		


			const int frq_l=getColFrequency(i_reader,i_raster_size,i_th.lt_x,i_th.th_h,i_th.th_l,freq_index1);
			const int frq_r=getColFrequency(i_reader,i_raster_size,i_th.rb_x,i_th.th_h,i_th.th_l,freq_index2);
			//���g���͂܂Ƃ��H
			if((frq_l<0 && frq_r<0) || frq_l==frq_r){
				return -1;
			}
			//�^�C�~���O�p�^�[������C���f�N�X���쐬
			if(frq_l>frq_r){
				freq_v=frq_l;
				index=freq_index1;
			}else{
				freq_v=frq_r;
				index=freq_index2;
			}
			//���������H
			if(freq_v!=freq_h){
				return -1;
			}

			for(int i=0;i<freq_v+freq_v-1;i++){
				const int w=index[i];
				const int w2=index[i+1]-w;
				o_index_col[i*2]=((w2)*2/5+w)+FRQ_EDGE;
				o_index_col[i*2+1]=((w2)*3/5+w)+FRQ_EDGE;
			}		
			//Lv4�ȏ�͖���
			if(freq_v>MAX_FREQ){
				return -1;
			}
			return freq_v;

		}	
	public:
		bool readDataBits(const INyARRgbPixelReader &i_reader,const TNyARIntSize &i_raster_size,struct TThreshold_t &i_th,MarkerPattEncoder& o_bitbuffer)
		{
			double index_x[MAX_DATA_BITS*2];
			double index_y[MAX_DATA_BITS*2];
			//�ǂݏo���ʒu���擾
			const int size=detectDataBitsIndex(i_reader,i_raster_size,i_th,index_x,index_y);
			const int resolution=size+size-1;
			if(size<0){
				return false;
			}
			if(!o_bitbuffer.initEncoder(size-1)){
				return false;
			}		

			const double (&cpara)[8]=this->_cparam;
			int ref_x[REF_ARRAY_LENGTH];
			int ref_y[REF_ARRAY_LENGTH];
			int pixcel_temp[REF_ARRAY_LENGTH*3];

			const double cpara_0=cpara[0];
			const double cpara_1=cpara[1];
			const double cpara_3=cpara[3];
			const double cpara_6=cpara[6];


			const int th=i_th.th;
			int p=0;
			for(int i=0;i<resolution;i++){
				//1�񕪂̃s�N�Z���̃C���f�b�N�X�l���v�Z����B
				double cy0=1+index_y[i*2+0];
				double cy1=1+index_y[i*2+1];			
				double cpy0_12=cpara_1*cy0+cpara[2];
				double cpy0_45=cpara[4]*cy0+cpara[5];
				double cpy0_7=cpara[7]*cy0+1.0;
				double cpy1_12=cpara_1*cy1+cpara[2];
				double cpy1_45=cpara[4]*cy1+cpara[5];
				double cpy1_7=cpara[7]*cy1+1.0;

				int pt=0;
				for(int i2=0;i2<resolution;i2++)
				{			

					double d;
					double cx0=1+index_x[i2*2+0];
					double cx1=1+index_x[i2*2+1];

					double cp6_0=cpara_6*cx0;
					double cpx0_0=cpara_0*cx0;
					double cpx3_0=cpara_3*cx0;

					double cp6_1=cpara_6*cx1;
					double cpx0_1=cpara_0*cx1;
					double cpx3_1=cpara_3*cx1;

					d=cp6_0+cpy0_7;
					ref_x[pt]=(int)((cpx0_0+cpy0_12)/d);
					ref_y[pt]=(int)((cpx3_0+cpy0_45)/d);
					pt++;

					d=cp6_0+cpy1_7;
					ref_x[pt]=(int)((cpx0_0+cpy1_12)/d);
					ref_y[pt]=(int)((cpx3_0+cpy1_45)/d);
					pt++;

					d=cp6_1+cpy0_7;
					ref_x[pt]=(int)((cpx0_1+cpy0_12)/d);
					ref_y[pt]=(int)((cpx3_1+cpy0_45)/d);
					pt++;

					d=cp6_1+cpy1_7;
					ref_x[pt]=(int)((cpx0_1+cpy1_12)/d);
					ref_y[pt]=(int)((cpx3_1+cpy1_45)/d);
					pt++;
				}
				//1�s���̃s�N�Z�����擾(�ꍇ�ɂ���Ă͐�p�A�N�Z�T����������������)
				i_reader.getPixelSet(ref_x,ref_y,resolution*4,pixcel_temp);
				//�O���[�X�P�[���ɂ��Ȃ���Aline��map�ւ̓]��
				for(int i2=0;i2<resolution;i2++){
					int index=i2*3*4;
					int pixel=(	pixcel_temp[index+0]+pixcel_temp[index+1]+pixcel_temp[index+2]+
						pixcel_temp[index+3]+pixcel_temp[index+4]+pixcel_temp[index+5]+
						pixcel_temp[index+6]+pixcel_temp[index+7]+pixcel_temp[index+8]+
						pixcel_temp[index+9]+pixcel_temp[index+10]+pixcel_temp[index+11])/(4*3);
					//�Ó_��1�A���_��0�ŕ\�����܂��B
					o_bitbuffer.setBitByBitIndex(p,pixel>th?0:1);
					p++;
				}
			}
			/*		
			for(int i=0;i<225*4;i++){
			this.vertex_x[i]=0;
			this.vertex_y[i]=0;
			}
			for(int i=0;i<(resolution)*2;i++){
			for(int i2=0;i2<(resolution)*2;i2++){
			this.vertex_x[i*(resolution)*2+i2]=(int)index_x[i2];
			this.vertex_y[i*(resolution)*2+i2]=(int)index_y[i];

			}
			}
			*/		return true;
		}
		bool setSquare(const TNyARIntPoint2d i_vertex[])
		{
			if (!this->_param_gen->getParam(i_vertex,this->_cparam)) {
				return false;
			}
			return true;
		}

	};


	NyIdMarkerPickup::NyIdMarkerPickup()
	{
		this->__pickFromRaster_encoder=new MarkerPattEncoder();
		this->_perspective_reader=new PerspectivePixelReader();
		return;
	}
	NyIdMarkerPickup::~NyIdMarkerPickup()
	{
		NyAR_SAFE_DELETE(this->__pickFromRaster_encoder);
		NyAR_SAFE_DELETE(this->_perspective_reader);
		return;
	}

	bool NyIdMarkerPickup::pickFromRaster(const INyARRgbRaster &image,const TNyARIntPoint2d i_vertexs[],TNyIdMarkerPattern &o_data,TNyIdMarkerParam& o_param)
	{
		const INyARRgbPixelReader &reader=image.getRgbPixelReader();
		const TNyARIntSize &raster_size=image.getSize();


		//���ߖ@�̃p�����[�^���v�Z
		if(!this->_perspective_reader->setSourceSquare(i_vertexs)){
			return false;
		};



		struct PerspectivePixelReader::TThreshold_t th;
		MarkerPattEncoder &encoder=*this->__pickFromRaster_encoder;
		//�}�[�J�p�����[�^���擾
		this->_perspective_reader->detectThresholdValue(reader,raster_size,th);

		if(!this->_perspective_reader->readDataBits(reader,raster_size,th, encoder)){
			return false;
		}
		const int d=encoder.encode(o_data);
		if(d<0){
			return false;
		}
		o_param.direction=d;
		o_param.threshold=th.th;

		return true;
	}


}
