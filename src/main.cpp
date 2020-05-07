#include "ui_Main.h"

#include <nodes/ConnectionStyle>

#include "mainwindow.h"

#include "Nodeeditor/nodeeditor.h"

#include <QApplication>
#include <QDebug>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int verbosity = 2;
bool log_file = true;

// Standardize writing debug statements with
void writeLine(const char *prefix, const char *file, int line, const char *function, const char *message)
{
    time_t now;
    time(&now);
    struct tm *time_info;
    time_info = localtime(&now);
    char buffer[10];
    strftime(buffer, 10, "%H:%M:%S", time_info);
    fprintf(stderr, "%s %s (%s:%d) %s -- %s\n", buffer, prefix, file, line, function, message);
    fflush(stderr);

    if (log_file)
    {
        FILE *log_file = fopen("log.txt", "a");
        fprintf(log_file, "%s %s (%s:%d) %s -- %s\n", buffer, prefix, file, line, function, message);
        fflush(log_file);
    }
}

// Custom handler for writing qDebug/Warning/etc messages
void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &message)
{
    QByteArray local_message = message.toLocal8Bit();
    const char *file = context.file ? context.file : "unknown_file";
    const char *function = context.function ? context.function : "unknown_function";
    switch (type)
    {
    case QtDebugMsg:
        if (verbosity >= 4)
            writeLine("[debug]", file, context.line, function, local_message.constData());
        break;
    case QtInfoMsg:
        if (verbosity >= 3)
            writeLine(" [info]", file, context.line, function, local_message.constData());
        break;
    case QtWarningMsg:
        if (verbosity >= 2)
            writeLine(" [warn]", file, context.line, function, local_message.constData());
        break;
    case QtCriticalMsg:
        if (verbosity >= 1)
            writeLine(" [crit]", file, context.line, function, local_message.constData());
        break;
    case QtFatalMsg:
        if (verbosity >= 0)
            writeLine("[fatal]", file, context.line, function, local_message.constData());
        break;
    }
}

int main(int argc, char *argv[])
{
    // Command line arguments to set verbosity levels
    for (int i = 1; i < argc; i++)
    {
        if ((QString(argv[i]) == "-v" || QString(argv[i]) == "--verbose") && verbosity < 3)
        {
            verbosity = 3;
        }
        if (QString(argv[i]) == "-d" || QString(argv[i]) == "--debug")
        {
            verbosity = 4;
        }
        if (QString(argv[i]) == "--no-log")
        {
            log_file = false;
        }
    }

    // Use custom debug print
    qInstallMessageHandler(messageHandler);

    QApplication app(argc, argv);

    // Should colorize the connections between nodes based on their data type
    // generated from the name of the data type (to fully customize I would
    // need to change nodeeditor itself, which I may yet do)
    QtNodes::ConnectionStyle::setConnectionStyle(
        R"(
            "ConnectionStyle": {
                "UseDataDefinedColors": true
            }
        )");

    // Load ui for main window and attach it to custom QMainWindow
    Ui::MainWindow ui;
    MainWindow main_window;

    // Setup the main window
    ui.setupUi(&main_window);
    main_window.setup(&ui);

    main_window.show();

    return app.exec();
}
