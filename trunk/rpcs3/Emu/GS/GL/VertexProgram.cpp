#include "stdafx.h"
#include "VertexProgram.h"

wxString VertexDecompilerThread::GetMask()
{
	wxString ret = wxEmptyString;

	if(d3.vec_writemask_x) ret += "x";
	if(d3.vec_writemask_y) ret += "y";
	if(d3.vec_writemask_z) ret += "z";
	if(d3.vec_writemask_w) ret += "w";

	return ret.IsEmpty() || ret == "xyzw" ? wxEmptyString : ("." + ret);
}

wxString VertexDecompilerThread::GetDST()
{
	static const wxString reg_table[] = 
	{
		"gl_Position",
		"col0", "col1",
		"bfc0", "bfc1",
		"fogc",
		"gl_Pointsize",
		"tc0", "tc1", "tc2", "tc3", "tc4", "tc5", "tc6", "tc7"
	};

	wxString ret = wxEmptyString;

	switch(d3.dst)
	{
	case 0x0: case 0x6:
		ret += reg_table[d3.dst];
	break;

	case 0x1f:
		ret += m_parr.AddParam(PARAM_NONE, "vec4", wxString::Format("tmp%d", d0.dst_tmp));
	break;

	default:
		if(d3.dst < WXSIZEOF(reg_table))
		{
			ret += m_parr.AddParam(PARAM_OUT, "vec4", reg_table[d3.dst]);
		}
		else
		{
			ConLog.Error("Bad dst reg num: %d", d3.dst);
			ret += m_parr.AddParam(PARAM_OUT, "vec4", "unk");
		}
	break;
	}

	return ret;
}

wxString VertexDecompilerThread::GetSRC(const u32 n)
{
	static const wxString reg_table[] = 
	{
		"in_pos", "in_weight", "in_normal",
		"in_col0", "in_col1",
		"in_fogc",
		"in_6", "in_7",
		"in_tc0", "in_tc1", "in_tc2", "in_tc3",
		"in_tc4", "in_tc5", "in_tc6", "in_tc7"
	};

	wxString ret = wxEmptyString;

	switch(src[n].reg_type)
	{
	case 1: //temp
		ret += m_parr.AddParam(PARAM_NONE, "vec4", wxString::Format("tmp%d", src[n].tmp_src));
	break;
	case 2: //input
		if(d1.input_src < WXSIZEOF(reg_table))
		{
			ret += m_parr.AddParam(PARAM_IN, "vec4", reg_table[d1.input_src]);
		}
		else
		{
			ConLog.Error("Bad input src num: %d", d1.input_src);
			ret += m_parr.AddParam(PARAM_IN, "vec4", "in_unk");
		}
	break;
	case 3: //const
		ret += m_parr.AddParam(PARAM_CONST, "vec4", wxString::Format("vc%d", d1.const_src));
	break;

	default:
		ConLog.Error("Bad src%d reg type: %d", n, src[n].reg_type);
		Emu.Pause();
	break;
	}

	static const wxString f[4] = {"x", "y", "z", "w"};

	wxString swizzle = wxEmptyString;
	swizzle += f[src[n].swz_x];
	swizzle += f[src[n].swz_y];
	swizzle += f[src[n].swz_z];
	swizzle += f[src[n].swz_w];
	if(swizzle != "xyzw") ret += "." + swizzle;

	return ret;
}

void VertexDecompilerThread::AddCode(wxString code, bool src_mask)
{
	if(d0.cond == 0) return;
	if(d0.cond != 7)
	{
		ConLog.Error("Bad cond! %d", d0.cond);
		Emu.Pause();
		return;
	}

	if(src_mask)
	{
		code = GetDST() + GetMask() + " = (" + code + ")" + GetMask();
	}
	else
	{
		code = GetDST() + GetMask() + " = " + code;
	}

	main += "\t" + code + ";\n";
}

wxString VertexDecompilerThread::BuildCode()
{
	wxString p = wxEmptyString;

	for(u32 i=0; i<m_parr.params.GetCount(); ++i)
	{
		for(u32 n=0; n<m_parr.params[i].names.GetCount(); ++n)
		{
			p += m_parr.params[i].type;
			p += " ";
			p += m_parr.params[i].names[n];
			p += ";\n";
		}
	}
		
	static const wxString& prot = 
		"#version 330 core\n"
		"\n"
		"%s\n"
		"void main()\n{\n%s}\n";

	return wxString::Format(prot, p, main);
}

wxThread::ExitCode VertexDecompilerThread::Entry()
{
	ConLog.Warning("VertexDecompilerThread::Entry()");
	for(u32 i=0;;)
	{
		d0.HEX = m_data[i++];
		d1.HEX = m_data[i++];
		d2.HEX = m_data[i++];
		d3.HEX = m_data[i++];

		src[0].HEX = d2.src0l | (d1.src0h << 9);
		src[1].HEX = d2.src1;
		src[2].HEX = d3.src2l | (d2.src2h << 11);

		switch(d1.vec_opcode)
		{
		case 0x0: break; //NOP
		case 0x1: AddCode(GetSRC(0)); break; //MOV
		case 0x2: AddCode(GetSRC(0) + " * " + GetSRC(1)); break; //MUL
		case 0x3: AddCode(GetSRC(0) + " + " + GetSRC(1)); break; //ADD
		case 0x4: AddCode(GetSRC(0) + " * " + GetSRC(1) + " + " + GetSRC(2)); break; //MAD
		//case 0x7: AddCode("dot(" + GetSRC(0) + ", " + GetSRC(1) + ").xxxx"); break; //DP4
		case 0x7://DP4
		{
			const wxString src0 = "(" + GetSRC(0) + ")";
			const wxString src1 = "(" + GetSRC(1) + ")";
			wxString result = wxEmptyString;
			result += src0 + ".x * " + src1 + ".x + ";
			result += src0 + ".y * " + src1 + ".y + ";
			result += src0 + ".z * " + src1 + ".z + ";
			result += src0 + ".w * " + src1 + ".w";
			AddCode(result, false);
		}
		break; 
		case 0xe: AddCode("fract(" + GetSRC(0) + ")"); break; //FRC
		case 0xf: AddCode("floor(" + GetSRC(0) + ")"); break; //FLR

		default:
			ConLog.Error("Unknown vp opcode 0x%x", d1.vec_opcode);
			Emu.Pause();
		break;
		}

		if(d3.end) break;
	}

	m_shader = BuildCode();

	return (ExitCode)0;
}

VertexProgram::VertexProgram() : m_decompiler_thread(NULL)
{
}

VertexProgram::~VertexProgram()
{
	if(m_decompiler_thread)
	{
		//m_decompiler_thread->Delete();
		safe_delete(m_decompiler_thread);
	}
}

void VertexProgram::Decompile()
{
	if(m_decompiler_thread)
	{
		//m_decompiler_thread->Delete();
		safe_delete(m_decompiler_thread);
	}

	m_decompiler_thread = new VertexDecompilerThread(data, shader, parr);
	m_decompiler_thread->Create();
	m_decompiler_thread->Run();
}

void VertexProgram::Compile()
{
	id = glCreateShader(GL_VERTEX_SHADER);

	const char* str = shader.c_str();
	const int strlen = shader.Len();

	glShaderSource(id, 1, &str, &strlen);
	glCompileShader(id);

	GLint r;
	glGetShaderiv(id, GL_COMPILE_STATUS, &r);
	char buf[0x1000];
	GLsizei len;
	memset(buf, 0, 0x1000);
	glGetShaderInfoLog(id, 0x1000, &len, buf);
	buf[0xfff] = 0;

	if (r != GL_TRUE)
	{
		ConLog.Write("Failed to compile shader! (%s)", buf);
		Emu.Pause();
		return;
	}

	ConLog.Write("Shader compiled successfully! %s", buf);
}

void VertexProgram::Delete()
{
	data.Clear();
	parr.params.Clear();
	constants4.Clear();
	shader.Clear();
	glDeleteShader(id);
}

void VertexData::Load(u32 start, u32 count)
{
	const u32 tsize = GetTypeSize();
	data.SetCount((start + count) * tsize * size);

	for(u32 i=start; i<start + count; ++i)
	{
		const u8* src = Memory.GetMemFromAddr(addr) + stride * i;
		u8* dst = &data[i * tsize * size];

		switch(tsize)
		{
		case 1:
		{
			const u8* c_src = (const u8*)src;
			u8* c_dst = (u8*)dst;
			for(u32 j=0; j<size; ++j) *c_dst++ = re(*c_src++);
		}
		break;

		case 2:
		{
			const u16* c_src = (const u16*)src;
			u16* c_dst = (u16*)dst;
			for(u32 j=0; j<size; ++j) *c_dst++ = re(*c_src++);
		}
		break;

		case 4:
		{
			const u32* c_src = (const u32*)src;
			u32* c_dst = (u32*)dst;
			for(u32 j=0; j<size; ++j) *c_dst++ = re(*c_src++);
		}
		break;
		}
	}
}

u32 VertexData::GetTypeSize()
{
	switch (type)
	{
	case 1: return 2;
	case 2: return 4;
	case 3: return 2;
	case 4: return 1;
	case 5: return 2;
	case 7: return 1;
	}

	ConLog.Error("Bad vertex data type! %d", type);
	return 1;
}