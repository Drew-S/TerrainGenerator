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
    // TODO: Move and use a dedicated GLSL shader file
    // TODO: Modify the source code, currently using testing code
    this->_program.addShaderFromSourceCode(QOpenGLShader::Vertex,
                                           "attribute mediump vec4 vertex;\n"
                                           "void main(void)\n"
                                           "{\n"
                                           "    gl_Position = vertex;\n"
                                           "}");

    // Initialize the frag shader (color)
    // TODO: Move and use dedicated GLSL shader file
    // TODO: Modify the source code, currently using testing code
    this->_program.addShaderFromSourceCode(QOpenGLShader::Fragment,
                                           "uniform mediump vec4 color;\n"
                                           "void main(void)\n"
                                           "{\n"
                                           "    gl_FragColor = color;\n"
                                           "}");

    // Link and bind the shader program for use
    this->_program.link();
    this->_program.bind();

    // Get the location of the pointers in the shader program
    this->_vertex_location = this->_program.attributeLocation("vertex");
    this->_color_location = this->_program.uniformLocation("color");
}

void Terrain::paintGL(QOpenGLFunctions *f)
{
    // TODO: Remove, testing triangle
    GLfloat triangle[] = {
        0.0f, 1.0f, 0.0f,
        -1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f};

    // TODO: Remove, testing color
    QColor color(255, 255, 255, 255);

    // Enable and attach data to the shader program
    // TODO: Rename/remove, use _vertices and _indexes for terrain data
    this->_program.enableAttributeArray(this->_vertex_location);
    this->_program.setAttributeArray(this->_vertex_location, triangle, 3);
    this->_program.setUniformValue(this->_color_location, color);

    // Draw the terrain
    f->glDrawArrays(GL_TRIANGLES, 0, 3);

    // Disable the attribute pointer
    this->_program.disableAttributeArray(this->_vertex_location);
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