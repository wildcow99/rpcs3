#include "stdafx.h"
#include "rpcs3.h"

template<typename T>
void safe_realloc( T* ptr, const uint new_size )
{
	if(new_size == 0) return;
	ptr = (T*)((ptr == NULL) ? malloc(new_size * sizeof(T)) : realloc(ptr, new_size * sizeof(T)));
}

IMPLEMENT_APP(TheApp)

bool TheApp::OnInit()
{
	return true;
}