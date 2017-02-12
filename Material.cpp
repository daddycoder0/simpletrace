#include <iostream>
#include <string.h>
#include <stdio.h>

#include "Material.h"


Material::Material(const char* name)
{
	snprintf(m_name, MATERIAL_MAX_NAMELENGTH, name);
}

bool Material::Parse(xml_node<>* node, char* sp, int spSize)
{
	if (node)
	{
		m_baseColour.Parse(node->first_node("colour"), sp, spSize);
		return true;
	}
	return false;
}
