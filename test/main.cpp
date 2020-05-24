#include <QtTest>

#include <QApplication>

#include "./tests/intensitymap_test.h"
#include "./tests/vectormap_test.h"
#include "./tests/pixmap_test.h"
#include "./tests/converters_test.h"
#include "./tests/normal_test.h"
#include "./tests/inputsimplexnoise_test.h"
#include "./tests/inputtexture_test.h"
#include "./tests/colorsplit_test.h"
#include "./tests/colorcombine_test.h"
#include "./tests/constantvalue_test.h"
#include "./tests/constantvector_test.h"
#include "./tests/invertintensity_test.h"
#include "./tests/normalize_test.h"
#include "./tests/vectordot_test.h"
#include "./tests/math_test.h"
#include "./tests/vectormath_test.h"
#include "./tests/settings_test.h"

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
    ASSERT_TEST(new VectorMapData_Test());

    ASSERT_TEST(new IntensityToVectorMapConverter_Test());
    ASSERT_TEST(new VectorToIntensityMapConverter_Test());

    ASSERT_TEST(new NormalMapGenerator_Test());

    ASSERT_TEST(new InputSimplexNoiseNode_Test());
    ASSERT_TEST(new InputTextureNode_Test());
    ASSERT_TEST(new InputConstantValueNode_Test());
    ASSERT_TEST(new InputConstantVectorNode_Test());

    ASSERT_TEST(new ConverterColorSplitNode_Test());
    ASSERT_TEST(new ConverterColorCombineNode_Test());
    ASSERT_TEST(new ConverterInvertIntensityNode_Test());
    ASSERT_TEST(new ConverterNormalizeNode_Test());
    ASSERT_TEST(new ConverterVectorDotNode_Test());
    // Skipping Math Node as it freezes system for a long period of time
    // ASSERT_TEST(new ConverterMathNode_Test());
    ASSERT_TEST(new ConverterVectorMathNode_Test());

    ASSERT_TEST(new Settings_Test());

    return 0;
}