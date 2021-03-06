#include "stdafx.h"

#if 1
class spectrum_vis_callback;
class spectrum_extension;

extern cfg_int cfg_vis_edge;

enum 
{
	MODE_STANDARD,
	MODE_BARS,
};

enum 
{
	scale_linear,
	scale_logarithmic,
};

// {DFA4E08C-325F-4b32-91EB-CD9FD5D0AD14}
const GUID g_guid_scale = 
{ 0xdfa4e08c, 0x325f, 0x4b32, { 0x91, 0xeb, 0xcd, 0x9f, 0xd5, 0xd0, 0xad, 0x14 } };

// {3323C764-875A-4464-AC8E-BB130E215A4C}
const GUID g_guid_vertical_scale = 
{ 0x3323c764, 0x875a, 0x4464, { 0xac, 0x8e, 0xbb, 0x13, 0xe, 0x21, 0x5a, 0x4c } };

cfg_int cfg_vis_mode(create_guid(0x3341d306,0xf8b6,0x6c60,0xbd,0x7e,0xe4,0xc5,0xab,0x51,0xf3,0xdd),MODE_BARS);
cfg_int cfg_scale(g_guid_scale, scale_logarithmic);
cfg_int cfg_vertical_scale(g_guid_vertical_scale, scale_logarithmic);

class spectrum_extension : public ui_extension::visualisation, public play_callback
{
	ui_extension::visualisation_host_ptr p_host;

protected:
public:

	bool b_active;

	HBRUSH br_foreground, br_background;

	COLORREF cr_fore;
	COLORREF cr_back;
	unsigned mode;

	t_size m_scale, m_vertical_scale;

	static pfc::ptr_list_t<spectrum_extension> list_vis;

	void make_bitmap(HDC hdc, const RECT * rc_area);
	//void flush_bitmap();
	void flush_brushes();

	spectrum_extension();

	~spectrum_extension();

	static const GUID extension_guid;

	virtual const GUID & get_extension_guid() const
	{
		return extension_guid;
	}

	virtual void get_name(pfc::string_base & out)const;


	virtual void set_config(stream_reader * p_reader, t_size p_size, abort_callback & p_abort);
	virtual void enable( const ui_extension::visualisation_host_ptr & p_host);
	virtual void paint_background(HDC dc, const RECT * rc_client);
	virtual void disable(); 

	inline void clear()
	{
		refresh(NULL);
	}

	virtual bool have_config_popup()const{return true;}

	virtual bool show_config_popup(HWND wnd_parent);

	void get_config(stream_writer * data, abort_callback & p_abort) const;

	static pfc::ptr_list_t<spectrum_extension> g_visualisations;

	static void g_register_stream(spectrum_extension * p_ext)
	{
		if (!g_visualisations.have_item(p_ext))
		{
			//console::printf("registering %x",p_ext);
			if (g_visualisations.add_item(p_ext) == 0)
			{
				//static_api_ptr_t<visualisation_manager>()->create_stream(g_stream, NULL);
				g_create_timer();
			}
		}
	}

	static void g_deregister_stream(spectrum_extension * p_ext, bool b_paused = false)
	{
		//console::printf("deregistering %x",p_ext);
		g_visualisations.remove_item(p_ext);
		if (!g_visualisations.get_count())
		{
			g_destroy_timer();
			//g_stream.release();
		}
		if (!b_paused)
		{
			if (p_ext->b_active)
				p_ext->clear();
			p_ext->flush_brushes();
		}
	}

	static bool g_is_stream_active(spectrum_extension * p_ext)
	{
		return g_visualisations.have_item(p_ext);
	}

	friend class spectrum_vis_callback;
	friend class spec_param;

private:
	static UINT_PTR g_timer_refcount;
	static UINT_PTR g_timer;
	static service_ptr_t<visualisation_stream> g_stream;

	static void CALLBACK g_timer_proc(HWND wnd, UINT msg,UINT_PTR id_event,DWORD time);
	static void g_refresh_all();

	void refresh(const audio_chunk * p_chunk);
	static void g_create_timer()
	{
		if (!g_timer)
		{
			g_timer = SetTimer(NULL, NULL, 25, g_timer_proc);
			g_refresh_all();
		}
	}

	static void g_destroy_timer()
	{
		if (g_timer)
		{
			KillTimer(NULL, g_timer);
			g_timer = NULL;
		}
	}

	virtual void FB2KAPI on_playback_starting(play_control::t_track_command p_command,bool p_paused)
	{
	};
	virtual void FB2KAPI on_playback_new_track(metadb_handle_ptr p_track)
	{
		g_register_stream(this);
	};
	virtual void FB2KAPI on_playback_stop(play_control::t_stop_reason p_reason)
	{
		g_deregister_stream(this, p_reason == play_control::stop_reason_shutting_down);
	};
	virtual void FB2KAPI on_playback_seek(double p_time){};
	virtual void FB2KAPI on_playback_pause(bool p_state)
	{
		if (p_state)
			g_deregister_stream(this, true);
		else
			g_register_stream(this);
	};
	virtual void FB2KAPI on_playback_edited(metadb_handle_ptr p_track) {};
	virtual void FB2KAPI on_playback_dynamic_info(const file_info & p_info) {};
	virtual void FB2KAPI on_playback_dynamic_info_track(const file_info & p_info) {};
	virtual void FB2KAPI on_playback_time(double p_time) {};
	virtual void FB2KAPI on_volume_change(float p_new_val) {};
};

UINT_PTR spectrum_extension::g_timer = NULL;
UINT_PTR spectrum_extension::g_timer_refcount = NULL;
service_ptr_t<visualisation_stream> spectrum_extension::g_stream;

pfc::ptr_list_t<spectrum_extension> spectrum_extension::list_vis;
pfc::ptr_list_t<spectrum_extension> spectrum_extension::g_visualisations;


spectrum_extension::spectrum_extension() : br_foreground(0), br_background(0), 
	cr_fore(cfg_vis2), cr_back(cfg_vis), b_active(false), mode(cfg_vis_mode), m_scale(cfg_scale), m_vertical_scale(cfg_vertical_scale)
{
};

spectrum_extension::~spectrum_extension()
{
}

void spectrum_extension::flush_brushes()
{
	if (br_background)
	{
		DeleteObject(br_background);
		br_background=0;
	}
	if (br_foreground)
	{
		DeleteObject(br_foreground); 
		br_foreground=0; 
	}
}

void spectrum_extension::paint_background(HDC dc, const RECT * rc_client)
{
	if (!br_background)
		br_background = CreateSolidBrush(cr_back);

	FillRect(dc,rc_client,br_background);
}

void spectrum_extension::enable( const ui_extension::visualisation_host_ptr & p_vis_host)
{
	p_host = p_vis_host;
	b_active = true;
	
	if (list_vis.add_item(this) == 0)
		static_api_ptr_t<visualisation_manager>()->create_stream(g_stream, NULL);

	static_api_ptr_t<play_callback_manager>()->register_callback(this, play_callback::flag_on_playback_new_track|play_callback::flag_on_playback_stop|play_callback::flag_on_playback_pause, false);
	if (static_api_ptr_t<play_control>()->is_playing())
		g_register_stream(this);
}


void spectrum_extension::disable()
{
	b_active = false;

	list_vis.remove_item(this);

	static_api_ptr_t<play_callback_manager>()->unregister_callback(this);
	if (static_api_ptr_t<play_control>()->is_playing())
		g_deregister_stream(this);

	if (!list_vis.get_count())
		g_stream.release();

	p_host.release();
}

class spec_param
{
public:
	modal_dialog_scope m_scope;
	COLORREF cr_fore;
	COLORREF cr_back;
	unsigned mode;
	t_size m_scale, m_vertical_scale;
	spectrum_extension * ptr;
	HBRUSH br_fore;
	HBRUSH br_back;
	unsigned frame;
	bool b_show_frame;
	void flush_back()
	{
		if (br_back) {
			DeleteObject(br_back);
			br_back = 0;
		}
	}
	void flush_fore()
	{
		if (br_fore){
			DeleteObject(br_fore);
			br_fore = 0;
		}
	}
	spec_param(COLORREF fore, COLORREF back, unsigned p_mode, t_size scale, t_size vertical_scale, spectrum_extension * p_spec, bool p_show_frame = false, unsigned p_frame = 0)
		: cr_fore(fore), cr_back(back), mode(p_mode), m_scale(scale), m_vertical_scale(vertical_scale), ptr(p_spec), br_fore(0), br_back(0), frame(p_frame), b_show_frame(p_show_frame)
	{};
	~spec_param() {
		flush_back();
		flush_fore();
	}
};

static BOOL CALLBACK SpectrumPopupProc(HWND wnd,UINT msg,WPARAM wp,LPARAM lp)
{
	switch(msg)
	{
	case WM_INITDIALOG:
		uSetWindowLong(wnd,DWL_USER,lp);
		{
			spec_param * ptr = reinterpret_cast<spec_param*>(lp);
			ptr->m_scope.initialize(FindOwningPopup(wnd));
			uSendDlgItemMessage(wnd, IDC_BARS, BM_SETCHECK, ptr->ptr->mode == MODE_BARS, 0);
			HWND wnd_combo = GetDlgItem(wnd, IDC_FRAME_COMBO);
			EnableWindow(wnd_combo, ptr->b_show_frame);
			if (ptr->b_show_frame)
			{
				ComboBox_AddString(wnd_combo, _T("None"));
				ComboBox_AddString(wnd_combo, _T("Sunken"));
				ComboBox_AddString(wnd_combo, _T("Grey"));
				ComboBox_SetCurSel(wnd_combo, ptr->frame);
			}
			wnd_combo = GetDlgItem(wnd, IDC_SCALE);
			ComboBox_AddString(wnd_combo, _T("Linear"));
			ComboBox_AddString(wnd_combo, _T("Logarithmic"));
			ComboBox_SetCurSel(wnd_combo, ptr->m_scale);

			wnd_combo = GetDlgItem(wnd, IDC_VERTICAL_SCALE);
			ComboBox_AddString(wnd_combo, _T("Linear"));
			ComboBox_AddString(wnd_combo, _T("Logarithmic"));
			ComboBox_SetCurSel(wnd_combo, ptr->m_vertical_scale);
		}
		return TRUE;
	case WM_ERASEBKGND:
		SetWindowLongPtr(wnd, DWL_MSGRESULT, TRUE);
		return TRUE;
	case WM_PAINT:
		ui_helpers::innerWMPaintModernBackground(wnd, GetDlgItem(wnd, IDOK));
		return TRUE;
	case WM_CTLCOLORSTATIC:
		{
			spec_param * ptr = reinterpret_cast<spec_param*>(uGetWindowLong(wnd,DWL_USER));
			if (GetDlgItem(wnd, IDC_PATCH_FORE) == (HWND)lp) {
				HDC dc =(HDC)wp;
				if (!ptr->br_fore) 
				{
					ptr->br_fore = CreateSolidBrush(ptr->cr_fore);
				}
				return (BOOL)ptr->br_fore;
			} 
			else if (GetDlgItem(wnd, IDC_PATCH_BACK) == (HWND)lp) 
			{
				HDC dc =(HDC)wp;
				if (!ptr->br_back) 
				{
					ptr->br_back = CreateSolidBrush(ptr->cr_back);
				}
				return (BOOL)ptr->br_back;
			}
			else
			return (BOOL)GetSysColorBrush(COLOR_3DHIGHLIGHT);
		}
		break;
	case WM_COMMAND:
		switch(wp)
		{
		case IDCANCEL:
			EndDialog(wnd,0);
			return TRUE;
		case IDC_CHANGE_BACK:
			{
				spec_param * ptr = reinterpret_cast<spec_param*>(uGetWindowLong(wnd,DWL_USER));
				COLORREF COLOR = ptr->cr_back;
				COLORREF COLORS[16] = {get_default_colour(colours::COLOUR_BACK),GetSysColor(COLOR_3DFACE),0,0,0,0,0,0,0,0,0,0,0,0,0,0};
				if (uChooseColor(&COLOR, wnd, &COLORS[0]))
				{
					ptr->cr_back = COLOR;
					ptr->flush_back();
					RedrawWindow(GetDlgItem(wnd, IDC_PATCH_BACK), 0, 0, RDW_INVALIDATE|RDW_UPDATENOW);
				}
			}
			break;
		case IDC_CHANGE_FORE:
			{
				spec_param * ptr = reinterpret_cast<spec_param*>(uGetWindowLong(wnd,DWL_USER));
				COLORREF COLOR = ptr->cr_fore;
				COLORREF COLORS[16] = {get_default_colour(colours::COLOUR_TEXT),GetSysColor(COLOR_3DSHADOW),0,0,0,0,0,0,0,0,0,0,0,0,0,0};
				if (uChooseColor(&COLOR, wnd, &COLORS[0]))
				{
					ptr->cr_fore = COLOR;
					ptr->flush_fore();
					RedrawWindow(GetDlgItem(wnd, IDC_PATCH_FORE), 0, 0, RDW_INVALIDATE|RDW_UPDATENOW);
				}
			}
			break;
		case IDC_BARS:
			{
				spec_param * ptr = reinterpret_cast<spec_param*>(uGetWindowLong(wnd,DWL_USER));
				ptr->mode = (uSendMessage((HWND)lp, BM_GETCHECK, 0, 0) != TRUE ? MODE_STANDARD : MODE_BARS);
			}
			break;
		case IDC_FRAME_COMBO|(CBN_SELCHANGE<<16):
			{
				spec_param * ptr = reinterpret_cast<spec_param*>(uGetWindowLong(wnd,DWL_USER));
				ptr->frame = ComboBox_GetCurSel(HWND(lp));
			}
			break;
		case IDC_SCALE|(CBN_SELCHANGE<<16):
			{
				spec_param * ptr = reinterpret_cast<spec_param*>(uGetWindowLong(wnd,DWL_USER));
				ptr->m_scale = ComboBox_GetCurSel(HWND(lp));
			}
			break;
		case IDC_VERTICAL_SCALE|(CBN_SELCHANGE<<16):
			{
				spec_param * ptr = reinterpret_cast<spec_param*>(uGetWindowLong(wnd,DWL_USER));
				ptr->m_vertical_scale = ComboBox_GetCurSel(HWND(lp));
			}
			break;
		case IDOK:
			{
				spec_param * ptr = reinterpret_cast<spec_param*>(uGetWindowLong(wnd,DWL_USER));
				EndDialog(wnd,1);
			}
			return TRUE;
		default:
			return FALSE;
		}
	default:
		return FALSE;
	}
}


bool spectrum_extension::show_config_popup(HWND wnd_parent)
{
	spec_param param(cr_fore, cr_back, mode, m_scale, m_vertical_scale, this);
	bool rv = !!uDialogBox(IDD_POPUP_SPECTRUM_NEW, wnd_parent, SpectrumPopupProc, (LPARAM)(&param));
	if (rv)
	{
		cr_fore = param.cr_fore;
		cfg_vis2 = param.cr_fore;
		cr_back = param.cr_back;
		cfg_vis = param.cr_back;
		mode = param.mode;
		cfg_vis_mode = param.mode;
		m_scale = param.m_scale;
		cfg_scale = param.m_scale;
		m_vertical_scale = param.m_vertical_scale;
		cfg_vertical_scale = param.m_vertical_scale;
		if (b_active)
		{
			flush_brushes();
			clear();
		}
	}
	return rv;
}

/*
void spectrum_extension::flush_bitmap()
{
	ui_extension::visualisation_host::vis_paint_struct ps;
	p_host->begin_paint(ps);
	paint_background(ps.dc, &ps.rc_client);
	p_host->end_paint(ps);
}
*/

void CALLBACK spectrum_extension::g_timer_proc(HWND wnd, UINT msg,UINT_PTR id_event,DWORD time)
{
	g_refresh_all();
}

double g_scale(double val)
{
	//return pow (pow (10, val), 1.0/3.0);
	return pow (10, val);
}
void g_scale_value(t_size source_count, t_size index, t_size dest_count, t_size & source_start, t_size & source_end, bool b_log)
{
	if (b_log)
	{
		const double exp_upper = 3.7, exp_lower=2.6, exp_range = exp_upper - exp_lower;
		double start = (double)source_count*( (g_scale(exp_lower + exp_range * ((double)(index)/(double)dest_count)) - g_scale(exp_lower) )/(g_scale(exp_upper) - g_scale(exp_lower)) );
		double end = (double)source_count*( (g_scale(exp_lower + exp_range * ((double)(index+1)/(double)dest_count)) - g_scale(exp_lower) )/(g_scale(exp_upper) - g_scale(exp_lower)) );
		source_start = pfc::rint32(start);
		source_end = pfc::rint32(end);
	}
	else
	{
		double start = (double)source_count*((double)(index)/(double)dest_count);
		double end = (double)source_count*((double)(index+1)/(double)dest_count);
		source_start = pfc::rint32(start);
		source_end = pfc::rint32(end);
	}
}

t_size g_scale_value_single(double val, t_size count, bool b_log)
{
	if (b_log)
	{
		const double exp_upper = 3.7, exp_lower=2.6, exp_range = exp_upper - exp_lower;
		double start = (double)count*( (g_scale(exp_lower + exp_range * val) - g_scale(exp_lower) )/(g_scale(exp_upper) - g_scale(exp_lower)) );
		return pfc::rint32(start);
	}
	else
	{
		double start = (double)count*val;
		return pfc::rint32(start);
	}
}

void spectrum_extension::refresh(const audio_chunk * p_chunk)
{
	ui_extension::visualisation_host::painter_ptr ps;
	p_host->create_painter(ps);

	HDC dc = ps->get_device_context();
	const RECT * rc_client = ps->get_area();
	{
		/*
		if (!br_background)
			br_background = CreateSolidBrush(cr_back);

		FillRect(dc,&rc_client,br_background);*/

		paint_background(dc, rc_client);

		//HPEN pn = CreatePen(PS_SOLID, 1, cr_fore);
		//HPEN pn_old = SelectPen(dc, pn);

		if (g_is_stream_active(this) && p_chunk)
		{
			if (!br_foreground)
				br_foreground = CreateSolidBrush(cr_fore);


			unsigned n;

			unsigned i;
			if (mode == MODE_BARS)
			{
				unsigned totalbars = rc_client->right /3;
				if (totalbars)
				{

#if 1
					const audio_sample * p_data = p_chunk->get_data();
					t_size sample_count = p_chunk->get_sample_count();
					t_size channel_count = p_chunk->get_channels();
					t_size i;
					for(i=0;i<totalbars;i++)
					{
						double val = 0;
						t_size starti, endi, j;
						g_scale_value(sample_count, i, totalbars, starti, endi, m_scale == scale_logarithmic);
						for (j=starti;j<=endi; j++)
						{
							if (j < sample_count)
							{
								t_size k;
								for (k=0; k<channel_count; k++)
									val += p_data[j*channel_count + k];
							}

						}
						val *= 1.0/( (endi-starti+1)*channel_count);
						val *= 0.80;
						if (val>1.0) val = 1.0;

						RECT r;
						r.left = 1 + i*3;
						r.right = r.left + 2;
						r.bottom = rc_client->bottom ? rc_client->bottom-1 : 0;
						r.top = g_scale_value_single(1.0-val, rc_client->bottom, m_vertical_scale == scale_logarithmic); //pfc::rint32((1.0 - val) * (rc_client->bottom));
						if (r.bottom>r.top)
							FillRect(dc,&r,br_foreground);
					}
#else
					int spread = MulDiv(1, p_chunk->get_sample_count(), totalbars);
					if (spread < 1) spread = 1;
					double div = 1.0 / (double) p_chunk->get_channels();
					unsigned left = 1;
					for(i=0;i<totalbars;i++)
					{
						unsigned right = left+MulDiv(1, rc_client->right-left-totalbars+i, totalbars-i);
						RECT r;
						r.left = left;
						r.right = right;

						double val = 0;
						unsigned start=MulDiv(i, p_chunk->get_sample_count()-spread, totalbars),co = 0;

						for (n=start;n<(start+spread);n++)
						{
							if (n<p_chunk->get_sample_count())
							{
								co++;
								unsigned c;
								for(c=0;c<p_chunk->get_channels();c++)
								{
									val += ((p_chunk->get_data()[n*p_chunk->get_channels() + c]));
								}
							}
						}
						val *= div;
						if (co) val *= (1.0/(double)co);
						//val = log((val/2.0) + 1.0)*2.0;
						val *= 0.80;
						if (val>1.0) val = 1.0;
						r.bottom = rc_client->bottom > 0 ? rc_client->bottom-1 : 0;
						int top = (int)( (val) * (double(r.bottom)) + 0.5);
						top= r.bottom - top;
						//						if (top==0) top++;
						/*
						for (;r.bottom-1>top; r.bottom-=2)
						{
							r.top = r.bottom-1;
							//FillRect(dc,&r,br_foreground);//LineToEx ?
							MoveToEx(dc, r.left, r.top, NULL);
							LineTo(dc, r.right, r.bottom);
						}*/
						{
							RECT rc = {left, top, right, rc_client->bottom > 0 ? rc_client->bottom-1 : 0};
							FillRect(dc,&rc,br_foreground);
						}
						left=right+1;
					}
#endif
					int j;
					for (j=rc_client->bottom; j>rc_client->top; j-=2)
					{
						RECT rc = {0, j-1, rc_client->right, j};
						FillRect(dc,&rc,br_background);
					}
				}
			}
			else
			{
#if 1
				//gdi_object_t<HPEN>::ptr_t pn = CreatePen(PS_SOLID, 1, cr_fore);
				//HPEN pn_old = SelectPen(dc, pn.get());
					const audio_sample * p_data = p_chunk->get_data();
					t_size sample_count = p_chunk->get_sample_count();
					t_size channel_count = p_chunk->get_channels();
					t_size i;
					for(i=0;i<(t_size)rc_client->right;i++)
					{
						double val = 0;
						//t_size starti = pfc::rint32(start), endi = pfc::rint32(end), j;
						t_size starti, endi, j;
						g_scale_value(sample_count, i, rc_client->right, starti, endi, m_scale == scale_logarithmic);
						for (j=starti;j<=endi; j++)
						{
							if (j < sample_count)
							{
								t_size k;
								for (k=0; k<channel_count; k++)
									val += p_data[j*channel_count + k];
							}

						}
						val *= 1.0/( (endi-starti+1)*channel_count);
						val *= 0.80;
						if (val>1.0) val = 1.0;

						RECT r;
						r.left = i;
						r.right = i+1;
						r.bottom = rc_client->bottom;
						r.top = g_scale_value_single (1.0-val, rc_client->bottom, m_vertical_scale == scale_logarithmic);//pfc::rint32((1.0 - val) * (rc_client->bottom));
						if (r.bottom>r.top)
							FillRect(dc,&r,br_foreground);
						//MoveToEx(dc, r.left, r.bottom, NULL);
						//LineTo(dc, r.left, r.top);
					}
					//SelectPen(dc, pn_old);
#else
				int spread = MulDiv(1, p_chunk->get_sample_count(), rc_client->right);
					if (spread < 1) spread = 1;
					double div = 1.0 / (double) p_chunk->get_channels();
					assert(rc_client->right>=0);
					for(i=0;i<(unsigned)rc_client->right;i++)
					{
						RECT r;
						r.left = i;//MulDiv(rc_client->right,n,data->spectrum_size+1);
						r.right = i+1;//MulDiv(rc_client->right,n+1,data->spectrum_size+1);
						//					if (r.left < r.right)
						double val = 0;
						unsigned start=MulDiv(i, p_chunk->get_sample_count()-spread, rc_client->right),co = 0;

						for (n=start;n<(start+spread);n++)
						{
							if (n<p_chunk->get_sample_count())
							{
								co++;
								unsigned c;
								for(c=0;c<p_chunk->get_channels();c++)
								{
									val += p_chunk->get_data()[n*p_chunk->get_channels() + c];
								}
							}
						}
						val *= div;
						if (co) val *= (1.0/(double)co);
						//val = log((val/2.0) + 1.0) * 2.0; //close to a str8 line
						val *= 0.80;
						if (val>1.0) val = 1.0;
						r.bottom = rc_client->bottom;
						r.top = (int)((1.0 - val) * (rc_client->bottom));
						/*double rt = sqrt(val);
						HBRUSH br_test = CreateSolidBrush(pfc::rint32((1.0-rt)*0xff + rt*(0x55)));
						FillRect(dc,&r,br_test);
						DeleteBrush(br_test);*/
						FillRect(dc,&r,br_foreground);
					}
#endif

			}
		}
	}
	ps.release();
}

void spectrum_extension::g_refresh_all()
{
	double p_time = NULL;
	g_stream->get_absolute_time(p_time);
	audio_chunk_impl p_chunk;

	//unsigned needed_fft_size = 1;
	//while (unsigned(rc_client.right - rc_client.left)*2 > needed_fft_size)
	//	needed_fft_size <<= 1;

	unsigned fft_size = 1024;//min(needed_fft_size, 512);
	bool ret = g_stream->get_spectrum_absolute(p_chunk, p_time, fft_size);

	unsigned n, count = spectrum_extension::g_visualisations.get_count();
	for (n=0; n<count; n++)
	{
		spectrum_extension * vis_ext = spectrum_extension::g_visualisations[n];
		if (ret)
			vis_ext->refresh(&p_chunk);
	}
}

#if 0
class spectrum_vis_callback : public visualization
{
public:
	virtual bool is_active() {return (spectrum_extension::list_vis.get_count() > 0);}
	virtual bool need_spectrum() {return true;}
	
	virtual void on_data(const vis_chunk * data)
	{
		unsigned n, count = spectrum_extension::list_vis.get_count();
		for (n=0; n<count; n++)
		{
			spectrum_extension * vis_ext = spectrum_extension::list_vis[n];

			if (data->flags & vis_chunk::FLAG_LAGGED) return;

			ui_extension::visualisation_host::vis_paint_struct ps;
			vis_ext->p_host->begin_paint(ps);

			HDC dc = ps.dc;
			RECT rc_client = ps.rc_client;
			
			{
				if (!vis_ext->br_background)
					vis_ext->br_background = CreateSolidBrush(vis_ext->cr_back);
				if (!vis_ext->br_foreground)
					vis_ext->br_foreground = CreateSolidBrush(vis_ext->cr_fore);

				FillRect(dc,&rc_client,vis_ext->br_background);
				
				//HPEN pn = CreatePen(PS_SOLID, 1, vis_ext->cr_fore);
				//HPEN pn_old = SelectPen(dc, pn);

				unsigned n;
				vis_sample * src = data->spectrum;

				unsigned i;
				if (vis_ext->mode == MODE_BARS)
				{
					unsigned totalbars = rc_client.right /3;
					if (totalbars)
					{
						
						int spread = MulDiv(1, data->spectrum_size, totalbars);
						if (spread < 1) spread = 1;
						double div = 1.0 / (double) data->nch;
						unsigned left = 1;
						for(i=0;i<totalbars;i++)
						{
							unsigned right = left+MulDiv(1, rc_client.right-left-totalbars+i, totalbars-i);
							RECT r;
							r.left = left;
							r.right = right;
							
							double val = 0;
							unsigned start=MulDiv(i, data->spectrum_size-spread, totalbars),co = 0;
							
							for (n=start;n<(start+spread);n++)
							{
								if (n<data->spectrum_size)
								{
									co++;
									unsigned c;
									for(c=0;c<data->nch;c++)
									{
										val += fabs((src[n*data->nch + c]));//*exp(((double)n*2)/(double)data->spectrum_size);
										//			val = log(val*exp((double)(n)/(double)data->spectrum_size)+3.0);
									}
								}
							}
							val *= div;
							if (co) val *= (1.0/(double)co);
							val = log((val/2) + 1) / log(2.0);
							val *= 0.25;
							if (val>1.0) val = 1.0;
							r.bottom = rc_client.bottom > 0 ? rc_client.bottom-1 : 0;
							int top = (int)((1.0 - val) * double(rc_client.bottom-1));
							//						if (top==0) top++;
							for (;r.bottom-1>top; r.bottom-=2)
							{
								r.top = r.bottom-1;
								FillRect(dc,&r,vis_ext->br_foreground);//LineToEx ?
							}
							left=right+1;
						}
					}
				}
				else
				{
					int spread = MulDiv(1, data->spectrum_size, rc_client.right);
					if (spread < 1) spread = 1;
					double div = 1.0 / (double) data->nch;
					assert(rc_client.right>=0);
					for(i=0;i<(unsigned)rc_client.right;i++)
					{
						RECT r;
						r.left = i;//MulDiv(rc_client.right,n,data->spectrum_size+1);
						r.right = i+1;//MulDiv(rc_client.right,n+1,data->spectrum_size+1);
						//					if (r.left < r.right)
						double val = 0;
						unsigned start=MulDiv(i, data->spectrum_size-spread, rc_client.right),co = 0;
						
						for (n=start;n<(start+spread);n++)
						{
							if (n<data->spectrum_size)
							{
								co++;
								unsigned c;
								for(c=0;c<data->nch;c++)
								{
									val += fabs((src[n*data->nch + c]));
								}
							}
						}
						val *= div;
						if (co) val *= (1.0/(double)co);
						val = log((val/2) + 1) / log(2.0);
						val *= 0.25;
						if (val>1.0) val = 1.0;
						r.bottom = rc_client.bottom;
						r.top = (int)((2.0 - 2*val) * (rc_client.bottom / 2));
						//MoveToEx(dc, r.left, r.bottom, NULL);
						//LineTo(dc, r.right, r.top);
						FillRect(dc,&r,vis_ext->br_foreground);
					}
				}

				//SelectPen(dc, pn_old);
				//DeletePen(pn);
				
			}
			vis_ext->p_host->end_paint(ps);
			
		}
		
	}
	virtual void on_flush()
	{
		
		unsigned n, count = spectrum_extension::list_vis.get_count();
		for (n=0; n<count; n++)
		{
			spectrum_extension * vis_ext = spectrum_extension::list_vis[n];

			// ???
			vis_ext->flush_bitmap();
		}
		
	}
};

static visualization_factory<spectrum_vis_callback> foo;
#endif


void spectrum_extension::get_name(pfc::string_base & out)const
{
	out.set_string("Spectrum analyser");
}

void spectrum_extension::set_config(stream_reader * r, t_size p_size, abort_callback & p_abort)
{
	if (p_size)
	{
		r->read_lendian_t(cr_fore, p_abort);
		r->read_lendian_t(cr_back, p_abort);
		r->read_lendian_t(mode, p_abort);
		try {
		r->read_lendian_t(m_scale, p_abort);
		r->read_lendian_t(m_vertical_scale, p_abort);
		} catch (const exception_io_data_truncation &)
		{};
	}
}


void spectrum_extension::get_config(stream_writer * data, abort_callback & p_abort) const
{
	data->write_lendian_t(cr_fore, p_abort);
	data->write_lendian_t(cr_back, p_abort);
	data->write_lendian_t(mode, p_abort);
	data->write_lendian_t(m_scale, p_abort);
	data->write_lendian_t(m_vertical_scale, p_abort);
}

// {D947777C-94C7-409a-B02C-9B0EB9E374FA}
const GUID spectrum_extension::extension_guid = 
{ 0xd947777c, 0x94c7, 0x409a, { 0xb0, 0x2c, 0x9b, 0xe, 0xb9, 0xe3, 0x74, 0xfa } };

ui_extension::visualisation_factory<spectrum_extension> blah;

class window_visualisation_spectrum : public window_visualisation
{
	virtual const GUID & get_visualisation_guid() const
	{
		return spectrum_extension::extension_guid;
	}
	virtual const GUID & get_extension_guid() const
	{
		return spectrum_extension::extension_guid;
	}
	virtual void get_menu_items (ui_extension::menu_hook_t & p_hook)
	{
		p_hook.add_node(uie::menu_node_ptr(new(std::nothrow) uie::menu_node_configure(this)));
	};
	virtual void set_config(stream_reader * r, t_size p_size, abort_callback & p_abort)
	{
		if (p_size)
		{
			t_uint32 m_frame;
			r->read_lendian_t(m_frame, p_abort);
			{
				set_frame_style(m_frame);
				unsigned size = 0;
				r->read_lendian_t(size, p_abort);
				pfc::array_t<t_uint8> m_data;
				m_data.set_size(size);
				r->read(m_data.get_ptr(), size, p_abort);
				set_vis_data(m_data.get_ptr(), m_data.get_size());
			}
		}
	}
	void get_config(stream_writer * data, abort_callback & p_abort) const
	{
		pfc::array_t<t_uint8> m_data;
		data->write_lendian_t(get_frame_style(), p_abort);
		get_vis_data(m_data);
		data->write_lendian_t(m_data.get_size(), p_abort);
		data->write(m_data.get_ptr(), m_data.get_size(), p_abort);
	}
	virtual bool have_config_popup()const{return true;}
	virtual bool show_config_popup(HWND wnd_parent)
	{
		uie::visualisation_ptr p_vis;
		service_ptr_t<spectrum_extension> p_this;
		get_vis_ptr(p_vis);
		if (p_vis.is_valid())
			p_this = static_cast<spectrum_extension*>(p_vis.get_ptr());

		service_ptr_t<spectrum_extension> p_temp = p_this;
		if (!p_temp.is_valid())
			uie::visualization::create_by_guid(get_visualisation_guid(), reinterpret_cast<uie::visualisation_ptr &>(p_temp));

		pfc::array_t<t_uint8> m_data;
		if (!p_temp->b_active)
		{
			try{
			get_vis_data(m_data);
			p_temp->set_config(&stream_reader_memblock_ref(m_data.get_ptr(), m_data.get_size()),m_data.get_size(),abort_callback_impl());
			}catch (const exception_io &) {};
		}

		spec_param param(p_temp->cr_fore, p_temp->cr_back, p_temp->mode, p_temp->m_scale, p_temp->m_vertical_scale, p_temp.get_ptr(), true, get_frame_style());

		bool rv = !!uDialogBox(IDD_POPUP_SPECTRUM_NEW, wnd_parent, SpectrumPopupProc, (LPARAM)(&param));
		if (rv)
		{
			p_temp->cr_fore = param.cr_fore;
			cfg_vis2 = param.cr_fore;
			p_temp->cr_back = param.cr_back;
			cfg_vis = param.cr_back;
			p_temp->mode = param.mode;
			cfg_vis_mode = param.mode;
			p_temp->m_scale = param.m_scale;
			cfg_scale = param.m_scale;
			p_temp->m_vertical_scale = param.m_vertical_scale;
			cfg_vertical_scale = param.m_vertical_scale;
			set_frame_style(param.frame);
			if (p_temp->b_active)
			{
				p_temp->flush_brushes();
				p_temp->clear();
			}
			else
			{
				m_data.set_size(0);
				try{
				p_temp->get_config(&stream_writer_memblock_ref(m_data), abort_callback_impl());
				set_vis_data(m_data.get_ptr(), m_data.get_size());
				}
				catch (pfc::exception & e)
				{};
			}

		}
		return rv;
	}
};

ui_extension::window_factory<window_visualisation_spectrum> blahg;
#endif