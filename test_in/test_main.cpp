#include <algorithm>
#include <list>


class Point{
public:
    float x;
    double y;
};
int main(int argc, char** argv){
  std::list<Point*> points;
  std::for_each(points.begin(), points.end(),
            [](const auto& p) { return p->x; });
    return 0;
}