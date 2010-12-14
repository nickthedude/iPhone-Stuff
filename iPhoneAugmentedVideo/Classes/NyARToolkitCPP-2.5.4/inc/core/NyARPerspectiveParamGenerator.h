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

#include "NyAR_types.h"
#include "NyARBaseClass.h"
namespace NyARToolkitCPP
{
	class NyARPerspectiveParamGenerator:public NyARBaseClass
	{
	protected:
		int _local_x;
		int _local_y;
		int _width;
		int _height;
	public:
		NyARPerspectiveParamGenerator(int i_local_x,int i_local_y,int i_width, int i_height);
		virtual ~NyARPerspectiveParamGenerator();
		virtual bool getParam(const TNyARIntPoint2d i_vertex[],double o_param[])const=0;
	};
}
