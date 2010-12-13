#pragma once
#include "NyArray.h"
namespace NyARToolkitCPP
{
	template <class T> class NyObjectStack
	{
	private:
		const static int ARRAY_APPEND_STEP=64;
	protected:
		NyArray<T*>* _items;
		int _length;
	private:
		int _allocated_size;
	public:
		NyObjectStack(int i_size)
		{
			// �|�C���^�����͂͂��߂Ɋm�ۂ��Ă���
			this->_items =new NyArray<T*>(i_size);

			// �A���P�[�g�σT�C�Y�ƁA�g�p���������Z�b�g
			this->_allocated_size = 0;
			this->_length = 0;
			return;
		}
		~NyObjectStack(void)
		{
			for(int i=0;i<this->_allocated_size;i++){
				delete *(this->_items->item+i);
			}
			delete this->_items;
			return;
		}
		 T* prePush()
		{
			// �K�v�ɉ����ăA���P�[�g
			if (this->_length >= this->_allocated_size) {
				// �v�����ꂽ�C���f�N�X�͔͈͊O
				if (this->_length >= this->_items->length) {
					throw NyARException();
				}
				// �ǉ��A���P�[�g�͈͂��v�Z
				int range = this->_length + ARRAY_APPEND_STEP;
				if (range >= this->_items->length) {
					range = this->_items->length;
				}
				// �A���P�[�g
				this->onReservRequest(this->_allocated_size, range, *this->_items);
				this->_allocated_size = range;
			}
			// �g�p�̈��+1���āA�\�񂵂��̈��Ԃ��B
			T* ret = this->_items->item[this->_length];
			this->_length++;
			return ret;
		}
		 T* pop()
		{
			if(this->_length<1){
				throw NyARException();
			}
			this->_length--;
			return this->getItem(this->_length);
		}

		const NyArray<T*>* getArray() const
		{
			return this->_items;
		}
		T* getItem(int i_index) const
		{
			return *(this->_items->item+i_index);
		}
		int getLength() const
		{
			return this->_length;
		}
		void clear()
		{
			this->_length = 0;
		}
	private:
		void onReservRequest(int i_start, int i_end, NyArray<T*>& i_buffer)
		{
			for (int i = i_start; i < i_end; i++) {
				i_buffer.item[i] = new T();
			}
			return;
		}
	public:
		 void reserv(int i_number_of_item)
		{
			// �K�v�ɉ����ăA���P�[�g
			if (i_number_of_item >= this->_allocated_size) {
				// �v�����ꂽ�C���f�N�X�͔͈͊O
				if (i_number_of_item >= this->_items->length) {
					throw NyARException();
				}
				// �ǉ��A���P�[�g�͈͂��v�Z
				int range = i_number_of_item+ARRAY_APPEND_STEP;
				if (range >= this->_items->length) {
					range = this->_items->length;
				}
				// �A���P�[�g
				this->onReservRequest(this->_allocated_size, range, *this->_items);
				this->_allocated_size = range;
			}
			//��������̔z��T�C�Y���w��
			this->_length=i_number_of_item;
			return;
		}
	};
}
