#pragma once
#include "Octant.h"
#include "DynamicMesh.h"
#include "Shader.h"
class testGenerator
{
public:
	testGenerator();
	~testGenerator();

	void generate(Octant* _ot, DynamicMesh* _dm);

private:

	std::vector<octantStackElement> octantStack;

	Shader mcShader;
	Shader scalarGen;

	//GLint locationDims;
	//GLint locationVposition;

	GLint volumeTex;
	GLint triTable;
	GLint edgeTable;

	GLint octantPosGen;
	GLint octantPosMC;
	//GLint isoValue;

	bool first = true;
};

