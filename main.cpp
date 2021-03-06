#include <iostream>
#include <fstream>

#include "Objects.h"
#include "Scene.h"
using namespace std;

char scene[] = "<?xml version='1.0' encoding='utf-8'?>"
"<objects>"
"<object type='simplemesh' name='plane'>"
"<verts>-1.0 0.0 -1.0,1.0 0.0 -1.0, 1.0 0.0 1.0, -1.0 0.0 1.0</verts>"
"<tris>0 1 2, 2 3 0</tris>"
"</object>"
"</objects>"
"<materials>"
	"<material name='brightyellow'>"
		"<colour>1.0 1.0 0.3</colour>"
	"</material>"
  "</materials>"
"<scene>"
	"<inst obj='plane' type='simplemesh' material='brightyellow'>"
		"<posrot>"
			"1.0 0.0 0.0 0.0 0.0 1.0 0.0 0.0 0.0 0.0 1.0 0.0 0.0 0.0 0.0 1.0"
		"</posrot>"
	"</inst>"
	"<inst type = 'camera' fov = '55.0' nearz = '0.1'>"
		"<eye>3.0 0.5 0.0</eye>"
		"<target>0.0 0.0 0.0</target>"
	"</inst>"

	"<ambient>"
		"<colour>0.25 0.25 0.25</colour>"
	"</ambient>"
"</scene>";

int main(int argc, char * argv[])
{
	int ret = 0;
	char* fileData = NULL;
	ifstream file(argc > 1 ? argv[1] : "../scene.xml", ios::in|ios::binary|ios::ate);

	if (file.is_open())
	{
		streampos size = file.tellg();
		fileData = new char [(unsigned int)size+1];
		file.seekg (0, ios::beg);
		file.read (fileData, size);
		file.close();
		fileData[(unsigned int)size] = 0;
	}
	
	Scene s;
	const int spSize = 1024 * 1024;
	char* sp = (char*)malloc(1024 * 1024);
	if (!sp || !s.ParseInput(fileData ? fileData : scene, sp, spSize))
	{
		cout << "Failed to parse XML" << endl;
		ret = 1;
	}
	else
	{
		cout << "Finished reading scene:\n\n " << s.LogInfo() << endl;
	}
	
	free(sp);
	delete[] fileData;
	

	int width = argc > 3 ? atoi(argv[2]) : 800;
	int height = argc > 3 ? atoi(argv[3]) : 600;
	int superSample = argc > 4 ? atoi(argv[4]) : 2;

	s.Render(width, height, superSample);

	return ret;
}
