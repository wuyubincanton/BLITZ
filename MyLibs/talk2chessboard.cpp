/*
* Copyright 2018 Wenbin Yang <bysin7@gmail.com>
* This file is part of BLITZ (Behavioral Learning In The Zebrafish),
* which is adapted from MindControl (Andrew Leifer et al <leifer@fas.harvard.edu>
* Leifer, A.M., Fang-Yen, C., Gershow, M., Alkema, M., and Samuel A. D.T.,
* 	"Optogenetic manipulation of neural activity with high spatial resolution in
*	freely moving Caenorhabditis elegans," Nature Methods, Submitted (2010).
*
* BLITZ is a free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the license, or
* (at your option) any later version.
*
* Filename: talk2screen.h
* Abstract: this file contains all function definitions
*			used to present visual stimulus to fish
*
* A great portion of code was adapted from learnopengl.com, which is a great
* website to learn OpenGL
*
* Current Version: 2.1
* Author: Wenbin Yang <bysin7@gmail.com>
* Modified on: May 12, 2018

* Replaced Version: 2.0
* Author: Wenbin Yang <bysin7@gmail.com>
* Created on: Jan. 1, 2018
*/


// Include user-defined libraries
#include "talk2chessboard.h"
#include <algorithm>
#include <string>


using namespace std;


bool ChessData::initialize(string imgName)
{
	initVertices();
	loadTextureIntoBuffers(imgName);
	return true;
}

void ChessData::initVertices()
{
	const unsigned int indices[2 * 3] =
	{
		0, 1, 2,
		0, 3, 2
	};

	float vertices[32] =
	{	// positions (0-2)      // colors (3-5)         // texture coordinates (6-7)
		 1, -1, 0.0f,  0.0f, 0.0f, 0.0f,  0.0f, 0.0f, // top left
		 1,  1, 0.0f,  0.0f, 0.0f, 0.0f,  0.0f, 1, // bottom left
		-1,  1, 0.0f,  0.0f, 0.0f, 0.0f,  1,    1, // bottom right
		-1, -1, 0.0f,  0.0f, 0.0f, 0.0f,  1,    0.0f // top right
	};

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// texture coordinate attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
}

void ChessData::updatePattern()
{
	shader.use();
	shader.setInt("patternIdx", pIdx);
}

// ///This function is not needed anymore
//bool AreaData::initialize(vector<int> yDivideVec, string imgName)
//{
//	for (int i = 0; i < numPatches; i++)
//	{
//		vector<float> patchRect = rect;
//		switch (i) {
//		case 0:
//
//			break;// do nothing
//		case 1:
//		{
//			patchRect[0] -= patchRect[2] / 2; // minus half area width
//			break;
//		}
//		case 2:
//		{
//			patchRect[1] += patchRect[3] / 2;
//			break;
//		}
//		case 3:
//		{
//			patchRect[0] -= patchRect[2] / 2;
//			patchRect[1] += patchRect[3] / 2;
//			break;
//		}
//		default:;
//		}
//		patchRect[2] /= 2; // the width of patch is half of area width
//		patchRect[3] /= 2; // the height of patch is half of area width
//		PatchData patch(patchRect, yDivideVec[i]);
//		patch.initialize();
//		allPatches.push_back(patch);
//	}
//	loadTextureIntoBuffers(imgName);
//	return true;
//}
//
// ///* Reverse the position of the CS pattern */
//void AreaData::reverseAllPatches() {
//	for (int j = 0; j < numPatches; j++)
//	{
//		allPatches[j].pIdx = !allPatches[j].pIdx;
//		allPatches[j].updatePattern();
//	}
//}


/* Load a texture for one area */
bool ChessData::loadTextureIntoBuffers(string imgName)
{
	glGenTextures(1, &texture0);
	glBindTexture(GL_TEXTURE_2D, texture0);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	// load image, create texture and generate mipmaps
	int width, height, nrChannels;
	//stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis
	unsigned char *data = stbi_load(imgName.c_str(), &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
		return false;
	}
	stbi_image_free(data);
	return true;
}

void ChessData::renderTexture()
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture0);
	shader.use();
	glUniform1i(glGetUniformLocation(shader.ID, "texture0"), 0);
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, 2 * 3, GL_UNSIGNED_INT, 0);

}

// ///
//void ScreenData::updatePattern()
//{
//	for (int i = 0; i < allAreas.size(); i++)
//	{
//		AreaData area = allAreas[i];
//		for (int j = 0; j < area.numPatches; j++)
//		{
//			area.allPatches[j].updatePattern();
//		}
//	}
//}

///* update pattern for specific area */
//void ScreenData::updatePattern(int cIdx)
//{
//
//	AreaData area = allAreas[cIdx];
//	for (int j = 0; j < area.numPatches; j++)
//	{
//		area.allPatches[j].updatePattern();
//	}
//
//}

//TODO: update the pattern first.
void ChessData::updatePatternInTest(int sElapsed) {
	int testInterval = 30; // seconds, the interval in test is fixed
	if (sElapsed > lastScreenPatternUpdate + testInterval)
	{
		cout << "Update pattern during test" << endl;
		lastScreenPatternUpdate = sElapsed;
		pIdx = !pIdx;
		updatePattern();
	}
}

void ChessData::updatePatternInBaseline(int sElapsed) {
	if (sElapsed > lastScreenPatternUpdate + baselineInterval)
	{
		cout << "Update pattern during baseline session " << endl;
		lastScreenPatternUpdate = sElapsed;
		// uniformly choose a time from 15s to 45s
		baselineInterval = rand() % 30 + 15;
		pIdx = !pIdx;
		updatePattern();
	}
}

void ChessData::updatePatternInBlackout() {
	pIdx = 2;
	updatePattern();
}

bool BoardData::initialize( std::string imgName)
{
	cout << "Initializing the projector screen .. " << endl;
	/* GLFW initialize and configure */
	if (!init_glfw_window())
		return false;

	/* glad: load all OpenGL function pointers */
	if (!init_glad())
		return false;

	Chess.reserve(1);
	ChessData chess({0.5f, -0.5f, 1.0f, 2.0f});
	chess.initialize(imgName);
	Chess.push_back(chess);
	// Initialize all areas
	// /// This part has been commented
	/*numAreas = imgNames.size();
	allAreas.reserve(numAreas);
	for (int i = 0; i < numAreas; i++)
	{
		AreaData area(allAreaPos[i], patchesOfAreas[i]);
		area.initialize(yPatternDivs[i], imgNames[i]);
		allAreas.push_back(area);
	}*/
	
	cout << "Screen initialization succeeded." << endl << endl;
	return true;
}

/* Initialize GLFW window */
bool BoardData::init_glfw_window()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_AUTO_ICONIFY, GL_FALSE);
	int count;
	monitors = glfwGetMonitors(&count);
	mode = glfwGetVideoMode(monitors[1]);
	// glfw window creation
	// --------------------
	window = glfwCreateWindow(mode->width, mode->height, "VR", monitors[1], NULL);
	cout << "Screen width: " << mode->width << endl;
	cout << "Screen height: " << mode->height << endl;
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return false;
	}
	glfwMakeContextCurrent(window);
	return true;
}

/* Initiate glad for using OpenGL functions */
bool BoardData::init_glad()
{
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		cout << "Failed to initialize GLAD" << endl;
		return false;
	}
	return true;
}

/* Render designed pattern on the screen */
void BoardData::renderTexture()
{
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	Chess[0].renderTexture();
	// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
	glfwSwapBuffers(window);
	glfwPollEvents();// DO NOT DELETE!!! It processes all pending events, such as mouse move 

}


