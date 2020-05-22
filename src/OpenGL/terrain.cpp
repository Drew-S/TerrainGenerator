#include "terrain.h"

#include <QOpenGLShader>
#include <QOpenGLBuffer>
#include <QColor>

#include <GL/gl.h>

// Create a terrain class with a set resolution, resolution is the
// number of vertices in the x and z coordinates, this results in resolution^2 vertices
Terrain::Terrain(int resolution)
{
    Q_ASSERT(resolution > 0);
    qDebug("Setting up terrain");
    // When created set the supplied resolution
    this->setResolution(resolution);

    // Translate the plane over to center it to (0, 0, 0)
    this->_transform.scale(2.0f);
    this->_transform.translate(-0.5f, 0.0, -0.5f);
}

Terrain::~Terrain() {}

// Initialize OpenGL items
void Terrain::initializeGL()
{
    // Attach a height map, used for testing shader code

    qDebug("Initialize Terrain OpenGL, Creating Default Maps, attaching shaders, and Attaching buffers");
    int res = 1;

    QImage height(res, res, QImage::Format_Indexed8);
    height.setColorCount(1);
    height.setColor(0, qRgba(0, 0, 0, 255));
    height.fill(0);

    this->_height = new QOpenGLTexture(height.mirrored());
    this->_height->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
    this->_height->setMagnificationFilter(QOpenGLTexture::LinearMipMapLinear);

    // Once transforms are applied in fragment shader the color (128, 128, 255) or (0.5, 0.5, 1.0) becomes (0, 1, 0) normal vector
    QImage normal(res, res, QImage::Format_Indexed8);
    normal.setColorCount(1);
    normal.setColor(0, qRgba(128, 128, 255, 255));
    normal.fill(0);

    this->_normal = new QOpenGLTexture(normal.mirrored());
    this->_normal->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
    this->_normal->setMagnificationFilter(QOpenGLTexture::LinearMipMapLinear);

    // Initialize the vertex shader
    // TODO: Long term include absolute path management
    // TODO: Implement shader file exist checks
    this->_program.addShaderFromSourceFile(QOpenGLShader::Vertex, "assets/shaders/terrain.vert");

    // Initialize the frag shader (color)
    this->_program.addShaderFromSourceFile(QOpenGLShader::Fragment, "assets/shaders/terrain.frag");

    // Link and bind the shader program for use
    this->_program.link();

    // Create and bind vertex array object
    this->_vao.create();
    this->_vao.bind();

    // Create, bind, and allocate vertex buffer data
    this->_vertex_buffer.create();
    this->_vertex_buffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    this->_vertex_buffer.bind();
    this->_vertex_buffer.allocate(this->_vertices.constData(), this->_vertices.size() * sizeof(GLfloat));

    // Set vertex buffer data on the shader
    this->_program.enableAttributeArray("vertex");
    this->_program.setAttributeBuffer("vertex", GL_FLOAT, 0, 3, 0);

    // Release buffers
    this->_vertex_buffer.release();
    this->_vao.release();
}

// Draw the terrain (technically, it can be drawn twice)
void Terrain::paintGL(QOpenGLFunctions *f, QMatrix4x4 camera_matrix, QVector3D camera_pos, QVector3D light_color, QVector3D light_pos, float light_intensity)
{
    Q_CHECK_PTR(f);
    // Draw the terrain
    // TODO: Add terrain color selector and line color selector
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    this->_paintGL(f, camera_matrix, camera_pos, light_color, light_pos, light_intensity, QVector3D(0.75f, 0.75f, 0.75f));

    // Draw the lines that make up the terrain faces
    // TODO: Include a switch to toggle lines
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    // this->_paintGL(f, camera_matrix, camera_pos, light_color, light_pos, light_intensity, QVector3D(1.0f, 1.0f, 1.0f));
}

// Draws the terrain givent the provided information
void Terrain::_paintGL(QOpenGLFunctions *f, QMatrix4x4 camera_matrix, QVector3D camera_pos, QVector3D light_color, QVector3D light_pos, float light_intensity, QVector3D color)
{
    Q_CHECK_PTR(f);
    // Bind the buffers
    this->_vao.bind();
    this->_vertex_buffer.bind();

    // Bind the shader
    this->_program.bind();

    // Attach uniform values
    // Attach model tranform matrix M(VP)
    this->_program.setUniformValue("model", this->_transform);

    // Attach combined view and projection matrix (M)VP
    this->_program.setUniformValue("camera", camera_matrix);

    this->_program.setUniformValue("camera_pos", camera_pos);

    // Attach model color value
    this->_program.setUniformValue("color", color);

    // Attach sun light direction
    this->_program.setUniformValue("light_pos", light_pos);
    // Attach sun light color
    this->_program.setUniformValue("light_color", light_color);
    // Attach sun light intensity
    this->_program.setUniformValue("light_intensity", light_intensity);

    // Set texture filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    // Attach textures
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, this->_height->textureId());

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, this->_normal->textureId());

    this->_program.setUniformValue("height_map", GL_TEXTURE0);
    this->_program.setUniformValue("normal_map", GL_TEXTURE1 - GL_TEXTURE0);

    // Draw the terrain
    f->glDrawElements(GL_TRIANGLES, this->_indexes.size(), GL_UNSIGNED_SHORT, this->_indexes.constData());

    // Release the textures
    this->_height->release();
    this->_normal->release();

    // Release the shader
    this->_program.release();

    // Release the buffers
    this->_vertex_buffer.release();
    this->_vao.release();
}

// Convert row and col index 2D vector into 1D vector index
static int getIndex(int row, int col, int resolution)
{
    Q_ASSERT(resolution > 0);
    return row * resolution + col;
}

// Sets the resolution of the terrain (vertices), resolution is vertices along row and
// and along col. Vertices = resolution^2
void Terrain::setResolution(int resolution)
{
    qDebug("Generating terrain mesh: (%dx%d), %d vertices", resolution, resolution, resolution * resolution);
    // Clear existing vertex data
    this->_vertices.clear();
    this->_indexes.clear();

    // Generate terrain
    for (int z = 0; z < resolution; z++) // row
    {
        for (int x = 0; x < resolution; x++) // col
        {
            // Does not include additional uv values as they are encoded in the x and z components
            // since the terrain is generated as a range from 0 to 1
            this->_vertices
                << (float)x / (float)(resolution - 1)
                << 0.0f
                << (float)z / (float)(resolution - 1);

            // Generates the indexes for drawing triangles
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

// Update the current height map texture with a new height map
void Terrain::setHeightMap(QImage height_map)
{
    Q_CHECK_PTR(this->_height);
    qDebug("Updating Height Map");
    delete this->_height;
    this->_height = new QOpenGLTexture(height_map);
    this->_height->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
    this->_height->setMagnificationFilter(QOpenGLTexture::LinearMipMapLinear);
}

// Update the current normal map texture with a new normal map
void Terrain::setNormalMap(QImage normal_map)
{
    Q_CHECK_PTR(this->_normal);
    qDebug("Updating Normal Map");
    delete this->_normal;
    this->_normal = new QOpenGLTexture(normal_map);
    this->_normal->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
    this->_normal->setMagnificationFilter(QOpenGLTexture::LinearMipMapLinear);
}
