#pragma once

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QOpenGLFunctions>
#include <QMatrix4x4>
#include <QVector> // Used here as QT replacement for std::vector
#include <QVector2D>
#include <QVector3D>

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
};