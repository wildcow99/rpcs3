#include "stdafx.h"
#include "GLGSRender.h"

#if	0
	#define CMD_LOG ConLog.Write
#else
	#define CMD_LOG(...)
#endif

gcmBuffer gcmBuffers[2];

static VBO m_vbo;
static u32 m_vao_id;

static ShaderProgram m_shader_prog;
static VertexData m_vertex_data[16];
static VertexProgram m_vertex_progs[16];
static VertexProgram* m_cur_vertex_prog;
static Program m_program;

void checkForGlError(const char* situation)
{
	GLenum err = glGetError();
	if(err != GL_NO_ERROR)
	{
		ConLog.Error("%s: opengl error 0x%04x", situation, err);
		Emu.Pause();
	}
}

GLGSFrame::GLGSFrame() : GSFrame(NULL, "GSFrame[OpenGL]")
{
	canvas = new wxGLCanvas(this, wxID_ANY, wxDefaultPosition);
	canvas->SetCurrent();

	glEnable(GL_TEXTURE_2D);

	Flip();

	//Connect(canvas->GetId(), wxEVT_LEFT_DCLICK, wxMouseEventHandler(GLGSFrame::OnLeftDclick));
	//Connect(wxEVT_LEFT_DCLICK, wxMouseEventHandler(GLGSFrame::OnLeftDclick));
}

void GLGSFrame::OnSize(wxSizeEvent& event)
{
	canvas->SetSize(GetClientSize());
	event.Skip();
}

void GLGSFrame::SetViewport(wxPoint pos, wxSize size)
{
	//ConLog.Warning("SetViewport(x=%d, y=%d, w=%d, h=%d)", pos.x, pos.y, size.GetWidth(), size.GetHeight());
}

GLGSRender::GLGSRender() : m_frame(NULL)
{
	m_update_timer = new wxTimer(this);
	Connect(m_update_timer->GetId(), wxEVT_TIMER, wxTimerEventHandler(GLGSRender::OnTimer));
	m_frame = new GLGSFrame();
	m_vao_id = 0;

	m_frame->GetCanvas()->SetCurrent();
	InitProcTable();

	m_vbo.Create();
	if(!m_vao_id) glGenVertexArrays(1, &m_vao_id);
}

GLGSRender::~GLGSRender()
{
	Close();
	m_frame->Close();
}

void GLGSRender::Enable(bool enable, const u32 cap)
{
	if(enable) glEnable(cap);
	else glDisable(cap);
}

void GLGSRender::Init(const u32 ioAddress, const u32 ctrlAddress)
{
	if(m_frame->IsShown()) return;

	m_frame->Show();

	m_ioAddress = ioAddress;
	m_ctrlAddress = ctrlAddress;
	m_ctrl = (CellGcmControl*)Memory.GetMemFromAddr(m_ctrlAddress);

	m_update_timer->Start(1);
}

void GLGSRender::Draw()
{
	//if(m_frame && !m_frame->IsBeingDeleted()) m_frame->Flip();
}

void GLGSRender::Close()
{
	m_frame->GetCanvas()->SetCurrent();

	m_vbo.Delete();
	m_cur_vertex_prog = 0;
	m_program.Delete();
	m_shader_prog.Delete();

	for(u32 i=0; i<16; ++i)
	{
		m_vertex_progs[i].Delete();
		memset(&m_vertex_data[i], 0, sizeof(m_vertex_data[i]));
	}

	m_update_timer->Stop();
	if(m_frame->IsShown()) m_frame->Hide();
	m_ctrl = NULL;
}

enum Method
{
	CELL_GCM_METHOD_FLAG_NON_INCREMENT	= 0x40000000,
	CELL_GCM_METHOD_FLAG_JUMP			= 0x20000000,
	CELL_GCM_METHOD_FLAG_CALL			= 0x00000002,
	CELL_GCM_METHOD_FLAG_RETURN			= 0x00020000,
};

void GLGSRender::OnTimer(wxTimerEvent&)
{
	while(m_ctrl->get != m_ctrl->put)
	{
		//static Array<u32> call_stack;

		const u32 get = re(m_ctrl->get);
		const u32 cmd = Memory.Read32(m_ioAddress + get);
		const u32 count = (cmd >> 18) & 0x7ff;
		mem32_t data(m_ioAddress + get + 4);

		/*
		if(cmd & CELL_GCM_METHOD_FLAG_JUMP)
		{
			m_ctrl->get = re32(cmd & ~(CELL_GCM_METHOD_FLAG_JUMP | CELL_GCM_METHOD_FLAG_NON_INCREMENT));
			return;//continue;
		}
		if(cmd & CELL_GCM_METHOD_FLAG_CALL)
		{
			call_stack.AddCpy(get + 4);
			m_ctrl->get = re32(cmd & ~CELL_GCM_METHOD_FLAG_CALL);
			return;//continue;
		}
		if(cmd & CELL_GCM_METHOD_FLAG_RETURN)
		{
			const u32 pos = call_stack.GetCount() - 1;
			m_ctrl->get = re32(call_stack[pos]);
			call_stack.RemoveAt(pos);
			return;//continue;
		}
		if(cmd & CELL_GCM_METHOD_FLAG_NON_INCREMENT)
		{
			//ConLog.Error("non increment cmd! 0x%x", cmd);
		}
		*/

		m_ctrl->get = re32(get + (count + 1) * 4);

		DoCmd(cmd, cmd & 0x3ffff, data, count);
		memset(Memory.GetMemFromAddr(m_ioAddress + get), 0, (count + 1) * 4);
	}
}

#define case_16(a, m) \
	case a + m: \
	case a + m * 2: \
	case a + m * 3: \
	case a + m * 4: \
	case a + m * 5: \
	case a + m * 6: \
	case a + m * 7: \
	case a + m * 8: \
	case a + m * 9: \
	case a + m * 10: \
	case a + m * 11: \
	case a + m * 12: \
	case a + m * 13: \
	case a + m * 14: \
	case a + m * 15: \
	index = (cmd - a) / m; \
	case a \


void GLGSRender::DoCmd(const u32 fcmd, const u32 cmd, mem32_t& args, const u32 count)
{
	static int draw_mode = 0;
	u32 index = 0;
	m_frame->GetCanvas()->SetCurrent();

	static u32 draw_array_count = 0;

	switch(cmd)
	{
	case NV4097_NO_OPERATION:
	break;

	case NV406E_SET_REFERENCE:
		m_ctrl->ref = re32(args[0]);
	break;

	case_16(NV4097_SET_TEXTURE_OFFSET, 32):
	{
		GLTexture& tex = m_frame->GetTexture(index);
		const u32 offset = args[0];
		const u8 location = args[1] & 0x3 - 1;
		const bool cubemap = (args[1] >> 2) & 0x1;
		const u8 dimension = (args[1] >> 4) & 0xf;
		const u8 format = (args[1] >> 8) & 0xff;
		const u16 mipmap = (args[1] >> 16) & 0xffff;
		CMD_LOG("offset=0x%x, location=0x%x, cubemap=0x%x, dimension=0x%x, format=0x%x, mipmap=0x%x",
			offset, location, cubemap, dimension, format, mipmap);

		tex.SetOffset(GetAddress(offset, location));
		tex.SetFormat(cubemap, dimension, format, mipmap);
	}
	break;

	case_16(NV4097_SET_TEXTURE_IMAGE_RECT, 32):
	{
		GLTexture& tex = m_frame->GetTexture(index);

		const u16 height = args[0] & 0xffff;
		const u16 width = args[0] >> 16;
		CMD_LOG("width=%d, height=%d", width, height);
		tex.SetRect(width, height);
	}
	break;

	case_16(NV4097_SET_TEXTURE_CONTROL0, 32):
	{
		GLTexture& tex = m_frame->GetTexture(index);
		tex.Enable(args[0] >> 31 ? true : false);
	}
	break;

	case NV4097_SET_SURFACE_FORMAT:
	{
		const u32 color_format = args[0] & 0x1f;
		const u32 depth_format = (args[0] >> 5) & 0x7;
		const u32 type = (args[0] >> 8) & 0xf;
		const u32 antialias = (args[0] >> 12) & 0xf;
		const u32 width = (args[0] >> 16) & 0xff;
		const u32 height = (args[0] >> 24) & 0xff;
		const u32 pitch_a = args[1];
		const u32 offset_a = args[2];
		const u32 offset_z = args[3];
		const u32 offset_b = args[4];
		const u32 pitch_b = args[5];

		CMD_LOG("color_format=%d, depth_format=%d, type=%d, antialias=%d, width=%d, height=%d, pitch_a=%d, offset_a=0x%x, offset_z=0x%x, offset_b=0x%x, pitch_b=%d",
			color_format, depth_format, type, antialias, width, height, pitch_a, offset_a, offset_z, offset_b, pitch_b);
	}
	break;

	case NV4097_SET_COLOR_MASK:
	{
		const u32 flags = args[0];

		glColorMask(
			flags & 0x0010000 ? GL_TRUE : GL_FALSE,
			flags & 0x0000100 ? GL_TRUE : GL_FALSE,
			flags & 0x0000001 ? GL_TRUE : GL_FALSE,
			flags & 0x1000000 ? GL_TRUE : GL_FALSE);
	}
	break;

	case NV4097_SET_COLOR_MASK_MRT:
	{
	}
	break;

	case NV4097_SET_ALPHA_TEST_ENABLE:
		Enable(args[0] ? true : false, GL_ALPHA_TEST);
	break;

	case NV4097_SET_BLEND_ENABLE:
		Enable(args[0] ? true : false, GL_BLEND);
	break;

	case NV4097_SET_DEPTH_BOUNDS_TEST_ENABLE:
		Enable(args[0] ? true : false, GL_DEPTH_CLAMP);
	break;

	case NV4097_SET_VIEWPORT_HORIZONTAL:
	{
		const u16 x = args[0] & 0xffff;
		const u16 w = args[0] >> 16;
		const u16 y = args[1] & 0xffff;
		const u16 h = args[1] >> 16;
		CMD_LOG("x=%d, y=%d, w=%d, h=%d", x, y, w, h);
		glViewport(x, y, w, h);
	}
	break;

	case NV4097_SET_CLIP_MIN:
	{
		const u32 clip_min = args[0];
		const u32 clip_max = args[1];

		CMD_LOG("clip_min=%.01f, clip_max=%.01f", *(float*)&clip_min, *(float*)&clip_max);

		glDepthRangef(*(float*)&clip_min, *(float*)&clip_max);
	}
	break;

	case NV4097_SET_DEPTH_FUNC:
		glDepthFunc(args[0]);
	break;

	case NV4097_SET_DEPTH_TEST_ENABLE:
		Enable(args[0] ? true : false, GL_DEPTH_TEST);
	break;

	case NV4097_SET_FRONT_POLYGON_MODE:
		glPolygonMode(GL_FRONT, args[0]);
	break;

	case 0x1d94: //clear surface
	{
		const u32 mask = args[0];
		GLbitfield f = 0;
		if (mask & 0x1) f |= GL_DEPTH_BUFFER_BIT;
		if (mask & 0x2) f |= GL_STENCIL_BUFFER_BIT;
		if (mask & 0x10) f |= GL_COLOR_BUFFER_BIT;
		glClear(f);
	}
	break;

	case NV4097_SET_BLEND_FUNC_SFACTOR:
	{
		const u16 src_rgb = args[0] & 0xffff;
		const u16 dst_rgb = args[0] >> 16;
		const u16 src_alpha = args[1] & 0xffff;
		const u16 dst_alpha = args[1] >> 16;
		CMD_LOG("src_rgb=0x%x, dst_rgb=0x%x, src_alpha=0x%x, dst_alpha=0x%x",
			src_rgb, dst_rgb, src_alpha, dst_alpha);

		glBlendFuncSeparate(src_rgb, dst_rgb, src_alpha, dst_alpha);
	}
	break;

	case_16(NV4097_SET_VERTEX_DATA_ARRAY_OFFSET, 4):
	{
		const u32 addr = GetAddress(args[0] & 0x7fffffff, args[0] >> 31);
		CMD_LOG("num=%d, addr=0x%x", index, addr);

		m_vertex_data[index].addr = addr;
	}
	break;

	case_16(NV4097_SET_VERTEX_DATA_ARRAY_FORMAT, 4):
	{
		const u16 frequency = args[0] >> 16;
		const u8 stride = (args[0] >> 8) & 0xff;
		const u8 size = (args[0] >> 4) & 0xf;
		const u8 type = args[0] & 0xf;

		CMD_LOG("index=%d, frequency=%d, stride=%d, size=%d, type=%d",
			index, frequency, stride, size, type);

		VertexData& cv = m_vertex_data[index];

		cv.frequency = frequency;
		cv.stride = stride;
		cv.size = size;
		cv.type = type;
	}
	break;

	case NV4097_DRAW_ARRAYS:
	{
		for(u32 i=0; i<count; ++i)
		{
			const u32 first = args[i] & 0xffffff;
			const u32 _count = (args[i] >> 24) + 1;

			CMD_LOG("draw array[%d](first=%d, count=%d)", i+1, first, _count);

			for(u32 j=0; j<16; ++j)
			{
				if(m_vertex_data[j].IsEnabled()) m_vertex_data[j].Load(first, _count);
			}

			draw_array_count += _count;
		}
	}
	break;

	case NV4097_SET_BEGIN_END:
	{
		if(args[0]) //begin
		{
			draw_mode = args[0] - 1;
		}
		else //end
		{
			{
				m_shader_prog.Wait();
				m_shader_prog.Compile();

				wxFile f(wxGetCwd() + "/FragmentProgram.txt", wxFile::write);
				f.Write(m_shader_prog.shader);
			}
			{
				m_cur_vertex_prog->Wait();
				m_cur_vertex_prog->Compile();

				wxFile f(wxGetCwd() + "/VertexProgram.txt", wxFile::write);
				f.Write(m_cur_vertex_prog->shader);
			}

			m_program.Create(m_cur_vertex_prog->id, m_shader_prog.id);
			m_program.Use();

			for(u32 i=0; i<m_cur_vertex_prog->constants4.GetCount(); ++i)
			{
				const VertexProgram::Constant4& c = m_cur_vertex_prog->constants4[i];
				const wxString& name = wxString::Format("vc%d", c.id);
				const int l = glGetUniformLocation(m_program.id, name);
				checkForGlError("glGetUniformLocation " + name);

				ConLog.Write(name + " x: %.02f y: %.02f z: %.02f w: %.02f", c.x, c.y, c.z, c.w);
				glUniform4f(l, c.x, c.y, c.z, c.w);
				checkForGlError("glUniform4f " + name);
			}

			for(u32 i=0; i<16; ++i)
			{
				GLTexture& tex = m_frame->GetTexture(i);
				if(!tex.IsEnabled()) continue;
				glActiveTexture(GL_TEXTURE0_ARB + i);
				m_program.SetTex(i);
				tex.Init();
				tex.Save();
			}

			ConLog.Warning("End!");
			Array<u32> offset_list;
			Array<u8> data;
			u32 cur_offset = 0;

			for(u32 i=0; i<16; ++i)
			{
				offset_list.AddCpy(cur_offset);

				if(!m_vertex_data[i].IsEnabled()) continue;

				cur_offset += m_vertex_data[i].data.GetCount();
				const u32 pos = data.GetCount();
				data.SetCount(pos + m_vertex_data[i].data.GetCount());
				memcpy(&data[pos], &m_vertex_data[i].data[0], m_vertex_data[i].data.GetCount());
			}

			glBindVertexArray(m_vao_id);
			m_vbo.SetData(&data[0], data.GetCount());

			wxFile dump("VertexDataArray.dump", wxFile::write);
				
			for(u32 i=0; i<16; ++i)
			{
				if(!m_vertex_data[i].IsEnabled()) continue;

				dump.Write(wxString::Format("VertexData[%d]:\n", i));

				u32 gltype;
				bool normalized = false;

				switch(m_vertex_data[i].type)
				{
				case 1:
					gltype = GL_SHORT; normalized = true;
					for(u32 j = 0; j<m_vertex_data[i].data.GetCount(); j+=2)
					{
						dump.Write(wxString::Format("%d\n", *(u16*)&data[offset_list[i] + j]));
						if(!(((j+2) / 2) % m_vertex_data[i].size)) dump.Write("\n");
					}
				break;

				case 2:
					gltype = GL_FLOAT;
					for(u32 j = 0; j<m_vertex_data[i].data.GetCount(); j+=4)
					{
						dump.Write(wxString::Format("%.01f\n", *(float*)&data[offset_list[i] + j]));
						if(!(((j+4) / 4) % m_vertex_data[i].size)) dump.Write("\n");
					}
				break;

				case 3:
					gltype = GL_HALF_FLOAT;
					for(u32 j = 0; j<m_vertex_data[i].data.GetCount(); j+=2)
					{
						dump.Write(wxString::Format("%.01f\n", *(float*)&data[offset_list[i] + j]));
						if(!(((j+2) / 2) % m_vertex_data[i].size)) dump.Write("\n");
					}
				break;

				case 4:
					gltype = GL_UNSIGNED_BYTE; normalized = true;
					for(u32 j = 0; j<m_vertex_data[i].data.GetCount(); ++j)
					{
						dump.Write(wxString::Format("%d\n", data[offset_list[i] + j]));
						if(!((j+1) % m_vertex_data[i].size)) dump.Write("\n");
					}
				break;

				case 5:
					gltype = GL_SHORT;
					for(u32 j = 0; j<m_vertex_data[i].data.GetCount(); j+=2)
					{
						dump.Write(wxString::Format("%d\n", *(u16*)&data[offset_list[i] + j]));
						if(!(((j+2) / 2) % m_vertex_data[i].size)) dump.Write("\n");
					}
				break;

				case 7:
					gltype = GL_UNSIGNED_BYTE;
					for(u32 j = 0; j<m_vertex_data[i].data.GetCount(); ++j)
					{
						dump.Write(wxString::Format("%d\n", data[offset_list[i] + j]));
						if(!((j+1) % m_vertex_data[i].size)) dump.Write("\n");
					}
				break;

				default:
					ConLog.Error("Bad cv type! %d", m_vertex_data[i].type);
				break;
				}

				checkForGlError("glGetAttribLocation");
				glVertexAttribPointer(i, m_vertex_data[i].size, gltype, normalized, 0, (void*)offset_list[i]);
				checkForGlError("glVertexAttribPointer");
				glEnableVertexAttribArray(i);
				checkForGlError("glEnableVertexAttribArray");

				dump.Write("\n");
			}

			glDrawArrays(draw_mode, 0, draw_array_count);
			checkForGlError("glDrawArrays");

			m_frame->Flip();
			for(u32 i=0; i<16; ++i)
			{
				if(m_vertex_data[i].IsEnabled()) m_vertex_data[i].data.Clear();
			}

			memset(m_vertex_data, 0, sizeof(VertexData));

			draw_array_count = 0;

			m_vbo.UnBind();
			m_program.Delete();
			m_cur_vertex_prog->Delete();
			m_shader_prog.Delete();

			m_flip_status = 0;
		}
	}
	break;

	case NV4097_SET_COLOR_CLEAR_VALUE:
	{
		const u8 a = (args[0] >> 24) & 0xff;
		const u8 r = (args[0] >> 16) & 0xff;
		const u8 g = (args[0] >> 8) & 0xff;
		const u8 b = args[0] & 0xff;
		CMD_LOG("a=%d, r=%d, g=%d, b=%d", a, r, g, b);
		glClearColor(
			(float)r / 255.0f,
			(float)g / 255.0f,
			(float)b / 255.0f,
			(float)a / 255.0f);
	}
	break;

	case NV4097_SET_SHADER_PROGRAM:
	{
		m_shader_prog.addr = GetAddress(args[0] & ~0x3, (args[0] & 0x3) - 1);
		//m_shader_prog.DetectSize();
		m_shader_prog.Decompile();
	}
	break;

	case NV4097_SET_VERTEX_ATTRIB_OUTPUT_MASK:
	{
		//VertexData[0].prog.attributeOutputMask = args[0];
		//FragmentData.prog.attributeInputMask = args[0]/* & ~0x20*/;
	}
	break;

	case NV4097_SET_SHADER_CONTROL:
	{
		const u32 arg0 = args[0];

		//const u8 controlTxp = (arg0 >> 15) & 0x1;
		//FragmentData.prog.registerCount = arg0 >> 24;
		//FragmentData.prog.
	}
	break;

	case NV4097_SET_TRANSFORM_PROGRAM_LOAD:
	{
		m_cur_vertex_prog = &m_vertex_progs[args[0]];
		m_cur_vertex_prog->data.Clear();
		m_cur_vertex_prog->constants4.Clear();

		if(count == 2)
		{
			const u32 start = args[1];
		}
	}
	break;

	case NV4097_SET_TRANSFORM_PROGRAM:
	{
		for(u32 i=0; i<count; ++i) m_cur_vertex_prog->data.AddCpy(args[i]);
	}
	break;

	case NV4097_SET_TRANSFORM_TIMEOUT:
		m_cur_vertex_prog->Decompile();
	break;

	case NV4097_SET_VERTEX_ATTRIB_INPUT_MASK:
		//VertexData[0].prog.attributeInputMask = args[0];
	break;

	case NV4097_SET_TRANSFORM_CONSTANT_LOAD:
	{
		if((count - 1) % 4)
		{
			CMD_LOG("NV4097_SET_TRANSFORM_CONSTANT_LOAD [%d]", count);
			break;
		}

		for(u32 id = args[0], i = 1; i<count; ++id)
		{
			const u32 x = args[i++];
			const u32 y = args[i++];
			const u32 z = args[i++];
			const u32 w = args[i++];

			VertexProgram::Constant4 c;
			c.id = id;
			c.x = *(float*)&x;
			c.y = *(float*)&y;
			c.z = *(float*)&z;
			c.w = *(float*)&w;

			m_cur_vertex_prog->constants4.AddCpy(c);
		}
	}
	break;

	default:
	{
		wxString log = getMethodName(cmd);
		log += "(";
		for(u32 i=0; i<count; ++i) log += (i ? ", " : "") + wxString::Format("0x%x", args[i]);
		log += ")";
		ConLog.Warning("TODO: " + log);
	}
	break;
	}
}