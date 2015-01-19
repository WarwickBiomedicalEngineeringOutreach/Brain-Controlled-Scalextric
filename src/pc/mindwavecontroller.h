#ifndef MINDWAVECONTROLLER_H
#define MINDWAVECONTROLLER_H

#include <QObject>
#include <QSerialPort>
#include <QVariantMap>
#include <QDebug>

#include "./defines.h"

//! \title MindWaveController Interface
//!
//! \brief Interface class for using MindWave Mobile BCI controllers.
//!
class MindWaveController : public QObject {
    Q_OBJECT

    // Q_PROPERTY definitions for QML integration
    Q_PROPERTY(QString portName           MEMBER m_portName        READ getPortName       WRITE setPortName NOTIFY portNameChanged)
    Q_PROPERTY(bool connected             MEMBER m_connectionState READ isConnected)

    Q_PROPERTY(int batteryData            MEMBER m_batteryData     READ getBatteryData    NOTIFY batteryDataChanged)
    Q_PROPERTY(int signalData             MEMBER m_signalData      READ getSignalData     NOTIFY batteryDataChanged)
    Q_PROPERTY(int heartRateData          MEMBER m_heartRateData   READ getHeartRateData  NOTIFY heartRateDataChanged)
    Q_PROPERTY(int attentionData          MEMBER m_attentionData   READ getAttentionData  NOTIFY attentionDataChanged)
    Q_PROPERTY(int meditationData         MEMBER m_meditationData  READ getMeditationData NOTIFY meditationDataChanged)
    Q_PROPERTY(int raw8BitData            MEMBER m_raw8BitData     READ getRaw8BitData    NOTIFY raw8BitDataChanged)
    Q_PROPERTY(int raw16BitData           MEMBER m_raw16BitData    READ getRaw16BitData   NOTIFY raw16BitDataChanged)
    Q_PROPERTY(int rrIntervalData         MEMBER m_rrIntervalData  READ getRrIntervalData NOTIFY rrIntervalDataChanged)
    Q_PROPERTY(QVariantMap eegPowerData   MEMBER m_eegPowerDataMap READ getEegPowerData   NOTIFY eegPowerDataChanged)
    Q_PROPERTY(QVariantMap asicEegData    MEMBER m_asicEegDataMap  READ getAsicEegData    NOTIFY asicEegDataChanged)

 public:
    explicit MindWaveController(QObject *parent = nullptr);
    ~MindWaveController();

 public slots:
    int initController(const QString portName);

    void    setPortName(const QString portName);
    QString getPortName() const;

    bool isConnected() const;
    int close();

    void read();

    // These allow data to be retrieved from the object
    uint16_t getBatteryData() const;
    uint16_t getSignalData() const;
    uint16_t getHeartRateData() const;
    uint16_t getAttentionData() const;
    uint16_t getMeditationData() const;
    uint16_t getRaw8BitData() const;
    uint16_t getRaw16BitData() const;
    uint16_t getRrIntervalData() const;
    QVariantMap  getEegPowerData();
    QVariantMap  getAsicEegData();

 signals:
    void serialConnectionSuccess();  //! \brief Indicates a successful connection with the specified serial port
    void serialConnectionFailed();   //! \brief Indicates a successful connection with the specified serial port

    void portNameChanged(QString portName);  //! \brief Indicates the MindWaveMobile serial port has changed

    void batteryDataChanged(uint16_t data);  //! \brief Indicates a new battery level reading
    void signalDataChanged(uint16_t data);   //! \brief Indicates a new Signal Strength reading

    void heartRateDataChanged(uint16_t data);  //! \brief Indicates a new heart rate reading
    void attentionDataChanged(uint16_t data);  //! \brief Indicates a new attention data reading
    void meditationDataChanged(uint16_t data); //! \brief Indicates a new meditation data reading

    void raw8BitDataChanged(uint16_t data);    //! \brief Indicates a new 8bit data reading
    void raw16BitDataChanged(uint16_t data);   //! \brief Indicates a new 16bit data reading

    void rrIntervalDataChanged(uint16_t data);  //! \brief Indicates a new rrInterval reading

    void eegPowerDataChanged(QVariantMap data);  //! \brief Indicates a new EEG reading reading
    void asicEegDataChanged(QVariantMap data);   //! \brief Indicates a new ASIC EEG reading reading

 private:
    QString m_portName;
    QSerialPort serialPort;
    ThinkGearStreamParser parser;

    bool m_connectionState  = false;

    uint16_t m_controllerID   = 0;

    uint16_t m_batteryData    = 0;
    uint16_t m_signalData     = 0;

    uint16_t m_heartRateData  = 0;
    uint16_t m_attentionData  = 0;
    uint16_t m_meditationData = 0;
    uint16_t m_raw8BitData    = 0;
    uint16_t m_raw16BitData   = 0;
    uint16_t m_rrIntervalData = 0;

    eegPowerData_t m_eegPowerData;
    asicEegData_t  m_asicEegData;

    QVariantMap m_eegPowerDataMap;
    QVariantMap m_asicEegDataMap;
    void convertEegPowerDataToVariant();
    void convertAsicEegDataToVariant();

    void parseSerialData(uchar extendedCodeLevel,
                         uchar code,
                         uchar valueLength,
                         const uchar *value,
                         void* /* Custom Data */);

    void writeSerialData(const QByteArray &data);

    int initParser(uchar parserType, void *customData);
    int parseByte(uchar byte);
    int parsePacketPayload();
};

#endif  // MINDWAVECONTROLLER_H
