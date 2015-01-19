#ifndef ARDUINOINTERFACE_H
#define ARDUINOINTERFACE_H

#include <QObject>
#include <QSerialPort>
#include <QDebug>

#include "./defines.h"

class ArduinoInterface : public QObject {
    Q_OBJECT

    Q_PROPERTY(QString portName MEMBER m_portName)
 public:
    explicit ArduinoInterface(QObject *parent = nullptr);
    explicit ArduinoInterface(QString portName, QObject *parent = nullptr);
    ~ArduinoInterface();

    int init(const QString portName);

    QString getPortName() const;

 signals:
    void serialConnectionSuccess();  //! \brief Indicates a successful serial port connection
    void serialConnectionFailed();   //! \brief Induciates a failed serial port connection

 public slots:
    void read();
    void write(const QByteArray &data);

 private:
    QString m_portName;
    QSerialPort serialPort;
};

#endif  // ARDUINOINTERFACE_H
