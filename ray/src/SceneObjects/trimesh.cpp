#include "trimesh.h"
#include <algorithm>
#include <assert.h>
#include <cmath>
#include <float.h>
#include <string.h>
#include <iostream>
#include "../ui/TraceUI.h"
#include <glm/gtx/io.hpp>

extern TraceUI *traceUI;
extern TraceUI *traceUI;

using namespace std;

Trimesh::~Trimesh()
{
	for (auto f : faces)
		delete f;
}

// must add vertices, normals, and materials IN ORDER
void Trimesh::addVertex(const glm::dvec3 &v) { vertices.emplace_back(v); }
void Trimesh::addNormal(const glm::dvec3 &n) { normals.emplace_back(n); }
void Trimesh::addTangent(const glm::dvec3 &t) { tangents.emplace_back(t); }
void Trimesh::addBitangent(const glm::dvec3 &bt) { tangents.emplace_back(bt); }

void Trimesh::addColor(const glm::dvec3 &c) { vertColors.emplace_back(c); }
void Trimesh::addUV(const glm::dvec2 &uv) { uvCoords.emplace_back(uv); }

// Returns false if the vertices a,b,c don't all exist
bool Trimesh::addFace(int a, int b, int c)
{
	int vcnt = vertices.size();
	if (a >= vcnt || b >= vcnt || c >= vcnt)
		return false;

	TrimeshFace *newFace = new TrimeshFace(this, a, b, c);
	if (!newFace->degen)
		faces.push_back(newFace);
	else
		delete newFace;

	// Don't add faces to the scene's object list so we can cull by bounding box.
	return true;
}

// Check to make sure that if we have per-vertex materials or normals
// they are the right number.
const char *Trimesh::doubleCheck()
{
	if (!vertColors.empty() && vertColors.size() != vertices.size())
		return "Bad Trimesh: Wrong number of vertex colors.";
	if (!uvCoords.empty() && uvCoords.size() != vertices.size())
		return "Bad Trimesh: Wrong number of UV coordinates.";
	if (!normals.empty() && normals.size() != vertices.size())
		return "Bad Trimesh: Wrong number of normals.";

	return 0;
}

void Trimesh::buildTree()
{
	if (this->tree == nullptr)
	{
		this->tree = new BVH<TrimeshFace>(faces);
	}
}

bool Trimesh::intersectLocal(ray &r, isect &i) const
{
	//    bool have_one = false;
	//    for (auto face : faces) {
	//        isect cur;
	//        if (face->intersectLocal(r, cur)) {
	//            if (!have_one || (cur.getT() < i.getT())) {
	//                i = cur;
	//                have_one = true;
	//            }
	//        }
	//    }
	//    if (!have_one){
	//        i.setT(1000.0);
	//    }
	i.setT(1000.0);
	bool have_one = this->tree->intersect(r, i);
	return have_one;
}

bool TrimeshFace::intersect(ray &r, isect &i) const
{
	return intersectLocal(r, i);
}

// Intersect ray r with the triangle abc.  If it hits returns true,
// and put the parameter in t and the barycentric coordinates of the
// intersection in u (alpha) and v (beta).
bool TrimeshFace::intersectLocal(ray &r, isect &i) const
{
	/* To determine the color of an intersection, use the following rules:
	 - If the parent mesh has non-empty `uvCoords`, barycentrically interpolate
	   the UV coordinates of the three vertices of the face, then assign it to
	   the intersection using i.setUVCoordinates().
	 - Otherwise, if the parent mesh has non-empty `vertexColors`,
	   barycentrically interpolate the colors from the three vertices of the
	   face. Create a new material by copying the parent's material, set the
	   diffuse color of this material to the interpolated color, and then
	   assign this material to the intersection.
	 - If neither is true, assign the parent's material to the intersection.
	*/
	// Get Triangle Coords + Vectors
	Trimesh *parent = this->getParent();
	glm::dvec3 a = parent->vertices[this->ids[0]];
	glm::dvec3 b = parent->vertices[this->ids[1]];
	glm::dvec3 c = parent->vertices[this->ids[2]];
	glm::dvec3 origin = r.getPosition();
	glm::dvec3 direction = r.getDirection();
	glm::dvec3 normal = getNormal();

	// Check to see if it intersects the plane
	double denom = glm::dot(normal, direction);
	// Parallel to plane.
	if (glm::abs(denom) < RAY_EPSILON)
	{
		return false;
	}
	// Use point a to define the plane
	double num = glm::dot(a - origin, normal);
	//    assert(glm::abs(glm::dot(normal, a - origin) - glm::dot(normal, b - origin)) < RAY_EPSILON);
	double t = num / denom;
	// Object was before ray cast
	//    assert(glm::dot((origin + direction  * t - a), normal) < RAY_EPSILON);
	if (t < 0)
	{
		return false;
	}

	glm::dvec3 intersectPoint = r.at(t);

	// Barycentric coordinates
	double bTerm1 = glm::dot(intersectPoint - a, b - a);
	double bTerm2 = glm::dot(intersectPoint - a, c - a);
	glm::dvec2 bVec(bTerm1, bTerm2);
	glm::dvec2 partialBary = AMat * bVec;
	glm::dvec3 fullBary(1 - partialBary[0] - partialBary[1], partialBary[0], partialBary[1]);

	// If not in triangle
	if (!(fullBary[0] >= 0 && fullBary[0] <= 1 && fullBary[1] >= 0 && fullBary[1] <= 1 && fullBary[2] >= 0 && fullBary[2] <= 1))
	{
		return false;
	}

	// If contains vertex norms
	if (!this->parent->normals.empty())
	{
		glm::dvec3 normalA = parent->normals[this->ids[0]] * fullBary[0];
		glm::dvec3 normalB = parent->normals[this->ids[1]] * fullBary[1];
		glm::dvec3 normalC = parent->normals[this->ids[2]] * fullBary[2];
		glm::dvec3 interpolatedNormal = normalA + normalB + normalC;
		// Don't forget to normalize
		interpolatedNormal = glm::normalize(interpolatedNormal);
		i.setN(interpolatedNormal);
	}
	else
	{
		i.setN(normal);
	}

	// If Mapped
	if (!this->parent->uvCoords.empty())
	{
		glm::dvec2 uvA = parent->uvCoords[this->ids[0]] * fullBary[0];
		glm::dvec2 uvB = parent->uvCoords[this->ids[1]] * fullBary[1];
		glm::dvec2 uvC = parent->uvCoords[this->ids[2]] * fullBary[2];
		glm::dvec2 interpolatedUV = uvA + uvB + uvC;
		i.setUVCoordinates(interpolatedUV);
	}
	// If Vertex Colors
	else if (!this->parent->vertColors.empty())
	{
		glm::dvec3 colorA = parent->vertColors[this->ids[0]] * fullBary[0];
		glm::dvec3 colorB = parent->vertColors[this->ids[1]] * fullBary[1];
		glm::dvec3 colorC = parent->vertColors[this->ids[2]] * fullBary[2];
		glm::dvec3 interpolatedColor = colorA + colorB + colorC;
		// Material is an object, so a deep copy should work... hopefully. If there are bugs cehck this statement.
		Material newMaterial = parent->material;
		newMaterial.setDiffuse(interpolatedColor);
		i.setMaterial(newMaterial);
	}
	else
	{
		i.setMaterial(parent->material);
	}

	i.setT(t);
	i.setObject(this->parent);
	i.triFace = this;
	return true;
}

// Once all the verts and faces are loaded, per vertex normals can be
// generated by averaging the normals of the neighboring faces.
void Trimesh::generateNormals()
{
	int cnt = vertices.size();
	normals.resize(cnt);
	std::vector<int> numFaces(cnt, 0);

	for (auto face : faces)
	{
		glm::dvec3 faceNormal = face->getNormal();

		for (int i = 0; i < 3; ++i)
		{
			normals[(*face)[i]] += faceNormal;
			++numFaces[(*face)[i]];
		}
	}

	for (int i = 0; i < cnt; ++i)
	{
		if (numFaces[i])
			normals[i] /= numFaces[i];
	}
	vertNorms = true;
	// ADDED FOR NORMAL MAP
	// Once normals are computed, we can compute tangent and bitangent spaces
	// for implementing a normal map.
	generateTangentsAndBitangents();
}

// ADDED FOR NORMAL MAP
void Trimesh::generateTangentsAndBitangents()
{
	tangents.resize(normals.size());
	bitangents.resize(normals.size());

	// Source for calculations:
	// http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-13-normal-mapping/
	for (int i = 0; i < normals.size(); i += 3)
	{
		// 1. Get the triangle normal and vertex positions.
		glm::dvec3 normal = normals[i];
		glm::dvec3 v0 = vertices[i],
				   v1 = vertices[i + 1],
				   v2 = vertices[i + 2];
		glm::vec2 uv0 = uvCoords[i],
				  uv1 = uvCoords[i + 1],
				  uv2 = uvCoords[i + 2];
		// Calculate vertex position delta to use as a direction.
		glm::vec3 dPos1 = v1 - v0,
				  dPos2 = v2 - v0;
		// Calculate UV delta.
		glm::vec2 dUV1 = uv1 - uv0,
				  dUV2 = uv2 - uv0;
		// Compute r, then use r for tangent and bitangent vectors.
		float r = 1.0f / (dUV1.x * dUV2.y - dUV1.y * dUV2.x);
		glm::vec3 tangent = (dPos1 * dUV2.y - dPos2 * dUV1.y) * r;
		glm::vec3 bitangent = (dPos2 * dUV1.x - dPos1 * dUV2.x) * r;
		tangents[i] = tangent;
		bitangents[i] = bitangent;
	}
}