#pragma once
#include "Array.h"

typedef u8 ID_TYPE;

struct ID
{
	bool used;
	wxString name;
	u8 attr;

	ID(bool _used = false, const wxString& _name = wxEmptyString, const u8 _attr = 0)
		: used(_used)
		, name(_name)
		, attr(_attr)
	{
	}
};

class IdManager
{
	ArrayF<ID> IDs;

	void Cleanup()
	{
		for(int i=IDs.GetCount()-1; i >= 0; i--)
		{
			ID& id = IDs.Get(i);
			if(id.used) break;
			IDs.RemoveAt(i);
		}
	}
	
public:
	IdManager()
	{
	}
	
	~IdManager()
	{
		Clear();
	}
	
	static ID_TYPE GetMaxID()
	{
		return (ID_TYPE)-1;
	}
	
	bool CheckID(const ID_TYPE id)
	{
		return id != 0 && id < NumToID(IDs.GetCount());
	}
	
	__forceinline const ID_TYPE NumToID(const ID_TYPE num) const
	{
		return num + 1;
	}
	
	__forceinline const ID_TYPE IDToNum(const ID_TYPE id) const
	{
		return id - 1;
	}

	void Clear()
	{
		IDs.Clear();
	}
	
	ID_TYPE GetNewID(const wxString& name = wxEmptyString, const u8 attr = 0)
	{
		for(uint i=0; i<IDs.GetCount(); ++i)
		{
			if(IDs.Get(i).used) continue;
			return NumToID(i);
		}

		const ID_TYPE pos = IDs.GetCount();
		if(NumToID(pos) >= GetMaxID()) return 0;
		IDs.Add(new ID(true, name, attr));
		return NumToID(pos);
	}
	
	ID& GetIDData(const ID_TYPE _id)
	{
		if(!CheckID(_id)) return IDs.Get(0); //TODO
		return IDs.Get(IDToNum(_id));
	}
	
	bool RemoveID(const ID_TYPE _id)
	{
		if(!CheckID(_id)) return false;
		ID& id = IDs.Get(IDToNum(_id));
		if(!id.used) return false;
		id.used = false;
		id.attr = 0;
		id.name = wxEmptyString;
		if(IDToNum(_id) == IDs.GetCount()-1) Cleanup();
		return true;
	}
};