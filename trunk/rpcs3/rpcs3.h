#pragma once

/*
template<typename T>
struct safe_realloc
{
	safe_realloc(T* ptr, uint new_size)
	{
		if(new_size != 0)
		{
			ptr = (T*)((ptr == NULL) ? malloc(new_size * sizeof(T)) : realloc(ptr, new_size * sizeof(T)));
		}

		delete this;
	}
};
*/

#define safe_delete(x) free(x);x=NULL

class TheApp : public wxApp
{
public:
	LogFrame* m_log;
	MainFrame* m_main;

    virtual bool OnInit();
};