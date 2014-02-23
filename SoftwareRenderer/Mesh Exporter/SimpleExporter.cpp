#include <maya/MIOStream.h>
#include <maya/MSimple.h>
#include <maya/MGlobal.h>
#include <maya/MDagPath.h>
#include <maya/MItDag.h>
#include <maya/MFnMesh.h>
#include <maya/MPointArray.h>
#include <maya/MFloatVectorArray.h>
#include <maya/MItMeshPolygon.h>
#include <assert.h>

DeclareSimpleCommand( ExportMesh, PLUGIN_COMPANY, "4.5");

struct Vertex
{
	float x,y,z;
	char R, G, B, A;
	float u,v;
	float xNormal, yNormal, zNormal;

	Vertex(){}
	~Vertex(){}

	void Serialize(char* buffer)
	{
		memcpy(buffer, this, sizeof(Vertex));
	}

	static bool Compare(const Vertex& v1, const Vertex& v2)
	{
		return (v1.x == v2.x && v1.y == v2.y && v1.z == v2.z && v1.xNormal == v2.xNormal && v1.yNormal == v2.yNormal && v1.zNormal == v2.zNormal && v1.u == v2.u && v1.v == v2.v);
	}

	inline void operator=(const Vertex& v)
	{
		x = v.x;
		y = v.y;
		z = v.z;
		u = v.u;
		R = v.R;
		G = v.G;
		B = v.B;
		A = v.A;
		this->v = v.v;
		xNormal = v.xNormal;
		yNormal = v.yNormal;
		zNormal = v.zNormal;
	}
};

struct Triangle
{
	unsigned short verts[3];
};

void ExportVertexBinary(Vertex* verts, int totalVerts, const char* filename)
{
	FILE* file = fopen(filename, "wb");
	if (file != NULL)
	{
		// Serialize and export the verts.
		fwrite(verts, sizeof(Vertex), totalVerts, file);
		fclose(file);
	}
}

void ExportIndexBinary(unsigned short* indices, int totalIndices, const char* filename)
{
	FILE* file = fopen(filename, "wb");
	if (file != NULL)
	{
		// Serialize and export the verts.
		fwrite(indices, sizeof(unsigned short), totalIndices, file);
		fclose(file);
	}
}

void Export(MFnMesh& fnMesh, const char* filename)
{
	//std::ofstream outFile;
	//outFile.open(filename);

	Vertex* verts = NULL;
	FILE* file = fopen(filename, "wt");
	if (file != NULL)
	{
		fprintf(file, "mesh %s\n", fnMesh.name().asChar());
		
		fprintf(file, "==== VERTICES ====\n");
		int i;
		MPointArray positions;
		fnMesh.getPoints(positions);
		MFloatArray uArray;
		MFloatArray vArray;
		fnMesh.getUVs(uArray, vArray);
		fprintf(file, "Total verts: %i\n",positions.length());
		verts = new Vertex[positions.length()];

		for (i = 0; i < positions.length(); i++)
		{
			fprintf(file, "[VERTEX]\n");
			MPoint& pos = positions[i];
			MVector n;
			float2 uvSet;
			//uArray.
			//fnMesh.getUvShellsIds
			fnMesh.getUVAtPoint(pos, uvSet);
			fnMesh.getVertexNormal(i, false, n);
			fprintf(file, "pos = %f,%f,%f\n", pos.x, pos.y, pos.z);
			fprintf(file, "normal = %f,%f,%f\n", n.x, n.y, n.z);
			fprintf(file, "uv = %f,%f\n", uvSet[0], uvSet[1]);
			fprintf(file, "[/VERTEX]\n");
			
			verts[i].A = 255;
			verts[i].R = 255;
			verts[i].G = 255;
			verts[i].B = 255;
			verts[i].u = uvSet[0];
			verts[i].v = uvSet[1];
			verts[i].x = pos.x;
			verts[i].y = pos.y;
			verts[i].z = pos.z;
			verts[i].xNormal = n.x;
			verts[i].yNormal = n.y;
			verts[i].zNormal = n.z;
		}


		MColorArray colors;
		fnMesh.getVertexColors(colors);

		unsigned short* indexs = NULL;
		int totalTris = 0;
		fprintf(file, "==== INDICES ====\n");
		MItMeshPolygon polyIter(fnMesh.object());
		for (; !polyIter.isDone(); polyIter.next())
		{
			int numTriangles = 0;
			polyIter.numTriangles(numTriangles);
			for (i = 0; i < numTriangles; i++)
			{
				MPointArray points;
				MIntArray indices;
				polyIter.getTriangle(i, points, indices);
				//polyIter.get
				assert(indices.length() == 3);
				fprintf(file, "triangle = %d,%d,%d\n", indices[0], indices[1], indices[2]);
			}
			totalTris += 1;
		}

		fprintf(file, "TOTAL TRIANGLES = %i\n", totalTris);

		// Export the indices.
		MItMeshPolygon iter(fnMesh.object());
		indexs = new unsigned short[totalTris * 3];
		int index = 0;
		for (; !iter.isDone(); iter.next())
		{
			int numTriangles = 0;
			iter.numTriangles(numTriangles);
			for (i = 0; i < numTriangles; i++)
			{
				MPointArray points;
				MIntArray indices;
				iter.getTriangle(i, points, indices);
				//iter.get
				assert(indices.length() == 3);
				indexs[index] = indices[0];
				indexs[index + 1] = indices[1];
				indexs[index + 2] = indices[2];


			}
			index += 3;
		}
		
		// Export the indices
		ExportIndexBinary(indexs, totalTris * 3, "D:\\MayaPlugins\\ModelIndexBinary.txt");
		ExportVertexBinary(verts, positions.length(), "D:\\MayaPlugins\\ModelVertexBinary.txt");
			

		fclose(file);

		// Destroy the verts.
		if (verts != NULL)
		{
			delete [] verts;
			verts = NULL;
		}

		// Destory the indices.
		if (indexs != NULL)
		{
			delete [] indexs;
			indexs = NULL;
		}
	}
}

void ExportToFile(MFnMesh mesh)
{
	// Iterate over the polygons and count the amount of vertices to export.
	int totalVerts = 0;
	int totalTris = 0;
	MItMeshPolygon polyIter(mesh.object());
	for (; !polyIter.isDone(); polyIter.next())
	{
		MIntArray vertexIDs;
		polyIter.getVertices(vertexIDs);
		if (vertexIDs.length() == 3)
		{
			totalVerts += 3;
			totalTris++;
		}
	}

	// Reset the iterator and re-iterate over the mesh to export the triangles.
	MItMeshPolygon iter(mesh.object());
	MFloatArray uSets;
	MFloatArray vSets;
	MPointArray positions;
	MFloatVectorArray normals;
	mesh.getUVs(uSets, vSets);
	mesh.getPoints(positions);
	mesh.getNormals(normals);

	Vertex* vertices = new Vertex[totalVerts];
	int currentVertex = 0;
	MPoint pos;
	float2 uvSet;
	MFloatVector n;

	for (; !iter.isDone(); iter.next())
	{
		for (int i = 0; i < 3; i++)
		{

			pos = positions[iter.vertexIndex(i)];
			n = normals[iter.normalIndex(i)];
			int uvIndex = -1;
			if (iter.getUVIndex(i, uvIndex))
			{
				uvSet[0] = uSets[uvIndex]; // Get U information.
				uvSet[1] = vSets[uvIndex]; // Get V information.
			}
			// Carry the properties to the vertex.
			vertices[currentVertex].A = 255;
			vertices[currentVertex].R = 255;
			vertices[currentVertex].G = 255;
			vertices[currentVertex].B = 255;
			vertices[currentVertex].u = uvSet[0];
			vertices[currentVertex].v = uvSet[1];
			vertices[currentVertex].x = pos.x;
			vertices[currentVertex].y = pos.y;
			vertices[currentVertex].z = pos.z;
			vertices[currentVertex].xNormal = n.x;
			vertices[currentVertex].yNormal = n.y;
			vertices[currentVertex].zNormal = n.z;
			currentVertex++;
		}
	}

	// Export the binary.
	ExportVertexBinary(vertices, totalVerts, "D:\\MayaPlugins\\ModelVertexBinary.txt");

	// Export a simple declaration file saying how many triangles we exported.
	FILE* file = fopen("D:\\MayaPlugins\\ModelDeclaration.txt", "wt");
	if (file != NULL)
	{
		fprintf(file, "Total vertices = %i\n", totalVerts);
		fprintf(file, "Total triangles = %i\n", totalTris);
	}

	fclose(file);


	// Delete the vertices.
	if (vertices != NULL)
	{
		delete [] vertices;
		vertices = NULL;
	}
}

MStatus ExportMesh::doIt( const MArgList& )
{
	for (MItDag dagIter; !dagIter.isDone(); dagIter.next())
	{
		MObject object = dagIter.currentItem();
		if (object.hasFn(MFn::kMesh))
		{
			char filename[1024] = {0};
			MFnMesh fnMesh(object);
			//Export(fnMesh, "D:\\MayaPlugins\\ModelTextDecl.txt");
			ExportToFile(fnMesh);
		}
	}

	return MS::kSuccess;
}
