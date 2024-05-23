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

// Used by std::stable_sort
bool operator<(const PointSlope& lhs, const PointSlope& rhs) { 
    // If the k-values are the same we want to sort by m-value
    if (lhs.slope.k == rhs.slope.k) {
        // If the m-values are the same we want to sort by x-value
        if (lhs.slope.m == rhs.slope.m) {
            // If the x-values are the same we want to sort by y-value
            if (lhs.point.position.x == rhs.point.position.x) {
                // Sort by y-value increasingly
                return lhs.point.position.y < rhs.point.position.y;
            } else {
                // Sort by x-value increasingly
                return lhs.point.position.x < rhs.point.position.x;
            }
        } else {
            // Sort by m-value increasingly
            return lhs.slope.m < rhs.slope.m;
        }
    } else {
        // Sort by k-value increasingly
        return lhs.slope.k < rhs.slope.k;
    }
}


// The main bulk of the program, reads all points and calculates their slopes, sorts these points and decides what points are part of a line
void writeLines(std::vector<rendering::Point> pointVector, std::string pointPath) {
    
    // Temporary values updated throught the following for-loop
    PointSlope ps;
    rendering::Point p;
    rendering::Point q;

    // Vector to store all possible slopes that appear in pointVector
    std::vector<PointSlope> slopes{};

    // If pointVector got size n, Slopes will need n^2 in size
    slopes.reserve(pointVector.size() * pointVector.size());

    // Read from pointVector time complexity: O(n^2)
    for (size_t i = 0; i < pointVector.size(); i++) {

        // Set p to the current position in pointVector
        p = pointVector[i];
        p.position.x *= 32767.0;
        p.position.y *= 32767.0;

        // Define the pointSlope point as p
        ps.point = p;

        // Calculate all possible slopes for a given point p
        for (size_t j = 0; j < (pointVector.size());j++) {
            // If we have reached the last element in pointVector we save what we have to slopes, this being the current point i and the last calculated slope
            if (j + 1 ==  pointVector.size()) {
                slopes.push_back(ps);
                continue;
            }

            // Set q to the next position in pointVector
            q = pointVector[j + 1];
            q.position.x *= 32767.0;
            q.position.y *= 32767.0;

            // Vertical slope
            if (abs(q.position.x - p.position.x) < 1e-10) {
                // Devision by 0 is not defined, thus the k-value is infinite
                ps.slope.k = std::numeric_limits<double>::infinity();
                
                // For easier comparisons while deciding lines the m-value is set to the x-position of q, could also be that of p given that they have the same x-position.
                ps.slope.m = q.position.x;
            } 
            // Horisontal slope
            else if ((abs(q.position.y - p.position.y) < 1e-10)) {
                // To ensure we get a perfeclty horisontal slope we set k to 0 given that doubles can result in rounding errors.
                ps.slope.k = 0.0;

                // Given that the m-value corresponds to where the line crosses the y-axis and we have a horisontal line we know that y = m.
                ps.slope.m = q.position.y;
            } 
            // Normal slope
            else {
                // k = (y2 - y1) / (x2 - x1)
                ps.slope.k = (q.position.y - p.position.y) / (q.position.x - p.position.x);

                // m = y - k * x
                ps.slope.m = q.position.y - ps.slope.k * q.position.x;
                
                // Round to 4 decimals, given problems with rounding of doubles
                ps.slope.k = std::round(ps.slope.k * 10000.0) / 10000.0;
                ps.slope.m = std::round(ps.slope.m * 10000.0) / 10000.0;
            }

            // Save the current point and it's slope to the vector slopes.
            slopes.push_back(ps);
        }
    }

    // sort slopes according to the operator<
    std::stable_sort(slopes.begin(), slopes.end());     // Time complexity: O(nlogn)

    // Erase possible duplicates that might be saved in slopes
    std::set<PointSlope> set(slopes.begin(), slopes.end()); // Time complexity: O(nlogn)

    // Reassign the set to a vector
    slopes.assign(set.begin(), set.end()); // Time complexity: O(n)
   
    // Given that each line must have at least four points we can assume that dubVec wont be bigger than pointVector.size()/4 thus we can reserve that size for dubVec.
    std::vector<std::vector<PointSlope>> dubVec{};  // Discovered lines, dubVec = "double Vector".
    dubVec.reserve(pointVector.size() / 4);

    // Temporary vector to store possible lines before they are saved to dubVec.
    std::vector<PointSlope> tempVec{};

    for (size_t i = 0; i < slopes.size(); i++) {

        // The last element in the vector slopes
        if (i + 1 == slopes.size()) {
            tempVec.push_back(slopes[i]);

            // Save the line only if tempVec contains four or more points with the same slope.
            if (tempVec.size() >= 4) dubVec.push_back(tempVec);

            // Clear tempVec to free up memory
            tempVec.clear();
        } 
        else {
            // Special if-statement that works for both normal lines and for std::numeric_limits<double>::infinity()
            if ((slopes[i].slope.k == slopes[i + 1].slope.k || abs(slopes[i].slope.k - slopes[i + 1].slope.k) < 1e-10) &&
                 (slopes[i].slope.m == slopes[i + 1].slope.m || abs(slopes[i].slope.m - slopes[i + 1].slope.m) < 1e-10)) {

                // Temporarily save save this point to tempVec
                tempVec.push_back(slopes[i]);

            // The next point does not share both k- and m-values with the current point thus we have found the end of a possible line
            } else {

                tempVec.push_back(slopes[i]);

               // Save the line only if tempVec contains four or more points with the same slope.
                if (tempVec.size() >= 4) {
                    dubVec.push_back(tempVec);
                }

                // Clear tempVec to move on to the next point
                tempVec.clear();
            }
        }
    }

    // Write all lines to the console according to the format seen in the lab PM. Time complexity O(n^2).
    for (size_t i = 0; i < dubVec.size(); i++) {
        for (size_t j = 0; j < dubVec[i].size(); j++) {
            // Last point
            if (j + 1 == dubVec[i].size()) {
                std::cout << "(" << std::to_string(static_cast<int>(dubVec[i][j].point.position.x)) << ","
                                        << std::to_string(static_cast<int>(dubVec[i][j].point.position.y)) << ")\n";
            // All other points
            } else {
                std::cout << "(" << std::to_string(static_cast<int>(dubVec[i][j].point.position.x)) << ","
                          << std::to_string(static_cast<int>(dubVec[i][j].point.position.y))<< ")->";
            }
        }
    }

    // The vector Lines will store all start and end coordinates of the discovered lines in dubVec.
    std::vector<std::string> lines{};
    lines.reserve(dubVec.size());

    // Declare the strings that will hold the coordinates.
    std::string lineStart = "";
    std::string lineEnd = "";

    // Declare the pointSlopes that will temporarily hold the coordinates.
    PointSlope fol; // first of line
    PointSlope lol; // last of line

    for (size_t i = 0; i < dubVec.size(); i++) {
    
        // First of line is set to the first element of the current line i.
        fol = dubVec[i][0];

        // Last of line is set to the last element of the current line i.
        lol = dubVec[i][dubVec[i].size() - 1];

        // Define lineStart and lineEnd from the coordinates held by fol and lol respectivly.
        lineStart = std::to_string(fol.point.position.x) + " " + std::to_string(fol.point.position.y) + " ";
        lineEnd = std::to_string(lol.point.position.x) + " " + std::to_string(lol.point.position.y);

        // Save the start and end values.
        lines.push_back(lineStart + lineEnd);
    }

    // Call the function which writes the lines to a file
    writeFile(lines, pointPath);
 }


void writeFile(std::vector<std::string> lines, std::string readFileName) {
    // File path only suited for Philips computer.
    std::filesystem::path points_name =
        "C:/Users/jarja/Documents/TND004/lab 3 part 2/detectionsystem/data/output/segments-" + readFileName;
    std::cout << points_name << '\n';
    std::ofstream out_file(points_name);
    if (!out_file) {
        std::cout << "Error opening output file!!\n";
        return;
    }

    // Write all the lines to the defined file.
    for (size_t i = 0; i < lines.size(); i++) {
        out_file << lines[i]<< std::endl;
    }
    // Close file and end the function.
    out_file.close();
}
