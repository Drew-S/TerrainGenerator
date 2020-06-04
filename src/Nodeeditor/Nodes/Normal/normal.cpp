#include "normal.h"

#include <QDebug>

/******************************************************************************
 *                                 WORKER                                     *
 ******************************************************************************/

// Sobel Kernels for calculating normal values from the height map
static double KERNEL_X[3][3] = {
    {-1.00, 0.00, 1.00},
    {-2.00, 0.00, 2.00},
    {-1.00, 0.00, 1.00}};

static double KERNEL_Y[3][3] = {
    {-1.00, -2.00, -1.00},
    {0.00, 0.00, 0.00},
    {1.00, 2.00, 1.00}};

/**
 * set
 * 
 * Function to set variables needed for the workers generation. Must be used
 * after the worker is attached to the other thread and the thread has started.
 * 
 * @param IntensityMap *height_map : The height map used to generate the normal
 *                                   map.
 */
void NormalWorker::set(IntensityMap *height_map)
{
    this->_height_map = height_map;
}

/**
 * generate
 * 
 * The generation function that operates in a separate thread to calculate the
 * normal map. Uses sobel kernel filtering for edge detection.
 * https://en.wikipedia.org/wiki/Sobel_operator
 * 
 * @signals started
 * @signals progress
 * @signals done
 */
void NormalWorker::generate()
{
    Q_CHECK_PTR(this->_height_map);
    qDebug() << this->thread()->currentThreadId();
    qDebug() << this->_height_map->width << this->_height_map->height;
    QImage normal(this->_height_map->width,
                  this->_height_map->height,
                  QImage::Format_RGBA64);

    normal.fill(QColor(0, 0, 255));
    emit this->started();
    int i = 0;
    int total = this->_height_map->width * this->_height_map->height;
    // Loop over all the pixels
    for (int x = 0; x < this->_height_map->width; x++)
    {
        for (int y = 0; y < this->_height_map->height; y++)
        {
            double matrix[3][3] = {
                {this->_getHeightIntensity(x - 1, y - 1, this->_height_map),
                    this->_getHeightIntensity(x, y - 1, this->_height_map),
                    this->_getHeightIntensity(x + 1, y - 1, this->_height_map)},
                {this->_getHeightIntensity(x - 1, y, this->_height_map),
                    0.00,
                    this->_getHeightIntensity(x + 1, y, this->_height_map)},
                {this->_getHeightIntensity(x - 1, y + 1, this->_height_map),
                    this->_getHeightIntensity(x, y + 1, this->_height_map),
                    this->_getHeightIntensity(x + 1, y + 1, this->_height_map)}
            };

// | a b c |   | 1 2 3 |
// | d e f | o | 4 5 6 | = a*1 + b*2 + c*3 + d*4 + e*5 + f*6 + g*7 + h*8 + i*9
// | g h i |   | 7 8 9 |
            double x_prime = KERNEL_X[0][0] * matrix[0][0] + KERNEL_X[0][1] *
                                 matrix[0][1] + KERNEL_X[0][2] * matrix[0][2] +
                             KERNEL_X[1][0] * matrix[1][0] + KERNEL_X[1][1] *
                                 matrix[1][1] + KERNEL_X[1][2] * matrix[1][2] +
                             KERNEL_X[2][0] * matrix[2][0] + KERNEL_X[2][1] *
                                 matrix[2][1] + KERNEL_X[2][2] * matrix[2][2];

            double y_prime = KERNEL_Y[0][0] * matrix[0][0] + KERNEL_Y[0][1] *
                                 matrix[0][1] + KERNEL_Y[0][2] * matrix[0][2] +
                             KERNEL_Y[1][0] * matrix[1][0] + KERNEL_Y[1][1] *
                                 matrix[1][1] + KERNEL_Y[1][2] * matrix[1][2] +
                             KERNEL_Y[2][0] * matrix[2][0] + KERNEL_Y[2][1] *
                                 matrix[2][1] + KERNEL_X[2][2] * matrix[2][2];

            double mag = glm::sqrt(x_prime * x_prime + y_prime * y_prime);

            // Get the normal vector, adjusted for [-1, 1] range to color range
            // [0, 1]
            glm::dvec3 n = glm::normalize(
                (glm::dvec3(-x_prime, -y_prime, mag) + 1.00) / 2.00);

            // Create color and apply to the normal image
            QColor color = QColor::fromRgbF(n.x, n.y, n.z);
            normal.setPixelColor(x, y, color);
            emit this->progress((int)round(100.0f * (float)i / (float)total));
            i++;
        }
    }
    emit this->done(normal);
}

/**
 * _getHeightIntensity
 * 
 * Helper function to get a specific intensity at an index and if it goes beyond
 * the bounds it truncates to the nearest pixel value.
 * 
 * @param int x : The column.
 * @param int y : The row.
 * @param IntensityMap* map : The map to get a value from.
 * 
 * @returns double : The resulting intensity.
 */
double NormalWorker::_getHeightIntensity(int x, int y, IntensityMap *map)
{
    if (x < 0)
        x = 0;
    else if (x >= map->width)
        x = map->width - 1;
    if (y < 0)
        y = 0;
    else if (y >= map->height)
        y = map->height - 1;


    double height = map->at(x, y);

    double strength = 25.00;
    return height * strength;
}

/******************************************************************************
 *                        NORMAL MAP GENERATOR                                *
 ******************************************************************************/

/**
 * NormalMapGenerator
 * 
 * Create a new empty normal map generator.
 */
NormalMapGenerator::NormalMapGenerator() {}

/**
 * NormalMagGenerator
 * 
 * Create a new normal map generator with a pre set height map.
 */
NormalMapGenerator::NormalMapGenerator(IntensityMap height_map)
{
    this->setImage(height_map);
}

/**
 * setImage
 * 
 * Sets the intensity map to be used to calculate the normal map from.
 * 
 * @param IntensityMap height_map : The updated height map.
 */
void NormalMapGenerator::setImage(IntensityMap height_map)
{
    this->_height_map = height_map;
    this->_width = this->_height_map.width;
    this->_height = this->_height_map.height;
}

/**
 * generate
 * 
 * Generates the normal map.
 */
void NormalMapGenerator::generate()
{
    qInfo("Generating normal map");
    NormalWorker *worker = new NormalWorker();

    worker->moveToThread(&this->_normal_thread);
    QObject::connect(&this->_normal_thread, &QThread::finished, worker, &QObject::deleteLater);
    QObject::connect(worker, &NormalWorker::done, this, &NormalMapGenerator::normalDone);
    QObject::connect(worker, &NormalWorker::started, [this]() {
        emit this->started();
    });
    QObject::connect(worker, &NormalWorker::progress, [this](int perc) {
        emit this->progress(perc);
    });
    qDebug() << this->thread()->currentThreadId();
    this->_normal_thread.start();
    worker->set(&this->_height_map);
    QMetaObject::invokeMethod(worker, "generate", Qt::QueuedConnection);
}


/**
 * normalDone @slot
 * 
 * Called when the generation by the worker object has completed the normal map.
 * 
 * @param QImage const& normal_map : The generated normal map.
 * 
 * @signals done
 */
void NormalMapGenerator::normalDone(QImage const &normal_map)
{
    qInfo("Generation of normal map complete");
    this->_normal_map = normal_map;
    emit this->done();
}

/**
 * toImage
 * 
 * Returns the normal map.
 * 
 * @returns QImage : The generated normal map.
 */
QImage NormalMapGenerator::toImage()
{
    return this->_normal_map;
}