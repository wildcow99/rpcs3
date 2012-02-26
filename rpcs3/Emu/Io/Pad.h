#pragma once

struct Button
{
	u32 m_offset;
	u32 m_keyCode;
	u32 m_outKeyCode;
	bool m_pressed;

	Button(u32 offset, u32 keyCode, u32 outKeyCode)
		: m_pressed(false)
		, m_offset(offset)
		, m_keyCode(keyCode)
		, m_outKeyCode(outKeyCode)
	{
	}
};

struct Pad
{
	ArrayF<Button> m_buttons;
	u32 m_port_status;
    u32 m_port_setting;
    u32 m_device_capability;
    u32 m_device_type;

	Pad(u32 port_status, u32 port_setting, u32 device_capability, u32 device_type)
		: m_port_status(port_status)
		, m_port_setting(port_setting)
		, m_device_capability(device_capability)
		, m_device_type(device_type)
	{
	}

	~Pad() { m_buttons.Clear(); }
};

class PadManager : public wxWindow
{
	ArrayF<Pad> m_pads;

	u32 m_max_connect;
    u32 m_now_connect;
    u32 m_system_info;

public:
	PadManager();
	~PadManager();

	void Init(const u32 max_connect);
	void Close();

	void LoadSetting();
	void KeyDown(wxKeyEvent& event);
	void KeyUp(wxKeyEvent& event);
	void SetKey(u32 code, bool set);

	void JoyKeyDown(wxJoystickEvent& event);

	int cellPadInit(u32 max_connect);
	int cellPadEnd();
	int cellPadClearBuf(u32 port_no);
	int cellPadGetData(u32 port_no, u64 data_addr);
	int cellPadGetDataExtra(u32 port_no, u64 device_type_addr, u64 data_addr);
	int cellPadSetActDirect(u32 port_no, u64 param_addr);
	int cellPadGetInfo2(u64 info_addr);
	int cellPadSetPortSetting(u32 port_no, u32 port_setting);

	DECLARE_EVENT_TABLE();
};