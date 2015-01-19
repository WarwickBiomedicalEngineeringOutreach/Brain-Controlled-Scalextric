#include "./arduinointerface.h"

ArduinoInterface::ArduinoInterface(QObject *parent)
    : QObject(parent) {
    connect(&serialPort, SIGNAL(readyRead()), this, SLOT(read()));
}

ArduinoInterface::ArduinoInterface(QString portName, QObject *parent)
    : QObject(parent) {

    connect(&serialPort, SIGNAL(readyRead()), this, SLOT(read()));

    init(portName);
}

ArduinoInterface::~ArduinoInterface() {
    serialPort.close();
}

//! \brief A More C-Friendly way of enabling serial communications
int ArduinoInterface::init(const QString portName) {
    qDebug() << "Initializing Arduino Port...";

    m_portName = portName;

    // Set Port Information
    // if using attention or meditation a BaudRate of 9600 is more than enough
    // it may require increasing if additional signal processing is used.
    serialPort.setPortName(m_portName);
    serialPort.setBaudRate(QSerialPort::Baud9600);

    if (!serialPort.open(QIODevice::ReadWrite)) {
        qDebug() << "Failed to Open Serial Port";
        emit serialConnectionFailed();
        return 1;
    }

    // reset car accel values to 0
    qDebug() << "Resetting arduino...";
    QByteArray resetData;
    resetData.reserve(4);
    resetData[0] = 0x10;
    resetData[1] = 0x00;
    resetData[2] = 0x20;
    resetData[3] = 0x00;
    write(resetData);

    qDebug() << "Arduino Ready.";
    emit serialConnectionSuccess();

    return 0;
}

QString ArduinoInterface::getPortName() const {
    return m_portName;
}

//! \brief Read any incoming serial communications (I.E. debug messages) from the arduino
void ArduinoInterface::read() {
    QByteArray data = serialPort.readAll();

    qDebug() << data;
}

//! \brief Write data via the serial port to the arduino controller
//!
//! Values for car speed should be between 0 and 100 and represent the duty cycle ratio of the
//! pwm controlling the motor.
//!
void ArduinoInterface::write(const QByteArray &data) {
    uint64_t bytesWritten = serialPort.write(data);

    if (bytesWritten == -1) {
        qDebug() << "Failed to write the data to port";
        return;
    } else if (bytesWritten != data.size()) {
        qDebug() << "Failed to write all the data to port";
        return;
    } else if (!serialPort.waitForBytesWritten(5000)) {
        qDebug() << "Operation timed out or an error occurred for port";
        return;
    }

    return; // write successful
}
