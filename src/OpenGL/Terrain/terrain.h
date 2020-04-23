#pragma once

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QOpenGLFunctions>
#include <QOpenGLTexture>
#include <QMatrix4x4>
#include <QVector> // Used here as QT replacement for std::vector
#include <QVector2D>
#include <QVector3D>
#include <QImage>

// Class to manage the terrain data, shaders, textures, and vertices
// Used to draw the terrain to the OpenGL widget
class Terrain
{
public:
    Terrain(int resolution);
    ~Terrain(){};

    // Set the resolution of the terrain (resolution^2 vertices)
    void setResolution(int resolution);
    // Initialize the OpenGL shader, pointers, etc.
    void initializeGL();
    // Draw the terrain, calls _paintGL which does drawing, this uses _paintGL twice to draw faces and the lines
    void paintGL(QOpenGLFunctions *f, QMatrix4x4 camera_matrix, QVector3D camera_pos, QVector3D light_color, QVector3D light_pos, float light_intensity);

    // Update the existing height/normal map textures
    void setHeightMap(QImage height_map);
    void setNormalMap(QImage normal_map);

private:
    // Draws the terrain
    void _paintGL(QOpenGLFunctions *f, QMatrix4x4 camera_matrix, QVector3D camera_pos, QVector3D light_color, QVector3D light_pos, float light_intensity, QVector3D color);
    // Plane transform (translate -0.5, 0, -0.5)
    QMatrix4x4 _transform;
    // Plane vertices
    QVector<GLfloat> _vertices;
    // Indexes for drawing plane with GL_TRIANGLES
    QVector<GLushort> _indexes;

    // Vertex array object
    QOpenGLVertexArrayObject _vao;
    // Vertex buffer data
    QOpenGLBuffer _vertex_buffer{QOpenGLBuffer::VertexBuffer};
    // Shader program
    QOpenGLShaderProgram _program;

    // Height map texture
    QOpenGLTexture *_height;
    // Normal map texture
    QOpenGLTexture *_normal;
};