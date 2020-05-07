#include <QtTest>

#include <QApplication>

#include "intensitymap_test.h"
#include "vectormap_test.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setAttribute(Qt::AA_Use96Dpi, true);

    int status;
    auto ASSERT_TEST = [&status, argc, argv](QObject *object) {
        status |= QTest::qExec(object, argc, argv);
        delete object;
    };

    ASSERT_TEST(new IntensityMap_Test());
    ASSERT_TEST(new VectorMap_Test());

    return status;
}