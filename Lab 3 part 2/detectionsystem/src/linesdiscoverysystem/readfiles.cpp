#include <linesdiscoverysystem/readfiles.h>

#include <cassert>
#include <string>
#include <fstream>
#include <algorithm>
#include<set>
#include <cmath>


std::vector<rendering::Point> readLineSegments(std::ifstream& file) {
    std::vector<rendering::Point> lines;

    rendering::Point start(glm::vec2{}, glm::vec4{1.0f, 1.0f, 0.0f, 1.0f}, 0.002f);
    rendering::Point end(glm::vec2{}, glm::vec4{1.0f, 1.0f, 0.0f, 1.0f}, 0.002f);
    while (file >> start.position.x >> start.position.y &&
           file >> end.position.x >> end.position.y) {
        start.position.x /= 32767.0;
        start.position.y /= 32767.0;
        end.position.x /= 32767.0;
        end.position.y /= 32767.0;
        lines.insert(lines.end(), {start, end});
    }
    return lines;
}

/*
 * Reads all line segments from file
 * Line segments are represented by the start point (x_1, y_1) and end point (x_2, y_2)
 * One line segment per line: x_1 y_1 x2 y_2
 * Returns a vector of points that can be rendered
 */
std::vector<rendering::Point> readLineSegments(const std::filesystem::path& file) {
    std::ifstream linesFile(file);
    if (!linesFile) {
        std::cout << "Line segments file error!!\n";
        return {};
    }
    return readLineSegments(linesFile);
}

std::vector<rendering::Point> readPoints(std::istream& is) {
    int n_points{0};
    is >> n_points;  // read number of particles

    std::vector<rendering::Point> points;
    points.reserve(n_points);
    for (int i = 0; i < n_points; ++i) {
        auto& p = points.emplace_back(glm::vec2{}, glm::vec4{1.0f, 1.0f, 0.0f, 1.0f}, 0.002f);
        is >> p.position.x >> p.position.y;
        p.position.x /= 32767.0;
        p.position.y /= 32767.0;
    }
    return points;
}

/*
 * Reads all points from a given input file -- see folder detectionsystem\data
 * Returns a vector of points that can be rendered
 */
std::vector<rendering::Point> readPoints(const std::filesystem::path& file) {
    std::ifstream pointsFile(file);
    if (!pointsFile) {
        std::cout << "Points file error!!\n";
        return {};
    }
    return readPoints(pointsFile);
}






class PointSlope {
public:
    double slope;
    rendering::Point point;
};
bool operator<(const PointSlope& lhs, const PointSlope& rhs) { 
    if (lhs.slope == rhs.slope) {
        if (lhs.point.position.x == rhs.point.position.x) {
            return lhs.point.position.y < rhs.point.position.y;
        } else {
            return lhs.point.position.x < rhs.point.position.x;
        }
    } else {
        return lhs.slope < rhs.slope;
    }
}

bool findSlope(std::vector<PointSlope>::iterator first, std::vector<PointSlope>::iterator last, double slope) {
    for (;  first != last; ++first) {
        if (slope == first->slope) {
            return true;
        }
    }
    return false;
}

std::vector<std::vector<PointSlope>> sortByY(std::vector<std::vector<PointSlope>> vec) {
    int i, j;
    bool swapped;

    for (i = 0; i < vec.size() - 1; i++) {
        swapped = false;
        for (j = 0; j < vec.size() - i - 1; j++) {
            if (vec[j][0].point.position.y > vec[j + 1][0].point.position.y &&
                 vec[j][0].point.position.x == vec[j + 1][0].point.position.x) {
                std::vector<PointSlope> temp = vec[j];
                vec[j] = vec[j + 1];
                vec[j + 1] = temp;
                swapped = true;
            }
        }

        // If no two elements were swapped
        // by inner loop, then break
        if (swapped == false) break;
    }
    return vec;
}

std::vector<std::vector<PointSlope>> sortByX(std::vector<std::vector<PointSlope>> vec) {
    int i, j;
    bool swapped;

    for (i = 0; i < vec.size() - 1; i++) {
        swapped = false;
        for (j = 0; j < vec.size() - i - 1; j++) {
            if (vec[j][0].point.position.x > vec[j + 1][0].point.position.x) {
                std::vector<PointSlope> temp = vec[j];
                vec[j] = vec[j + 1];
                vec[j + 1] = temp;
                swapped = true;
            }
        }

        // If no two elements were swapped
        // by inner loop, then break
        if (swapped == false) break;
    }
    return vec;
}


void writeLines(std::vector<rendering::Point> pointVector, std::string pointPath) {

    PointSlope ps;

    rendering::Point p;
    rendering::Point q;
    std::vector<std::vector<PointSlope>> APS; // All Possible Slopes

    for (size_t i = 0; i < pointVector.size(); i++) {

        std::vector<PointSlope> slopes{};

        p = pointVector[i];
        p.position.x *= 32767.0;
        p.position.y *= 32767.0;
        ps.point = p;

        for (size_t j = 0; j < (pointVector.size());j++) {  // Kollar alla möjliga slopes för punkten p
            if (j + 1 ==  pointVector.size()) {
                slopes.push_back(ps);
                continue;
            }
            q = pointVector[j + 1];
            q.position.x *= 32767.0;
            q.position.y *= 32767.0;

            // för att inte dela på noll
            if (abs(q.position.x - p.position.x) < 1e-10) {
                ps.slope = std::numeric_limits<double>::infinity();
            } else if ((abs(q.position.y - p.position.y) < 1e-10)) {
                ps.slope = 0.0;
            } else {
                ps.slope = (q.position.y - p.position.y) / (q.position.x - p.position.x);
            }
            slopes.push_back(ps);
        }
        APS.push_back(slopes);
    }

    for (size_t i = 0; i < APS.size(); i++) {
        std::stable_sort(APS[i].begin(), APS[i].end());
    }

    APS = sortByX(APS);  // O(n^2)
    APS = sortByY(APS);  // O(n^2)

    /*
    for (size_t i = 0; i < APS.size(); i++) {
        for (size_t j = 0; j < APS[i].size(); j++) {
            std::cout << "Slope: " << APS[i][j].slope << " Coordinate: ("
                      << APS[i][j].point.position.x << ","
                      << APS[i][j].point.position.y << ")\n";
        }
        std::cout << "\n";
    }
    */

    std::vector<PointSlope> slopes{};

    for (size_t i = 0; i < APS.size(); i++) {
        for (size_t j = 0; j < APS[i].size(); j++) {
            slopes.push_back(APS[i][j]);
        }
    }
    


    int sameCounter = 0;
    std::vector<PointSlope> psSegment{};
    std::vector<std::vector<PointSlope>> completePSLineSegment{};
    PointSlope tempPS;
    int curposx = 0;
    int curposy = 0;

    for (size_t i = 0; i < slopes.size(); i++) {

        if (i == slopes.size() - 1) {
            if (sameCounter >= 2) {
                curposx = static_cast<int>(slopes[i].point.position.x);
                curposy = static_cast<int>(slopes[i].point.position.y);

                tempPS.point.position.x = curposx;
                tempPS.point.position.y = curposy;
                tempPS.slope = slopes[i].slope;

                psSegment.push_back(tempPS);
                completePSLineSegment.push_back(psSegment);
            }
            break;
        }

        if (slopes[i].slope == slopes[i + 1].slope) {
            if (slopes[i].slope == std::numeric_limits<double>::infinity() &&
                slopes[i].point.position.x != slopes[i + 1].point.position.x) {
                if (sameCounter >= 2) {
                    curposx = static_cast<int>(slopes[i].point.position.x);
                    curposy = static_cast<int>(slopes[i].point.position.y);

                    tempPS.point.position.x = curposx;
                    tempPS.point.position.y = curposy;
                    tempPS.slope = slopes[i].slope;

                    psSegment.push_back(tempPS);
                    completePSLineSegment.push_back(psSegment);
                }
                sameCounter = 0;
                psSegment.clear();
                continue;
            }

            if (slopes[i].slope == 0 &&
                slopes[i].point.position.y != slopes[i + 1].point.position.y) {
                if (sameCounter >= 2) {
                    curposx = static_cast<int>(slopes[i].point.position.x);
                    curposy = static_cast<int>(slopes[i].point.position.y);

                    tempPS.point.position.x = curposx;
                    tempPS.point.position.y = curposy;
                    tempPS.slope = slopes[i].slope;
                    psSegment.push_back(tempPS);
                    completePSLineSegment.push_back(psSegment);
                }
                sameCounter = 0;
                psSegment.clear();
                continue;
            }

            sameCounter++;
            curposx = static_cast<int>(slopes[i].point.position.x);
            curposy = static_cast<int>(slopes[i].point.position.y);

            tempPS.point.position.x = curposx;
            tempPS.point.position.y = curposy;
            tempPS.slope = slopes[i].slope;

            psSegment.push_back(tempPS);
        }

        if (slopes[i].slope != slopes[i + 1].slope) {
            if (sameCounter >= 2) {
                curposx = static_cast<int>(slopes[i].point.position.x);
                curposy = static_cast<int>(slopes[i].point.position.y);

                tempPS.point.position.x = curposx;
                tempPS.point.position.y = curposy;
                tempPS.slope = slopes[i].slope;

                psSegment.push_back(tempPS);
                completePSLineSegment.push_back(psSegment);
            }
            sameCounter = 0;
            psSegment.clear();
        }
    }

    for (size_t i = 0; i < completePSLineSegment.size(); i++) {
        for (size_t j = 0; j < completePSLineSegment[i].size(); j++) {
            psSegment.push_back(completePSLineSegment[i][j]);
        }
    }

    std::set<PointSlope> set(psSegment.begin(), psSegment.end());
    psSegment.assign(set.begin(), set.end());

    for (size_t i = 0; i < psSegment.size(); i++) {
        std::cout << "Slope: " << psSegment[i].slope << " Coordinate: (" << psSegment[i].point.position.x
                  << "," << psSegment[i].point.position.y << ")\n";
    }
    std::cout << "\n";




    std::vector<std::string> lines{};
    std::string lineStart = "";
    std::string lineEnd = "";


    for (size_t i = 0; i < lines.size(); i++) {
        std::cout << lines[i] << "\n";
    }

    writeFile(lines, pointPath);
 }


void writeFile(std::vector<std::string> lines, std::string readFileName) {
    std::filesystem::path points_name =
        "C:/Users/jarja/Documents/TND004/lab 3 part 2/detectionsystem/data/output/segments-" + readFileName;
    std::cout << points_name << '\n';
    std::ofstream out_file(points_name);
    if (!out_file) {
        std::cout << "Error opening output file!!\n";
        return;
    }
    for (size_t i = 0; i < lines.size(); i++) {
        out_file << lines[i]<< std::endl;
    }
    out_file.close();
}
