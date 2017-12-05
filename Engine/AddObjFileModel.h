#pragma once

#include "Vec3.h"
#include "IndexedTriangleList.h"

class AddObjFileModel
{
public:
	template<class V>
	static IndexedTriangleList<V> GetSkinnedFromObjFile(float size, const std::wstring& filename)
	{
		FILE * file = _wfopen(filename.c_str(), L"r");
		//gota add exception

		std::vector<Vec3> vertices;
		std::vector<Vec2> tc;

		std::vector<V> verts;
		std::vector<size_t> triangles;

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

				V vertsToBePushedBack[3];
				vertsToBePushedBack[0].pos = vertices.at(vertexIndex[0]-1);
				vertsToBePushedBack[0].t = tc.at(uvIndex[0]-1);
				vertsToBePushedBack[1].pos = vertices.at(vertexIndex[1]-1);
				vertsToBePushedBack[1].t = tc.at(uvIndex[1]-1);
				vertsToBePushedBack[2].pos = vertices.at(vertexIndex[2]-1);
				vertsToBePushedBack[2].t = tc.at(uvIndex[2]-1);

				verts.push_back(vertsToBePushedBack[0]);
				verts.push_back(vertsToBePushedBack[1]);
				verts.push_back(vertsToBePushedBack[2]);

				triangles.push_back(verts.size() - 3);
				triangles.push_back(verts.size() - 2);
				triangles.push_back(verts.size() - 1);
			}
		}
		return{
			std::move(verts),
			std::move(triangles)
		};
	}
};