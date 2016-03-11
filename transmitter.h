/*
 * empty.h
 *
 *  Created on: Feb 29, 2016
 *      Author: bryan
 */
#include <driverlib/rf_mailbox.h>
#include <driverlib/rf_common_cmd.h>
#include <driverlib/rf_prop_cmd.h>
#include <ti/drivers/rf/RF.h>
#include <rf_patches/rf_overrides_ieee.h>

#ifndef EMPTY_H_
#define EMPTY_H_

/// \brief defines the largest Tx/Rx payload that the interface can support
#define MAX_DATA_LENGTH        128

#define MAX_ADDR_SIZE           8

/// \brief Structure for the TX Packet
typedef struct {
	uint8_t dstAddr;     /// Dst Address
	/*TODO: should we add acks and hence need the seq number
	 uint8_t seqNum;           ///Sequnce number, if 0 EasyLink
	 */
	uint32_t absTime;        ///Absolute time to Tx packet (0 for immediate)
	///Layer will use last SeqNum used + 1
	uint8_t len;             ///Payload Length
	uint8_t payload[MAX_DATA_LENGTH];       ///Payload
} TxPacket_802_15_4;

// rfc_CMD_RADIO_SETUP_t
rfc_CMD_RADIO_SETUP_t ext_radiosetup =
{
	.commandNo = 0x0802,
	.status = 0x0000,
	.pNextOp = 0,
	.startTime = 0x00000000,
	.startTrigger.triggerType = 0x0,
	.startTrigger.bEnaCmd = 0x0,
	.startTrigger.triggerNo = 0x0,
	.startTrigger.pastTrig = 0x0,
	.condition.rule = 0x1,
	.condition.nSkip = 0x0,
	.mode = 0x01,
	.__dummy0 = 0x00,
	.config.frontEndMode = 0x0,
	.config.biasMode = 0x1, //just changed prev:1
	.config.bNoFsPowerUp = 0x0,
	.txPower = 0x9330,
	.pRegOverride = ieee_overrides_contiki,
};

// CMD_FS
rfc_CMD_FS_t ext_radiofscmd =
{
	.commandNo = 0x0803,
	.status = 0x0000,
	.pNextOp = 0x00000000,
	.startTime = 0x00000000,
	.startTrigger.triggerType = 0x0,
	.startTrigger.bEnaCmd = 0x0,
	.startTrigger.triggerNo = 0x0,
	.startTrigger.pastTrig = 0x0,
	.condition.rule = 0x1,
	.condition.nSkip = 0x0,
	.frequency = 0x0965,
	.fractFreq = 0x0000,
	.synthConf.bTxMode = 0x1, //0: RX mode; 1: TX mode
	.synthConf.refFreq = 0x0,
	.__dummy0 = 0x00,
	.midPrecal = 0x00,
	.ktPrecal = 0x00,
	.tdcPrecal = 0x0000,
};

// CMD_RX_TEST
rfc_CMD_RX_TEST_t ext_radiorxcmd =
{
	.commandNo = 0x0807,
	.status = 0x0000,
	.pNextOp = 0x00000000,
	.startTime = 0x00000000,
	.startTrigger.triggerType = 0x0,
	.startTrigger.bEnaCmd = 0x0,
	.startTrigger.triggerNo = 0x0,
	.startTrigger.pastTrig = 0x0,
	.condition.rule = 0x1,
	.condition.nSkip = 0x0,
	.config.bEnaFifo = 0x0,
	.config.bFsOff = 0x0,
	.config.bNoSync = 0x1,
	.endTrigger.triggerType = 0x1,
	.endTrigger.bEnaCmd = 0x0,
	.endTrigger.triggerNo = 0x0,
	.endTrigger.pastTrig = 0x0,
	.syncWord = 0x71764129,
	.endTime = 0x00000000
};

// Packet TX command
rfc_CMD_IEEE_TX_t ext_packetTXcmd =
{
	.commandNo = 0x2c01,
	.status = 0x0000,
	.pNextOp = 0x00000000,
	.startTime = 0x00000000,
	.startTrigger.triggerType = 0x0,
	.startTrigger.bEnaCmd = 0x0,
	.startTrigger.triggerNo = 0x0,
	.startTrigger.pastTrig = 0x0,
	.condition.rule = 0x1,
	.condition.nSkip = 0x0,
	.txOpt.bIncludePhyHdr = 0x0,
	.txOpt.bIncludeCrc = 0x1,
	.txOpt.payloadLenMsb = 0x1,
	.payloadLen = 0x1e,
	.timeStamp = 0x0,
};

// CMD_TX_TEST
rfc_CMD_TX_TEST_t ext_radioTXtestCmd =
{
	.commandNo = 0x0808,
	.status = 0x0000,
	.pNextOp = 0x00000000,
	.startTime = 0x00000000,
	.startTrigger.triggerType = 0x0,
	.startTrigger.bEnaCmd = 0x0,
	.startTrigger.triggerNo = 0x0,
	.startTrigger.pastTrig = 0x0,
	.condition.rule = 0x1,
	.condition.nSkip = 0x0,
	.config.bUseCw = 0x0,
	.config.bFsOff = 0x0,
	.config.whitenMode = 0x2,
	.__dummy0 = 0x00,
	.txWord = 0xaaaa,    //!<        Value to send to the modem before whitening
	.__dummy1 = 0x00,
	.endTrigger.triggerType = 0x1,
	.endTrigger.bEnaCmd = 0x0,
	.endTrigger.triggerNo = 0x0,
	.endTrigger.pastTrig = 0x0,
	.syncWord = 0x71764129,
	.endTime = 0x00000000,
};

// Packet RX command
rfc_CMD_IEEE_RX_t ext_packetRXcmd =
{
	.commandNo = 0x2801,
	.status = 0x0000,
	.pNextOp = 0x00000000,
	.startTime = 0x00000000,
	.startTrigger.triggerType = 0x0,
	.startTrigger.bEnaCmd = 0x0,
	.startTrigger.triggerNo = 0x0,
	.startTrigger.pastTrig = 0x0,
	.condition.rule = 0x1,
	.condition.nSkip = 0x0,
	.channel = 0x00,

	.rxConfig.bAutoFlushCrc = 0x0,
	.rxConfig.bAutoFlushIgn = 0x0,
	.rxConfig.bIncludePhyHdr = 0x0,
	.rxConfig.bIncludeCrc = 0x0,
	.rxConfig.bAppendRssi = 0x1,
	.rxConfig.bAppendCorrCrc = 0x0,
	.rxConfig.bAppendSrcInd = 0x0,
	.rxConfig.bAppendTimestamp = 0x0,

	.frameFiltOpt.frameFiltEn = 0x0,
	.frameFiltOpt.frameFiltStop = 0x0,
	.frameFiltOpt.autoAckEn = 0x0,
	.frameFiltOpt.slottedAckEn = 0x0,
	.frameFiltOpt.autoPendEn = 0x0,
	.frameFiltOpt.defaultPend = 0x0,
	.frameFiltOpt.bPendDataReqOnly = 0x0,
	.frameFiltOpt.bPanCoord = 0x0,
	.frameFiltOpt.maxFrameVersion = 0x0,
	.frameFiltOpt.fcfReservedMask = 0x0,
	.frameFiltOpt.modifyFtFilter = 0x0,
	.frameFiltOpt.bStrictLenFilter = 0x0,

	.frameTypes.bAcceptFt0Beacon = 0x1,
	.frameTypes.bAcceptFt1Data = 0x1,
	.frameTypes.bAcceptFt2Ack = 0x1,
	.frameTypes.bAcceptFt3MacCmd = 0x1,
	.frameTypes.bAcceptFt4Reserved = 0x1,
	.frameTypes.bAcceptFt5Reserved = 0x1,
	.frameTypes.bAcceptFt6Reserved = 0x1,
	.frameTypes.bAcceptFt7Reserved = 0x1,

	.ccaRssiThr = 0x64,

	.ccaOpt.ccaEnEnergy = 0x0,
	.ccaOpt.ccaEnCorr = 0x0,
	.ccaOpt.ccaEnSync = 0x0,
	.ccaOpt.ccaCorrOp = 0x1,
	.ccaOpt.ccaSyncOp = 0x1,
	.ccaOpt.ccaCorrThr = 0x0,

	.endTrigger.triggerType = 0x1,
	.endTrigger.bEnaCmd = 0x0,
	.endTrigger.triggerNo = 0x0,
	.endTrigger.pastTrig = 0x0,
};

rfc_CMD_STOP_t ext_radiostopcmd =
{
	.commandNo = 0x0402,
};

rfc_CMD_PROP_RADIO_SETUP_t ext_radiopropsetup =
{
	.commandNo = 0x3807,
	.status = 0x0000,
	.pNextOp = 0, // INSERT APPLICABLE POINTER: (uint8_t*)&xxx
	.startTime = 0x00000000,
	.startTrigger.triggerType = 0x0,
	.startTrigger.bEnaCmd = 0x0,
	.startTrigger.triggerNo = 0x0,
	.startTrigger.pastTrig = 0x0,
	.condition.rule = 0x1,
	.condition.nSkip = 0x0,
	.modulation.modType = 0x1,
	.modulation.deviation = 0x64,
	.symbolRate.preScale = 0xf,
	.symbolRate.rateWord = 0x8000,
	.rxBw = 0x24,
	.preamConf.nPreamBytes = 0x4,
	.preamConf.preamMode = 0x0,
	.formatConf.nSwBits = 0x20,
	.formatConf.bBitReversal = 0x0,
	.formatConf.bMsbFirst = 0x1,
	.formatConf.fecMode = 0x0,
	.formatConf.whitenMode = 0x0,
	.config.frontEndMode = 0x0,
	.config.biasMode = 0x1,
	.config.bNoFsPowerUp = 0x0,
	.txPower = 0xa73f,
	.pRegOverride = ieeeIdOverrides,
};

// CMD_FS
rfc_CMD_FS_t RF_cmdFs =
{
	.commandNo = 0x0803,
	.status = 0x0000,
	.pNextOp = 0, // INSERT APPLICABLE POINTER: (uint8_t*)&xxx
	.startTime = 0x00000000,
	.startTrigger.triggerType = 0x0,
	.startTrigger.bEnaCmd = 0x0,
	.startTrigger.triggerNo = 0x0,
	.startTrigger.pastTrig = 0x0,
	.condition.rule = 0x1,
	.condition.nSkip = 0x0,
	.frequency = 0x0965,
	.fractFreq = 0x0000,
	.synthConf.bTxMode = 0x0,
	.synthConf.refFreq = 0x0,
	.__dummy0 = 0x00,
	.midPrecal = 0x00,
	.ktPrecal = 0x00,
	.tdcPrecal = 0x0000,
};

// CMD_PROP_TX
rfc_CMD_PROP_TX_t RF_cmdPropTx =
{
	.commandNo = 0x3801,
	.status = 0x0000,
	.pNextOp = 0, // INSERT APPLICABLE POINTER: (uint8_t*)&xxx
	.startTime = 0x00000000,
	.startTrigger.triggerType = 0x0,
	.startTrigger.bEnaCmd = 0x0,
	.startTrigger.triggerNo = 0x0,
	.startTrigger.pastTrig = 0x0,
	.condition.rule = 0x1,
	.condition.nSkip = 0x0,
	.pktConf.bFsOff = 0x0,
	.pktConf.bUseCrc = 0x1,
	.pktConf.bVarLen = 0x1,
	.pktLen = 0x1e,// SET APPLICATION PAYLOAD LENGTH
	.syncWord = 0x930b51de,
	.pPkt = 0,// INSERT APPLICABLE POINTER: (uint8_t*)&xxx
};

RF_Mode RF_prop =
{
	.rfMode = 0x02,
	.cpePatchFxn = 0,
	.mcePatchFxn = 0,
	.rfePatchFxn = 0,
};

#endif /* EMPTY_H_ */
