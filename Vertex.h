#ifndef _VERTEX_H_
#define _VERTEX_H_

#include "Matrix.h"

class Vertex
{
public:
			Vertex(){};
			Vertex(Vector3& pos)
			{
				m_pos = pos;
			}

			void SetPos(Vector3& pos)
			{
				m_pos = pos;
			}

			Vector3& Pos(){return m_pos;}
private:
	Vector3	m_pos;
};


#endif