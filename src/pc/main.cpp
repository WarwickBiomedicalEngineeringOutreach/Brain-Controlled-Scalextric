#include <QCoreApplication>

#include "./mindwavecontroller.h"
#include "./arduinointerface.h"

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);

    if (app.arguments().size() != 3) {
        qDebug() << "Invalid Arguments provided";
        qDebug() << "Correct arguement syntax is [MindWaveSerialPort] [ArduinoSerialPort]";
        return 1;
    }

    MindWaveController controller1;
    if (controller1.initController(app.arguments().at(1))) {
        return 2;  // failed to open Serial Port with MindWave controller
    }

    ArduinoInterface arduino;
    if (arduino.init(app.arguments().at(2))) {
        return 3; // failed to connect to the arduino's serial port
    }

    // write BCI data to Arduino
    // changed the connected signal to change the output data written to the arduino
    arduino.connect(&controller1, &MindWaveController::attentionDataChanged,
                       [&](uint16_t data){
        arduino.write(QByteArray(1, data&0xFF));
    });

    return app.exec(); // start event loop
}
