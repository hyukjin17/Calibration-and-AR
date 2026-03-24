/**
 * Hyuk Jin Chung
 * 3/21/2026
 *
 * Using the 3D points defined by the TargetDetector, calibrates the camera
 */

#include <fstream>
#include <sstream>
#include "Renderer.hpp"

// Constructor
Renderer::Renderer() {}

// Draw a wireframe crystal near the origin (as a floating virtual object)
void Renderer::drawCrystal(cv::Mat &dst, const cv::Mat &rvec, const cv::Mat &tvec,
                           const cv::Mat &cameraMatrix, const cv::Mat &distCoef)
{
    float L = Config::MARKER_LENGTH;

    // Define the 6 vertices of the wireframe crystal shape
    std::vector<cv::Point3f> object3D;
    object3D.push_back(cv::Point3f(2.3f * L, 2.3f * L, -3.0f * L)); // Top point
    object3D.push_back(cv::Point3f(2.3f * L, 2.3f * L, -0.5f * L)); // Bottom point
    object3D.push_back(cv::Point3f(1.6f * L, 2.3f * L, -2.0f * L)); // Mid Left
    object3D.push_back(cv::Point3f(3.0f * L, 2.3f * L, -2.0f * L)); // Mid Right
    object3D.push_back(cv::Point3f(2.3f * L, 1.6f * L, -2.0f * L)); // Mid Front
    object3D.push_back(cv::Point3f(2.3f * L, 3.0f * L, -2.0f * L)); // Mid Back

    // Project all 6 points from 3D space to 2D screen
    std::vector<cv::Point2f> proj2D;
    cv::projectPoints(object3D, rvec, tvec, cameraMatrix, distCoef, proj2D);

    // Draw the lines connecting the points
    // top half (Cyan color)
    cv::line(dst, proj2D[0], proj2D[2], cv::Scalar(255, 255, 0), Config::LINE_THICKNESS);
    cv::line(dst, proj2D[0], proj2D[3], cv::Scalar(255, 255, 0), Config::LINE_THICKNESS);
    cv::line(dst, proj2D[0], proj2D[4], cv::Scalar(255, 255, 0), Config::LINE_THICKNESS);
    cv::line(dst, proj2D[0], proj2D[5], cv::Scalar(255, 255, 0), Config::LINE_THICKNESS);
    // bottom half (Magenta color)
    cv::line(dst, proj2D[1], proj2D[2], cv::Scalar(255, 0, 255), Config::LINE_THICKNESS);
    cv::line(dst, proj2D[1], proj2D[3], cv::Scalar(255, 0, 255), Config::LINE_THICKNESS);
    cv::line(dst, proj2D[1], proj2D[4], cv::Scalar(255, 0, 255), Config::LINE_THICKNESS);
    cv::line(dst, proj2D[1], proj2D[5], cv::Scalar(255, 0, 255), Config::LINE_THICKNESS);
    // middle ring (Yellow color)
    cv::line(dst, proj2D[2], proj2D[4], cv::Scalar(0, 255, 255), Config::LINE_THICKNESS);
    cv::line(dst, proj2D[4], proj2D[3], cv::Scalar(0, 255, 255), Config::LINE_THICKNESS);
    cv::line(dst, proj2D[3], proj2D[5], cv::Scalar(0, 255, 255), Config::LINE_THICKNESS);
    cv::line(dst, proj2D[5], proj2D[2], cv::Scalar(0, 255, 255), Config::LINE_THICKNESS);
}

// Draw the 3D axes on the origin
void Renderer::drawAxes(cv::Mat &dst, const cv::Mat &rvec, const cv::Mat &tvec,
                        const cv::Mat &cameraMatrix, const cv::Mat &distCoef, float axisLength)
{
    cv::drawFrameAxes(dst, cameraMatrix, distCoef, rvec, tvec, axisLength);
}

// Load the .obj file and parse it into vertices and surface normals
bool Renderer::loadOBJ(const std::string &path)
{
    // Temporary variables to hold the raw, unordered data from the file
    std::vector<cv::Point3f> temp_vertices;
    std::vector<cv::Point3f> temp_normals;

    std::ifstream file(path);
    if (!file.is_open())
    {
        std::cerr << "ERROR: Could not open .obj file: " << path << std::endl;
        return false;
    }

    // Parse line by line
    std::string line;
    while (std::getline(file, line))
    {
        std::istringstream iss(line);
        std::string header;
        iss >> header;

        // Parse vertices (v)
        if (header == "v")
        {
            float x, y, z;
            iss >> x >> y >> z;
            temp_vertices.push_back(cv::Point3f(x, -y, -z)); // add negatives to match the axis orientation
        }
        // Parse normals (vn)
        else if (header == "vn")
        {
            float x, y, z;
            iss >> x >> y >> z;
            temp_normals.push_back(cv::Point3f(x, y, z));
        }
        // Parse faces (f)
        else if (header == "f")
        {
            std::string vertex1, vertex2, vertex3;
            unsigned int vIndex[3], nIndex[3], uvIndex[3]; // uv is for textures (ignored for this project)

            // Read the three vertex blocks
            iss >> vertex1 >> vertex2 >> vertex3;

            // sscanf reads the string and extracts the integers (for all 3 corners of triangle)
            int matches = sscanf(vertex1.c_str(), "%d/%d/%d", &vIndex[0], &uvIndex[0], &nIndex[0]);
            matches = sscanf(vertex2.c_str(), "%d/%d/%d", &vIndex[1], &uvIndex[1], &nIndex[1]);
            matches = sscanf(vertex3.c_str(), "%d/%d/%d", &vIndex[2], &uvIndex[2], &nIndex[2]);

            // Construct the final ordered arrays and push into Renderer memory
            // Need to subtract 1 because .obj files start counting at 1
            for (int i = 0; i < 3; i++)
            {
                modelVertices.push_back(temp_vertices[vIndex[i] - 1]);
                if (!temp_normals.empty())
                {
                    modelNormals.push_back(temp_normals[nIndex[i] - 1]);
                }
            }
        }
    }

    std::cout << "Successfully loaded object: " << path << " (" << modelVertices.size() / 3 << " triangles)" << std::endl;
    return true;
}

// Struct to hold the triangles for rendering
struct Face
{
    std::vector<cv::Point> pts; // 2D screen coordinates
    double depth;               // Z-distance from the camera
    cv::Scalar color;           // calculated shaded color
};

// Render the imported object as a shaded 3D object on screen
void Renderer::drawObject(cv::Mat &dst, const cv::Mat &rvec, const cv::Mat &tvec,
                          const cv::Mat &cameraMatrix, const cv::Mat &distCoef)
{
    // Check if the object has been loaded first
    if (modelVertices.empty())
        return;

    // Total width = (Number of markers * length) + (Number of gaps * separation)
    // Compute the center point of ArUco board
    float boardWidth = (Config::COLS * Config::MARKER_LENGTH) +
                       ((Config::COLS - 1) * Config::MARKER_SEPARATION);
    float boardHeight = (Config::ROWS * Config::MARKER_LENGTH) +
                        ((Config::ROWS - 1) * Config::MARKER_SEPARATION);
    float centerX = boardWidth / 2.0f;
    float centerY = boardHeight / 2.0f;

    // Shift vertices to render the image at the center of ArUco board (and floating slightly above the board)
    std::vector<cv::Point3f> worldVertices;
    worldVertices.reserve(modelVertices.size());
    for (const auto &v : modelVertices)
    {
        worldVertices.push_back(cv::Point3f(v.x + centerX, v.y + centerY, v.z - 2.5f));
    }

    // Project the shifted points onto 2D screen
    std::vector<cv::Point2f> proj2D;
    cv::projectPoints(worldVertices, rvec, tvec, cameraMatrix, distCoef, proj2D);

    // Convert rvec into 3x3 rotation matrix
    cv::Mat R;
    cv::Rodrigues(rvec, R);
    // Define a light source direction (ex. pointing straight down from the top)
    cv::Vec3f lightDir(0.0f, 1.0f, 0.0f);
    // Color for the object (ex. cyan)
    cv::Vec3f baseColor(255, 255, 0);

    std::vector<Face> faces;
    // Process the geometry, 3 vertices (1 triangle) at a time
    for (size_t i = 0; i < worldVertices.size(); i += 3)
    {
        cv::Point3f v0 = worldVertices[i];
        cv::Point3f v1 = worldVertices[i + 1];
        cv::Point3f v2 = worldVertices[i + 2];

        // Depth calculation (transform all three vertices to camera space)
        cv::Mat ptMat0 = (cv::Mat_<double>(3, 1) << v0.x, v0.y, v0.z);
        cv::Mat ptMat1 = (cv::Mat_<double>(3, 1) << v1.x, v1.y, v1.z);
        cv::Mat ptMat2 = (cv::Mat_<double>(3, 1) << v2.x, v2.y, v2.z);
        // Apply rotation and translation
        cv::Mat camPt0 = R * ptMat0 + tvec;
        cv::Mat camPt1 = R * ptMat1 + tvec;
        cv::Mat camPt2 = R * ptMat2 + tvec;
        // Calculate depth of all 3 points
        double z0 = camPt0.at<double>(2, 0);
        double z1 = camPt1.at<double>(2, 0);
        double z2 = camPt2.at<double>(2, 0);
        // Find the depth of the centroid
        double depth = (z0 + z1 + z2) / 3.0;

        // Calculating normals
        // Find the vectors of two edges of the triangle
        cv::Vec3f edge1 = v1 - v0;
        cv::Vec3f edge2 = v2 - v0;
        // Cross product gives the normal of the face
        cv::Vec3f normal = edge1.cross(edge2);
        normal = normal / cv::norm(normal); // Normalize to length 1.0

        // Transform the normal into camera space
        cv::Mat normalMat = (cv::Mat_<double>(3, 1) << normal[0], normal[1], normal[2]);
        cv::Mat camNormal = R * normalMat;
        // Convert to 3D vector
        cv::Vec3f n(camNormal.at<double>(0, 0), camNormal.at<double>(1, 0), camNormal.at<double>(2, 0));

        // Backface Culling (skip any internal faces)
        if (n[2] > 0)
            continue;

        // Shading calculations
        // Dot product calculates the direction the light hits the face
        float intensity = std::abs(n.dot(lightDir));
        // Clamp intensity so the darkest shadows dont show up as black (some ambient lighting)
        intensity = std::max(0.3f, std::min(1.0f, intensity));
        // Apply shadow to base color
        cv::Scalar shadedColor(baseColor[0] * intensity, baseColor[1] * intensity, baseColor[2] * intensity);

        // Save the computed face information to the struct
        Face f;
        f.pts = {cv::Point(proj2D[i].x, proj2D[i].y),
                 cv::Point(proj2D[i + 1].x, proj2D[i + 1].y),
                 cv::Point(proj2D[i + 2].x, proj2D[i + 2].y)};
        f.depth = depth;
        f.color = shadedColor;
        faces.push_back(f);
    }

    // Painter's Algorithm
    // Sort the faces based on depth (Larger Z means further away)
    std::sort(faces.begin(), faces.end(), [](const Face &a, const Face &b)
              { return a.depth > b.depth; });

    // Draw all the triangles
    for (const auto &f : faces)
    {
        // Wrap the points in a vector of vectors for fillPoly
        std::vector<std::vector<cv::Point>> poly = {f.pts};

        // Paint the solid shaded triangle
        cv::fillPoly(dst, poly, f.color);
    }
}