#include "cubeMap.h"
#include "../scene/material.h"
#include "../ui/TraceUI.h"
#include "ray.h"
extern TraceUI *traceUI;

glm::dvec3 CubeMap::getColor(ray r) const
{
  // https://en.wikipedia.org/wiki/Cube_mapping
  glm::dvec3 dir = glm::normalize(r.getDirection());
  // std::cout << d << std::endl;
  double absX = fabs(dir.x), absY = fabs(dir.y), absZ = fabs(dir.z);
  double maxAxis;
  double u, v;
  int idx;
  dir.z *= -1;
  bool positiveX = dir.x > 0 ? 1 : 0;
  bool positiveY = dir.y > 0 ? 1 : 0;
  bool positiveZ = dir.z > 0 ? 1 : 0;
  if (positiveX && absX >= absY && absX >= absZ)
  {
    maxAxis = absX;
    u = -dir.z;
    v = dir.y;
    idx = 0;
  }
  if (!positiveX && absX >= absY && absX >= absZ)
  {
    maxAxis = absX;
    u = dir.z;
    v = dir.y;
    idx = 1;
  }
  if (positiveY && absY >= absX && absY >= absZ)
  {
    maxAxis = absY;
    u = dir.x;
    v = -dir.z;
    idx = 2;
  }
  if (!positiveY && absY >= absX && absY >= absZ)
  {
    maxAxis = absY;
    u = dir.x;
    v = dir.z;
    idx = 3;
  }
  if (positiveZ && absZ >= absX && absZ >= absY)
  {
    maxAxis = absZ;
    u = dir.x;
    v = dir.y;
    idx = 4;
  }
  if (!positiveZ && absZ >= absX && absZ >= absY)
  {
    maxAxis = absZ;
    u = -dir.x;
    v = dir.y;
    idx = 5;
  }
  glm::dvec2 coord = 0.5 * glm::dvec2(u / maxAxis + 1.0, v / maxAxis + 1.0);
  return tMap[idx]->getMappedValue(coord);
}

CubeMap::CubeMap() {}

CubeMap::~CubeMap() {}

void CubeMap::setNthMap(int n, TextureMap *m)
{
  if (m != tMap[n].get())
    tMap[n].reset(m);
}
