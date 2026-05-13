#include "utils.hpp"
#include "iofmtguard.hpp"
#include <iostream>
#include <algorithm>
#include <numeric>
#include <iterator>
#include <array>
#include <limits>
#include <string>
#include <stdexcept>

void cmdArea(const std::vector<Polygon>& polygons, const std::string& param)
{
    iofmtguard guard(std::cout);
    std::cout << std::fixed;
    std::cout.precision(1);

    if (param == "EVEN")
    {
        const double total = std::accumulate(polygons.begin(), polygons.end(), 0.0,
            [](double acc, const Polygon& poly)
            {
                return (poly.points.size() % 2 == 0)
                    ? acc + polygonArea(poly)
                    : acc;
            });
        std::cout << total << '\n';
    }
    else if (param == "ODD")
    {
        const double total = std::accumulate(polygons.begin(), polygons.end(), 0.0,
            [](double acc, const Polygon& poly)
            {
                return (poly.points.size() % 2 != 0)
                    ? acc + polygonArea(poly)
                    : acc;
            });
        std::cout << total << '\n';
    }
    else if (param == "MEAN")
    {
        if (polygons.empty())
        {
            std::cout << "<INVALID COMMAND>\n";
            return;
        }
        const double total = std::accumulate(polygons.begin(), polygons.end(), 0.0,
            [](double acc, const Polygon& poly) { return acc + polygonArea(poly); });
        std::cout << total / static_cast<double>(polygons.size()) << '\n';
    }
    else
    {
        try
        {
            const std::size_t nv = std::stoul(param);
            if (nv < 3)
            {
                std::cout << "<INVALID COMMAND>\n";
                return;
            }
            const double total = std::accumulate(polygons.begin(), polygons.end(), 0.0,
                [nv](double acc, const Polygon& poly)
                {
                    return (poly.points.size() == nv)
                        ? acc + polygonArea(poly)
                        : acc;
                });
            std::cout << total << '\n';
        }
        catch (const std::exception&)
        {
            std::cout << "<INVALID COMMAND>\n";
        }
    }
}

void cmdMax(const std::vector<Polygon>& polygons, const std::string& param)
{
    if (polygons.empty())
    {
        std::cout << "<INVALID COMMAND>\n";
        return;
    }

    if (param == "AREA")
    {
        iofmtguard guard(std::cout);
        std::cout << std::fixed;
        std::cout.precision(1);

        const auto it = std::max_element(polygons.begin(), polygons.end(),
            [](const Polygon& a, const Polygon& b)
            { return polygonArea(a) < polygonArea(b); });
        std::cout << polygonArea(*it) << '\n';
    }
    else if (param == "VERTEXES")
    {
        const auto it = std::max_element(polygons.begin(), polygons.end(),
            [](const Polygon& a, const Polygon& b)
            { return a.points.size() < b.points.size(); });
        std::cout << it->points.size() << '\n';
    }
    else
    {
        std::cout << "<INVALID COMMAND>\n";
    }
}

void cmdMin(const std::vector<Polygon>& polygons, const std::string& param)
{
    if (polygons.empty())
    {
        std::cout << "<INVALID COMMAND>\n";
        return;
    }

    if (param == "AREA")
    {
        iofmtguard guard(std::cout);
        std::cout << std::fixed;
        std::cout.precision(1);

        const auto it = std::min_element(polygons.begin(), polygons.end(),
            [](const Polygon& a, const Polygon& b)
            { return polygonArea(a) < polygonArea(b); });
        std::cout << polygonArea(*it) << '\n';
    }
    else if (param == "VERTEXES")
    {
        const auto it = std::min_element(polygons.begin(), polygons.end(),
            [](const Polygon& a, const Polygon& b)
            { return a.points.size() < b.points.size(); });
        std::cout << it->points.size() << '\n';
    }
    else
    {
        std::cout << "<INVALID COMMAND>\n";
    }
}

void cmdCount(const std::vector<Polygon>& polygons, const std::string& param)
{
    if (param == "EVEN")
    {
        const auto cnt = std::count_if(polygons.begin(), polygons.end(),
            [](const Polygon& poly) { return poly.points.size() % 2 == 0; });
        std::cout << cnt << '\n';
    }
    else if (param == "ODD")
    {
        const auto cnt = std::count_if(polygons.begin(), polygons.end(),
            [](const Polygon& poly) { return poly.points.size() % 2 != 0; });
        std::cout << cnt << '\n';
    }
    else
    {
        try
        {
            const std::size_t nv = std::stoul(param);
            if (nv < 3)
            {
                std::cout << "<INVALID COMMAND>\n";
                return;
            }
            const auto cnt = std::count_if(polygons.begin(), polygons.end(),
                [nv](const Polygon& poly) { return poly.points.size() == nv; });
            std::cout << cnt << '\n';
        }
        catch (const std::exception&)
        {
            std::cout << "<INVALID COMMAND>\n";
        }
    }
}

void cmdInframe(const std::vector<Polygon>& polygons, const Polygon& poly)
{
    if (polygons.empty())
    {
        std::cout << "<FALSE>\n";
        return;
    }

    using BBox = std::array<int, 4>;

    const BBox bbox = std::accumulate(polygons.begin(), polygons.end(),
        BBox{ std::numeric_limits<int>::max(),
              std::numeric_limits<int>::min(),
              std::numeric_limits<int>::max(),
              std::numeric_limits<int>::min() },
        [](BBox acc, const Polygon& p) -> BBox
        {
            return std::accumulate(p.points.begin(), p.points.end(), acc,
                [](BBox a, const Point& pt) -> BBox
                {
                    return BBox{
                        std::min(a[0], pt.x),
                        std::max(a[1], pt.x),
                        std::min(a[2], pt.y),
                        std::max(a[3], pt.y)
                    };
                });
        });

    const int minX = bbox[0], maxX = bbox[1];
    const int minY = bbox[2], maxY = bbox[3];

    const bool inside = std::all_of(poly.points.begin(), poly.points.end(),
        [minX, maxX, minY, maxY](const Point& pt)
        {
            return pt.x >= minX && pt.x <= maxX &&
                pt.y >= minY && pt.y <= maxY;
        });

    std::cout << (inside ? "<TRUE>" : "<FALSE>") << '\n';
}

static bool hasRightAngle(const Polygon& poly)
{
    const auto& pts = poly.points;
    const std::size_t n = pts.size();

    std::vector<std::size_t> idx(n);
    std::iota(idx.begin(), idx.end(), std::size_t{ 0 });

    return std::any_of(idx.begin(), idx.end(),
        [&](std::size_t i)
        {
            const std::size_t prev = (i + n - 1) % n;
            const std::size_t next = (i + 1) % n;
            const int dx1 = pts[i].x - pts[prev].x;
            const int dy1 = pts[i].y - pts[prev].y;
            const int dx2 = pts[next].x - pts[i].x;
            const int dy2 = pts[next].y - pts[i].y;
            return (dx1 * dx2 + dy1 * dy2) == 0;
        });
}

void cmdRightshapes(const std::vector<Polygon>& polygons)
{
    const auto cnt = std::count_if(polygons.begin(), polygons.end(), hasRightAngle);
    std::cout << cnt << '\n';
}

bool parsePolygonArg(std::istream& in, Polygon& poly)
{
    std::size_t n = 0;
    if (!(in >> n) || n < 3) return false;

    std::vector<Point> pts;
    for (std::size_t i = 0; i < n; ++i) {
        Point p;
        if (!(in >> p)) return false;
        pts.push_back(p);
    }
    std::string extra;
    if (in >> extra) return false;

    poly.points = std::move(pts);
    return true;
}
