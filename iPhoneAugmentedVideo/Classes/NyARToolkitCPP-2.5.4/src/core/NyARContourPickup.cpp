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
#include "nyarcore.h"
#include "NyARContourPickup.h"
#include "NyAR_types.h"
#include "INyARPca2d.h"
#include "NyARLabelingImage.h"
#include "NyARBinRaster.h"

namespace NyARToolkitCPP
{
	//����Q�Ƃł���悤�ɁA�e�[�u�����d��
	//                                           0  1  2  3  4  5  6  7   0  1  2  3  4  5  6
	const static int _getContour_xdir[15] = { 0, 1, 1, 1, 0,-1,-1,-1 , 0, 1, 1, 1, 0,-1,-1};
	const static int _getContour_ydir[15] = {-1,-1, 0, 1, 1, 1, 0,-1 ,-1,-1, 0, 1, 1, 1, 0};


	int NyARContourPickup::getContour(const NyARBinRaster& i_raster,int i_entry_x,int i_entry_y,int i_array_size,int* o_coord_x,int* o_coord_y)const
	{
		const int* xdir = _getContour_xdir;// static int xdir[8] = { 0, 1, 1, 1, 0,-1,-1,-1};
		const int* ydir = _getContour_ydir;// static int ydir[8] = {-1,-1, 0, 1, 1, 1, 0,-1};

		int* i_buf=(int*)i_raster.getBuffer();
		int width=i_raster.getWidth();
		int height=i_raster.getHeight();
		//�N���b�v�̈�̏�[�ɐڂ��Ă���|�C���g�𓾂�B


		int coord_num = 1;
		o_coord_x[0] = i_entry_x;
		o_coord_y[0] = i_entry_y;
		int dir = 5;

		int c = i_entry_x;
		int r = i_entry_y;
		for (;;) {
			dir = (dir + 5) % 8;//dir�̐��K��
			//�����͊撣��΂����ƍœK���ł���Ǝv����B
			//4���ȊO�̋��E�ڒn�̏ꍇ�ɁA���E�`�F�b�N���ȗ�����Ƃ��ˁB
			if(c>=1 && c<width-1 && r>=1 && r<height-1){
				for(;;){//goto�̃G�~�����[�g�p��for��
					//���E�ɐڂ��Ă��Ȃ��Ƃ�
					if (i_buf[(r + ydir[dir])*width+(c + xdir[dir])] == 0) {
						break;
					}
					dir++;
					if (i_buf[(r + ydir[dir])*width+(c + xdir[dir])] == 0) {
						break;
					}
					dir++;
					if (i_buf[(r + ydir[dir])*width+(c + xdir[dir])] == 0) {
						break;
					}
					dir++;
					if (i_buf[(r + ydir[dir])*width+(c + xdir[dir])] == 0) {
						break;
					}
					dir++;
					if (i_buf[(r + ydir[dir])*width+(c + xdir[dir])] == 0) {
						break;
					}
					dir++;
					if (i_buf[(r + ydir[dir])*width+(c + xdir[dir])] == 0) {
						break;
					}
					dir++;
					if (i_buf[(r + ydir[dir])*width+(c + xdir[dir])] == 0) {
						break;
					}
					dir++;
					if (i_buf[(r + ydir[dir])*width+(c + xdir[dir])] == 0) {
						break;
					}
					/*
					try{
					BufferedImage b=new BufferedImage(width,height,ColorSpace.TYPE_RGB);
					NyARRasterImageIO.copy(i_raster, b);
					ImageIO.write(b,"png",new File("bug.png"));
					}catch(Exception e){

					}*/
					//8�����S�Ē��ׂ����ǃ��x����������H
					throw NyARException();			
				}
			}else{
				//���E�ɐڂ��Ă���Ƃ�				
				int i;
				for (i = 0; i < 8; i++){				
					const int x=c + xdir[dir];
					const int y=r + ydir[dir];
					//���E�`�F�b�N
					if(x>=0 && x<width && y>=0 && y<height){
						if (i_buf[(y)*width+(x)] == 0) {
							break;
						}
					}
					dir++;//�{���e�[�u�����Q�Ƃ���̂Ŗ��Ȃ�
				}
				if (i == 8) {
					//8�����S�Ē��ׂ����ǃ��x����������H
					throw NyARException();// return(-1);
				}				
			}

			dir=dir% 8;//dir�̐��K��

			// xcoord��ycoord��c,r�ɂ��ۑ�
			c = c + xdir[dir];
			r = r + ydir[dir];
			o_coord_x[coord_num] = c;
			o_coord_y[coord_num] = r;
			// �I����������
			if (c == i_entry_x && r == i_entry_y){
				coord_num++;
				break;
			}
			coord_num++;
			if (coord_num == i_array_size) {
				//�֊s�����[�ɒB����
				return coord_num;
			}
		}
		return coord_num;
	}
	int NyARContourPickup::getContour(const NyARLabelingImage& i_raster,int i_entry_x,int i_entry_y,int i_array_size,int* o_coord_x,int* o_coord_y)const
	{
		const int* xdir = _getContour_xdir;// static int xdir[8] = { 0, 1, 1, 1, 0,-1,-1,-1};
		const int* ydir = _getContour_ydir;// static int ydir[8] = {-1,-1, 0, 1, 1, 1, 0,-1};

		const int* i_buf=(const int*)i_raster.getBuffer();
		const int width=i_raster.getWidth();
		const int height=i_raster.getHeight();
		//�N���b�v�̈�̏�[�ɐڂ��Ă���|�C���g�𓾂�B
		int sx=i_entry_x;
		int sy=i_entry_y;

		int coord_num = 1;
		o_coord_x[0] = sx;
		o_coord_y[0] = sy;
		int dir = 5;

		int c = o_coord_x[0];
		int r = o_coord_y[0];
		for (;;) {
			dir = (dir + 5) % 8;//dir�̐��K��
			//�����͊撣��΂����ƍœK���ł���Ǝv����B
			//4���ȊO�̋��E�ڒn�̏ꍇ�ɁA���E�`�F�b�N���ȗ�����Ƃ��ˁB
			if(c>=1 && c<width-1 && r>=1 && r<height-1){
				for(;;){//goto�̃G�~�����[�g�p��for��
					//���E�ɐڂ��Ă��Ȃ��Ƃ�
					if (i_buf[(r + ydir[dir])*width+(c + xdir[dir])] > 0) {
						break;
					}
					dir++;
					if (i_buf[(r + ydir[dir])*width+(c + xdir[dir])] > 0) {
						break;
					}
					dir++;
					if (i_buf[(r + ydir[dir])*width+(c + xdir[dir])] > 0) {
						break;
					}
					dir++;
					if (i_buf[(r + ydir[dir])*width+(c + xdir[dir])] > 0) {
						break;
					}
					dir++;
					if (i_buf[(r + ydir[dir])*width+(c + xdir[dir])] > 0) {
						break;
					}
					dir++;
					if (i_buf[(r + ydir[dir])*width+(c + xdir[dir])] > 0) {
						break;
					}
					dir++;
					if (i_buf[(r + ydir[dir])*width+(c + xdir[dir])] > 0) {
						break;
					}
					dir++;
					if (i_buf[(r + ydir[dir])*width+(c + xdir[dir])] > 0) {
						break;
					}
					//8�����S�Ē��ׂ����ǃ��x����������H
					throw NyARException();			
				}
			}else{
				//���E�ɐڂ��Ă���Ƃ�
				int i;
				for (i = 0; i < 8; i++){				
					const int x=c + xdir[dir];
					const int y=r + ydir[dir];
					//���E�`�F�b�N
					if(x>=0 && x<width && y>=0 && y<height){
						if (i_buf[(y)*width+(x)] > 0) {
							break;
						}
					}
					dir++;//�{���e�[�u�����Q�Ƃ���̂Ŗ��Ȃ�
				}
				if (i == 8) {
					//8�����S�Ē��ׂ����ǃ��x����������H
					throw NyARException();// return(-1);
				}				
			}

			dir=dir% 8;//dir�̐��K��

			// xcoord��ycoord��c,r�ɂ��ۑ�
			c = c + xdir[dir];
			r = r + ydir[dir];
			o_coord_x[coord_num] = c;
			o_coord_y[coord_num] = r;
			// �I����������
			if (c == sx && r == sy){
				coord_num++;
				break;
			}
			coord_num++;
			if (coord_num == i_array_size) {
				//�֊s�����[�ɒB����
				return coord_num;
			}
		}
		return coord_num;
	}

}

