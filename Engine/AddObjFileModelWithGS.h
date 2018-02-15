#pragma once

#include "Vec3.h"
#include "IndexedTriangleList.h"

class AddObjFileModelWithGS
{
public:
	template<class V>
	static IndexedTriangleListWithTC<V> GetSkinnedFromObjFileWithGS(float size, const std::wstring& filename)
	{
		FILE * file = _wfopen(filename.c_str(), L"r");
		//gota add exception

		std::vector<V> vertices;
		std::vector<size_t> triangles;

		std::vector<Vec2> tc;
		std::vector<size_t> uvMapping;
		while (1) {
			char lineHeader[128];
			// read the first word of the line

			int res = fscanf(file, "%s", lineHeader);
			if (res == EOF)
				break; // EOF = End Of File. Quit the loop.

			if (strcmp(lineHeader, "v") == 0) {
				Vec3 vertex;
				fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
				vertex *= size;
				vertices.push_back(vertex);
			}
			else if (strcmp(lineHeader, "vt") == 0) {
				Vec2 uv;
				fscanf(file, "%f %f\n", &uv.x, &uv.y);
				uv.y = 1 - uv.y;
				tc.push_back(uv);
			}
			else if (strcmp(lineHeader, "f") == 0) {
				unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
				fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);

				uvMapping.push_back(uvIndex[0] - 1);
				uvMapping.push_back(uvIndex[1] - 1);
				uvMapping.push_back(uvIndex[2] - 1);

				triangles.push_back(vertexIndex[0] - 1);
				triangles.push_back(vertexIndex[1] - 1);
				triangles.push_back(vertexIndex[2] - 1);
			}
		}
		return{
			std::move(vertices),
			std::move(triangles),
			std::move(tc),
			std::move(uvMapping)
		};
	}
};