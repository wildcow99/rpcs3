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
		m_count--;
		T* new_array = new T[m_count];

		memmove(new_array, m_array, from * sizeof(T));
		if(from < m_count)
		{
			const u64 mp = from + count;
			const u64 p = mp > m_count ? from + (mp - (m_count + 1)) : mp;
			memmove(&new_array[p], &m_array[p + 1], (m_count - from) * sizeof(T));
		}

		free(m_array);
		m_array = new_array;

		return true;
	}

	inline void Add(T* data)
	{
		Add(*data)
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
		safe_delete(m_array);
	}

	inline T& Get(u64 num)
	{
		if(num >= m_count) return new T();
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

		m_count -= count;
		T** new_array = (T**)malloc(sizeof(T*) * m_count);

		memmove(&new_array[0], &m_array[0], from * sizeof(T*));
		if(from < m_count)
		{
			const u64 p = from + count;
			memmove(&new_array[p], &m_array[p + 1], (m_count - from) * sizeof(T*));
		}

		free(m_array);
		m_array = new_array;

		return true;
	}

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
		Add(new T(m_count)); //Set ID
	}

	inline void Add(T& data)
	{
		Add(&data)
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

	inline void Clear()
	{
		if(m_count <= 0) return;
		m_count = 0;
		free(*m_array);
		safe_delete(m_array);
	}

	inline T& Get(u64 num)
	{
		if(m_count <= num) *m_array[0]; //TODO
		return *m_array[num];
	}

	u64 GetCount() const { return m_count; }
};