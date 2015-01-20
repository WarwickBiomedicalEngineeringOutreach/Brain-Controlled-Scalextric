#include <QCoreApplication>

#include <QTimer>

#include "./mindwavecontroller.h"
#include "./arduinointerface.h"

const uint8_t maxSpeed= 70;

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
        QByteArray outputdata;
        outputdata.append(0x10);

        if (data > maxSpeed) {
            outputdata.append(maxSpeed);
        } else {
            outputdata.append(data&0xFF);
        }

        qDebug() << "Player Code: " << static_cast<uint8_t>(outputdata.at(0))
                 << "Player Level: "<< static_cast<uint8_t>(outputdata.at(1));

        arduino.write(outputdata);
    });

    QTimer *controller2 = new QTimer();
    uint8_t controller2Value = 50;
    controller2->setInterval(1000);

    arduino.connect(controller2, &QTimer::timeout, [&](){
        QByteArray outputdata;
        outputdata.append(0x20);

        if (controller2Value > maxSpeed) {
            controller2Value = 50;
        }
        outputdata.append(controller2Value);

        controller2Value++;

        qDebug() << "Player Code: " << static_cast<uint8_t>(outputdata.at(0))
                 << "Player Level: "<< static_cast<uint8_t>(outputdata.at(1));

        arduino.write(outputdata);
    });

    controller2->start();

    return app.exec(); // start event loop
}
