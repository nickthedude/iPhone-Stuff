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

#include "NyARBaseClass.h"
#include "NyARRaster.h"
#include "NyAR_types.h"
#include "NyARMatchPattDeviationColorData.h"
namespace NyARToolkitCPP
{
	class NyARCode:public NyARBaseClass
	{
		friend class NyARCodeFileReader;
	private:
		NyARMatchPattDeviationColorData* _color_pat[4];
		int _width;
		int _height;

	public:
		NyARCode(int i_width, int i_height);
		virtual ~NyARCode();
		const NyARMatchPattDeviationColorData* getColorData(int i_index)const;
		//const NyARMatchPattDeviationBlackWhiteData* getBlackWhiteData(int i_index)const;
		int getWidth()const;
		int getHeight()const;
		void loadARPattFromFile(const char* i_filename);
		void setRaster(const NyARRaster i_raster[]);
		void loadARPatt(const unsigned char* i_stream);
	};

}
