/*
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef DETAILED_CUBE_H
#define DETAILED_CUBE_H


#include "model.h"

namespace Models {
	class DetailedCube:public Model {
		public:
			DetailedCube();
			virtual ~DetailedCube();
			virtual void drawSolid();
		private:
			void initCube();
			void freeCube();
			void wall();
			void quad(int subdiv,int i1, int i2, float x, float y, float back, float nx, float ny, float s,float t,int pos);
	};
	
	extern DetailedCube detailedCube;
}




#endif 
