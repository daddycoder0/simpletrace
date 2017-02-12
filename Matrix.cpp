#include <iostream>
#include <string.h>
#include <stdio.h>
#include "Matrix.h"

using namespace rapidxml;
using namespace std;

char* strtk(char* tok, char** context)
{
#ifdef WIN32
	return strtok_s(tok, " ,\n\r", context);
#else
	return strtok(tok, " ,\n\r");
#endif
}

bool Vector3::Parse(rapidxml::xml_node<>* n, char* sp, int spSize)
{
	if (n)
	{
		if (strlen(n->value()) >= spSize)
		{
			cout << "Very bad vector input: " << n->name() << endl;
			Set(0);
			return false;
		}

		char* context = NULL;
		unsigned int numElements = 0;

		snprintf(sp, spSize, n->value());
		char* c = strtk(sp, &context);

		while (c != NULL)
		{
			if (strlen(c))
			{
				numElements++;
			}
			c = strtk(NULL, &context);
		}

		if (numElements != 3)
		{
			cout << "Bad number of vector elements: " << n->name() << endl;
			Set(0);
			return false;
		}
		context = NULL;
		snprintf(sp, spSize, n->value());

		char* x = strtk(sp, &context);
		char* y = strtk(NULL, &context);
		char* z = strtk(NULL, &context);

		Set(atof(x),atof(y),atof(z));
	}
	else
	{
		Set(0);
	}
	return true;
}

void Matrix4::InitRotFromAngles(real x, real y, real z)
{
	float A = cos(x);
	float B = sin(x);
	float C = cos(y);
	float D = sin(y);
	float E = cos(z);
	float F = sin(z);

	float AD = A * D;
	float BD = B * D;

	m[0][0]  =   C * E;
	m[0][1]  =  -C * F;
	m[0][2]  =  -D;
	m[1][0]  = -BD * E + A * F;
	m[1][1]  =  BD * F + A * E;
	m[1][2]  =  -B * C;
	m[2][0]  =  AD * E + B * F;
	m[2][1]  = -AD * F + B * E;
	m[2][2] =   A * C;

	m[0][3] = m[1][3] = m[2][3] = 0.f;
	m[3][3] =  1.f;
}

bool Matrix4::Parse(xml_node<>* n, char* sp, int spSize)
{
	if (n)
	{
		if (n->first_node("posrot"))
		{
			char* context = NULL;
			n = n->first_node("posrot");
			unsigned int numElements = 0;
			if (strlen(n->value()) >= spSize)
			{
				cout << "Very bad matrix input: " << n->name() << endl;
				SetToIdentity();
				return false;	
			}
			snprintf(sp, spSize, n->value());
			char* c = strtk(sp, &context);

			while (c != NULL)
			{
				if (strlen(c))
				{
					numElements++;
				}
				c = strtk(NULL, &context);
			}

			if (numElements != 16)
			{
				cout << "Bad number of matrix elements: " << n->name() << endl;
				SetToIdentity();
				return false;
			}
			context = NULL;
			snprintf(sp, spSize, n->value());

			c = strtk(sp, &context);
			numElements = 0;
			while (c != NULL)
			{
				if (strlen(c))
				{
					SetElement(numElements++, (real)atof(c));
				}
				c = strtk(NULL, &context);
			}
		}
		else
		{
			Vector3 pos, angles;
			pos.Parse(n->first_node("pos"), sp, spSize);
			angles.Parse(n->first_node("rot"), sp, spSize);
			InitRotFromAngles(angles.m_x*DEG2RAD, angles.m_y*DEG2RAD, angles.m_z*DEG2RAD);
			SetPos(pos.m_x, pos.m_y, pos.m_z);
		}
	}
	else
	{
		SetToIdentity();
	}
	return true;
}

void Matrix4::SetToIdentity()
{
	m[0][0] = 1.f; m[0][1] = 0.f; m[0][2] = 0.f; m[0][3] = 0.f;
	m[1][0] = 0.f; m[1][1] = 1.f; m[1][2] = 0.f; m[1][3] = 0.f;
	m[2][0] = 0.f; m[2][1] = 0.f; m[2][2] = 1.f; m[2][3] = 0.f;
	m[3][0] = 0.f; m[3][1] = 0.f; m[3][2] = 0.f; m[3][3] = 1.f;
}
