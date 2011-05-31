#pragma once

enum PadStatus
{
	PAD_Dissconnected = 0,
	PAD_Connected = 1,
	PAD_LDD_Connected = 5,
};

enum PadOffset
{
	PAD_btn_Offset_Digital1 = 2,
	PAD_btn_Offset_Digital2 = 3,

	PAD_btn_Offset_Analog_Right_X = 4,
	PAD_btn_Offset_Analog_Right_Y = 5,
	PAD_btn_Offset_Analog_Left_X  = 6,
	PAD_btn_Offset_Analog_Left_Y  = 7,

	PAD_btn_Offset_Press_Right    = 8,
	PAD_btn_Offset_Press_Left     = 9,
	PAD_btn_Offset_Press_Up       = 10,
	PAD_btn_Offset_Press_Down     = 11,
	PAD_btn_Offset_Press_Triangle = 12,
	PAD_btn_Offset_Press_Circle   = 13,
	PAD_btn_Offset_Press_Cross    = 14,
	PAD_btn_Offset_Press_Square   = 15,
	PAD_btn_Offset_Press_L1       = 16,
	PAD_btn_Offset_Press_R1       = 17,
	PAD_btn_Offset_Press_L2       = 18,
	PAD_btn_Offset_Press_R2       = 19,

	PAD_btn_Offset_Sensor_X       = 20,
	PAD_btn_Offset_Sensor_Y       = 21,
	PAD_btn_Offset_Sensor_Z       = 22,
	PAD_btn_Offset_Sensor_G       = 23,
};

enum PadCtrl
{
	PAD_Ctrl_LEFT		= (1 << 7),
	PAD_Ctrl_DOWN		= (1 << 6),
	PAD_Ctrl_RIGHT		= (1 << 5),
	PAD_Ctrl_UP			= (1 << 4),
	PAD_Ctrl_START		= (1 << 3),
	PAD_Ctrl_R3			= (1 << 2),
	PAD_Ctrl_L3			= (1 << 1),
	PAD_Ctrl_SELECT		= (1 << 0),

	PAD_Ctrl_SQUARE		= (1 << 7),
	PAD_Ctrl_CROSS		= (1 << 6),
	PAD_Ctrl_CIRCLE		= (1 << 5),
	PAD_Ctrl_TRIANGLE	= (1 << 4),
	PAD_Ctrl_R1			= (1 << 3),
	PAD_Ctrl_L1			= (1 << 2),
	PAD_Ctrl_R2			= (1 << 1),
	PAD_Ctrl_L2			= (1 << 0),
};

enum PadPressMode
{
	PAD_PressMode_OFF = 0,
	PAD_PressMode_ON  = 1,
};

enum PadSensorMode
{
	PAD_SensorMode_OFF = 0,
	PAD_SensorMode_ON  = 1,
};

enum PadCapabilityOffset
{
	PAD_Capability_Offset_Func1 = 0,
	PAD_Capability_Offset_Func2 = 1,
	PAD_Capability_Reserved = 2
};

enum PadCapability
{
	PAD_Capability_PS3_Conformity	= (1 << 0),
	PAD_Capability_PressMode		= (1 << 1),
	PAD_Capability_SensorMode		= (1 << 2),
	PAD_Capability_HighPrecision	= (1 << 3),
};