#ifndef CAN_IDS_H
#define CAN_IDS_H

const int CAN_NODE_ID = 1; //0x001
const int DASH_ID = 2; //0x002
const int VCU_ID = 3; //0x003
const int RIGHT_MOTOR_ID = 485; //0x1e5
const int LEFT_MOTOR_ID = 486; //0x1e6
const int RIGHT_MOTOR_REQUEST_ID = 592; //0x250
const int LEFT_MOTOR_REQUEST_ID = 593; //0x251
const int BMS_SUMMARY_ID = 1536; //0x600
const int BMS_FAULT_ID = 1570; //0x622
const int BMS_VOLTAGE_ID = 1571; //0x623
const int BMS_CURRENT_ID = 1572; //0x624
const int BMS_SOC_ID = 1574; //0x626
const int BMS_TEMP_ID = 1575; //0x627
const int BMS_RESISTANCE_ID = 1576; //0x628

#endif //CAN_IDS_H
