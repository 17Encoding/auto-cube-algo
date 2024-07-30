// cubepars.cpp - Cube Parser class implementation

#include "cubepars.h"	// CubeParser
#include "facecube.h"	// FaceletCube
#include <string.h>

CubeParser::CubeParser(void)
{
	// Nothing to do here
}

CubeParser::~CubeParser()
{
	// Nothing to do here
}

// Parse the input and initialize a FaceletCube(解析输入并且初始化FaceletCube)
int CubeParser::ParseInput(int argc, char* argv[], FaceletCube& faceletCube)
{
	int face;
	int faces[6];
	int i;
	int status;

	// All six face specifiers must be present(所有六个面的说明符都必须存在)
	if (argc != 6+1) return SYNTAX_ERROR;

	// Reset the face count for all faces(重置所有面的面数)
	for (i = 0; i < 6; i++) faces[i] = 0;

	// Loop through each face specifier(遍历每个面的说明符)
	for (i = 0; i < 6; i++)
	{
		// Parse each face(解析每个面)
		if ((status = ParseFace(faceletCube, argv[i+1], face)) != VALID)
			return status;

		// Initialize this face in the FaceletCube(初始化FaceletCube上的每个面)
		faceletCube.SetFaceMarkings(face, argv[i+1]+2);
		
		// Count this face(给每个面计数)
		faces[face]++;
	}
		
	// Each face specifier must be found exactly once(每个面的说明符必须精确找到一次)
	for (i = 0; i < 6; i++)
	{
		if (faces[i] != 1)
			return INCOMPLETE_INPUT;
	}
	
	return VALID;
}

// Return the text associated with an error return code(返回与错误返回码关联的文本)
int CubeParser::ParseFace(FaceletCube& faceletCube, char* faceString, int& face)
{
	int facelet;

	// Check specifier length f:mmmmmmmmm(检查说明符长度)
	if (strlen(faceString) != 1+1+9)
		return SYNTAX_ERROR;

	// Validate face name (f)(验证面的名称)
	if ((face = faceletCube.FaceNameToOffset(faceString[0])) < 0)
		return INVALID_FACE;
	
	// Parse the colon(解析冒号)
	if (faceString[1] != ':')
		return SYNTAX_ERROR;
	
	// Check each facelet(检查每个facelet)
	for (facelet = 0; facelet < 9; facelet++)
	{
		// Only printable characters are allowed(仅允许可以被打印的字符)
		if (faceString[2+facelet] <= ' ' ||
			faceString[2+facelet] > '~')
			return SYNTAX_ERROR;
	}

	return VALID;
}

// Return the text associated with an error return code
char* CubeParser::ErrorText(unsigned int error)
{
	if (error >= NumberOfErrors) error = 0;
	return errorText[error];
}

char* CubeParser::errorText[NumberOfErrors] = {
	(char*)"",
	(char*)"Invalid face specifier",
	(char*)"Invalid marker",
	(char*)"Incomplete input",
	(char*)"Syntax error" };

