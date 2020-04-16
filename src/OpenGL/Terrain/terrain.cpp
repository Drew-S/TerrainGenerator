#include "terrain.h"

#include <QOpenGLShader>
#include <QOpenGLBuffer>
#include <QColor>

#include <GL/gl.h>

static const glm::vec3 UP(0.0f, 1.0f, 0.0f);

Terrain::Terrain(int resolution)
{
    // When created set the supplied resolution
    this->setResolution(resolution);

    // Translate the plane over to center it to (0, 0, 0)
    this->_transform.translate(-0.5f, 0.0f, -0.5f);
}

Terrain::~Terrain()
{
    this->_vertex_buffer.destroy();
    this->_vao.destroy();
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

    // Create and bind vertex array object
    this->_vao.create();
    this->_vao.bind();

    // Create, bind, and allocate vertex buffer data
    this->_vertex_buffer.create();
    this->_vertex_buffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    this->_vertex_buffer.bind();
    this->_vertex_buffer.allocate(this->_vertices.constData(), this->_vertices.size() * sizeof(QVector3D));

    // Set vertex buffer data on the shader
    this->_program.enableAttributeArray("vertex");
    this->_program.setAttributeBuffer("vertex", GL_FLOAT, 0, 3, 5 * sizeof(GLfloat));

    // Set uv buffer data on the shader
    this->_program.enableAttributeArray("uv");
    this->_program.setAttributeBuffer("uv", GL_FLOAT, 3, 2, 5 * sizeof(GLfloat));
}

void Terrain::paintGL(QOpenGLFunctions *f, QMatrix4x4 camera)
{
    // TODO: Remove, testing color
    QColor color(255, 255, 255, 255);

    this->_program.bind();

    // Attach uniform values
    // TODO: include light param
    // Attach model tranform matrix M(VP)
    this->_program.setUniformValue("model", this->_transform);

    // Attach combined view and projection matrix (M)VP
    this->_program.setUniformValue("camera", camera);

    // Attach model color value
    this->_program.setUniformValue("color", color);

    // TODO: Include UI switch to enable/disable wireframe mode
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // Draw the terrain
    // f->glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, this->_indexes.constData());
    f->glDrawElements(GL_TRIANGLES, this->_indexes.size(), GL_UNSIGNED_SHORT, this->_indexes.constData());
}

// Convert row and col index 2D vector into 1D vector index
static int getIndex(int row, int col, int resolution)
{
    return row * resolution + col;
}

// Sets the resolution of the terrain (vertices), resolution is vertices along row and
// and along col. Vertices = resolution^2
void Terrain::setResolution(int resolution)
{
    // Clear existing vertex data
    this->_vertices.clear();
    this->_indexes.clear();

    // Generate terrain
    for (int z = 0; z < resolution; z++) // row
    {
        for (int x = 0; x < resolution; x++) // col
        {
            this->_vertices
                << (float)x / (float)(resolution - 1)
                << 0.0f
                << (float)z / (float)(resolution - 1)
                << (float)x / (float)(resolution - 1)
                << (float)z / (float)(resolution - 1);

            if (x < resolution - 1 && z > 0)
            {
                // index = row * resolution + col
                this->_indexes << getIndex(z, x, resolution);         // a   b+-+c
                this->_indexes << getIndex(z - 1, x, resolution);     // b    |/
                this->_indexes << getIndex(z - 1, x + 1, resolution); // c   a+

                this->_indexes << getIndex(z, x, resolution);         // a      +c
                this->_indexes << getIndex(z - 1, x + 1, resolution); // d     /|
                this->_indexes << getIndex(z, x + 1, resolution);     // d   a+-+d
            }
        }
    }
}