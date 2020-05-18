#include <QtTest>

#include <QApplication>

#include "./tests/intensitymap_test.h"
#include "./tests/vectormap_test.h"
#include "./tests/pixmap_test.h"
#include "./tests/converters_test.h"
#include "./tests/normal_test.h"
#include "./tests/inputsimplexnoise_test.h"
#include "./tests/inputtexture_test.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setAttribute(Qt::AA_Use96Dpi, true);

    int failed;
    auto ASSERT_TEST = [&failed, argc, argv](QObject *object) {
        failed += QTest::qExec(object, argc, argv);
        delete object;
    };

    ASSERT_TEST(new IntensityMap_Test());
    ASSERT_TEST(new VectorMap_Test());
    ASSERT_TEST(new IntensityMapData_Test());
    ASSERT_TEST(new IntensityToVectorMapConverter_Test());
    ASSERT_TEST(new VectorToIntensityMapConverter_Test());
    ASSERT_TEST(new NormalMapGenerator_Test());
    ASSERT_TEST(new InputSimplexNoiseNode_Test());
    ASSERT_TEST(new InputTextureNode_Test());

    return 0;
}