#include "stdafx.h"
#include "Pad.h"
#include "Emu/SysCalls/ErrorCodes.h"
#include <wx/joystick.h>

static const u32 CELL_PAD_MAX_PORT_NUM = 7;

enum CELL_PAD_ERROR_CODE
{
	CELL_PAD_ERROR_FATAL						= 0x80121101,
	CELL_PAD_ERROR_INVALID_PARAMETER			= 0x80121102,
	CELL_PAD_ERROR_ALREADY_INITIALIZED			= 0x80121103,
	CELL_PAD_ERROR_UNINITIALIZED				= 0x80121104,
	CELL_PAD_ERROR_RESOURCE_ALLOCATION_FAILED	= 0x80121105,
	CELL_PAD_ERROR_DATA_READ_FAILED				= 0x80121106,
	CELL_PAD_ERROR_NO_DEVICE					= 0x80121107,
	CELL_PAD_ERROR_UNSUPPORTED_GAMEPAD			= 0x80121108,
	CELL_PAD_ERROR_TOO_MANY_DEVICES				= 0x80121109,
	CELL_PAD_ERROR_EBUSY						= 0x8012110a,
};

enum PortStatus
{
	CELL_PAD_STATUS_DISCONNECTED	= 0x00000000,
	CELL_PAD_STATUS_CONNECTED		= 0x00000001,
};

enum PortSettings
{
	CELL_PAD_SETTING_PRESS_ON		= 0x00000002,
	CELL_PAD_SETTING_SENSOR_ON		= 0x00000004,
	CELL_PAD_SETTING_PRESS_OFF		= 0x00000000,
	CELL_PAD_SETTING_SENSOR_OFF		= 0x00000000,
};

enum Digital1Flags
{
	CELL_PAD_CTRL_LEFT		= 0x00000080,
	CELL_PAD_CTRL_DOWN		= 0x00000040,
	CELL_PAD_CTRL_RIGHT		= 0x00000020,
	CELL_PAD_CTRL_UP		= 0x00000010,
	CELL_PAD_CTRL_START		= 0x00000008,
	CELL_PAD_CTRL_R3		= 0x00000004,
	CELL_PAD_CTRL_L3		= 0x00000002,
	CELL_PAD_CTRL_SELECT	= 0x00000001,
};

enum Digital2Flags
{
	CELL_PAD_CTRL_SQUARE	= 0x00000080,
	CELL_PAD_CTRL_CROSS		= 0x00000040,
	CELL_PAD_CTRL_CIRCLE	= 0x00000020,
	CELL_PAD_CTRL_TRIANGLE	= 0x00000010,
	CELL_PAD_CTRL_R1		= 0x00000008,
	CELL_PAD_CTRL_L1		= 0x00000004,
	CELL_PAD_CTRL_R2		= 0x00000002,
	CELL_PAD_CTRL_L2		= 0x00000001,
};

enum DeviceCapability
{
	CELL_PAD_CAPABILITY_PS3_CONFORMITY  = 0x00000001, //PS3 Conformity Controller
	CELL_PAD_CAPABILITY_PRESS_MODE      = 0x00000002, //Press mode supported
	CELL_PAD_CAPABILITY_SENSOR_MODE     = 0x00000004, //Sensor mode supported
	CELL_PAD_CAPABILITY_HP_ANALOG_STICK = 0x00000008, //High Precision analog stick
	CELL_PAD_CAPABILITY_ACTUATOR        = 0x00000010, //Motor supported
};

enum DeviceType
{
	CELL_PAD_DEV_TYPE_STANDARD		= 0,
	CELL_PAD_DEV_TYPE_BD_REMOCON	= 4,
	CELL_PAD_DEV_TYPE_LDD			= 5,
};

enum ButtonDataOffset
{
	CELL_PAD_BTN_OFFSET_DIGITAL1		= 2,
	CELL_PAD_BTN_OFFSET_DIGITAL2		= 3,
	CELL_PAD_BTN_OFFSET_ANALOG_RIGHT_X	= 4,
	CELL_PAD_BTN_OFFSET_ANALOG_RIGHT_Y	= 5,
	CELL_PAD_BTN_OFFSET_ANALOG_LEFT_X	= 6,
	CELL_PAD_BTN_OFFSET_ANALOG_LEFT_Y	= 7,
	CELL_PAD_BTN_OFFSET_PRESS_RIGHT		= 8,
	CELL_PAD_BTN_OFFSET_PRESS_LEFT		= 9,
	CELL_PAD_BTN_OFFSET_PRESS_UP		= 10,
	CELL_PAD_BTN_OFFSET_PRESS_DOWN		= 11,
	CELL_PAD_BTN_OFFSET_PRESS_TRIANGLE	= 12,
	CELL_PAD_BTN_OFFSET_PRESS_CIRCLE	= 13,
	CELL_PAD_BTN_OFFSET_PRESS_CROSS		= 14,
	CELL_PAD_BTN_OFFSET_PRESS_SQUARE	= 15,
	CELL_PAD_BTN_OFFSET_PRESS_L1		= 16,
	CELL_PAD_BTN_OFFSET_PRESS_R1		= 17,
	CELL_PAD_BTN_OFFSET_PRESS_L2		= 18,
	CELL_PAD_BTN_OFFSET_PRESS_R2		= 19,
	CELL_PAD_BTN_OFFSET_SENSOR_X		= 20,
	CELL_PAD_BTN_OFFSET_SENSOR_Y		= 21,
	CELL_PAD_BTN_OFFSET_SENSOR_Z		= 22,
	CELL_PAD_BTN_OFFSET_SENSOR_G		= 23,
};

BEGIN_EVENT_TABLE(PadManager, wxWindow)
EVT_JOYSTICK_EVENTS(PadManager::JoyKeyDown)
END_EVENT_TABLE()

PadManager::PadManager()
{
	Close();
	wxGetApp().Connect(wxEVT_KEY_DOWN, wxKeyEventHandler(PadManager::KeyDown), (wxObject*)0, this);
	wxGetApp().Connect(wxEVT_KEY_UP, wxKeyEventHandler(PadManager::KeyUp), (wxObject*)0, this);

	//TODO: joystick
	/*
	wxEVT_JOY_BUTTON_DOWN
    wxEVT_JOY_BUTTON_UP
    wxEVT_JOY_MOVE
    wxEVT_JOY_ZMOVE
	*/
}

PadManager::~PadManager()
{
}

void PadManager::Init(const u32 max_connect)
{
	m_max_connect = max_connect;
	m_now_connect = 1;
	m_system_info = 0;
}

void PadManager::Close()
{
	for(uint i=0; i<m_pads.GetCount(); ++i) m_pads[i].m_buttons.Clear();
	m_pads.Clear();

	m_max_connect = 0;
	m_now_connect = 0;
	m_system_info = 0;
}

void PadManager::JoyKeyDown(wxJoystickEvent& event)
{
	wxPoint pos = event.GetPosition();
	wxJoystick j(event.GetJoystick());

	ConLog.Write("pos[x: 0x%x, y: 0x%x]", pos.x, pos.y);
	ConLog.Write("product ID: 0x%x", j.GetProductId());
	ConLog.Write("product name: %s", j.GetProductName());
}

void PadManager::KeyDown(wxKeyEvent& event)
{
	if(!m_pads.GetCount())
	{
		event.Skip();
		return;
	}

	SetKey(event.GetKeyCode(), true);
}

void PadManager::KeyUp(wxKeyEvent& event)
{
	if(!m_pads.GetCount())
	{
		event.Skip();
		return;
	}
	SetKey(event.GetKeyCode(), false);
}

void PadManager::SetKey(u32 code, bool set)
{
	for(u64 p=0; p<m_pads.GetCount(); ++p)
	{
		for(u64 b=0; b<m_pads[p].m_buttons.GetCount(); b++)
		{
			if(m_pads[p].m_buttons[b].m_pressed == set) continue;
			if(m_pads[p].m_buttons[b].m_keyCode == code)
			{
				if(set) ConLog.Write("Press \"%c\"", code);
				m_pads[p].m_buttons[b].m_pressed = set;
			}
		}
	}
}

void PadManager::LoadSetting()
{
	//TODO: Load from ini
	for(uint i=0; i<CELL_PAD_MAX_PORT_NUM; ++i)
	{
		m_pads.Add(new Pad(
			i==0 ? CELL_PAD_STATUS_CONNECTED : CELL_PAD_STATUS_DISCONNECTED,
			i==0 ? CELL_PAD_SETTING_PRESS_ON | CELL_PAD_SETTING_SENSOR_OFF : 0,
			i==0 ? CELL_PAD_CAPABILITY_PS3_CONFORMITY | CELL_PAD_CAPABILITY_PRESS_MODE : 0,
			CELL_PAD_DEV_TYPE_STANDARD
		));
	}

	m_pads[0].m_buttons.Add(new Button(CELL_PAD_BTN_OFFSET_DIGITAL1, 'A', CELL_PAD_CTRL_LEFT));
	m_pads[0].m_buttons.Add(new Button(CELL_PAD_BTN_OFFSET_DIGITAL1, 'S', CELL_PAD_CTRL_DOWN));
	m_pads[0].m_buttons.Add(new Button(CELL_PAD_BTN_OFFSET_DIGITAL1, 'D', CELL_PAD_CTRL_RIGHT));
	m_pads[0].m_buttons.Add(new Button(CELL_PAD_BTN_OFFSET_DIGITAL1, 'W', CELL_PAD_CTRL_UP));
	m_pads[0].m_buttons.Add(new Button(CELL_PAD_BTN_OFFSET_DIGITAL1, WXK_RETURN, CELL_PAD_CTRL_START));
	m_pads[0].m_buttons.Add(new Button(CELL_PAD_BTN_OFFSET_DIGITAL1, 'X', CELL_PAD_CTRL_R3));
	m_pads[0].m_buttons.Add(new Button(CELL_PAD_BTN_OFFSET_DIGITAL1, 'Z', CELL_PAD_CTRL_L3));
	m_pads[0].m_buttons.Add(new Button(CELL_PAD_BTN_OFFSET_DIGITAL1, WXK_SPACE, CELL_PAD_CTRL_SELECT));

	m_pads[0].m_buttons.Add(new Button(CELL_PAD_BTN_OFFSET_DIGITAL2, 'K', CELL_PAD_CTRL_SQUARE));
	m_pads[0].m_buttons.Add(new Button(CELL_PAD_BTN_OFFSET_DIGITAL2, 'L', CELL_PAD_CTRL_CROSS));
	m_pads[0].m_buttons.Add(new Button(CELL_PAD_BTN_OFFSET_DIGITAL2, ';', CELL_PAD_CTRL_CIRCLE));
	m_pads[0].m_buttons.Add(new Button(CELL_PAD_BTN_OFFSET_DIGITAL2, 'O', CELL_PAD_CTRL_TRIANGLE));
	m_pads[0].m_buttons.Add(new Button(CELL_PAD_BTN_OFFSET_DIGITAL2, 'I', CELL_PAD_CTRL_R1));
	m_pads[0].m_buttons.Add(new Button(CELL_PAD_BTN_OFFSET_DIGITAL2, 'Q', CELL_PAD_CTRL_L1));
	m_pads[0].m_buttons.Add(new Button(CELL_PAD_BTN_OFFSET_DIGITAL2, 'P', CELL_PAD_CTRL_R2));
	m_pads[0].m_buttons.Add(new Button(CELL_PAD_BTN_OFFSET_DIGITAL2, 'E', CELL_PAD_CTRL_L2));
}

int PadManager::cellPadInit(u32 max_connect)
{
	ConLog.Warning("cellPadInit[max_connect: %d]", max_connect);
	if(m_pads.GetCount() != 0) return CELL_PAD_ERROR_ALREADY_INITIALIZED;
	Init(max_connect);
	LoadSetting();
	return CELL_OK;
}

int PadManager::cellPadEnd()
{
	ConLog.Warning("cellPadEnd");
	if(m_pads.GetCount() == 0) return CELL_PAD_ERROR_UNINITIALIZED;
	Close();
	return CELL_OK;
}

int PadManager::cellPadClearBuf(u32 port_no)
{
	ConLog.Warning("cellPadClearBuf[port_no: %d]", port_no);
	if(m_pads.GetCount() == 0) return CELL_PAD_ERROR_UNINITIALIZED;
	if(port_no >= m_pads.GetCount()) return CELL_PAD_ERROR_INVALID_PARAMETER;
	return CELL_OK;
}

int PadManager::cellPadGetData(u32 port_no, u64 data_addr)
{
	//ConLog.Warning("cellPadGetData[port_no: %d, data_addr: 0x%llx]", port_no, data_addr);
	if(m_pads.GetCount() == 0) return CELL_PAD_ERROR_UNINITIALIZED;
	if(port_no >= m_pads.GetCount()) return CELL_PAD_ERROR_INVALID_PARAMETER;

	u16 d1 = 0;
	u16 d2 = 0;

	const ArrayF<Button>& buttons = m_pads[port_no].m_buttons;
	u32 len = 0;
	for(uint i=0; i<buttons.GetCount(); ++i)
	{
		if(!buttons[i].m_pressed) continue;

		switch(buttons[i].m_offset)
		{
		case CELL_PAD_BTN_OFFSET_DIGITAL1: d1 |= buttons[i].m_outKeyCode; len++; break;
		case CELL_PAD_BTN_OFFSET_DIGITAL2: d2 |= buttons[i].m_outKeyCode; len++; break;
		}
	}

	Memory.Write32(data_addr, len); data_addr += 4; //len
	Memory.Write16(data_addr, 0); data_addr += 2; //0
	Memory.Write16(data_addr, 0); data_addr += 2; //1
	Memory.Write16(data_addr, d1); data_addr += 2; //2 - CELL_PAD_BTN_OFFSET_DIGITAL1
	Memory.Write16(data_addr, d2); data_addr += 2; //3 - CELL_PAD_BTN_OFFSET_DIGITAL2
	Memory.Write16(data_addr, 0); data_addr += 2; //4 - CELL_PAD_BTN_OFFSET_ANALOG_RIGHT_X
	Memory.Write16(data_addr, 0); data_addr += 2; //5 - CELL_PAD_BTN_OFFSET_ANALOG_RIGHT_Y
	Memory.Write16(data_addr, 0); data_addr += 2; //6 - CELL_PAD_BTN_OFFSET_ANALOG_LEFT_X
	Memory.Write16(data_addr, 0); data_addr += 2; //7 - CELL_PAD_BTN_OFFSET_ANALOG_LEFT_Y
	Memory.Write16(data_addr, 0); data_addr += 2; //8 - CELL_PAD_BTN_OFFSET_PRESS_RIGHT
	Memory.Write16(data_addr, 0); data_addr += 2; //9 - CELL_PAD_BTN_OFFSET_PRESS_LEFT
	Memory.Write16(data_addr, 0); data_addr += 2; //10 - CELL_PAD_BTN_OFFSET_PRESS_UP
	Memory.Write16(data_addr, 0); data_addr += 2; //11 - CELL_PAD_BTN_OFFSET_PRESS_DOWN
	Memory.Write16(data_addr, 0); data_addr += 2; //12 - CELL_PAD_BTN_OFFSET_PRESS_TRIANGLE
	Memory.Write16(data_addr, 0); data_addr += 2; //13 - CELL_PAD_BTN_OFFSET_PRESS_CIRCLE
	Memory.Write16(data_addr, 0); data_addr += 2; //14 - CELL_PAD_BTN_OFFSET_PRESS_CROSS
	Memory.Write16(data_addr, 0); data_addr += 2; //15 - CELL_PAD_BTN_OFFSET_PRESS_SQUARE
	Memory.Write16(data_addr, 0); data_addr += 2; //16 - CELL_PAD_BTN_OFFSET_PRESS_L1
	Memory.Write16(data_addr, 0); data_addr += 2; //17 - CELL_PAD_BTN_OFFSET_PRESS_R1
	Memory.Write16(data_addr, 0); data_addr += 2; //18 - CELL_PAD_BTN_OFFSET_PRESS_L2
	Memory.Write16(data_addr, 0); data_addr += 2; //19 - CELL_PAD_BTN_OFFSET_PRESS_R2
	Memory.Write16(data_addr, 0); data_addr += 2; //20 - CELL_PAD_BTN_OFFSET_SENSOR_X
	Memory.Write16(data_addr, 0); data_addr += 2; //21 - CELL_PAD_BTN_OFFSET_SENSOR_Y
	Memory.Write16(data_addr, 0); data_addr += 2; //22 - CELL_PAD_BTN_OFFSET_SENSOR_Z
	Memory.Write16(data_addr, 0); data_addr += 2; //23 - CELL_PAD_BTN_OFFSET_SENSOR_G
	for(uint i=24; i<64; ++i, data_addr += 2) Memory.Write16(data_addr, 0);

	return CELL_OK;
}

int PadManager::cellPadGetDataExtra(u32 port_no, u64 device_type_addr, u64 data_addr)
{
	ConLog.Warning("cellPadGetDataExtra[port_no: %d, device_type_addr: 0x%llx, device_type_addr: 0x%llx]", port_no, device_type_addr, data_addr);
	if(m_pads.GetCount() == 0) return CELL_PAD_ERROR_UNINITIALIZED;
	if(port_no >= m_pads.GetCount()) return CELL_PAD_ERROR_INVALID_PARAMETER;
	return CELL_OK;
}

int PadManager::cellPadSetActDirect(u32 port_no, u64 param_addr)
{
	ConLog.Warning("cellPadSetActDirect[port_no: %d, param_addr: 0x%llx]", port_no, param_addr);
	if(m_pads.GetCount() == 0) return CELL_PAD_ERROR_UNINITIALIZED;
	if(port_no >= m_pads.GetCount()) return CELL_PAD_ERROR_INVALID_PARAMETER;
	return CELL_OK;
}

int PadManager::cellPadGetInfo2(u64 info_addr)
{
	ConLog.Warning("cellPadGetInfo2[info_addr: 0x%llx]", info_addr);
	if(m_pads.GetCount() != CELL_PAD_MAX_PORT_NUM) return CELL_PAD_ERROR_UNINITIALIZED;

	Memory.Write32(info_addr, m_max_connect); info_addr+=4;
	Memory.Write32(info_addr, m_now_connect); info_addr+=4;
	Memory.Write32(info_addr, m_system_info); info_addr+=4;
	for(uint i=0; i<m_pads.GetCount(); ++i, info_addr+=4) Memory.Write32(info_addr, m_pads[i].m_port_status);
	for(uint i=0; i<m_pads.GetCount(); ++i, info_addr+=4) Memory.Write32(info_addr, m_pads[i].m_port_setting);
	for(uint i=0; i<m_pads.GetCount(); ++i, info_addr+=4) Memory.Write32(info_addr, m_pads[i].m_device_capability);
	for(uint i=0; i<m_pads.GetCount(); ++i, info_addr+=4) Memory.Write32(info_addr, m_pads[i].m_device_type);

	return CELL_OK;
}

int PadManager::cellPadSetPortSetting(u32 port_no, u32 port_setting)
{
	ConLog.Warning("cellPadSetPortSetting[port_no: %d, port_setting: 0x%x]", port_no, port_setting);
	if(m_pads.GetCount() == 0) return CELL_PAD_ERROR_UNINITIALIZED;
	if(port_no >= m_pads.GetCount()) return CELL_PAD_ERROR_INVALID_PARAMETER;
	m_pads[port_no].m_port_setting = port_setting;
	return CELL_OK;
}