#pragma once
#include "INyARRgbRaster.h"
#include "NyARSquare.h"

namespace NyARToolkitCPP
{
	class INyARColorPatt
	{
	public:
		virtual ~INyARColorPatt(){};
	public:
		virtual int getWidth()const=0;
		virtual int getHeight()const=0;
		/**
		* �J�����p�^�[�����i�[�����z��ւ̎Q�ƒl��Ԃ��܂��B �z��͍Œ�ł�[height][wight][3]�̃T�C�Y�������܂����A
		* �z���length��width,height�̐��͈�v���Ȃ����Ƃ�����܂��B
		* 
		* @return
		*/
		virtual const int* getPatArray()const=0;
		virtual bool pickFromRaster(const INyARRgbRaster& image, const NyARSquare& i_square)=0;
	};
}
