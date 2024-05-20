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



class Slope {
public:
    double k;
    double m;
};


class PointSlope {
public:
    rendering::Point point;
    Slope slope;
};
bool operator<(const PointSlope& lhs, const PointSlope& rhs) { 
    if (lhs.slope.k == rhs.slope.k) {
        if (lhs.slope.m == rhs.slope.m) {
            if (lhs.point.position.x == rhs.point.position.x) {
                return lhs.point.position.y < rhs.point.position.y;
            } else {
                return lhs.point.position.x < rhs.point.position.x;
            }
        } else {
            return lhs.slope.m < rhs.slope.m;
        }
    } else {
        return lhs.slope.k < rhs.slope.k;
    }
}



void writeLines(std::vector<rendering::Point> pointVector, std::string pointPath) {

    PointSlope ps;

    rendering::Point p;
    rendering::Point q;
    // std::vector<std::vector<PointSlope>> APS; // All Possible Slopes
    std::vector<PointSlope> slopes{};

    // Read from pointVector time complexity: O(n^2)
    for (size_t i = 0; i < pointVector.size(); i++) {

       

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

            // vertikal linje
            if (abs(q.position.x - p.position.x) < 1e-10) {
                ps.slope.k = std::numeric_limits<double>::infinity();
                ps.slope.m = q.position.x;
            } 
            // horisontell linje
            else if ((abs(q.position.y - p.position.y) < 1e-10)) {
                ps.slope.k = 0.0;
                ps.slope.m = q.position.y;
            } 
            // vanlig linje
            else {
                ps.slope.k = (q.position.y - p.position.y) / (q.position.x - p.position.x);
                ps.slope.m = q.position.y - ps.slope.k * q.position.x;
                
                // avrundar till 4 decimaler, pga doubles är dåliga på avrundning
                ps.slope.k = std::round(ps.slope.k * 10000.0) / 10000.0;
                ps.slope.m = std::round(ps.slope.m * 10000.0) / 10000.0;
            }
            
            slopes.push_back(ps);
        }
    }

    // sortera alla PS
    std::stable_sort(slopes.begin(), slopes.end());     // O(nlogn)
    std::set<PointSlope> set(slopes.begin(), slopes.end());
    slopes.assign(set.begin(), set.end());
   
    
    std::vector<std::vector<PointSlope>> dubvec;
    std::vector<PointSlope> tempvec;
    for (size_t i = 0; i < slopes.size(); i++) {

        if (i + 1 == slopes.size()) {
            tempvec.push_back(slopes[i]);
            // endast lägg till om det finns fyra eller fler punkter med samma slope
            if (tempvec.size() >= 4) dubvec.push_back(tempvec);
            tempvec.clear();
        } 
        else {
            // om de har samma linje-ekvation
            if ((slopes[i].slope.k == slopes[i + 1].slope.k || abs(slopes[i].slope.k - slopes[i + 1].slope.k) < 1e-10) &&
                 (slopes[i].slope.m == slopes[i + 1].slope.m || abs(slopes[i].slope.m - slopes[i + 1].slope.m) < 1e-10)) {

                tempvec.push_back(slopes[i]);
            } else {

                tempvec.push_back(slopes[i]);

                // endast lägg till om det finns fyra eller fler punkter med samma slope
                if (tempvec.size() >= 4) dubvec.push_back(tempvec);
                tempvec.clear();
            }
        }
    }


    std::vector<std::string> lines{};
    std::string lineStart = "";
    std::string lineEnd = "";

    PointSlope fol; // first of line
    PointSlope lol; // last of line
    for (size_t i = 0; i < dubvec.size(); i++) {
    
        fol = dubvec[i][0];
        lol = dubvec[i][dubvec[i].size() - 1];

        lineStart = std::to_string(fol.point.position.x) + " " + std::to_string(fol.point.position.y) + " ";
        lineEnd = std::to_string(lol.point.position.x) + " " + std::to_string(lol.point.position.y);

        lines.push_back(lineStart + lineEnd);
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
