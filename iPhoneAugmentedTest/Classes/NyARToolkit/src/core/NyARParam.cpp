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
#include "NyARParam.h"
#include "../../inc/utils/NyStdLib.h"
#include "nyarcore.h"
#include <exception>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <cmath>
using namespace std;
namespace NyARToolkitCPP
{

	NyARParam::NyARParam()
	{
		this->_endian=TNyAREndian_LITTLE;
		return;
	}


	const TNyARIntSize* NyARParam::getScreenSize()const
	{
		return &this->_screen_size;
	}

	const NyARPerspectiveProjectionMatrix* NyARParam::getPerspectiveProjectionMatrix()const
	{
		return &this->_projection_matrix;
	}
	const NyARCameraDistortionFactor* NyARParam::getDistortionFactor()const
	{
		return &this->_dist;
	}

	/**
	* ARToolKit�W���t�@�C������1�ڂ̐ݒ�����[�h����B
	* 
	* @param i_filename
	* @throws NyARException
	*/
	void NyARParam::loadARParamFromFile(const char* i_filename)
	{
		ifstream inf;
		NyARParamFileStruct_t tmp;
		try {
			inf.open(i_filename);
			inf.read((char*)&tmp,sizeof(NyARParamFileStruct_t));
			if(inf.gcount()!=sizeof(NyARParamFileStruct_t))
			{
				throw NyARException();
			}
			/*�G���f�B�A���ۏ�*/
			if(this->_endian==NyARToolkitCPP::TNyAREndian_LITTLE){
				tmp.x=NyStdLib::byteSwap(tmp.x);
				tmp.y=NyStdLib::byteSwap(tmp.y);
				for(int i=0;i<12;i++){
					tmp.projection[i]=NyStdLib::byteSwap(tmp.projection[i]);
				}
				for(int i=0;i<4;i++){
					tmp.distortion[i]=NyStdLib::byteSwap(tmp.distortion[i]);
				}
			}
			loadARParam(tmp);
			inf.close();
		} catch(NyARException e){
			throw;
		} catch (exception e) {
			throw NyARException(e);
		}
		return;
	}

	/**
	* int arParamChangeSize( ARParam *source, int xsize, int ysize, ARParam *newparam );
	* �֐��̑�֊֐� �T�C�Y�v���p�e�B��i_xsize,i_ysize�ɕύX���܂��B
	* @param i_xsize
	* @param i_ysize
	* @param newparam
	* @return
	* 
	*/
	void NyARParam::changeScreenSize(int i_xsize, int i_ysize)
	{
		const double scale = (double) i_xsize / (double) (this->_screen_size.w);// scale = (double)xsize / (double)(source->xsize);
		//�X�P�[����ύX
		this->_dist.changeScale(scale);
		this->_projection_matrix.changeScale(scale);

		this->_screen_size.w = i_xsize;// newparam->xsize = xsize;
		this->_screen_size.h = i_ysize;// newparam->ysize = ysize;
		return;
	}


	/**
	* int arParamLoad( const char *filename, int num, ARParam *param, ...);
	* i_stream�̓��̓X�g���[������i_num�̐ݒ��ǂݍ��݁A�p�����^��z��ɂ��ĕԂ��܂��B
	* 
	* @param i_stream
	* @throws Exception
	*/
	void NyARParam::loadARParam(const NyARParamFileStruct_t& i_stream)
	{
		/*�K�v�ɉ�����byteswap���Ȃ���l���R�s�[*/
		this->_screen_size.w = i_stream.x;
		this->_screen_size.h = i_stream.y;
		//Projection�I�u�W�F�N�g�ɃZ�b�g
		this->_projection_matrix.setValue(i_stream.projection);
		//Factor�I�u�W�F�N�g�ɃZ�b�g
		this->_dist.setValue(i_stream.distortion);
		return;
	}

	void NyARParam::saveARParam()
	{
		NyARException::trap("���`�F�b�N�̃p�X");
		/*
		byte[] buf = new byte[SIZE_OF_PARAM_SET];
		// �o�b�t�@�����b�v
		ByteBuffer bb = ByteBuffer.wrap(buf);
		bb.order(ByteOrder.BIG_ENDIAN);

		// ��������
		bb.putInt(this._screen_size.w);
		bb.putInt(this._screen_size.h);
		double[] tmp=new double[12];
		//Projection��ǂݏo��
		this._projection_matrix.getValue(tmp);
		//double�l��12��������
		for(int i=0;i<12;i++){
		tmp[i]=bb.getDouble();
		}
		//Factor��ǂݏo��
		this._dist.getValue(tmp);
		//double�l��4��������
		for (int i = 0; i < 4; i++) {
		tmp[i]=bb.getDouble();
		}
		i_stream.write(buf);
		return;
		*/
	};

	void NyARParam::setEndian(TNyAREndian i_new_endian)
	{
		this->_endian=i_new_endian;
		return;
	}

}
