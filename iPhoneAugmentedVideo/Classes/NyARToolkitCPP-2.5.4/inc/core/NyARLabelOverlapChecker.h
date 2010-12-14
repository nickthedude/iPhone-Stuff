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
#include "nyarcore.h"
#include <cstdlib>
namespace NyARToolkitCPP
{
	/**
	* ���x�����m�̏d�Ȃ�i����֌W�j�𒲂ׂ�N���X�ł��B 
	* ���x�����X�g�ɓ���郉�x����~�ς��A����Ƀ^�[�Q�b�g�̃��x���������Ă��邩 ���m�F���܂��B
	*/
	template <class T> class NyARLabelOverlapChecker
	{
	private:
		//�R�s�[�R���X�g���N�^�폜
		NyARLabelOverlapChecker(const NyARLabelOverlapChecker& );
		NyARLabelOverlapChecker& operator=(const NyARLabelOverlapChecker&);
	private:
		const T** _labels;
		int _length;
		/*
		*/
	public:
		NyARLabelOverlapChecker(int i_max_label)
		{
			this->_labels = new const T*[i_max_label];
		}
		~NyARLabelOverlapChecker()
		{
			NyAR_SAFE_ARRAY_DELETE(this->_labels);
		}

		/**
		* �`�F�b�N�Ώۂ̃��x����ǉ�����B
		* 
		* @param i_label_ref
		*/
		void push(const T* i_label_ref)
		{
			this->_labels[this->_length] = i_label_ref;
			this->_length++;
		}

		/**
		* ���݃��X�g�ɂ��郉�x���Əd�Ȃ��Ă��邩��Ԃ��B
		* 
		* @param i_label
		* @return ���ꂩ�̃��x���̓����ɂ���Ȃ��false,�Ɨ��������x���ł���\�����������true�ł��D
		*/
		bool check(const T& i_label)const
		{
			// �d�Ȃ菈�����ȁH
			const T** label_pt = this->_labels;
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
		/**
		* �ő�i_max_label�̃��x����~�ςł���悤�ɃI�u�W�F�N�g�����Z�b�g����
		* 
		* @param i_max_label
		*/
		void setMaxLabels(int i_max_label)
		{
			if (i_max_label > this->_length){
				NyAR_SAFE_ARRAY_DELETE(this->_labels);
				this->_labels=new const T*[i_max_label];
			}
			this->_length = 0;
		}	


	};

}
