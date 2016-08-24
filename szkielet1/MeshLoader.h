#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <fstream>
#include <iostream>

using namespace std;
class MeshLoader
{
public:
	MeshLoader();
	~MeshLoader();
	void LoadMesh(char path[], vector<float> &outVert, vector<float> &outNorm, vector<float> &outUv);
};

