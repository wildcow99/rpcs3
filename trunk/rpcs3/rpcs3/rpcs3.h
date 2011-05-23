#pragma once

template<typename T> void safe_realloc( T* ptr, const uint new_size );
#define safe_delete(x) free(x);x=NULL

class TheApp : public wxApp
{
public:
    virtual bool OnInit();
};

//TODO
class MainFrame : public wxFrame
{
};