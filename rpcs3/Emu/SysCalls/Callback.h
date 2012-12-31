#pragma once

struct Callback
{
	u64 m_addr;
	u32 m_slot;

	u64 m_status;
	u64 m_param;
	u64 m_userdata;
	bool m_has_data;

	Callback(u32 slot, u64 addr, u64 userdata);
	void Handle(u64 status, u64 param);
	void Branch();
};

struct Callbacks
{
	Array<Callback> m_callbacks;
	bool m_in_manager;

	Callbacks() : m_in_manager(false)
	{
	}

	void Register(u32 slot, u64 addr, u64 userdata)
	{
		Unregister(slot);
		m_callbacks.AddCpy(Callback(slot, addr, userdata));
	}

	void Unregister(u32 slot)
	{
		for(u32 i=0; i<m_callbacks.GetCount(); ++i)
		{
			if(m_callbacks[i].m_slot == slot)
			{
				m_callbacks.RemoveAt(i);
				break;
			}
		}
	}

	void Handle(u64 status, u64 param)
	{
		for(u32 i=0; i<m_callbacks.GetCount(); ++i)
		{
			m_callbacks[i].Handle(status, param);
		}
	}

	bool Check()
	{
		bool handled = false;

		for(u32 i=0; i<m_callbacks.GetCount(); ++i)
		{
			if(m_callbacks[i].m_has_data)
			{
				handled = true;
				m_callbacks[i].Branch();
			}
		}

		return handled;
	}
};

struct CallbackManager
{
	ArrayF<Callbacks> m_callbacks;
	Callbacks m_exit_callback;

	void Add(Callbacks& c)
	{
		if(c.m_in_manager) return;

		c.m_in_manager = true;
		m_callbacks.Add(c);
	}

	void Init()
	{
		Add(m_exit_callback);
	}

	void Clear()
	{
		for(u32 i=0; i<m_callbacks.GetCount(); ++i)
		{
			m_callbacks[i].m_callbacks.Clear();
			m_callbacks[i].m_in_manager = false;
		}

		m_callbacks.ClearF();
	}
};