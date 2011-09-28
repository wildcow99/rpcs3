#pragma once

template<typename T> class Array
{
	u64 m_count;
	T* m_array;

public:
	Array()
		: m_count(0)
		, m_array(NULL)
	{
	}

	inline bool RemoveAt(const u64 from, const u64 count = 1)
	{
		if(from + count >= m_count) return false;
		
		/*
		for(uint i=from; i<from+count; ++i)
		{
			free(m_array[i]);
		}
		*/

		memmove(&m_array[from], &m_array[from+count], (m_count-(from+count)) * sizeof(T));

		m_count -= count;
		return true;
	}

	inline void Add(T* data)
	{
		Add(*data);
	}

	inline void Add(T& data)
	{
		if(!m_array)
		{
			m_array = new T[1];
		}
		else
		{
			m_array = (T*)realloc(m_array, sizeof(T) * (m_count + 1));
		}

		m_array[m_count] = data;
		m_count++;
	}

	inline void Clear()
	{
		m_count = 0;
		safe_delete(m_array);
	}

	inline T& Get(u64 num)
	{
		//if(num >= m_count) return *new T();
		return m_array[num];
	}

	u64 GetCount() const { return m_count; }
};

template<typename T> class ArrayF
{
	u64 m_count;
	T** m_array;

public:
	ArrayF()
		: m_count(0)
		, m_array(NULL)
	{
	}

	inline bool RemoveAt(const u64 from, const u64 count = 1)
	{
		if(from + count >= m_count) return false;

		for(uint i=from; i<from+count; ++i)
		{
			free(m_array[i]);
		}

		memmove(&m_array[from], &m_array[from+count], (m_count-(from+count)) * sizeof(T**));

		m_count -= count;
		return true;
	}
	/*
	inline void SetCount(const u64 count)
	{
		while(m_count != count)
		{
			if(m_count < count)
			{
				Add();
			}
			else
			{
				RemoveAt(count, m_count - count);
			}
		}
	}

	inline void Add()
	{
		Add(new T());
	}*/

	inline void Add(T& data)
	{
		Add(&data);
	}

	inline void Add(T* data)
	{
		if(!m_array)
		{
			m_array = (T**)malloc(sizeof(T*));
		}
		else
		{
			m_array = (T**)realloc(m_array, sizeof(T*) * (m_count + 1));
		}

		m_array[m_count] = data;
		m_count++;
	}

	inline void ClearF()
	{
		if(m_count <= 0) return;
		m_count = 0;
		m_array = NULL;
	}

	inline void Clear()
	{
		if(m_count <= 0) return;
		m_count = 0;
		free(*m_array);
		safe_delete(m_array);
	}

	inline T& Get(const u64 num)
	{
		//if(m_count <= num) *m_array[0]; //TODO
		return *m_array[num];
	}

	inline u64 GetCount() const { return m_count; }
};