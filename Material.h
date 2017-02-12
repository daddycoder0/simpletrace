#ifndef _MATERIAL_H_
#define _MATERIAL_H_

#include "Matrix.h"
#include <string.h>
#include "ThirdParty/rapidxml/rapidxml.hpp"

#define MATERIAL_MAX_NAMELENGTH 32
using namespace rapidxml;

class Material
{
public:
					Material				(const char* name);
	bool			Parse					(xml_node<>* node, char* sp, int spSize);
	bool Is			(char* name)			{return strcmp(m_name, name) == 0;}
	void			GetColour(Vector3& out) {out = m_baseColour;}

private:
	
	char			m_name[MATERIAL_MAX_NAMELENGTH];
	Vector3			m_baseColour;
};

#endif