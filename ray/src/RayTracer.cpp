// The main ray tracer.

#pragma warning(disable : 4786)

#include "RayTracer.h"
#include "scene/light.h"
#include "scene/material.h"
#include "scene/ray.h"

#include "parser/JsonParser.h"
#include "parser/Parser.h"
#include "parser/Tokenizer.h"
#include <json.hpp>

#include "ui/TraceUI.h"
#include <algorithm>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtx/io.hpp>
#include <string.h> // for memset

#include <fstream>
#include <iostream>

using namespace std;
extern TraceUI *traceUI;

// Use this variable to decide if you want to print out debugging messages. Gets
// set in the "trace single ray" mode in TraceGLWindow, for example.
bool debugMode = true;

// Done?
// Trace a top-level ray through pixel(i,j), i.e. normalized window coordinates
// (x,y), through the projection plane, and out into the scene. All we do is
// enter the main ray-tracing method, getting things started by plugging in an
// initial ray weight of (0.0,0.0,0.0) and an initial recursion depth of 0.
glm::dvec3 RayTracer::trace(double x, double y)
{
	// Clear out the ray cache in the scene for debugging purposes,
	if (TraceUI::m_debug)
	{
		scene->clearIntersectCache();
	}

	ray r(glm::dvec3(0, 0, 0), glm::dvec3(0, 0, 0), glm::dvec3(1, 1, 1),
		  ray::VISIBILITY);
	scene->getCamera().rayThrough(x, y, r);
	double dummy;
	glm::dvec3 ret =
		traceRay(r, glm::dvec3(1.0, 1.0, 1.0), traceUI->getDepth(), dummy);
	ret = glm::clamp(ret, 0.0, 1.0);
	return ret;
}

// Done.
glm::dvec3 RayTracer::tracePixel(int i, int j)
{
	glm::dvec3 col(0, 0, 0);

	if (!sceneLoaded())
		return col;

	double x = double(i) / double(buffer_width);
	double y = double(j) / double(buffer_height);

	unsigned char *pixel = buffer.data() + (i + j * buffer_width) * 3;
	col = trace(x, y);

	pixel[0] = (int)(255.0 * col[0]);
	pixel[1] = (int)(255.0 * col[1]);
	pixel[2] = (int)(255.0 * col[2]);
	return col;
}

#define VERBOSE 0

// Do recursive ray tracing! You'll want to insert a lot of code here (or places
// called from here) to handle reflection, refraction, etc etc.
glm::dvec3 RayTracer::traceRay(ray &r, const glm::dvec3 &thresh, int depth,
							   double &t)
{
	isect i;
	glm::dvec3 colorC;
#if VERBOSE
	std::cerr << "== current depth: " << depth << std::endl;
#endif
<<<<<<< Updated upstream
//	if (depth < 0)
//		return glm::dvec3(0, 0, 0);
	// Added condition: Depth must be >= 0.
	if (depth >= 0 && scene->intersect(r, i))
=======
	// Recursive base case.
	if (depth < 0)
		return glm::dvec3(0, 0, 0);
	// Added condition: Depth must be >= 0.
	if (scene->intersect(r, i))
>>>>>>> Stashed changes
	{
		// An intersection occurred!  We've got work to do. For now, this code gets
		// the material for the surface that was intersected, and asks that material
		// to provide a color for the ray.

		// This is a great place to insert code for recursive ray tracing. Instead
		// of just returning the result of shade(), add some more steps: add in the
		// contributions from reflected and refracted rays.

		// MY CODE HERE:
		const Material &m = i.getMaterial(); // 1. get the material.
		colorC = m.shade(scene.get(), r, i);
<<<<<<< Updated upstream
//		glm::dvec3 normal = i.getN();
//		// Inside the mesh if the ray is facing the opposite direction as the
//		// normal of the mesh.
//		bool insideMesh = glm::dot(1, normal) < 0;
//		// Reflect if the matrial is reflective.
//		bool reflective = m.Refl();
//		if (reflective)
//		{
//			glm::dvec3 reflectRayDir = glm::dot(1, n) * 2 * n - 1;
//			glm::dvec3 reflectRayPos = r.at(i) + n;
//			ray reflectRay = ray(reflectRayPos, reflectRayDir, glm::dvec3(1, 1, 1), ray::REFLECTION);
//			colorC += traceRay(reflectRay, thresh, depth - 1, 0);
//		}
//		// Refract if the material is transparent.
//		bool refractive = m.Trans();
=======
		glm::dvec3 normal = i.getN();
		bool insideMesh = glm::dot(-r.getDirection(), normal) <= 0;
		if (m.Refl() && !insideMesh)
		{
			glm::dvec3 reflDir = 2 * glm::dot(-r.getDirection(), normal) * normal + r.getDirection();
			glm::dvec3 reflPos = r.at(i) + RAY_EPSILON * normal;
			ray reflRay = ray(reflPos, reflDir, glm::dvec3(1.0, 1.0, 1.0), ray::REFLECTION);
			double x = 0.0; // Needs an explicit var.
			glm::dvec3 reflResult = m.kr(i) * traceRay(reflRay, thresh, depth - 1, x);
			colorC += reflResult;
		}
>>>>>>> Stashed changes
	}
	else
	{
		// No intersection. This ray travels to infinity, so we color
		// it according to the background color, which in this (simple)
		// case is just black.
		//
		// FIXME: Add CubeMap support here.
		// TIPS: CubeMap object can be fetched from
		// traceUI->getCubeMap();
		//       Check traceUI->cubeMap() to see if cubeMap is loaded
		//       and enabled.

		// MY CODE HERE:
		if (traceUI->getCubeMap())
		{
			// There is a cube map.
		}
		else
		{
			// THere is no cube map, return black.
			colorC = glm::dvec3(0.0, 0.0, 0.0);
		}
	}
#if VERBOSE
	std::cerr << "== depth: " << depth + 1 << " done, returning: " << colorC
			  << std::endl;
#endif
	return colorC;
}

// Ignore for now.
RayTracer::RayTracer()
	: scene(nullptr), buffer(0), thresh(0), buffer_width(0), buffer_height(0),
	  m_bBufferReady(false)
{
}

// Ignore for now.
RayTracer::~RayTracer() {}

void RayTracer::getBuffer(unsigned char *&buf, int &w, int &h)
{
	buf = buffer.data();
	w = buffer_width;
	h = buffer_height;
}

// Done.
double RayTracer::aspectRatio()
{
	return sceneLoaded() ? scene->getCamera().getAspectRatio() : 1;
}

// Done.
bool RayTracer::loadScene(const char *fn)
{
	ifstream ifs(fn);
	if (!ifs)
	{
		string msg("Error: couldn't read scene file ");
		msg.append(fn);
		traceUI->alert(msg);
		return false;
	}

	// Check if fn ends in '.ray'
	bool isRay = false;
	const char *ext = strrchr(fn, '.');
	if (ext && !strcmp(ext, ".ray"))
		isRay = true;

	// Strip off filename, leaving only the path:
	string path(fn);
	if (path.find_last_of("\\/") == string::npos)
		path = ".";
	else
		path = path.substr(0, path.find_last_of("\\/"));

	if (isRay)
	{
		// .ray Parsing Path
		// Call this with 'true' for debug output from the tokenizer
		Tokenizer tokenizer(ifs, false);
		Parser parser(tokenizer, path);
		try
		{
			scene.reset(parser.parseScene());
		}
		catch (SyntaxErrorException &pe)
		{
			traceUI->alert(pe.formattedMessage());
			return false;
		}
		catch (ParserException &pe)
		{
			string msg("Parser: fatal exception ");
			msg.append(pe.message());
			traceUI->alert(msg);
			return false;
		}
		catch (TextureMapException e)
		{
			string msg("Texture mapping exception: ");
			msg.append(e.message());
			traceUI->alert(msg);
			return false;
		}
	}
	else
	{
		// JSON Parsing Path
		try
		{
			JsonParser parser(path, ifs);
			scene.reset(parser.parseScene());
		}
		catch (ParserException &pe)
		{
			string msg("Parser: fatal exception ");
			msg.append(pe.message());
			traceUI->alert(msg);
			return false;
		}
		catch (const json::exception &je)
		{
			string msg("Invalid JSON encountered ");
			msg.append(je.what());
			traceUI->alert(msg);
			return false;
		}
	}

	if (!sceneLoaded())
		return false;

	return true;
}

void RayTracer::traceSetup(int w, int h)
{
	size_t newBufferSize = w * h * 3;
	if (newBufferSize != buffer.size())
	{
		bufferSize = newBufferSize;
		buffer.resize(bufferSize);
	}
	buffer_width = w;
	buffer_height = h;
	std::fill(buffer.begin(), buffer.end(), 0);
	m_bBufferReady = true;

	/*
	 * Sync with TraceUI
	 */

	threads = traceUI->getThreads();
	block_size = traceUI->getBlockSize();
	thresh = traceUI->getThreshold();
	samples = traceUI->getSuperSamples();
	aaThresh = traceUI->getAaThreshold();

	// YOUR CODE HERE
	// FIXME: Additional initializations
}

/*
 * RayTracer::traceImage
 *
 *	Trace the image and store the pixel data in RayTracer::buffer.
 *
 *	Arguments:
 *		w:	width of the image buffer
 *		h:	height of the image buffer
 *
 */
void RayTracer::traceImage(int w, int h)
{
	// Always call traceSetup before rendering anything.
	traceSetup(w, h);

	// YOUR CODE HERE
	// FIXME: Start one or more threads for ray tracing
	//
	// TIPS: Ideally, the traceImage should be executed asynchronously,
	//       i.e. returns IMMEDIATELY after working threads are launched.
	//
	//       An asynchronous traceImage lets the GUI update your results
	//       while rendering.
	for (int i = 0; i < w; i++)
	{
		for (int j = 0; j < h; j++)
		{
			tracePixel(i, j);
		}
	}
    tracePixel(300, 200);
}

int RayTracer::aaImage()
{
	// YOUR CODE HERE
	// FIXME: Implement Anti-aliasing here
	//
	// TIP: samples and aaThresh have been synchronized with TraceUI by
	//      RayTracer::traceSetup() function
	return 0;
}

bool RayTracer::checkRender()
{
	// YOUR CODE HERE
	// FIXME: Return true if tracing is done.
	//        This is a helper routine for GUI.
	//
	// TIPS: Introduce an array to track the status of each worker thread.
	//       This array is maintained by the worker threads.
	return true;
}

void RayTracer::waitRender()
{
	// YOUR CODE HERE
	// FIXME: Wait until the rendering process is done.
	//        This function is essential if you are using an asynchronous
	//        traceImage implementation.
	//
	// TIPS: Join all worker threads here.
}

// Done.
glm::dvec3 RayTracer::getPixel(int i, int j)
{
	unsigned char *pixel = buffer.data() + (i + j * buffer_width) * 3;
	return glm::dvec3((double)pixel[0] / 255.0, (double)pixel[1] / 255.0,
					  (double)pixel[2] / 255.0);
}

// Done.
void RayTracer::setPixel(int i, int j, glm::dvec3 color)
{
	unsigned char *pixel = buffer.data() + (i + j * buffer_width) * 3;

	pixel[0] = (int)(255.0 * color[0]);
	pixel[1] = (int)(255.0 * color[1]);
	pixel[2] = (int)(255.0 * color[2]);
}
