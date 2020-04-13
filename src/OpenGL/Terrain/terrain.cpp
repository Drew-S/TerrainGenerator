#include "terrain.h"

#include <QOpenGLShader>
#include <QColor>

#include <GL/gl.h>

static const glm::vec3 UP(0.0f, 1.0f, 0.0f);

Terrain::Terrain(int resolution)
{
    // When created set the supplied resolution
    this->setResolution(resolution);
}

void Terrain::initializeGL()
{
    // Initialize the vertex shader
    // TODO: Long term include absolute path management
    this->_program.addShaderFromSourceFile(QOpenGLShader::Vertex, "assets/shaders/shader.vert");

    // Initialize the frag shader (color)
    this->_program.addShaderFromSourceFile(QOpenGLShader::Fragment, "assets/shaders/shader.frag");

    // Link and bind the shader program for use
    this->_program.link();

    // Get the location of the pointers in the shader program
    this->_vertex_location = this->_program.attributeLocation("vertex");
    this->_camera_location = this->_program.uniformLocation("camera");
    this->_color_location = this->_program.uniformLocation("color");
}

void Terrain::paintGL(QOpenGLFunctions *f, QMatrix4x4 camera)
{
    // TODO: Remove, testing triangle
    QVector3D triangle[] = {
        QVector3D(-1.0f, 0.0f, -1.0f),
        QVector3D(-1.0f, 0.0f, 1.0f),
        QVector3D(1.0f, 0.0f, 1.0f),
        QVector3D(-1.0f, 0.0f, -1.0f),
        QVector3D(1.0f, 0.0f, 1.0f),
        QVector3D(1.0f, 0.0f, -1.0f)};

    // TODO: Remove, testing color
    QColor color(255, 255, 255, 255);

    this->_program.bind();

    // Enable and attach data to the shader program
    // TODO: Rename/remove, use _vertices and _indexes for terrain data
    this->_program.enableAttributeArray(this->_vertex_location);
    this->_program.setAttributeArray(this->_vertex_location, triangle, 0);
    this->_program.setUniformValue(this->_camera_location, camera);
    this->_program.setUniformValue(this->_color_location, color);

    // Draw the terrain
    f->glDrawArrays(GL_TRIANGLES, 0, 6);

    // Disable the attribute pointer
    this->_program.disableAttributeArray(this->_vertex_location);
    this->_program.release();
}

void Terrain::setResolution(int resolution)
{
    // Clear existing vertex data
    this->_vertices.clear();
    this->_indexes.clear();

    // Generate terrain
    for (int z = 0; z < resolution; z++)
    {
        for (int x = 0; x < resolution; x++)
        {
            this->_vertices.push_back(
                glm::vec3(
                    (float)x / (float)resolution,
                    0.0f,
                    (float)z / (float)resolution));
            // TODO: Fix index population to be triples of triangle faces to be drawn with GL_TRIANGLES
            this->_indexes.push_back(z * resolution + x);
        }
    }
}