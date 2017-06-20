#include <iostream>
#include <string.h>
#include <stdio.h>

#include "Material.h"


Material::Material(const char* name)
{
	snprintf(m_name, MATERIAL_MAX_NAMELENGTH, "%s", name);
	m_reflectivity = 0.f;
}

bool Material::Parse(xml_node<>* node, char* sp, int spSize)
{
	if (node)
	{
		m_baseColour.Parse(node->first_node("colour"), sp, spSize);
	
		xml_node<>* n = node->first_node("reflectivity");
		if (n)
		{
			snprintf(sp, spSize, "%s", n->value());
			m_reflectivity = (float)atof(sp);
		}

		return true;
	}
	return false;
}
