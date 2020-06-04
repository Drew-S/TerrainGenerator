#pragma once

#include <QImage>
#include <QMatrix4x4>
#include <QOpenGLBuffer>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QOpenGLVertexArrayObject>
#include <QVector> // Used here as QT replacement for std::vector
#include <QVector2D>
#include <QVector3D>

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

/**
 * Terrain
 * 
 * Class manages the terrain mesh and is used to set the shader parameters and
 * set the height and normal map.
 */
class Terrain
{
public:
    Terrain(int resolution);

    // Set the resolution of the terrain (resolution^2 vertices)
    void setResolution(int resolution);
    // Initialize the OpenGL shader, pointers, etc.
    void initializeGL();
    // Draw the terrain, calls _paintGL which does drawing, this uses _paintGL
    // twice to draw faces and the lines
    void paintGL(QOpenGLFunctions *f,
                 QMatrix4x4 camera_matrix,
                 QVector3D camera_pos,
                 QVector3D light_color,
                 QVector3D light_pos,
                 float light_intensity,
                 QVector3D ambient);

    // Update the existing height/normal map textures
    void setHeightMap(QImage height_map);
    void setNormalMap(QImage normal_map);

    // Set adjustable values
    void setDrawLines(bool lines);
    void setTerrainColor(QColor color);
    void setLineColor(QColor color);

    // Get adjustable values
    bool drawLines();
    QColor terrainColor();
    QColor lineColor();

private:
    // Draws the terrain
    void _paintGL(QOpenGLFunctions *f,
                  QMatrix4x4 camera_matrix,
                  QVector3D camera_pos,
                  QVector3D light_color,
                  QVector3D light_pos,
                  float light_intensity,
                  QVector3D color,
                  QVector3D ambient,
                  bool lines_mode = false);

    bool _draw_lines = false;
    QVector3D _terrain_color{0.75f, 0.75f, 0.75f};
    QVector3D _line_color{1.0f, 1.0f, 1.0f};
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