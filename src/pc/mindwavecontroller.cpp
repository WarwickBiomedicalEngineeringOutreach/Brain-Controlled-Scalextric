#include "./mindwavecontroller.h"

MindWaveController::MindWaveController(QObject *parent) : QObject(parent) {
    initParser(PARSER_TYPE_PACKETS, NULL);

    connect(&serialPort, SIGNAL(readyRead()), this, SLOT(read()));
}

//! \brief Close the current serial port
MindWaveController::~MindWaveController() {
    serialPort.close();
}

//! \brief A More C-Friendly way of enabling serial communications
int MindWaveController::initController(const QString portName) {
    qDebug() << "Initializing MindWaveMobile Port...";

    m_portName = portName;

    // Set Port Information
    serialPort.setPortName(m_portName);
    serialPort.setBaudRate(QSerialPort::Baud9600);

    if (!serialPort.open(QIODevice::ReadWrite)) {
        qDebug() << "Failed to Open Serial Port";
        emit serialConnectionFailed();
        return 1;
    }

    // Write command bits to MindWaveMobile
    // 0x02 enables 57.6k baud raw transfers
    QByteArray mindWaveControlInfo;
    mindWaveControlInfo.append(0x03);
    writeSerialData(mindWaveControlInfo);
    serialPort.setBaudRate(QSerialPort::Baud57600);

    qDebug() << "Initialization Complete.";
    m_connectionState = true;
    emit serialConnectionSuccess();

    return 0;
}

//! \brief Set's the serial port and automatically initiallises the connection
void MindWaveController::setPortName(const QString portName)  {
    initController(portName);
}

//! \brief Retrieve a string containing the current serial port address
QString MindWaveController::getPortName() const {
    return m_portName;
}

//! \brief Return the current connection state of the serial port
bool MindWaveController::isConnected() const {
    return m_connectionState;
}

//! \brief Close the current serial port
int MindWaveController::close() {
    if (!serialPort.isOpen()) {
        qDebug() << "Error, Serial Port is not open";
        return 1;
    }

    if (!m_connectionState) {
        qDebug() << "Something Went wrong here";
        return 2;
    }

    serialPort.close();
    m_connectionState = false;
    return 0;
}

//! \brief Decode function to parse payload packets recieved serially
void MindWaveController::parseSerialData(uchar extendedCodeLevel,
                                         uchar code,
                                         uchar valueLength,
                                         const uchar *value,
                                         void* /* customData */) {
    if (extendedCodeLevel == 0) {
        switch (code) {
        case 0x01:
            m_batteryData = value[0] & 0xFF;
            emit batteryDataChanged(m_batteryData);
            break;

        case 0x02:
            m_signalData = value[0] & 0xFF;
            emit signalDataChanged(m_signalData);
            break;

        case 0x03:
            m_heartRateData = value[0] & 0xFF;
            emit heartRateDataChanged(m_heartRateData);
            break;

        case 0x04:
            m_attentionData = value[0] & 0xFF;
            emit attentionDataChanged(m_attentionData);
            break;

        case 0x05:
            m_meditationData = value[0] & 0xFF;
            emit meditationDataChanged(m_attentionData);
            break;

        case 0x06:
            m_raw8BitData = value[0] & 0xFF;
            emit raw8BitDataChanged(m_raw8BitData);
            break;

        case 0x07:  // raw start marker
            break;

        case 0x80:
            m_raw16BitData = (value[0]<<8) | value[1];
            emit raw16BitDataChanged(m_raw16BitData);
            break;

        case 0x81:
            m_eegPowerData.delta    = ( (value[0]&0xFF)<<24)  |  ((value[1]&0xFF)<<16) |  ((value[2]&0xFF)<<8) |  (value[3]&0xFF);
            m_eegPowerData.theta    = ( (value[4]&0xFF)<<24)  |  ((value[5]&0xFF)<<16) |  ((value[6]&0xFF)<<8) |  (value[7]&0xFF);
            m_eegPowerData.lowAlpha = ( (value[8]&0xFF)<<24)  |  ((value[9]&0xFF)<<16) | ((value[10]&0xFF)<<8) | (value[11]&0xFF);
            m_eegPowerData.highAlpha= ((value[12]&0xFF)<<24)  | ((value[13]&0xFF)<<16) | ((value[14]&0xFF)<<8) | (value[15]&0xFF);
            m_eegPowerData.lowBeta  = ((value[16]&0xFF)<<24)  | ((value[17]&0xFF)<<16) | ((value[18]&0xFF)<<8) | (value[19]&0xFF);
            m_eegPowerData.highBeta = ((value[20]&0xFF)<<24)  | ((value[21]&0xFF)<<16) | ((value[22]&0xFF)<<8) | (value[23]&0xFF);
            m_eegPowerData.lowGamma = ((value[24]&0xFF)<<24)  | ((value[25]&0xFF)<<16) | ((value[26]&0xFF)<<8) | (value[27]&0xFF);
            m_eegPowerData.midGamma = ((value[28]&0xFF)<<24)  | ((value[29]&0xFF)<<16) | ((value[30]&0xFF)<<8) | (value[31]&0xFF);
            convertEegPowerDataToVariant();
            emit eegPowerDataChanged(m_eegPowerDataMap);
            break;

        case 0x083:
            m_asicEegData.delta    = ( (value[0]&0xFF)<<16) |  ((value[1]&0xFF)<<8) |  (value[2]&0xFF);
            m_asicEegData.theta    = ( (value[3]&0xFF)<<16) |  ((value[4]&0xFF)<<8) |  (value[5]&0xFF);
            m_asicEegData.lowAlpha = ( (value[8]&0xFF)<<16) |  ((value[9]&0xFF)<<8) | (value[10]&0xFF);
            m_asicEegData.highAlpha= ((value[12]&0xFF)<<16) | ((value[13]&0xFF)<<8) | (value[14]&0xFF);
            m_asicEegData.lowBeta  = ((value[16]&0xFF)<<16) | ((value[17]&0xFF)<<8) | (value[18]&0xFF);
            m_asicEegData.highBeta = ((value[20]&0xFF)<<16) | ((value[21]&0xFF)<<8) | (value[22]&0xFF);
            m_asicEegData.lowGamma = ((value[24]&0xFF)<<16) | ((value[25]&0xFF)<<8) | (value[26]&0xFF);
            m_asicEegData.midGamma = ((value[28]&0xFF)<<16) | ((value[29]&0xFF)<<8) | (value[30]&0xFF);
            convertAsicEegDataToVariant();
            emit asicEegDataChanged(m_asicEegDataMap);
            break;

        case 0x86:
            m_rrIntervalData = (value[0]<<8) | value[1];
            emit rrIntervalDataChanged(m_rrIntervalData);
            break;

        default:
            qDebug() << "Unrecognized Signal Detected...";
            qDebug() << "EXCODE: " << (value[0] & 0xFF)
                     <<" CODE: "   << code
                     <<" vLength: "<< valueLength;

            qDebug() << "DataValues: ";
            for (int i = 0; i!= valueLength; i++) {
                qDebug() << (value[i] & 0xFF);
            }
            break;
        }
    }
}

//! \brief Write data serially to the open Serial Port
void MindWaveController::writeSerialData(const QByteArray &data) {
    if (data.count()>169) {
        qDebug() << "Payload too large. Max payload size is 169 Bytes";
        return;
    }

    // create ThinkGearPacket
    QByteArray writeData;
    writeData.append(0xAA);              // SYNC BYTE
    writeData.append(0xAA);              // SYNC BYTE
    writeData.append(data.size()&0xFF);  // PAYLOAD LENGTH
    writeData.append(data);              // PAYLOAD

    // calculate CHKSUM
    char chksum = 0;
    for (const auto &x : data) {
        chksum += x;
    }
    chksum &= 0xFF;
    chksum = (~chksum) & 0xFF;

    writeData.append(chksum);  // CHKSUM

    uint64_t bytesWritten = serialPort.write(writeData);
    qDebug() << "MindWaveMobile Data Sent...";

    if (bytesWritten == -1) {
        qDebug() << "Failed to write the data to port";
        return;
    } else if (bytesWritten != writeData.size()) {
        qDebug() << "Failed to write all the data to port";
        return;
    } else if (!serialPort.waitForBytesWritten(5000)) {
        qDebug() << "Operation timed out or an error occurred for port";
        return;
    }

    qDebug() << "Data Write Sucessfull";
    return;
}

//! \brief Reads all available data in the buffer.
//!        This function is triggered by the readReady() signal of QSerialPort
void MindWaveController::read() {
    QByteArray data = serialPort.readAll();

    for (auto &x : data) {
        parseByte(x);
    }
}

uint16_t MindWaveController::getBatteryData() const {
    return m_batteryData;
}

uint16_t MindWaveController::getSignalData() const {
    return m_signalData;
}

uint16_t MindWaveController::getHeartRateData() const {
    return m_heartRateData;
}

uint16_t MindWaveController::getAttentionData() const {
    return m_attentionData;
}

uint16_t MindWaveController::getMeditationData() const {
    return m_meditationData;
}

uint16_t MindWaveController::getRaw8BitData() const {
    return m_raw8BitData;
}

uint16_t MindWaveController::getRaw16BitData() const {
    return m_raw16BitData;
}

uint16_t MindWaveController::getRrIntervalData() const {
    return m_rrIntervalData;
}

QVariantMap MindWaveController::getEegPowerData() {
    convertEegPowerDataToVariant();
    return m_eegPowerDataMap;
}

QVariantMap MindWaveController::getAsicEegData() {
    convertAsicEegDataToVariant();
    return m_asicEegDataMap;
}

void MindWaveController::convertEegPowerDataToVariant() {
    m_eegPowerDataMap["delta"]     = m_eegPowerData.delta;
    m_eegPowerDataMap["theta"]     = m_eegPowerData.theta;
    m_eegPowerDataMap["lowAlpha"]  = m_eegPowerData.lowAlpha;
    m_eegPowerDataMap["highAlpha"] = m_eegPowerData.highAlpha;
    m_eegPowerDataMap["lowBeta"]   = m_eegPowerData.lowBeta;
    m_eegPowerDataMap["highBeta"]  = m_eegPowerData.highBeta;
    m_eegPowerDataMap["lowGamma"]  = m_eegPowerData.lowGamma;
    m_eegPowerDataMap["midGamma"]  = m_eegPowerData.midGamma;
}

void MindWaveController::convertAsicEegDataToVariant() {
    m_asicEegDataMap["delta"]     = static_cast<int>(m_asicEegData.delta);
    m_asicEegDataMap["theta"]     = static_cast<int>(m_asicEegData.theta);
    m_asicEegDataMap["lowAlpha"]  = static_cast<int>(m_asicEegData.lowAlpha);
    m_asicEegDataMap["highAlpha"] = static_cast<int>(m_asicEegData.highAlpha);
    m_asicEegDataMap["lowBeta"]   = static_cast<int>(m_asicEegData.lowBeta);
    m_asicEegDataMap["highBeta"]  = static_cast<int>(m_asicEegData.highBeta);
    m_asicEegDataMap["lowGamma"]  = static_cast<int>(m_asicEegData.lowGamma);
    m_asicEegDataMap["midGamma"]  = static_cast<int>(m_asicEegData.midGamma);
}

int MindWaveController::initParser(uchar parserType,
                      void *customData ) {
    /* Initialize the parser's state based on the parser type */
    switch (parserType) {
        case PARSER_TYPE_PACKETS:
            parser.state = PARSER_STATE_SYNC;
            break;
        case PARSER_TYPE_2BYTERAW:
            parser.state = PARSER_STATE_WAIT_HIGH;
            break;
        default: return( -2 );
    }

    /* Save parser type */
    parser.type = parserType;

    /* Save user-defined handler function and data pointer */
    parser.customData = customData;

    return 0;
}

int MindWaveController::parseByte(uchar byte ) {
    int returnValue = 0;

    /* Pick handling according to current state... */
    switch (parser.state) {
        /* Waiting for SyncByte */
        case PARSER_STATE_SYNC:
            if (byte == PARSER_SYNC_BYTE) {
                parser.state = PARSER_STATE_SYNC_CHECK;
            }
            break;

        /* Waiting for second SyncByte */
        case PARSER_STATE_SYNC_CHECK:
            if (byte == PARSER_SYNC_BYTE) {
                parser.state = PARSER_STATE_PAYLOAD_LENGTH;
            } else {
                parser.state = PARSER_STATE_SYNC;
            }
            break;

        /* Waiting for Data[] length */
        case PARSER_STATE_PAYLOAD_LENGTH:
            parser.payloadLength = byte;
            if (parser.payloadLength > 170) {
                parser.state = PARSER_STATE_SYNC;
                returnValue = -3;
            } else if (parser.payloadLength == 170) {
                returnValue = -4;
            } else {
                parser.payloadBytesReceived = 0;
                parser.payloadSum = 0;
                parser.state = PARSER_STATE_PAYLOAD;
            }
            break;

        /* Waiting for Payload[] bytes */
        case PARSER_STATE_PAYLOAD:
            parser.payload[parser.payloadBytesReceived++] = byte;
            parser.payloadSum = static_cast<uchar>(parser.payloadSum + byte);
            if (parser.payloadBytesReceived >= parser.payloadLength) {
                parser.state = PARSER_STATE_CHKSUM;
            }
            break;

        /* Waiting for CKSUM byte */
        case PARSER_STATE_CHKSUM:
            parser.chksum = byte;
            parser.state = PARSER_STATE_SYNC;
            if (parser.chksum != ((~parser.payloadSum)&0xFF)) {
                returnValue = -2;
            } else {
                returnValue = 1;
                parsePacketPayload();
            }
            break;

        /* Waiting for high byte of 2-byte raw value */
        case PARSER_STATE_WAIT_HIGH:

            /* Check if current byte is a high byte */
            if ((byte & 0xC0) == 0x80) {
                /* High byte recognized, will be saved as parser.lastByte */
                parser.state = PARSER_STATE_WAIT_LOW;
            }
            break;

        /* Waiting for low byte of 2-byte raw value */
        case PARSER_STATE_WAIT_LOW:

            /* Check if current byte is a valid low byte */
            if ((byte & 0xC0) == 0x40) {
                /* Stuff the high and low part of the raw value into an array */
                parser.payload[0] = parser.lastByte;
                parser.payload[1] = byte;

                /* Notify the handler function of received raw value */
                    parseSerialData(0, PARSER_CODE_RAW_SIGNAL, 2,
                                             parser.payload,
                                             parser.customData);

                returnValue = 1;
            }

            /* Return to start state waiting for high */
            parser.state = PARSER_STATE_WAIT_HIGH;

            break;

        /* unrecognized state */
        default:
            parser.state = PARSER_STATE_SYNC;
            returnValue = -5;
            break;
    }

    /* Save current byte */
    parser.lastByte = byte;

    return returnValue;
}

int MindWaveController::parsePacketPayload() {
    uchar i = 0;
    uchar extendedCodeLevel = 0;
    uchar code = 0;
    uchar numBytes = 0;

    /* Parse all bytes from the payload[] */
    while (i < parser.payloadLength) {
        /* Parse possible EXtended CODE bytes */
        while (parser.payload[i] == PARSER_EXCODE_BYTE) {
            extendedCodeLevel++;
            i++;
        }

        /* Parse CODE */
        code = parser.payload[i++];

        /* Parse value length */
        if (code >= 0x80) numBytes = parser.payload[i++];
        else               numBytes = 1;

        /* Call the callback function to handle the DataRow value */
            parseSerialData(extendedCodeLevel, code, numBytes,
                                     parser.payload+i, parser.customData);
        i = static_cast<uchar>(i + numBytes);
    }

    return 0;
}
