#include "terrain.h"

#include <QColor>
#include <QDir>
#include <QFile>
#include <QOpenGLBuffer>
#include <QOpenGLShader>

#include <GL/gl.h>

#include "Globals/settings.h"

/**
 * Terrain
 * 
 * Create the terrain handler class with the set resolution of vertices along
 * an edge. Total vertices are resolution squared.
 * 
 * @param int resolution : The size of the vertices along a single edge.
 */
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

/**
 * initializeGL
 * 
 * Initialize the opengl functionality of the terrain. Create and attach shaders
 * textures, and vertex buffers.
 */
void Terrain::initializeGL()
{
    // Attach a height map, used for testing shader code

    qDebug("Initialize Terrain OpenGL, Creating Default Maps, attaching shaders"
    ", and Attaching buffers");
    int res = 1;

    QImage height(res, res, QImage::Format_Indexed8);
    height.setColorCount(1);
    height.setColor(0, qRgba(0, 0, 0, 255));
    height.fill(0);

    this->_height = new QOpenGLTexture(height.mirrored());
    this->_height->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
    this->_height->setMagnificationFilter(QOpenGLTexture::LinearMipMapLinear);

    // Once transforms are applied in fragment shader the color (128, 128, 255)
    // or (0.5, 0.5, 1.0) becomes (0, 1, 0) normal vector
    QImage normal(res, res, QImage::Format_Indexed8);
    normal.setColorCount(1);
    normal.setColor(0, qRgba(128, 128, 255, 255));
    normal.fill(0);

    this->_normal = new QOpenGLTexture(normal.mirrored());
    this->_normal->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
    this->_normal->setMagnificationFilter(QOpenGLTexture::LinearMipMapLinear);

    // Initialize the vertex shader
    Q_CHECK_PTR(SETTINGS);
    QString assets = QDir::cleanPath(SETTINGS->getAssetDirectories()[0].path() 
                                     + QString("/shaders"));

    QString vert = QDir::cleanPath(assets + QString("/terrain.vert"));
    QString frag = QDir::cleanPath(assets + QString("/terrain.frag"));

    if (!QFile::exists(vert))
        qFatal("Vertex shader program missing '%s'", qPrintable(vert));

    if (!QFile::exists(frag))
        qFatal("Fragment shader program missing '%s'", qPrintable(frag));

    this->_program.addShaderFromSourceFile(QOpenGLShader::Vertex, vert);

    // Initialize the frag shader (color)
    this->_program.addShaderFromSourceFile(QOpenGLShader::Fragment, frag);

    // Link and bind the shader program for use
    this->_program.link();

    // Create and bind vertex array object
    this->_vao.create();
    this->_vao.bind();

    // Create, bind, and allocate vertex buffer data
    this->_vertex_buffer.create();
    this->_vertex_buffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    this->_vertex_buffer.bind();
    this->_vertex_buffer.allocate(this->_vertices.constData(),
                                  this->_vertices.size() * sizeof(GLfloat));

    // Set vertex buffer data on the shader
    this->_program.enableAttributeArray("vertex");
    this->_program.setAttributeBuffer("vertex", GL_FLOAT, 0, 3, 0);

    // Release buffers
    this->_vertex_buffer.release();
    this->_vao.release();
}

// Set drawing lines mode
/**
 * setDrawLines
 * 
 * Set whether or not to draw texture lines.
 * 
 * @param bool lines : Wether or not to draw lines.
 */
void Terrain::setDrawLines(bool lines)
{
    this->_draw_lines = lines;
}

/**
 * setTerrainColor
 * 
 * Set the colour of the terrain.
 * 
 * @param QColor color : The colour of the terrain.
 */
void Terrain::setTerrainColor(QColor color)
{
    this->_terrain_color = QVector3D(color.redF(), color.greenF(), color.blueF());
}

/**
 * setLineColor
 * 
 * Set the colour of the terrain lines.
 * 
 * @param QColor color : The colour of the lines.
 */
void Terrain::setLineColor(QColor color)
{
    this->_line_color = QVector3D(color.redF(), color.greenF(), color.blueF());
}

/**
 * drawLines
 * 
 * Get whether or not we are drawing lines.
 * 
 * @returns bool : Whether or not to draw lines.
 */
bool Terrain::drawLines()
{
    return this->_draw_lines;
}

/**
 * terrainColor
 * 
 * Get the current colour of the terrai.
 */
QColor Terrain::terrainColor()
{
    return QColor::fromRgbF(this->_terrain_color.x(), this->_terrain_color.y(), this->_terrain_color.z());
}

/**
 * lineColor
 * 
 * Get the current line colour.
 * 
 * @returns QColor : The line colour.
 */
QColor Terrain::lineColor()
{
    return QColor::fromRgbF(this->_line_color.x(), this->_line_color.y(), this->_line_color.z());
}

/**
 * paintGl
 * 
 * Draw the terrain to the opengl widget.
 * 
 * @param QOpenGLFunctions* f : The valid opengl functions.
 * @param QMatrix4x4 camera_matrix : The combined view and projection matrices.
 * @param QVector3D camera_pos : The camera's position in world space.
 * @param QVector3D light_color : The suns colour.
 * @param QVector3D light_pos : The position of the sun.
 * @param float light_intensity : The intensity of the light.
 * @param QVector3D ambient : The sky colour/ambient light colour.
 */
void Terrain::paintGL(QOpenGLFunctions *f,
                      QMatrix4x4 camera_matrix,
                      QVector3D camera_pos,
                      QVector3D light_color,
                      QVector3D light_pos,
                      float light_intensity,
                      QVector3D ambient)
{
    Q_CHECK_PTR(f);
    // Draw the terrain
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    this->_paintGL(f,
                   camera_matrix,
                   camera_pos,
                   light_color,
                   light_pos,
                   light_intensity,
                   this->_terrain_color,
                   ambient);

    // Draw the lines that make up the terrain faces
    if (this->_draw_lines)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        this->_paintGL(f,
                       camera_matrix,
                       camera_pos,
                       light_color,
                       light_pos,
                       light_intensity,
                       this->_line_color,
                       ambient,
                       true);
    }
}

/**
 * _paintGl
 * 
 * Draw the terrain to the opengl widget. With a toggle whether to draw lines
 * or the terrain.
 * 
 * @param QOpenGLFunctions* f : The valid opengl functions.
 * @param QMatrix4x4 camera_matrix : The combined view and projection matrices.
 * @param QVector3D camera_pos : The camera's position in world space.
 * @param QVector3D light_color : The suns colour.
 * @param QVector3D light_pos : The position of the sun.
 * @param float light_intensity : The intensity of the light.
 * @param QVector3D ambient : The sky colour/ambient light colour.
 * @param bool lines_mode : Whether or not to draw lines or the terrain.
 */
void Terrain::_paintGL(QOpenGLFunctions *f,
                       QMatrix4x4 camera_matrix,
                       QVector3D camera_pos,
                       QVector3D light_color,
                       QVector3D light_pos,
                       float light_intensity,
                       QVector3D color,
                       QVector3D ambient,
                       bool lines_mode)
{
    Q_CHECK_PTR(f);
    // Bind the buffers
    this->_vao.bind();
    this->_vertex_buffer.bind();
    this->_vertex_buffer.allocate(this->_vertices.constData(),
                                  this->_vertices.size() * sizeof(GLfloat));

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
    // Set the sky color (ambient)
    this->_program.setUniformValue("ambient_color", ambient);
    // Set lines mode toggle
    this->_program.setUniformValue("lines", lines_mode);

    // Attach textures
    glActiveTexture(GL_TEXTURE0);
    // Set texture filtering
    glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR_MIPMAP_LINEAR);

    glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_MAG_FILTER,
                    GL_LINEAR_MIPMAP_LINEAR);

    glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_WRAP_S,
                    GL_CLAMP_TO_EDGE);

    glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_WRAP_T,
                    GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_2D, this->_height->textureId());

    glActiveTexture(GL_TEXTURE1);

    // Set texture filtering
    glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR_MIPMAP_LINEAR);

    glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_MAG_FILTER,
                    GL_LINEAR_MIPMAP_LINEAR);

    glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_WRAP_S,
                    GL_CLAMP_TO_EDGE);

    glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_WRAP_T,
                    GL_CLAMP_TO_EDGE);
                    
    glBindTexture(GL_TEXTURE_2D, this->_normal->textureId());

    this->_program.setUniformValue("height_map", GL_TEXTURE0);
    this->_program.setUniformValue("normal_map", GL_TEXTURE1 - GL_TEXTURE0);

    // Draw the terrain
    f->glDrawElements(GL_TRIANGLES,
                      this->_indexes.size(),
                      GL_UNSIGNED_SHORT,
                      this->_indexes.constData());

    // Release the textures
    this->_height->release();
    this->_normal->release();

    // Release the shader
    this->_program.release();

    // Release the buffers
    this->_vertex_buffer.release();
    this->_vao.release();
}

/**
 * getIndex
 * 
 * Converts an x and y 2D index to a 1Dimensional index value with the
 * resolution. Used for the vertices index reference
 * 
 * @param int row : The row.
 * @param int col : The col.
 * @param int resolution : The size of the mesh.
 */
static int getIndex(int row, int col, int resolution)
{
    Q_ASSERT(resolution > 0);
    return row * resolution + col;
}

/**
 * setResolution
 * 
 * Sets the resolution of the mesh along an edge.
 * 
 * @param int resolution : The new resolution of the mesh.
 */
void Terrain::setResolution(int resolution)
{
    qDebug("Generating terrain mesh: (%dx%d), %d vertices",
           resolution,
           resolution,
           resolution * resolution);

    // Clear existing vertex data
    this->_vertices.clear();
    this->_indexes.clear();

    // Generate terrain
    for (int z = 0; z < resolution; z++) // row
    {
        for (int x = 0; x < resolution; x++) // col
        {
            // Does not include additional uv values as they are encoded in the
            // x and z components since the terrain is generated as a range from
            // 0 to 1
            this->_vertices
                << (float)x / (float)(resolution - 1)
                << 0.0f
                << (float)z / (float)(resolution - 1);

            // Generates the indexes for drawing triangles
            if (x < resolution - 1 && z > 0)
            {
                // index = row * resolution + col
                // a   b+-+c
                // b    |/
                // c   a+
                this->_indexes << getIndex(z, x, resolution);
                this->_indexes << getIndex(z - 1, x, resolution);
                this->_indexes << getIndex(z - 1, x + 1, resolution);

                // a      +c
                // d     /|
                // d   a+-+d
                this->_indexes << getIndex(z, x, resolution);
                this->_indexes << getIndex(z - 1, x + 1, resolution);
                this->_indexes << getIndex(z, x + 1, resolution);
            }
        }
    }
}

/**
 * setHeightMap
 * 
 * Set the height map image.
 * 
 * @param QImage height_map : The height map image.
 */
void Terrain::setHeightMap(QImage height_map)
{
    Q_CHECK_PTR(this->_height);
    qDebug("Updating Height Map");
    delete this->_height;
    this->_height = new QOpenGLTexture(height_map);
    this->_height->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
    this->_height->setMagnificationFilter(QOpenGLTexture::LinearMipMapLinear);
}

/**
 * setNormalMap
 * 
 * Set the normal map image.
 * 
 * @param QImage normal_map : The normal map image.
 */
void Terrain::setNormalMap(QImage normal_map)
{
    Q_CHECK_PTR(this->_normal);
    qDebug("Updating Normal Map");
    delete this->_normal;
    this->_normal = new QOpenGLTexture(normal_map);
    this->_normal->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
    this->_normal->setMagnificationFilter(QOpenGLTexture::LinearMipMapLinear);
}
