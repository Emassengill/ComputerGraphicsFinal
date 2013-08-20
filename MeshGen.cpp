#include "MeshGen.h"
#include "Definitions.h"
#include "MatMath.h"

//SquareGen
void SquareGen::generate(	const color4& color0, const color4& color1,
							const color4& color2, const color4& color3	)
{
	//set vertices
	elements[0] = point4(-1.0, -1.0, 0.0, 1.0); elements[1] = point4(1.0, -1.0, 0.0, 1.0);
	elements[2] = point4(1.0, 1.0, 0.0, 1.0); elements[3] = point4(-1.0, 1.0, 0.0, 1.0);
	//set colors
	elements[4] = color0; elements[5] = color1; elements[6] = color2; elements[7] = color3;
	//set normals
	elements[8] = MatMath::zNORM; elements[9] = -MatMath::zNORM;

	//front square
	data[0] = elements[0]; data[1] = elements[1]; data[2] = elements[2]; data[3] = elements[3];
	//back square
	data[4] = elements[3]; data[5] = elements[2]; data[6] = elements[1]; data[7] = elements[0];
	//front colors
	data[8] = elements[4]; data[9] = elements[5]; data[10] = elements[6]; data[11] = elements[7];
	//back colors
	data[12] = elements[7]; data[13] = elements[6]; data[14] = elements[5]; data[15] = elements[4];
	//front normals
	data[16] = data[17] = data[18] = data[19] = elements[8];
	//back normals
	data[20] = data[21] = data[22] = data[23] = elements[9];

	//front triangles
	indices[0] = 0; indices[1] = 1; indices[2] = 2;
	indices[3] = 0; indices[4] = 2; indices[5] = 3;
	//back triangles
	indices[6] = 4; indices[7] = 5; indices[8] = 6;
	indices[9] = 4; indices[10] = 6; indices[11] = 7;
}

SquareGen::SquareGen(	const color4& color_1, const color4& color_2,
						const color4& color_3, const color4& color_4	)
{
	generate(color_1, color_2, color_3, color_4);
}

SquareGen::SquareGen(const color4& color) { generate(color, color, color, color); }
//End SquareGen

//Begin TriangleGen
void TriangleGen::generator(const color4& color0, const color4& color1, const color4& color2) {
	//set vertices
	elements[0] = point4(-sqrt(3.0)/2.0, -0.5, 0.0, 1.0);
	elements[1] = point4(sqrt(3.0)/2.0, -0.5, 0.0, 1.0);
	elements[2] = point4(0.0, 1.0, 0.0, 1.0);
	//set colors
	elements[3] = color0; elements[4] = color1; elements[5] = color2;
	//set normals
	elements[6] = MatMath::zNORM; elements[7] = -MatMath::zNORM;

	//front triangle
	data[0] = elements[0]; data[1] = elements[1]; data[2] = elements[2];
	//back triangle
	data[3] = elements[2]; data[4] = elements[1]; data[5] = elements[0];
	//front colors
	data[6] = elements[3]; data[7] = elements[4]; data[8] = elements[5];
	//back colors
	data[9] = elements[5]; data[10] = elements[4]; data[11] = elements[3];
	//front normals
	data[12] = data[13] = data[14] = elements[6];
	//back normals
	data[15] = data[16] = data[17] = elements[7];

	//front indices
	indices[0] = 0; indices[1] = 1; indices[2] = 2;
	//back indices
	indices[3] = 3; indices[4] = 4; indices[5] = 5;
}

TriangleGen::TriangleGen(const color4& color0, const color4& color1, const color4& color2) {
	generator(color0, color1, color2);
}

TriangleGen::TriangleGen(const color4& color) { generator(color, color, color); }
//End TriangleGen

//Begin CircleGen
void CircleGen::generate(int numEdgePoints, const color4& insideColor, const color4& outsideColor) {
	const int numPoints = numEdgePoints + 1;
	const float theta = 2.0 * M_PI / numEdgePoints;

	//numPoints vertices, 2 colors, 2 normals
	//2 sides, numEdgePoints triangles per side, 3 vertices per triangle, 3 attribute per vertex
	numElements = numPoints + 2 + 2;
	numVertices = 2 * numPoints;
	numIndices = 2 * numEdgePoints * 3;

	elements = new vec4[numElements];
	data = new vec4[3 * numVertices];
	indices = new GLubyte[numIndices];

	sizeData = 3 * numVertices * sizeof(vec4);
	sizeIndices = numIndices * sizeof(GLubyte);

	colors = numVertices * sizeof(vec4);
	normals = numVertices * 2 * sizeof(vec4);

	//set vertices
	elements[0] = MatMath::ORIGIN;
	for (int i = 0; i < numEdgePoints; ++i)
		elements[i + 1] = point4(cos(i * theta), sin(i * theta), 0.0, 1.0);
	//set colors
	elements[numPoints] = insideColor;
	elements[numPoints + 1] = outsideColor;
	//set normals
	elements[numPoints + 2] = MatMath::zNORM; elements[numPoints + 3] = -MatMath::zNORM;

	//front circle
	data[0] = elements[0];
	for (int i = 0; i < numEdgePoints; ++i)
		data[i + 1] = elements[i + 1];
	//back circle
	data[numPoints] = elements[0];
	for (int i = 0; i < numEdgePoints; ++i)
		data[i + numPoints + 1] = elements[numEdgePoints - i];
	//front colors
	data[2 * numPoints] = insideColor;
	for (int i = 0; i < numEdgePoints; ++i)
		data[i + 2 * numPoints + 1] = outsideColor;
	//back colors
	data[3 * numPoints] = insideColor;
	for (int i = 0; i < numEdgePoints; ++i)
		data[i + 3 * numPoints + 1] = outsideColor;
	//front normals
	for (int i = 0; i < numPoints; ++i)
		data[i + 4 * numPoints] = MatMath::zNORM;
	//back normals
	for (int i = 0; i < numPoints; ++i)
		data[i + 5 * numPoints] = -MatMath::zNORM;


	//front indices
	for (int i = 0; i < numEdgePoints - 1; ++i) {
		indices[i * 3] = 0; indices[i * 3 + 1] = i + 1; indices[i * 3 + 2] = i + 2;
	}
	indices[(numEdgePoints - 1) * 3] = 0;
	indices[(numEdgePoints - 1) * 3 + 1] = numEdgePoints;
	indices[(numEdgePoints - 1) * 3 + 2] = 1;
	//back indices
	for (int i = 0; i < numEdgePoints - 1; ++i) {
		indices[3 * (i + numEdgePoints)] = numPoints;
		indices[3 * (i + numEdgePoints) + 1] = i + 1 + numPoints;
		indices[3 * (i + numEdgePoints) + 2] = i + 2 + numPoints;
	}
	indices[(2 * numEdgePoints - 1) * 3] = numPoints;
	indices[(2 * numEdgePoints - 1) * 3 + 1] = numEdgePoints + numPoints;
	indices[(2 * numEdgePoints - 1) * 3 + 2] = 1 + numPoints;
}

CircleGen::CircleGen(int numEdgePoints, const color4& color0, const color4& color1) {
	generate(numEdgePoints, color0, color1);
}

CircleGen::CircleGen(int numEdgePoints, const color4& color) { generate(numEdgePoints, color, color); }

CircleGen::~CircleGen() { delete[] elements; delete[] data; delete[] indices; }
//End CircleGen

//Begin RingGen
void RingGen::generate (float holeR, int numEdgePoints, const color4& insideColor, const color4& outsideColor ) {
	if (holeR > 1.0 || holeR < 0.0) throw 11;

	const float theta = 2.0 * M_PI / numEdgePoints;

	//2 * numEdgePoints points, 2 colors, 2 normals
	//2 sides, numEdgePoints quadrilaterals per side, 2 triangles per quadrilateral,
	//3 vertices per triangle, 3 attributes per vertex
	numElements = 2 * numEdgePoints + 2 + 2;
	numVertices = 2 * numEdgePoints * 2;
	numIndices = 2 * numEdgePoints * 2 * 3;

	elements = new vec4[numElements];
	data = new vec4[3 * numVertices];
	indices = new GLubyte[numIndices];

	sizeData = 3 * numVertices * sizeof(vec4);
	sizeIndices = numIndices * sizeof(GLubyte);

	colors = numVertices * sizeof(vec4);
	normals = numVertices * 2 * sizeof(vec4);

	//set vertices
	//inside
	for (int i = 0; i < numEdgePoints; ++i) {
		//inside
		elements[i] = point4(holeR * cos(i * theta), holeR * sin(i * theta), 0.0, 1.0);
		//outside
		elements[i + numEdgePoints] = point4(cos(i * theta), sin(i * theta), 0.0, 1.0);
	}
	//set colors
	int temp = 2 * numEdgePoints;
	elements[temp] = insideColor; elements[temp + 1] = outsideColor;
	//set normals
	elements[temp + 2] = MatMath::zNORM; elements[temp + 3] = -MatMath::zNORM;

	//points
	for (int i = 0; i < numEdgePoints; ++i) {
		//front
		//inside
		data[i] = elements[i];
		//outside
		data[i + numEdgePoints] = elements[i + numEdgePoints];
		//back
		//inside
		data[i + 2 * numEdgePoints] = elements[numEdgePoints - 1 - i];
		//outside
		data[i + 3 * numEdgePoints] = elements[2 * numEdgePoints - 1 - i];
	}
	//colors
	for (int i = 0; i < numEdgePoints; ++i) {
		//front
		//inside
		data[i + 4 * numEdgePoints] = insideColor;
		//outside
		data[i + 5 * numEdgePoints] = outsideColor;
		//back
		//inside
		data[i + 6 * numEdgePoints] = insideColor;
		//outside
		data[i + 7 * numEdgePoints] = outsideColor;
	}
	//normals
	for (int i = 0; i < 2 * numEdgePoints; ++i) {
		//front
		data[i + 8 * numEdgePoints] = MatMath::zNORM;
		//back
		data[i + 10 * numEdgePoints] = -MatMath::zNORM;
	}
		
	//front ring
	for (int i = 0; i < numEdgePoints - 1; ++i) {
		//first triangle
		indices[i * 6] = i + 1;
		indices[i * 6 + 1] = i;
		indices[i * 6 + 2] = i + numEdgePoints;
		//second triangle
		indices[i * 6 + 3] = i + numEdgePoints;
		indices[i * 6 + 4] = i + 1 + numEdgePoints;
		indices[i * 6 + 5] = i + 1;
	}
	//first triangle
	indices[6 * (numEdgePoints - 1)] = 0;
	indices[6 * (numEdgePoints - 1) + 1] = numEdgePoints - 1;
	indices[6 * (numEdgePoints - 1) + 2] = 2 * numEdgePoints - 1;
	//second triangle
	indices[6 * (numEdgePoints - 1) + 3] = 2 * numEdgePoints - 1;
	indices[6 * (numEdgePoints - 1) + 4] = numEdgePoints;
	indices[6 * (numEdgePoints - 1) + 5] = 0;
	//back ring
	for (int i = 0; i < numEdgePoints - 1; ++i) {
		//first triangle
		indices[(i + numEdgePoints) * 6] = i + 1 + 2 * numEdgePoints;
		indices[(i + numEdgePoints) * 6 + 1] = i + 2 * numEdgePoints;
		indices[(i + numEdgePoints) * 6 + 2] = i + numEdgePoints + 2 * numEdgePoints;
		//second triangle
		indices[(i + numEdgePoints) * 6 + 3] = i + numEdgePoints + 2 * numEdgePoints;
		indices[(i + numEdgePoints) * 6 + 4] = i + 1 + numEdgePoints + 2 * numEdgePoints;
		indices[(i + numEdgePoints) * 6 + 5] = i + 1 + 2 * numEdgePoints;
	}
	//first triangle
	indices[12 * numEdgePoints - 6] =  2 * numEdgePoints;
	indices[12 * numEdgePoints - 5] = 3 * numEdgePoints - 1;
	indices[12 * numEdgePoints - 4] = 4 * numEdgePoints - 1;
	//second triangle
	indices[12 * numEdgePoints - 3] = 4 * numEdgePoints - 1;
	indices[12 * numEdgePoints - 2] = 3 * numEdgePoints;
	indices[12 * numEdgePoints - 1] = 2 * numEdgePoints;
}

RingGen::RingGen(float holeR, int numEdgePoints, const color4& insideColor, const color4& outsideColor ) {
	generate(holeR, numEdgePoints, insideColor, outsideColor);
}

RingGen::RingGen(float holeR, int numEdgePoints, const color4& color) {
	generate(holeR, numEdgePoints, color, color);
}

RingGen::RingGen(int numEdgePoints, const color4& color) { generate(0.5, numEdgePoints, color, color); }

RingGen::~RingGen() { delete[] elements; delete[] data; delete[] indices; }
//End RingGen

//Begin FanGen
void FanGen::generate(const color4& insideColor, const color4& outsideColor) {
	const float init = M_PI / 4.0;
	const float theta = M_PI / 9.0;
	//set vertices
	elements[0] = MatMath::ORIGIN;
	for (int i = 1; i < 5; ++i)
		elements[i] = point4(cos(init + (i - 1) * theta), sin(init + (i - 1) * theta), 0.0, 1.0);
	//set colors
	elements[5] = insideColor; elements[6] = outsideColor;
	//set normals
	elements[7] = MatMath::zNORM; elements[8] = -MatMath::zNORM;


	//front points
	data[0] = elements[0];
	for (int i = 1; i < 5; ++i)
		data[i] = elements[i];
	//back points
	data[5] = elements[0];
	for (int i = 1; i < 5; ++i)
		data[i + 5] = elements[5 - i];
	//colors
	data[10] = data[15] = insideColor;
	for (int i = 1; i < 5; ++i) {
		//front
		data[i + 10] = outsideColor;
		//back
		data[i + 15] = outsideColor;
	}
	//normals
	for (int i = 0; i < 5; ++i) {
		//front
		data[20 + i] = MatMath::zNORM;
		//bacl
		data[25 + i] = -MatMath::zNORM;
	}

	//front fan
	for (int i = 0; i < 3; ++i) {
		indices[3 * i] = 0;
		indices[3 * i + 1] = i + 1;
		indices[3 * i + 2] = i + 2;
	}
	//back fan
	for (int i = 0; i < 3; ++i) {
		indices[3 * (i + 3)] = 5;
		indices[3 * (i + 3) + 1] = i + 6;
		indices[3 * (i + 3) + 2] = i + 7;
	}
}

FanGen::FanGen(const color4& insideColor, const color4& outsideColor) { generate(insideColor, outsideColor); }

FanGen::FanGen(const color4& color) { generate(color, color); }
//End FanGen

//Begin CubeGen
void CubeGen::generate(	const color4& color_0, const color4& color_1,
						const color4& color_2, const color4& color_3,
						const color4& color_4, const color4& color_5,
						const color4& color_6, const color4& color_7	)
{
	//set vertices
	elements[0] = point4(-1.0, -1.0, 1.0, 1.0); elements[1] = point4(1.0, -1.0, 1.0, 1.0);
	elements[2] = point4(1.0, 1.0, 1.0, 1.0); elements[3] = point4(-1.0, 1.0, 1.0, 1.0);
	elements[4] = point4(-1.0, 1.0, -1.0, 1.0); elements[5] = point4(1.0, 1.0, -1.0, 1.0);
	elements[6] = point4(1.0, -1.0, -1.0, 1.0); elements[7] = point4(-1.0, -1.0, -1.0, 1.0);
	//set colors
	elements[8] = color_0; elements[9] = color_1; elements[10] = color_2; elements[11] = color_3;
	elements[12] = color_4; elements[13] = color_5; elements[14] = color_6; elements[15] = color_7;
	//normals
	elements[16] = MatMath::zNORM; elements[17] = -MatMath::zNORM;
	elements[18] = MatMath::xNORM; elements[19] = -MatMath::xNORM;
	elements[20] = MatMath::yNORM; elements[21] = -MatMath::yNORM;


	//points
	//front
	data[0] = elements[0]; data[1] = elements[1]; data[2] = elements[2]; data[3] = elements[3];
	//back
	data[4] = elements[4]; data[5] = elements[5]; data[6] = elements[6]; data[7] = elements[7];
	//right
	data[8] = elements[1]; data[9] = elements[6]; data[10] = elements[5]; data[11] = elements[2];
	//left
	data[12] = elements[0]; data[13] = elements[3]; data[14] = elements[4]; data[15] = elements[7];
	//top
	data[16] = elements[3]; data[17] = elements[2]; data[18] = elements[5]; data[19] = elements[4];
	//bottom
	data[20] = elements[0]; data[21] = elements[7]; data[22] = elements[6]; data[23] = elements[1];
	//colors
	//front
	data[24] = color_0; data[25] = color_1; data[26] = color_2; data[27] = color_3;
	//back
	data[28] = color_4; data[29] = color_5; data[30] = color_6; data[31] = color_7;
	//right
	data[32] = color_1; data[33] = color_6; data[34] = color_5; data[35] = color_2;
	//left
	data[36] = color_0; data[37] = color_3; data[38] = color_4; data[39] = color_7;
	//top
	data[40] = color_3; data[41] = color_2; data[42] = color_5; data[43] = color_4;
	//botom
	data[44] = color_0; data[45] = color_7; data[46] = color_6; data[47] = color_1;
	//normals
	//front
	for (int i = 48; i < 52; ++i)
		data[i] = MatMath::zNORM;
	//back
	for (int i = 52; i < 56; ++i)
		data[i] = -MatMath::zNORM;
	//right
	for (int i = 56; i < 60; ++i)
		data[i] = MatMath::xNORM;
	//left
	for (int i = 60; i < 64; ++i)
		data[i] = -MatMath::xNORM;
	//top
	for (int i = 64; i < 68; ++i)
		data[i] = MatMath::yNORM;
	//bottom
	for (int i = 68; i < 72; ++i)
		data[i] = -MatMath::yNORM;

	//indices
	for (int i = 0; i < 6; ++i) {
		//first triangle
		indices[i * 6] = 4 * i;
		indices[i * 6 + 1] = 4 * i + 1;
		indices[i * 6 + 2] = 4 * i + 2;
		//back triangle
		indices[i * 6 + 3] = 4 * i;
		indices[i * 6 + 4] = 4 * i + 2;
		indices[i * 6 + 5] = 4 * i + 3;
	}
}

void CubeGen::generate(	const color4& frontColor, const color4& backColor,
						const color4& rightColor, const color4& leftColor,
						const color4& topColor,   const color4& bottomColor	)
{
	//set vertices
	elements[0] = point4(-1.0, -1.0, 1.0, 1.0); elements[1] = point4(1.0, -1.0, 1.0, 1.0);
	elements[2] = point4(1.0, 1.0, 1.0, 1.0); elements[3] = point4(-1.0, 1.0, 1.0, 1.0);
	elements[4] = point4(-1.0, 1.0, -1.0, 1.0); elements[5] = point4(1.0, 1.0, -1.0, 1.0);
	elements[6] = point4(1.0, -1.0, -1.0, 1.0); elements[7] = point4(-1.0, -1.0, -1.0, 1.0);
	//set colors
	elements[8] = frontColor; elements[9] = backColor;
	elements[10] = rightColor; elements[11] = leftColor;
	elements[12] = topColor; elements[13] = bottomColor;
	//normals
	elements[16] = MatMath::zNORM; elements[17] = -MatMath::zNORM;
	elements[18] = MatMath::xNORM; elements[19] = -MatMath::xNORM;
	elements[20] = MatMath::yNORM; elements[21] = -MatMath::yNORM;

	//points
	//front
	data[0] = elements[0]; data[1] = elements[1]; data[2] = elements[2]; data[3] = elements[3];
	//back
	data[4] = elements[4]; data[5] = elements[5]; data[6] = elements[6]; data[7] = elements[7];
	//right
	data[8] = elements[1]; data[9] = elements[6]; data[10] = elements[5]; data[11] = elements[2];
	//left
	data[12] = elements[0]; data[13] = elements[3]; data[14] = elements[4]; data[15] = elements[7];
	//top
	data[16] = elements[3]; data[17] = elements[2]; data[18] = elements[5]; data[19] = elements[4];
	//bottom
	data[20] = elements[0]; data[21] = elements[7]; data[22] = elements[6]; data[23] = elements[1];
	//colors
	//front
	for (int i = 24; i < 28; ++i)
		data[i] = frontColor;
	//back
	for (int i = 28; i < 32; ++i)
		data[i] = backColor;
	//right
	for (int i = 32; i < 36; ++i)
		data[i] = rightColor;
	//left
	for (int i = 36; i < 40; ++i)
		data[i] = leftColor;
	//top
	for (int i = 40; i < 44; ++i)
		data[i] = topColor;
	//bottom
	for (int i = 44; i < 48; ++i)
		data[i] = bottomColor;
	//normals
	//front
	for (int i = 48; i < 52; ++i)
		data[i] = MatMath::zNORM;
	//back
	for (int i = 52; i < 56; ++i)
		data[i] = -MatMath::zNORM;
	//right
	for (int i = 56; i < 60; ++i)
		data[i] = MatMath::xNORM;
	//left
	for (int i = 60; i < 64; ++i)
		data[i] = -MatMath::xNORM;
	//top
	for (int i = 64; i < 68; ++i)
		data[i] = MatMath::yNORM;
	//bottom
	for (int i = 68; i < 72; ++i)
		data[i] = -MatMath::yNORM;

	//indices
	for (int i = 0; i < 6; ++i) {
		//first triangle
		indices[i * 6] = 4 * i;
		indices[i * 6 + 1] = 4 * i + 1;
		indices[i * 6 + 2] = 4 * i + 2;
		//back triangle
		indices[i * 6 + 3] = 4 * i;
		indices[i * 6 + 4] = 4 * i + 2;
		indices[i * 6 + 5] = 4 * i + 3;
	}
}

CubeGen::CubeGen(	const color4& color_0, const color4& color_1,
					const color4& color_2, const color4& color_3,
					const color4& color_4, const color4& color_5,
					const color4& color_6, const color4& color_7	)
{
	generate(color_0, color_1, color_2, color_3, color_4, color_5, color_6, color_7);
}

CubeGen::CubeGen(const color4& color) { generate(color, color, color, color, color, color); }
//End CubeGen

//Begin TetraGen
void TetraGen::generate(	const color4& color_0, const color4& color_1,
							const color4& color_2, const color4& color_3,
							bool uniform									)
{
	//set vertices
	elements[0] = point4(0.0, 0.0, 1.0, 1.0);
	elements[1] = point4(0.0, 0.943, -0.3333, 1.0);
	elements[2] = point4(-0.816, -0.471, -0.3333, 1.0);
	elements[3] = point4(0.816, -0.471, -0.3333, 1.0);
	//set colors
	elements[4] = color_0; elements[5] = color_1; elements[6] = color_2; elements[7] = color_3;
	//set normals
	elements[8] = -normalize(elements[3]); elements[9] = -normalize(elements[2]);
	elements[10] = -normalize(elements[1]); elements[11] = -normalize(elements[0]);

	//points
	//left
	data[0] = elements[0]; data[1] = elements[1]; data[2] = elements[2];
	//right
	data[3] = elements[0]; data[4] = elements[3]; data[5] = elements[1];
	//bottom
	data[6] = elements[0]; data[7] = elements[2]; data[8] = elements[3];
	//back
	data[9] = elements[3]; data[10] = elements[2]; data[11] = elements[1];
	//colors
	if (!uniform) {
		//left
		data[12] = color_0; data[13] = color_1; data[14] = color_2;
		//right
		data[15] = color_0; data[16] = color_0; data[17] = color_1;
		//bottom
		data[18] = color_0; data[19] = color_2; data[20] = color_3;
		//back
		data[21] = color_3; data[22] = color_2; data[23] = color_1;
	} else {
		//left
		for (int i = 12; i < 15; ++i)
			data[i] = color_0;
		//right
		for (int i = 15; i < 18; ++i)
			data[i] = color_1;
		//bottom
		for (int i = 18; i < 21; ++i)
			data[i] = color_2;
		//back
		for (int i = 21; i < 24; ++i)
			data[i] = color_3;
	}
	//normals
	//left
	for (int i = 24; i < 27; ++i)
		data[i] = elements[8];
	//right
	for (int i = 27; i < 30; ++i)
		data[i] = elements[9];
	//bottom
	for (int i = 30; i < 33; ++i)
		data[i] = elements[10];
	//back
	for (int i = 33; i < 36; ++i)
		data[i] = elements[11];

	//indices
	for (int i = 0; i < 12; ++i)
		indices[i] = i;
}

TetraGen::TetraGen(	const color4& color_0, const color4& color_1,
					const color4& color_2, const color4& color_3,
					bool uniform									)
{
	generate(color_0, color_1, color_2, color_3, uniform);
}

TetraGen::TetraGen(const color4& color) { generate(color, color, color, color, true); }
//End TetraGen

//Begin CylinderGen
void CylinderGen::generate(	int numEdgePoints, const color4& frontColor,
							const color4& backColor, const color4& sideColor	)
{
	int numPoints = numEdgePoints + 1;

	//2 ends, numPoints points per end, 3 colors, numEdgePoints + 2 normals
	//numEdgePoints wedges, 4 triangles per wedge, 3 vertices per triangle, 3 attributes per vertex
	numElements = 2 * numPoints + 3 + numEdgePoints + 2;
	numVertices = 2 * (numPoints + numEdgePoints);
	numIndices = numEdgePoints * 4 * 3;

	elements = new vec4[numElements];
	data = new vec4[3 * numVertices];
	indices = new GLubyte[numIndices];

	sizeData = numVertices * 3 * sizeof(vec4);
	sizeIndices = numIndices * sizeof(GLubyte);

	colors = numVertices * sizeof(vec4);
	normals = numVertices * 2 * sizeof(vec4);

	const float theta = 2.0 * M_PI / numEdgePoints;

	//set vertices
	//front circle
	elements[0] = point4(0.0, 0.0, 1.0, 1.0);
	for (int i = 0; i < numEdgePoints; ++i)
		elements[i + 1] = point4(cos(i * theta), sin(i * theta), 1.0, 1.0);
	//back circle
	elements[numPoints] = point4(0.0, 0.0, -1.0, 1.0);
	int temp = numPoints + 1;
	for (int i = 0; i < numEdgePoints; ++i)
		elements[i + temp] = point4(cos(i * theta), sin(i * theta), -1.0, 1.0);
	//set colors
	elements[2 * numPoints] = frontColor;
	elements[2 * numPoints + 1] = backColor;
	elements[2 * numPoints + 2] = sideColor;
	//set normals
	elements[2 * numPoints + 3] = MatMath::zNORM;
	elements[2 * numPoints + 4] = -MatMath::zNORM;
	temp = 2 * numPoints + 5;
	for (int i = 0; i < numEdgePoints; ++i)
		elements[i + temp] = normal4(cos(i * theta), sin(i * theta), 0.0, 0.0);

	//points
	//front circle
	data[0] = elements[0];
	for (int i = 0; i < numEdgePoints; ++i)
		data[i + 1] = elements[i + 1];
	//back circle
	data[numPoints] = elements[numPoints];
	for (int i = 0; i < numEdgePoints; ++i)
		data[i + numPoints + 1] = elements[2 * numPoints - 1 - i];
	//sides
	for (int i = 0; i < numEdgePoints; ++i) {
		data[i + 2 * numPoints] = elements[i + 1];
		data[i + 2 * numPoints + numEdgePoints] = elements[i + numPoints + 1];
	}
	//colors
	//front
	for (int i = 2 * (numPoints + numEdgePoints); i < 3 * numPoints + 2 * numEdgePoints; ++i)
		data[i] = frontColor;
	//back
	for (int i = 3 * numPoints + 2 * numEdgePoints; i < 4 * numPoints + 2 * numEdgePoints; ++i)
		data[i] = backColor;
	//side
	for (int i = 4 * numPoints + 2 * numEdgePoints; i < 4 * (numPoints + numEdgePoints); ++i)
		data[i] = sideColor;
	//normals
	//front
	for (int i = 4 * (numPoints + numEdgePoints); i < 5 * numPoints + 4 * numEdgePoints; ++i)
		data[i] = MatMath::zNORM;
	//back
	for (int i = 5 * numPoints + 4 * numEdgePoints; i < 6 * numPoints + 4 * numEdgePoints; ++i)
		data[i] = -MatMath::zNORM;
	//side
	temp = 6 * numPoints + 4 * numEdgePoints;
	for (int i = 0; i < numEdgePoints; ++i)
		data[i + temp] = data[i + numEdgePoints + temp] = elements[i + 2 * numPoints + 5];

	//indices
	//front circle
	for (int i = 0; i < numEdgePoints - 1; ++i) {
		indices[i * 3] = 0; indices[i * 3 + 1] = i + 1; indices[i * 3 + 2] = i + 2;
	}
	indices[(numEdgePoints - 1) * 3] = 0;
	indices[(numEdgePoints - 1) * 3 + 1] = numPoints - 1;
	indices[(numEdgePoints - 1) * 3 + 2] = 1;
	//back circle
	for (int i = 0; i < numEdgePoints - 1; ++i) {
		indices[3 * (i + numEdgePoints)] = numPoints;
		indices[3 * (i + numEdgePoints) + 1] = i + 1 + numPoints;
		indices[3 * (i + numEdgePoints) + 2] = i + 2 + numPoints;
	}
	indices[3 * (2 * numEdgePoints - 1)] = numPoints;
	indices[3 * (2 * numEdgePoints - 1) + 1] = 2 * numPoints - 1;
	indices[3 * (2 * numEdgePoints - 1) + 2] = numPoints + 1;
	//side squares
	temp = 2 * numPoints;
	for (int i = 0; i < numEdgePoints - 1; ++i) {
		//first triangle
		indices[6 * (i + numEdgePoints)] = i + 1 + temp;
		indices[6 * (i + numEdgePoints) + 1] = i + temp;
		indices[6 * (i + numEdgePoints) + 2] = i + numEdgePoints + temp;
		//second triangle
		indices[6 * (i + numEdgePoints) + 3] = i + numEdgePoints + temp;
		indices[6 * (i + numEdgePoints) + 4] = i + numEdgePoints + 1 + temp;
		indices[6 * (i + numEdgePoints) + 5] = i + 1 + temp;
	}
	//first triangle
	indices[12 * numEdgePoints - 6] = temp;
	indices[12 * numEdgePoints - 5] = numEdgePoints - 1 + temp;
	indices[12 * numEdgePoints - 4] = 2 * numEdgePoints - 1 + temp;
	//second triangle
	indices[12 * numEdgePoints - 3] = 2 * numEdgePoints - 1 + temp;
	indices[12 * numEdgePoints - 2] = numEdgePoints + temp;
	indices[12 * numEdgePoints - 1] = temp;
}

CylinderGen::CylinderGen(	int numEdgePoints, const color4& frontColor,
									const color4& backColor, const color4& sideColor	)
{
	generate(numEdgePoints, frontColor, backColor, sideColor);
}

CylinderGen::CylinderGen(int numEdgePoints, const color4& endColor, const color4& sideColor) {
	generate(numEdgePoints, endColor, endColor, sideColor);
}

CylinderGen::CylinderGen(int numEdgePoints, const color4& color) {
	generate(numEdgePoints, color, color, color);
}

CylinderGen::~CylinderGen() { delete[] elements; delete[] data; delete[] indices; }
//End CylinderGen

//Begin ConeGen
void ConeGen::generate(int numEdgePoints, const color4& baseColor, const color4& pointColor) {
	int numPoints = numEdgePoints + 1;
	//numEdgePoints + 2 points, 2 colors, 2 * numEdgePoints + 1 normals
	//numEdgePoints wedges, 2 triangles per wedge, 3 vertices per triangle, 3 attributes per vertex
	numElements = 3 * numEdgePoints + 5;
	numVertices = numPoints + 2 * numEdgePoints;
	numIndices = numVertices * 3;

	elements = new vec4[numElements];
	data = new vec4[3 * numVertices];
	indices = new unsigned char[numIndices];

	sizeData = numVertices * 3 * sizeof(vec4);
	sizeIndices = numIndices * sizeof(GLubyte);

	colors = numVertices * sizeof(vec4);
	normals = numVertices * 2 * sizeof(vec4);

	const float theta = 2.0 * M_PI / numEdgePoints;

	//set vertices
	for (int i = 0; i < numEdgePoints; ++i)
		elements[i] = point4(cos(i * theta), sin(i * theta), 1.0, 1.0);
	elements[numEdgePoints] = point4(0.0, 0.0, 1.0, 1.0);
	elements[numPoints] = point4(0.0, 0.0, -1.0, 1.0);
	//set colors
	elements[numEdgePoints + 2] = baseColor; elements[numEdgePoints + 3] = pointColor;
	//set normals
	elements[numEdgePoints + 4] = MatMath::zNORM;
	int temp = numEdgePoints + 5;
	mat4 tempRotate;
	const normal4 tempNorm1 = normal4(0.4472, 0.0, -0.1056, 0.0);
	const normal4 tempNorm2 = MatMath::rZ(theta/2.0) * tempNorm1;
	for (int i = 0; i < numEdgePoints; ++i) {
		tempRotate = MatMath::rZ(i * theta);
		elements[i + temp] = tempRotate * tempNorm1;
		elements[i + numEdgePoints + temp] = tempRotate * tempNorm2;
	}

	//points
	//base
	for (int i = 0; i < numEdgePoints; ++i)
		data[i] = elements[i];
	data[numEdgePoints] = elements[numEdgePoints];
	//edge
	for (int i = 0; i < numEdgePoints; ++i)
		data[i + numPoints] = elements[i];
	//tip
	for (int i = 0; i < numEdgePoints; ++i)
		data[i + 2 * numEdgePoints + 1] = elements[numPoints];
	//colors
	//base
	for (int i = 3 * numEdgePoints + 1; i < 5 * numEdgePoints + 2; ++i)
		data[i] = baseColor;
	//tip
	for (int i = 5 * numEdgePoints + 2; i < 6 * numEdgePoints + 2; ++i)
		data[i] = pointColor;
	//normals
	//base
	for (int i = 6 * numEdgePoints + 2; i < 7 * numEdgePoints + 3; ++i)
		data[i] = MatMath::zNORM;
	//edge
	temp = 7 * numEdgePoints + 3;
	for (int i = 0; i < numEdgePoints; ++i)
		data[i + temp] = elements[i + numEdgePoints + 5];
	//tip
	temp = 8 * numEdgePoints + 3;
	for (int i = 0; i < numEdgePoints; ++i)
		data[i + temp] = elements[i + 2 * numEdgePoints + 5];

	//indices
	//base
	for (int i = 0; i < numEdgePoints - 1; ++i) {
		indices[3 * i] = numEdgePoints; indices[3 * i + 1] = i; indices[3 * i + 2] = i + 1;
	}
	indices[3 * numEdgePoints - 3] = numEdgePoints;
	indices[3 * numEdgePoints - 2] = numEdgePoints - 1;
	indices[3 * numEdgePoints - 1] = 0;
	//cone
	for (int i = 0; i < numEdgePoints - 1; ++i) {
		indices[3 * (i + numEdgePoints)] = i + 1 + numPoints;
		indices[3 * (i + numEdgePoints) + 1] = i + numPoints;
		indices[3 * (i + numEdgePoints) + 2] = i + numPoints + numEdgePoints;
	}
	indices[6 * numEdgePoints - 3] = numPoints;
	indices[6 * numEdgePoints - 2] = numPoints + numEdgePoints - 1;
	indices[6 * numEdgePoints - 1] = 3 * numEdgePoints;
}

ConeGen::ConeGen(int numEdgePoints, const color4& baseColor, const color4& pointColor) {
	generate(numEdgePoints, baseColor, pointColor);
}

ConeGen::ConeGen(int numEdgePoints, const color4& color) { generate(numEdgePoints, color, color); }

ConeGen::~ConeGen() { delete[] elements; delete[] data; delete[] indices; }
//End ConeGen

//Begin TubeGen
void TubeGen::generate(	float holeR, int numEdgePoints,
						const color4& insideColor, const color4& outsideColor	)
{
	if (holeR > 1.0 || holeR < 0.0) throw 11;

	const float theta = 2.0 * M_PI / numEdgePoints;

	//2 sides, 2 * numEdgePoints points, 2 colors, 2 * numEdgePoints + 2 normals
	//numEdgePoints wedges, 4 sides per wedge, 1 quadrilateral per side,
	//2 triangles per quadrilateral, 3 vertices per triangle, 3 attributes per vertex
	numElements = 2 * 2 * numEdgePoints + 2 + 2 * numEdgePoints + 2;
	numVertices = 2 * numEdgePoints * 4;
	numIndices = numEdgePoints * 4 * 2 * 3;

	elements = new vec4[numElements];
	data = new vec4[numVertices * 3];
	indices = new GLubyte[numIndices];

	sizeData = numVertices * 3 * sizeof(vec4);
	sizeIndices = numIndices * sizeof(GLubyte);

	colors = numVertices * sizeof(vec4);
	normals = numVertices * 2 * sizeof(vec4);

	//set vertices
	//front
	//inside
	for (int i = 0; i < numEdgePoints; ++i)
		elements[i] = point4(holeR * cos(i * theta), holeR * sin(i * theta), 1.0, 1.0);
	//outside
	for (int i = 0; i < numEdgePoints; ++i)
		elements[i + numEdgePoints] = point4(cos(i * theta), sin(i * theta), 1.0, 1.0);
	//back
	//outside
	int temp = 2 * numEdgePoints;
	for (int i = 0; i < numEdgePoints; ++i)
		elements[i + temp] = point4(cos(i * theta), sin(i * theta), -1.0, 1.0);
	//inside
	temp = 3 * numEdgePoints;
	for (int i = 0; i < numEdgePoints; ++i)
		elements[i + temp] = point4(holeR * cos(i * theta), holeR * sin(i * theta), -1.0, 1.0);
	//set colors
	elements[4 * numEdgePoints] = insideColor; elements[4 * numEdgePoints + 1] = outsideColor;
	//set normals
	elements[4 * numEdgePoints + 2] = MatMath::zNORM; elements[4 * numEdgePoints + 3] = -MatMath::zNORM;
	temp = 4 * (numEdgePoints + 1);
	for (int i = 0; i < numEdgePoints; ++i) {
		elements[i + temp] = normal4(cos(i * theta), sin(i * theta), 0.0, 0.0);
		elements[i + numEdgePoints + temp] = -1.0 * elements[i + temp];
	}

	//points
	//front
	for (int i = 0; i < 2 * numEdgePoints; ++i)
		data[i] = elements[i];
	//back
	for (int i = 2 * numEdgePoints; i < 4 * numEdgePoints; ++i)
		data[i] = elements[i];
	//inside
	temp = 4 * numEdgePoints;
	for (int i = 0; i < numEdgePoints; ++i) {
		data[i + temp] = elements[i + 3 * numEdgePoints];
		data[i + numEdgePoints + temp] = elements[i];
	}
	//outside
	temp = 6 * numEdgePoints;
	for (int i = 0; i < numEdgePoints; ++i) {
		data[i + temp] = elements[i + numEdgePoints];
		data[i + numEdgePoints + temp] = elements[i + 2 * numEdgePoints];
	}
	//colors
	//front
	temp = 8 * numEdgePoints;
	for (int i = 0; i < numEdgePoints; ++i) {
		data[i + temp] = insideColor;
		data[i + numEdgePoints + temp] = outsideColor;
	}
	//back
	temp = 10 * numEdgePoints;
	for (int i = 0; i < numEdgePoints; ++i) {
		data[i + temp] = outsideColor;
		data[i + numEdgePoints + temp] = insideColor;
	}
	//inside and outside
	for (int i = 12 * numEdgePoints; i < 14 * numEdgePoints; ++i) {
		//inside
		data[i] = insideColor;
		//outside
		data[i + 2 * numEdgePoints] = outsideColor;
	}
	//normals
	//front and back
	for (int i = 16 * numEdgePoints; i < 18 * numEdgePoints; ++i) {
		data[i] = MatMath::zNORM;
		data[i + 2 * numEdgePoints] = -MatMath::zNORM;
	}
	//inside
	temp = 20 * numEdgePoints;
	for (int i = 0; i < numEdgePoints; ++i)
		data[i + temp] = data[i + numEdgePoints + temp] = elements[i + 5 * numEdgePoints + 4];
	//outside
	temp = 22 * numEdgePoints;
	for (int i =0; i < numEdgePoints; ++i)
		data[i + temp] = data[i + numEdgePoints + temp] = elements[i + 4 * (numEdgePoints + 1)];

	//indices
	//front
	for (int i = 0; i < numEdgePoints - 1; ++i) {
		//first triangle
		indices[i * 6] = i + 1;
		indices[i * 6 + 1] = i;
		indices[i * 6 + 2] = i + numEdgePoints;
		//second triangle
		indices[i * 6 + 3] = i + numEdgePoints;
		indices[i * 6 + 4] = i + 1 + numEdgePoints;
		indices[i * 6 + 5] = i + 1;
	}
	//first triangle
	indices[numEdgePoints * 6 - 6] = 0;
	indices[numEdgePoints * 6 - 5] = numEdgePoints - 1;
	indices[numEdgePoints * 6 - 4] = 2 * numEdgePoints - 1;
	//second triangle
	indices[numEdgePoints * 6 - 3] = 2 * numEdgePoints - 1;
	indices[numEdgePoints * 6 - 2] = numEdgePoints;
	indices[numEdgePoints * 6 - 1] = 0;
	//back
	temp = 2 * numEdgePoints;
	for (int i = 0; i < numEdgePoints - 1; ++i) {
		//first triangle
		indices[6 * (i + numEdgePoints)] = i + 1 + temp;
		indices[6 * (i + numEdgePoints) + 1] = i + temp;
		indices[6 * (i + numEdgePoints) + 2] = i + numEdgePoints + temp;
		//second triangle
		indices[6 * (i + numEdgePoints) + 3] = i + numEdgePoints + temp;
		indices[6 * (i + numEdgePoints) + 4] = i + 1 + numEdgePoints + temp;
		indices[6 * (i + numEdgePoints) + 5] = i + 1 + temp;
	}
	//first triangle
	indices[12 * numEdgePoints - 6] = temp;
	indices[12 * numEdgePoints - 5] = numEdgePoints - 1 + temp;
	indices[12 * numEdgePoints - 4] = 2 * numEdgePoints - 1 + temp;
	//second triangle
	indices[12 * numEdgePoints - 3] = 2 * numEdgePoints - 1 + temp;
	indices[12 * numEdgePoints - 2] = numEdgePoints + temp;
	indices[12 * numEdgePoints - 1] = temp;
	//inside
	temp = 4 * numEdgePoints;
	for (int i = 0; i < numEdgePoints - 1; ++i) {
		//first triangle
		indices[6 * (i + 2 * numEdgePoints)] = i + 1 + temp;
		indices[6 * (i + 2 * numEdgePoints) + 1] = i + temp;
		indices[6 * (i + 2 * numEdgePoints) + 2] = i + numEdgePoints + temp;
		//second triangle
		indices[6 * (i + 2 * numEdgePoints) + 3] = i + numEdgePoints + temp;
		indices[6 * (i + 2 * numEdgePoints) + 4] = i + 1 + numEdgePoints + temp;
		indices[6 * (i + 2 * numEdgePoints) + 5] = i + 1 + temp;
	}
	//first triangle
	indices[18 * numEdgePoints - 6] = temp;
	indices[18 * numEdgePoints - 5] = numEdgePoints - 1 + temp;
	indices[18 * numEdgePoints - 4] = 2 * numEdgePoints - 1 + temp;
	//second triangle
	indices[18 * numEdgePoints - 3] = 2 * numEdgePoints - 1 + temp;
	indices[18 * numEdgePoints - 2] = numEdgePoints + temp;
	indices[18 * numEdgePoints - 1] = temp;
	//outside
	temp = 6 * numEdgePoints;
	for (int i = 0; i < numEdgePoints - 1; ++i) {
		//first triangle
		indices[6 * (i + 3 * numEdgePoints)] = i + 1 + temp;
		indices[6 * (i + 3 * numEdgePoints) + 1] = i + temp;
		indices[6 * (i + 3 * numEdgePoints) + 2] = i + numEdgePoints + temp;
		//second triangle
		indices[6 * (i + 3 * numEdgePoints) + 3] = i + numEdgePoints + temp;
		indices[6 * (i + 3 * numEdgePoints) + 4] = i + 1 + numEdgePoints + temp;
		indices[6 * (i + 3 * numEdgePoints) + 5] = i + 1 + temp;
	}
	//first triangle
	indices[24 * numEdgePoints - 6] = temp;
	indices[24 * numEdgePoints - 5] = numEdgePoints - 1 + temp;
	indices[24 * numEdgePoints - 4] = 2 * numEdgePoints - 1 + temp;
	//second triangle
	indices[24 * numEdgePoints - 3] = 2 * numEdgePoints - 1 + temp;
	indices[24 * numEdgePoints - 2] = numEdgePoints + temp;
	indices[24 * numEdgePoints - 1] = temp;
}

TubeGen::TubeGen(	float holeR, int numEdgePoints,
					const color4& insideColor, const color4& outsideColor	) {
	generate(holeR, numEdgePoints, insideColor, outsideColor);
}

TubeGen::TubeGen(float holeR, int numEdgePoints, const color4& color) {
	generate(holeR, numEdgePoints, color, color);
}

TubeGen::TubeGen(int numEdgePoints, const color4& color) {
	generate(0.5, numEdgePoints, color, color);
}

TubeGen::~TubeGen() { delete[] elements; delete[] data; delete[] indices; }
//End TubeGen

//Begin SphereGen
void SphereGen::generate(	int numLatPoints, int numLongPoints, const color4& color,
							const color4& northColor, const color4& southColor, const bool random	)
{
	if (numLongPoints < 2) throw 25;

	//numLatPoints * numLongPoints + 2 points, 1 color, numLatPoints * numLongPoints + 2 normals
	//numLatPoints * (numLongPoints - 1) squares, 2 triangles per square, plus 2 cones,
	//numLatPoints triangles per cone, 3 vertices per triangle, 3 attributes per vertex
	numElements = (numLatPoints * numLongPoints + 2) * 2 + 1;
	numVertices = numLatPoints * numLongPoints + 2;
	numIndices = (numLatPoints * (numLongPoints - 1) * 2 + 2 * numLatPoints) * 3;

	elements = nullptr;
	data = new vec4[3 * numVertices];
	indices = new GLubyte[numIndices];

	sizeData = 3 * numVertices * sizeof(vec4);
	sizeIndices = numIndices * sizeof(GLubyte);

	colors = numVertices * sizeof(vec4);
	normals = 2 * numVertices * sizeof(vec4);

	const float theta = 2.0 * M_PI / numLatPoints;
	const float rho = M_PI / (numLongPoints + 1.0);

	//points
	//center
	for (int i = 0; i < numLongPoints; ++i)
		for (int j = 0; j < numLatPoints; ++j)
			data[j + i * numLatPoints] = point4(sin((i + 1) * rho) * cos(j * theta),
												sin((i + 1) * rho) * sin(j * theta),
												cos((i + 1) * rho), 1.0				);
	//top and bottom poles
	data[numLatPoints * numLongPoints] = point4(0.0, 0.0, 1.0, 1.0);
	data[numLatPoints * numLongPoints + 1] = point4(0.0, 0.0, -1.0, 1.0);
	//colors
	if (!random) {
		//center
		for (int i = numLatPoints * numLongPoints + 2; i < 2 * (numLatPoints * numLongPoints + 1); ++i)
			data[i] = color;
		//top pole
		data[2 * (numLatPoints * numLongPoints + 1)] = northColor;
		//bottom pole
		data[2 * (numLatPoints * numLongPoints + 1) + 1] = southColor;
	} else {
		for (int i = numLatPoints * numLongPoints + 2; i < 2 * (numLatPoints * numLongPoints + 2); ++i) {
			float red = (float) rand() / RAND_MAX;
			float green = (float) rand() / RAND_MAX;
			float blue = (float) rand() / RAND_MAX;
			data[i] = color4(red, green, blue, 1.0);
		}
	}
	//normals
	int temp = 2 * (numLatPoints * numLongPoints + 2);
	for (int i = 0; i < numLatPoints * numLongPoints + 2; ++i) {
		data[i + temp] = data[i]; data[i + temp].w = 0.0;
	}

	//indices
	//center
	for (int i = 0; i < numLongPoints - 1; ++i) {
		for (int j = 0; j < numLatPoints - 1; ++j) {
			//first triangle
			indices[6 * (j + i * numLatPoints)] = j + i * numLatPoints + 1;
			indices[6 * (j + i * numLatPoints) + 1] = j + i * numLatPoints;
			indices[6 * (j + i * numLatPoints) + 2] = j + (i + 1) * numLatPoints;
			//second triangle
			indices[6 * (j + i * numLatPoints) + 3] = j + (i + 1) * numLatPoints;
			indices[6 * (j + i * numLatPoints) + 4] = j + (i + 1) * numLatPoints + 1;
			indices[6 * (j + i * numLatPoints) + 5] = j + i * numLatPoints + 1;
		}
		//first triangle
		indices[6 * (1 + i) * numLatPoints - 6] = i * numLatPoints;
		indices[6 * (1 + i) * numLatPoints - 5] = (i + 1) * numLatPoints - 1;
		indices[6 * (1 + i) * numLatPoints - 4] = (i + 2) * numLatPoints - 1;
		//second triangle
		indices[6 * (1 + i) * numLatPoints - 3] = (i + 2) * numLatPoints - 1;
		indices[6 * (1 + i) * numLatPoints - 2] = (i + 1) * numLatPoints;
		indices[6 * (1 + i) * numLatPoints - 1] = i * numLatPoints;
	}
	//top cone
	temp = 6 * (numLongPoints - 1) * numLatPoints;
	for (int i = 0; i < numLatPoints - 1; ++i) {
		indices[3 * i + temp] = numLatPoints * numLongPoints;
		indices[3 * i + temp + 1] = i;
		indices[3 * i + temp + 2] = i + 1;
	}
	indices[3 * numLatPoints + temp - 3] = numLatPoints * numLongPoints;
	indices[3 * numLatPoints + temp - 2] = numLatPoints - 1;
	indices[3 * numLatPoints + temp - 1] = 0;
	//bottom cone
	temp = 3 * (numLatPoints + 2 * (numLongPoints - 1) * numLatPoints);
	for (int i = 0; i < numLatPoints - 1; ++i) {
		indices[3 * i + temp] = (numLongPoints - 1) * numLatPoints + i + 1;
		indices[3 * i + temp + 1] = (numLongPoints - 1) * numLatPoints + i;
		indices[3 * i + temp + 2] = numLatPoints * numLongPoints + 1;
	}
	indices[3 * numLatPoints + temp - 3] = (numLongPoints - 1) * numLatPoints;
	indices[3 * numLatPoints + temp - 2] = numLongPoints * numLatPoints - 1;
	indices[3 * numLatPoints + temp - 1] = numLongPoints * numLatPoints + 1;
}

SphereGen::SphereGen(	int numLatPoints, int numLongPoints,
						const color4& color, const color4& northColor, const color4& southColor	)
{
	generate(numLatPoints, numLongPoints, color, northColor, southColor, false);
}

SphereGen::SphereGen(int numLatPoints, int numLongPoints, const color4& color) {
	generate(numLatPoints, numLongPoints, color, color, color, false);
}

SphereGen::~SphereGen() { delete[] data; delete[] indices; }
//End SphereGen