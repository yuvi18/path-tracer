#ifndef __LIGHT_H__
#define __LIGHT_H__

#ifndef _WIN32
#include <algorithm>
#include <random>
using std::max;
using std::min;
#endif

#include "../ui/TraceUI.h"
#include "scene.h"
#include <FL/gl.h>

class Light : public SceneElement
{
public:
	virtual glm::dvec3 shadowAttenuation(const ray &r,
										 const glm::dvec3 &pos) const = 0;
	virtual double distanceAttenuation(const glm::dvec3 &P) const = 0;
	virtual glm::dvec3 getColor() const = 0;
	virtual glm::dvec3 getDirection(const glm::dvec3 &P) const = 0;
	virtual glm::dvec3 getRelativeDirection(const glm::dvec3 &P) const = 0;
	bool isPoint()
	{
		return pointLight;
	}

protected:
	Light(Scene *scene, const glm::dvec3 &col)
		: SceneElement(scene), color(col) {}

	glm::dvec3 color;
	bool pointLight;

public:
	virtual void glDrawLight([[maybe_unused]] GLenum lightID) const {}
	virtual void glDrawLight() const {}
};

class DirectionalLight : public Light
{
public:
	DirectionalLight(Scene *scene, const glm::dvec3 &orien,
					 const glm::dvec3 &color)
		: Light(scene, color), orientation(glm::normalize(orien))
	{
		pointLight = false;
	}
	virtual glm::dvec3 shadowAttenuation(const ray &r,
										 const glm::dvec3 &pos) const;
	virtual double distanceAttenuation(const glm::dvec3 &P) const;
	virtual glm::dvec3 getColor() const;
	virtual glm::dvec3 getDirection(const glm::dvec3 &P) const;
	virtual glm::dvec3 getRelativeDirection(const glm::dvec3 &P) const;

protected:
	glm::dvec3 orientation;

public:
	void glDrawLight(GLenum lightID) const;
	void glDrawLight() const;
};

class PointLight : public Light
{
public:
	PointLight(Scene *scene, const glm::dvec3 &pos, const glm::dvec3 &color,
			   float constantAttenuationTerm, float linearAttenuationTerm,
			   float quadraticAttenuationTerm)
		: Light(scene, color), position(pos),
		  constantTerm(constantAttenuationTerm),
		  linearTerm(linearAttenuationTerm),
		  quadraticTerm(quadraticAttenuationTerm)
	{
		pointLight = true;
	}

	virtual glm::dvec3 shadowAttenuation(const ray &r,
										 const glm::dvec3 &pos) const;
	virtual double distanceAttenuation(const glm::dvec3 &P) const;
	virtual glm::dvec3 getColor() const;
	virtual glm::dvec3 getDirection(const glm::dvec3 &P) const;
	virtual glm::dvec3 getRelativeDirection(const glm::dvec3 &P) const;

	void setAttenuationConstants(float a, float b, float c)
	{
		constantTerm = a;
		linearTerm = b;
		quadraticTerm = c;
	}

protected:
	glm::dvec3 position;
	// These three values are the a, b, and c in the distance attenuation function
	// (from the slide labelled "Intensity drop-off with distance"):
	//    f(d) = min( 1, 1/( a + b d + c d^2 ) )
	float constantTerm;	 // a
	float linearTerm;	 // b
	float quadraticTerm; // c

public:
	void glDrawLight(GLenum lightID) const;
	void glDrawLight() const;

protected:
};

class RectangleAreaLight : public Light
{
public:
    RectangleAreaLight(Scene *scene, const glm::dvec3 &pos, const glm::dvec3 &u, const glm::dvec3 &v,
                       double uL, double vL, const glm::dvec3 &color,
               float constantAttenuationTerm, float linearAttenuationTerm,
               float quadraticAttenuationTerm)
            : Light(scene, color), corner(pos), uVec(u), vVec(v), uLength(uL), vLength(vL),
              constantTerm(constantAttenuationTerm),
              linearTerm(linearAttenuationTerm),
              quadraticTerm(quadraticAttenuationTerm)
    {
        generator = default_random_engine(rd());
        uDist = uniform_real_distribution<double>(0, uLength);
        vDist = uniform_real_distribution<double>(0, vLength);
        center = uL / 2 * uVec + vL / 2 * vVec + corner;
    }

    virtual glm::dvec3 shadowAttenuation(const ray &r,
                                         const glm::dvec3 &pos) const;
    virtual double distanceAttenuation(const glm::dvec3 &P) const;
    virtual glm::dvec3 getColor() const;
    virtual glm::dvec3 getDirection(const glm::dvec3 &P) const;
    virtual glm::dvec3 getRelativeDirection(const glm::dvec3 &P) const;

    void setAttenuationConstants(float a, float b, float c)
    {
        constantTerm = a;
        linearTerm = b;
        quadraticTerm = c;
    }

protected:
    glm::dvec3 center;
    glm::dvec3 corner;
    glm::dvec3 uVec;
    glm::dvec3 vVec;
    double uLength;
    double vLength;

    std::random_device rd;
    std::default_random_engine generator; // rd() provides a random seed
    std::uniform_real_distribution<double> uDist;
    std::uniform_real_distribution<double> vDist;


    // These three values are the a, b, and c in the distance attenuation function
    // (from the slide labelled "Intensity drop-off with distance"):
    //    f(d) = min( 1, 1/( a + b d + c d^2 ) )
    float constantTerm;	 // a
    float linearTerm;	 // b
    float quadraticTerm; // c

public:

    //Debugging, don't know why it bugs when not having this
//    void glDrawLight(GLenum lightID) const;
//    void glDrawLight() const;

private:
    glm::dvec3 samplePoint();
};

#endif // __LIGHT_H__
