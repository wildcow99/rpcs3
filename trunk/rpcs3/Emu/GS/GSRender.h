#pragma once

struct GSRender
{
	virtual void Init(const u32 width, const u32 height)=0;
	virtual void SetData(u8* pixels, const u32 width, const u32 height)=0;
	virtual void Draw()=0;
	virtual void Close()=0;
};