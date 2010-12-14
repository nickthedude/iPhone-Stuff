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

#include "NyARRgbRaster_RGB.h"
#include "nyarcore.h"
namespace NyARToolkitCPP
{
	NyARRgbRaster_RGB::NyARRgbRaster_RGB(int i_width, int i_height,bool i_is_alloc)
		:NyARRgbRaster(i_width,i_height,NyARBufferType::BYTE1D_R8G8B8_24,i_is_alloc)
	{
	}

	NyARRgbRaster_RGB::NyARRgbRaster_RGB(int i_width, int i_height)
		:NyARRgbRaster(i_width,i_height,NyARBufferType::BYTE1D_R8G8B8_24)
	{
	}

}

