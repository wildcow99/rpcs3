#pragma once
#include "VertexProgram.h"
#include "FragmentProgram.h"

struct Program
{
	u32 id;
	void Create(const u32 vp, const u32 fp);
	void Use();
	void SetTex(u32 index);
	void Delete();
};