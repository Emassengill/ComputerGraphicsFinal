#ifndef __MESH_GEN__
#define __MESH_GEN__

#include "Definitions.h"

struct SquareGen {
	//four points, four colors, two normals
	//two sides, two triangles per side, three vertices per triangle, 3 attributes per vertex
	static const int numElements = 4 + 4 + 2;
	static const int numVertices = 4 + 4;
	static const int numIndices = 2 * 2 * 3;

	vec4 elements[numElements];
	vec4 data[3 * numVertices];
	GLubyte indices[numIndices];

	static const GLsizei sizeData = 3 * numVertices * sizeof(vec4);
	static const GLsizei sizeIndices = numIndices * sizeof(GLubyte);

	static const int points = 0;
	static const int colors = numVertices * sizeof(vec4);
	static const int normals = 2 * numVertices * sizeof(vec4);

	void generate(const color4& color0, const color4& color1, const color4& color2, const color4& color3);
	SquareGen(const color4& color_1, const color4& color_2, const color4& color_3, const color4& color_4);
	SquareGen(const color4& color);
};

struct TriangleGen {
	//3 points, 3 colors, 2 normals
	//2 sides, 3 vertices per side, 3 attributes per vertex
	static const int numElements = 3 + 3 + 2;
	static const int numVertices = 3 + 3;
	static const int numIndices = 2 * 3;

	vec4 elements[numElements];
	vec4 data[3 * numVertices];
	GLubyte indices[numIndices];

	static const int sizeData = 3 * numVertices * sizeof(vec4);
	static const int sizeIndices = numIndices * sizeof(GLubyte);

	static const int points = 0;
	static const int colors = numVertices * sizeof(vec4);
	static const int normals = numVertices * 2 * sizeof(vec4);

	void generator(const color4& color0, const color4& color1, const color4& color2);
	TriangleGen(const color4& color0, const color4& color1, const color4& color2);
	TriangleGen(const color4& color);
};

struct CircleGen {
	//unknown number of vertices
	int numElements;
	int numVertices;
	int numIndices;

	vec4* elements;
	vec4* data;
	GLubyte* indices;

	int sizeData;
	int sizeIndices;

	static const int points = 0;
	int colors;
	int normals;

	void generate(int numEdgePoints, const color4& insideColor, const color4& outsideColor);
	CircleGen(int numEdgePoints, const color4& color0, const color4& color1);
	CircleGen(int numEdgePoints, const color4& color);
	~CircleGen();
};

struct RingGen {
	//unknown number of vertices
	int numElements;
	int numVertices;
	int numIndices;

	vec4* elements;
	vec4* data;
	GLubyte* indices;

	int sizeData;
	int sizeIndices;

	static const int points = 0;
	int colors;
	int normals;

	void generate (const float holeR, int numEdgePoints,
		const color4& insideColor, const color4& outsideColor );
	RingGen(const float holeR, int numEdgePoints,
					const color4& insideColor, const color4& outsideColor );
	RingGen(const float holeR, int numEdgePoints, const color4& color);
	RingGen(int numEdgePoints, const color4& color);
	~RingGen();
};

struct FanGen {
	//5 points, 2 colors, and 2 normals
	//2 sides, three triangles per side, three vertices per triangle, 3 attributes per vertex
	static const int numElements = 5 + 2 + 2;
	static const int numVertices = 2 * 5;
	static const int numIndices = numVertices * 3;

	vec4 elements[numElements];
	vec4 data[3 * numVertices];
	GLubyte indices[numIndices];

	static const int sizeData = 3 * numVertices * sizeof(vec4);
	static const int sizeIndices = numIndices * sizeof(GLubyte);

	static const int points = 0;
	static const int colors = numVertices * sizeof(vec4);
	static const int normals = numVertices * 2 * sizeof(vec4);

	void generate(const color4& insideColor, const color4& outsideColor);
	FanGen(const color4& insideColor, const color4& outsideColor);
	FanGen(const color4& color);
};

struct CubeGen {
	//8 points, 8 colors, 6 normals
	//6 faces, 2 triangles per face, 3 vertices per triangle, 3 attributes per vertex
	static const int numElements = 8 + 8 + 6;
	static const int numVertices = 4 * 6;
	static const int numIndices = 6 * 2 * 3;

	vec4 elements[numElements];
	vec4 data[3 * numVertices];
	GLubyte indices[numIndices];

	static const int sizeData = 3 * numVertices * sizeof(vec4);
	static const int sizeIndices = numIndices * sizeof(GLubyte);

	static const int points = 0;
	static const int colors = numVertices * sizeof(vec4);
	static const int normals = numVertices * 2 * sizeof(vec4);

	void generate(	const color4& color_0, const color4& color_1,
					const color4& color_2, const color4& color_3,
					const color4& color_4, const color4& color_5,
					const color4& color_6, const color4& color_7	);
	void generate(	const color4& frontColor, const color4& backColor,
					const color4& rightColor, const color4& leftColor,
					const color4& topColor,   const color4& bottomColor	);
	CubeGen(	const color4& color_0, const color4& color_1,
				const color4& color_2, const color4& color_3,
				const color4& color_4, const color4& color_5,
				const color4& color_6, const color4& color_7	);
	CubeGen(const color4& color);
};

struct TetraGen {
	//4 points, 4 colors, 4 normals
	//4 sides, 3 vertices per side, 3 attributes per vertex
	static const int numElements = 4 * 3;
	static const int numVertices = numElements;
	static const int numIndices = numElements;

	vec4 elements[numElements];
	vec4 data[3 * numVertices];
	GLubyte indices[numIndices];

	static const int sizeData = 3 * numVertices * sizeof(vec4);
	static const int sizeIndices = numIndices * sizeof(GLubyte);

	static const int points = 0;
	static const int colors = numVertices * sizeof(vec4);
	static const int normals = numVertices * 2 * sizeof(vec4);

	void generate(	const color4& color_0, const color4& color_1,
					const color4& color_2, const color4& color_3,
					bool uniform = false						);
	TetraGen(	const color4& color_0, const color4& color_1,
				const color4& color_2, const color4& color_3,
				bool uniform = false						);
	TetraGen(const color4& color);
};

struct CylinderGen {
	//unknown number of vertices
	int numElements;
	int numVertices;
	int numIndices;

	vec4* elements;
	vec4* data;
	GLubyte* indices;

	int sizeData;
	int sizeIndices;

	static const int points = 0;
	int colors;
	int normals;

	void generate(	int numEdgePoints, const color4& frontColor,
					const color4& backColor, const color4& sideColor	);
	CylinderGen(	int numEdgePoints, const color4& frontColor,
					const color4& backColor, const color4& sideColor	);
	CylinderGen(int numEdgePoints, const color4& endColor, const color4& sideColor);
	CylinderGen(int numEdgePoints, const color4& color);
	~CylinderGen();
};

struct ConeGen {
	int numElements;
	int numVertices;
	int numIndices;

	vec4* elements;
	vec4* data;
	GLubyte* indices;

	int sizeData;
	int sizeIndices;

	static const int points = 0;
	int colors;
	int normals;

	void generate(int numEdgePoints, const color4& baseColor, const color4& pointColor);
	ConeGen(int numEdgePoints, const color4& baseColor, const color4& pointColor);
	ConeGen(int numEdgePoints, const color4& color);
	~ConeGen();
};

struct TubeGen {
	int numElements;
	int numVertices;
	int numIndices;

	vec4* elements;
	vec4* data;
	GLubyte* indices;

	int sizeData;
	int sizeIndices;

	static const int points = 0;
	int colors;
	int normals;

	void generate(	const float holeR, int numEdgePoints,
					const color4& insideColor, const color4& outsideColor	);
	TubeGen(	const float holeR, int numEdgePoints,
					const color4& insideColor, const color4& outsideColor	);
	TubeGen(const float holeR, int numEdgePoints, const color4& color);
	TubeGen(int numEdgePoints, const color4& color);
	~TubeGen();
};

struct SphereGen {
	int numElements;
	int numVertices;
	int numIndices;

	vec4* elements;
	vec4* data;
	GLubyte* indices;

	int sizeData;
	int sizeIndices;

	static const int points = 0;
	int colors;
	int normals;

	void generate(	int numLatPoints, int numLongPoints, const color4& color,
					const color4& northColor, const color4& southColor, bool random	);
	SphereGen(	int numLatPoints, int numLongPoints,
				const color4& color, const color4& northColor, const color4& southColor	);
	SphereGen(int numLatPoints, int numLongPoints, const color4& color);
	~SphereGen();
};

#endif