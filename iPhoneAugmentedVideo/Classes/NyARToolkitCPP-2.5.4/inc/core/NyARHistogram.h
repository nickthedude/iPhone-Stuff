/* 
* PROJECT: NyARToolkitCPP (Extension)
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
#include "NyARBaseClass.h"

namespace NyARToolkitCPP
{
	/**
	 * �q�X�g�O�������i�[����N���X�ł��B
	 */
	class NyARHistogram:public NyARBaseClass
	{
	public:
		int* data;
		int length;
		int total_of_data;
	public:
		NyARHistogram(int i_length);
		~NyARHistogram();
		int getTotal(int i_st,int i_ed);
		/**
		 * �w�肵��i_pos�����T���v�����O�ɂ��܂��B
		 * @param i_pos
		 */
		void lowCut(int i_pos);
		/**
		 * �w�肵��i_pos�ȏ�̃T���v�����O�ɂ��܂��B
		 * @param i_pos
		 */
		void highCut(int i_pos);
		/**
		 * �ŏ��̒l���i�[����Ă���T���v���ԍ���Ԃ��܂��B
		 */
		int getMinSample();
		/**
		 * �T���v���̒��ōŏ��̒l��Ԃ��܂��B
		 * @return
		 */
		int getMinData();
		/**
		 * ���ϒl���v�Z���܂��B
		 * @return
		 */
		int getAverage();
	};
}
