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
#include <random>

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
	glm::dvec3 initialColorMulitplier(1.0, 1.0, 1.0);
	glm::dvec3 ret =
		tracePath(r, glm::dvec3(1.0, 1.0, 1.0), 0, initialColorMulitplier);
//            traceRay(r, glm::dvec3(1.0, 1.0, 1.0), traceUI->getDepth(), dummy, initialColorMulitplier);
	ret = glm::clamp(ret, 0.0, 1.0);
	return ret;
}

// Done.
glm::dvec3 RayTracer::tracePixel(int i, int j)
{
    int N = 100;
	glm::dvec3 color(0, 0, 0);
	if (!sceneLoaded())
		return color;

	unsigned char *pixel = buffer.data() + (i + j * buffer_width) * 3;
	bool antiAlias = traceUI->aaSwitch();
	if (!antiAlias || traceUI->getSuperSamples() <= 1)
	{
		double x = double(i) / double(buffer_width);
		double y = double(j) / double(buffer_height);
        for (int i = 0; i < N; i ++) {
            color += trace(x, y);
        }
		color /= glm::dvec3(N);
	}
	else
	{
		// Sample NxN pixels and average the color.
        int aaLevel = traceUI->getSuperSamples();
		double aaOffsetStep = 2.0 / double(aaLevel);
		int samples = 0;
		for (double xAaOffset = aaOffsetStep - 1; xAaOffset <= 1 - aaOffsetStep; xAaOffset += aaOffsetStep)
		{
			double x = (double(i) + xAaOffset) / double(buffer_width);
			for (double yAaOffset = aaOffsetStep - 1; yAaOffset <= 1 - aaOffsetStep; yAaOffset += aaOffsetStep)
			{
				double y = (double(j) + yAaOffset) / double(buffer_height);
                for (int i = 0; i < N; i ++) {
                    color += trace(x, y);
                    samples += 1;
                }
			}
		}
		color /= glm::dvec3(samples);
	}
	pixel[0] = (int)(255.0 * color[0]);
	pixel[1] = (int)(255.0 * color[1]);
	pixel[2] = (int)(255.0 * color[2]);
	return color;
}

#define VERBOSE 0

// Do recursive ray tracing! You'll want to insert a lot of code here (or places
// called from here) to handle reflection, refraction, etc etc.
glm::dvec3 RayTracer::traceRay(ray &r, const glm::dvec3 &thresh, int depth,
							   double &t, glm::dvec3 colorMultiplier)
{
	isect i;
	glm::dvec3 colorC;
#if VERBOSE
	std::cerr << "== current depth: " << depth << std::endl;
#endif
	if (depth < 0)
	{
		return glm::dvec3(0, 0, 0);
	}
	else if (scene->intersect(r, i))
	{
		const Material &m = i.getMaterial(); // 1. get the material.
		colorC = m.shade(scene.get(), r, i);
		glm::dvec3 normal = i.getN();
		bool insideMesh = glm::dot(-r.getDirection(), normal) < 0;
		double d = 0;
		if (insideMesh)
		{
			// Came out of transculenct material
			glm::dvec3 startPos = r.getPosition();
			glm::dvec3 endPos = r.at(i);
			d = glm::distance(startPos, endPos);
            colorMultiplier *= pow(m.kt(i), glm::dvec3(d));
		}
		//Initial Terminate
		if(glm::l2Norm(colorMultiplier) <= this->thresh){
		    return glm::dvec3(0.0, 0.0, 0.0);
		}
		// Reflect
		if (m.Refl())
		{
			glm::dvec3 reflectNorm = normal;
			if (insideMesh)
			{
				reflectNorm = -reflectNorm;
			}
			//			glm::dvec3 reflDir = 2 * glm::dot(-r.getDirection(), reflectNorm) * reflectNorm + r.getDirection();
			glm::dvec3 reflDir = glm::reflect(r.getDirection(), reflectNorm);
			glm::dvec3 reflPos = r.at(i) + RAY_EPSILON * reflectNorm;
			ray reflRay = ray(reflPos, reflDir, glm::dvec3(1.0, 1.0, 1.0), ray::REFLECTION);
			glm::dvec3 reflResult = m.kr(i) * traceRay(reflRay, thresh, depth - 1, t, colorMultiplier * m.kr(i));
			colorC += reflResult;
		}
		if (m.Trans())
		{
			// Normal Refraction
			double ratio = 1 / m.index(i);
			glm::dvec3 refractNorm = normal;
			if (insideMesh)
			{
				ratio = m.index(i);
				refractNorm = -normal;
			}
			glm::dvec3 refractPos = r.at(i.getT() + RAY_EPSILON);
			glm::dvec3 refractDir = glm::refract(r.getDirection(), refractNorm, ratio);
			if (refractDir == glm::dvec3(0))
			{
				refractDir = glm::reflect(r.getDirection(), refractNorm);
				refractPos = r.at(i.getT() - RAY_EPSILON);
			}
			ray refractRay = ray(refractPos, refractDir, glm::dvec3(1.0, 1.0, 1.0), ray::REFRACTION);
			glm::dvec3 refractResult = traceRay(refractRay, thresh, depth - 1, t, colorMultiplier);
			colorC += refractResult;
		}
		if (insideMesh)
		{
			// Exiting, so attenuiate by the distance
			colorC *= glm::pow(m.kt(i), glm::dvec3(d));
		}
		return colorC;
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
            CubeMap* theMap = traceUI->getCubeMap();
            colorC = theMap->getColor(r);
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

double ndfRay(double alpha, glm::dvec3 H, glm::dvec3 N) {
    double alphaSquared = alpha * alpha;
    double ndoth = glm::dot(N, H);
    double denom = M_PI * glm::pow((ndoth * ndoth) * (alphaSquared - 1) + 1, 2);
    double ret = (alphaSquared) / denom;
    return ret;
}

glm::dvec3 RayTracer::tracePath(ray &r, const glm::dvec3 &thresh, int depth, glm::dvec3 colorMultiplier)
{
    isect i;
    glm::dvec3 colorC;
    if (scene->intersect(r, i))
    {
        const Material &m = i.getMaterial();
        double russianRoulette = (double) rand() / (double)RAND_MAX;
        if (russianRoulette < 0.1) {
            return glm::dvec3(0);
        }
        glm::dvec3 indirectColor = glm::dvec3(0);
        glm::dvec3 normal = i.getN();
        glm::dvec3 startPos = r.at(i);

        glm::dvec3 Nt;
        if (glm::abs(normal.x) > glm::abs(normal.y))
            Nt = glm::dvec3(normal.z, 0, -normal.x) / glm::sqrt(normal.x * normal.x + normal.z * normal.z);
        else
            Nt = glm::dvec3(0, -normal.z, normal.y) / glm::sqrt (normal.y * normal.y + normal.z * normal.z);
        glm::dvec3 Nb = glm::cross(normal, Nt);


        double r1 = (double) rand() / (double)RAND_MAX; // cos(theta)

        double sinTheta = glm::sqrt(1 - r1 * r1);
        double phi =  (double) rand() / (double)RAND_MAX * 2 * M_PI;
        float x = sinTheta * glm::cos(phi);
        float z = sinTheta * glm::sin(phi);
        glm::dvec3 randomDir = glm::dvec3(x, r1, z);

        glm:: dvec3 convertedRandomDir = glm::dvec3(
                randomDir.x * Nb.x + randomDir.y * normal.x + randomDir.z * Nt.x,
                randomDir.x * Nb.y + randomDir.y * normal.y + randomDir.z * Nt.y,
                randomDir.x * Nb.z + randomDir.y * normal.z + randomDir.z * Nt.z
        );
        convertedRandomDir = glm::normalize(convertedRandomDir);

        ray randomRay = ray(startPos + convertedRandomDir * RAY_EPSILON, convertedRandomDir, glm::dvec3(1.0, 1.0, 1.0), ray::VISIBILITY);

        double pdf = 1 / (2 * M_PI);
        indirectColor += tracePath(randomRay, thresh, depth + 1, colorMultiplier);
        indirectColor /= pdf;

        ray wIn = ray(startPos, -convertedRandomDir, glm::dvec3(1.0, 1.0, 1.0), ray::VISIBILITY);
        ray wOut = ray(r.at(i), glm::normalize(-r.getDirection()), glm::dvec3(1.0, 1.0, 1.0), ray::VISIBILITY);
        colorC = m.shadeBRDF(scene.get(), wIn, wOut, indirectColor, i);
        double fireReflection = (double) rand() / (double)RAND_MAX;
        if (m.roughness(i) < fireReflection) {
            glm::dvec3 reflDir = glm::reflect(r.getDirection(), normal);
            glm::dvec3 reflPos = r.at(i) + RAY_EPSILON * normal;
            ray reflRay = ray(reflPos, reflDir, glm::dvec3(1.0, 1.0, 1.0), ray::REFLECTION);
            glm::dvec3 reflResult = tracePath(reflRay, thresh, depth + 1, colorMultiplier);
            colorC += reflResult;
            colorC /= 2;
        }
        colorC /= 0.9; // russian roulette
    }
    else
    {
//        printf("Missed intersection\n");
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
            CubeMap* theMap = traceUI->getCubeMap();
            colorC = theMap->getColor(r);
        }
        else
        {
            // THere is no cube map, return black.
            colorC = glm::dvec3(0.0, 0.0, 0.0);
        }
    }
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

void RayTracer::processChunk(int start, int end, int h) {
    for (int i = start; i < end; i++) {
        for (int j = 0; j < h; j++) {
            tracePixel(i, j);
        }
    }
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
//	for (int i = 0; i < w; i++)
//	{
//        cout << i << endl;
//		for (int j = 0; j < h; j++)
//		{
//            tracePixel(i, j);
//        }
//	}
    int chunkSize = w / this->threads;
    for (int t = 0; t < this->threads; t++) {
        int start = t * chunkSize;
        int end = (t == this->threads - 1) ? w : start + chunkSize; // Ensure last thread covers the remainder
        threadsVec.emplace_back([this, start, end, h]() { this->processChunk(start, end, h); });
    }

    waitRender();

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
    for (auto &th : threadsVec) {
        th.join();
    }
    threadsVec.clear();
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
