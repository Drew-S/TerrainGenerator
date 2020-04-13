#pragma once

#include <vector>

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions>
#include <QMatrix4x4>

class Terrain
{
public:
    Terrain(int resolution);
    ~Terrain();

    // Set the resolution of the terrain (resolution^2 vertices)
    void setResolution(int resolution);
    // Initialize the OpenGL shader, pointers, etc.
    void initializeGL();
    // Draw the terrain using the supplied functions
    void paintGL(QOpenGLFunctions *f, QMatrix4x4 camera);

private:
    // Plane vertices
    std::vector<glm::vec3> _vertices;
    // Indexes for drawing plane with GL_TRIANGLES
    std::vector<int> _indexes;

    // Shader program, for vertex and color shader
    QOpenGLShaderProgram _program;

    // TODO: remove/change these location names, used for temp testing triangle
    int _vertex_location;
    int _camera_location;
    int _color_location;
};