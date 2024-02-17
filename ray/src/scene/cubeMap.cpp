#include "cubeMap.h"
#include "../scene/material.h"
#include "../ui/TraceUI.h"
#include "ray.h"
extern TraceUI *traceUI;

glm::dvec3 CubeMap::getColor(ray r) const
{
  // YOUR CODE HERE
  // FIXME: Implement Cube Map here
  glm::dvec3 dir = r.getDirection();
  // std::cout << d << std::endl;
  double absX = fabs(dir.x), absY = fabs(dir.y), absZ = fabs(dir.z);
  double max = fmax(fabs(absX), fmax(fabs(absY), fabs(absZ)));
  double u, v;
  int idx;
  if (max == absX) // X MAX
  {
    u = dir.x > 0 ? dir.z : -dir.z, v = dir.y;
    idx = dir.x > 0 ? 0 : 1;
  }
  else if (max == absY) // Y MAX
  {
    u = dir.x, v = dir.y > 0 ? dir.z : -dir.z;
    idx = dir.y > 0 ? 2 : 3;
  }
  else if (max == absZ) // Z MAX
  {
    max = absZ;
    u = -dir.z > 0 ? dir[0] : -dir[0], v = dir.y;
    idx = -dir.x > 0 ? 4 : 5;
  }
  glm::dvec2 coord = 0.5 * glm::dvec2(u / max + 1.0, v / max + 1.0);
  return tMap[idx]->getMappedValue(coord);
}

CubeMap::CubeMap() {}

CubeMap::~CubeMap() {}

void CubeMap::setNthMap(int n, TextureMap *m)
{
  if (m != tMap[n].get())
    tMap[n].reset(m);
}
