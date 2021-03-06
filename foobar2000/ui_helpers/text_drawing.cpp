#include "stdafx.h"

SCRIPT_DIGITSUBSTITUTE ui_helpers::ScriptString_instance::m_sdg;
bool ui_helpers::ScriptString_instance::m_sdg_valid;

template <typename char_t>
unsigned strtoul_t(const char_t * p_val, unsigned p_val_length, unsigned base)
{
	unsigned rv = 0;
	const char_t * ptr = p_val;

	while (t_size(ptr - p_val) < p_val_length && *ptr)
	{
		if (*ptr >= '0' && *ptr <='9')
		{
			rv *= base;
			rv += *ptr - '0';
		}
		else if (base > 10 && *ptr >= 'a' && *ptr < (char)('a'+base-10))
		{
			rv *= base;
			rv += *ptr - 'a' + 10;
		}
		else if (base > 10 && *ptr >= 'A' && *ptr < char('A'+base-10))
		{
			rv *= base;
			rv += *ptr - 'A' + 10;
		}
		else break;
		ptr++;
	}
	return rv;
}

unsigned strtoul_n(const char * p_val, unsigned p_val_length, unsigned base)
{
	unsigned rv = 0;
	const char * ptr = p_val;

	while (t_size(ptr - p_val) < p_val_length && *ptr)
	{
		if (*ptr >= '0' && *ptr <='9')
		{
			rv *= base;
			rv += *ptr - '0';
		}
		else if (base > 10 && *ptr >= 'a' && *ptr < (char)('a'+base-10))
		{
			rv *= base;
			rv += *ptr - 'a' + 10;
		}
		else if (base > 10 && *ptr >= 'A' && *ptr < char('A'+base-10))
		{
			rv *= base;
			rv += *ptr - 'A' + 10;
		}
		else break;
		ptr++;
	}
	return rv;
}

t_int64 strtol64_n(const char * p_val, unsigned p_val_length, unsigned base)
{
	t_int64 rv = 0;
	const char * ptr = p_val;

	t_int8 sign = 1;

	if (*ptr == '-') {sign = -1; ptr++;}
	else if (*ptr == '+') ptr++;

	while (t_size(ptr - p_val) < p_val_length && *ptr)
	{
		if (*ptr >= '0' && *ptr <='9')
		{
			rv *= base;
			rv += *ptr - '0';
		}
		else if (base > 10 && *ptr >= 'a' && *ptr < (char)('a'+base-10))
		{
			rv *= base;
			rv += *ptr - 'a' + 10;
		}
		else if (base > 10 && *ptr >= 'A' && *ptr < char('A'+base-10))
		{
			rv *= base;
			rv += *ptr - 'A' + 10;
		}
		else break;
		ptr++;
	}
	rv *= sign;
	return rv;
}

t_uint64 strtoul64_n(const char * p_val, unsigned p_val_length, unsigned base)
{
	t_uint64 rv = 0;
	const char * ptr = p_val;

	while (t_size(ptr - p_val) < p_val_length && *ptr)
	{
		if (*ptr >= '0' && *ptr <='9')
		{
			rv *= base;
			rv += *ptr - '0';
		}
		else if (base > 10 && *ptr >= 'a' && *ptr < (char)('a'+base-10))
		{
			rv *= base;
			rv += *ptr - 'a' + 10;
		}
		else if (base > 10 && *ptr >= 'A' && *ptr < char('A'+base-10))
		{
			rv *= base;
			rv += *ptr - 'A' + 10;
		}
		else break;
		ptr++;
	}
	return rv;
}

#if 0
unsigned strtoul_n(const char * p_val, unsigned p_val_length)
{
	unsigned rv = 0;
	const char * ptr = p_val;
	/*while (ptr - p_val < p_val_length && *ptr == '0')
	{
	ptr++;
	}*/
	while ((ptr - p_val) < p_val_length && *ptr)
	{
		if (*ptr >= '0' && *ptr <='9')
		{
			rv *= 16;
			rv += *ptr - '0';
		}
		else if (*ptr >= 'a' && *ptr <= 'f')
		{
			rv *= 16;
			rv += *ptr - 'a' + 10;
		}
		else if (*ptr >= 'A' && *ptr <= 'F')
		{
			rv *= 16;
			rv += *ptr - 'A' + 10;
		}
		else break;
		ptr++;
	}
	return rv;
}
#endif
#define ELLIPSIS "\xe2\x80\xa6"//"\x85"
#define ELLIPSIS_LEN 3

namespace ui_helpers
{
BOOL _uExtTextOut(HDC dc,int x,int y,const RECT * rect,const char * text,UINT text_length,bool b_ellipsis = false, alignment p_align = ALIGN_LEFT)
{
	pfc::stringcvt::string_os_from_utf8 temp(text,text_length);
	RECT rc = *rect;
	rc.left = x;
	rc.top = y;
	UINT flags = DT_SINGLELINE|DT_TOP|DT_NOPREFIX;
	if (b_ellipsis)
		flags |= DT_WORD_ELLIPSIS;
	if (p_align == ALIGN_LEFT)
		flags |= DT_LEFT;
	else if (p_align == ALIGN_LEFT)
		flags |= DT_CENTER;
	else if (p_align == ALIGN_RIGHT)
		flags |= DT_RIGHT;
	BOOL ret = TRUE;
	//DrawTextEx(dc, const_cast<wchar_t*>(temp.get_ptr()), temp.length(), &rc, flags, NULL);

	ScriptString_instance p_ScriptString(dc, temp.get_ptr(), temp.length(), NULL, NULL);
	p_ScriptString.text_out(x, y, ETO_CLIPPED, rect);

	return ret;

#if 0
	return ExtTextOut(dc,x,y,flags,rect,temp,pfc::downcast_guarded<int>(_tcslen(temp)),lpdx);
	Gdiplus::Graphics  graphics(dc);
	COLORREF cr = GetTextColor(dc);
	graphics.SetTextRenderingHint(Gdiplus::TextRenderingHintSystemDefault);
	return Gdiplus::Ok == graphics.DrawString(temp.get_ptr(), temp.length(), &Gdiplus::Font(dc), Gdiplus::RectF(max(x,rect->left), max(rect->top,y), RECT_CX((*rect)), RECT_CY((*rect))), &Gdiplus::StringFormat(), &Gdiplus::SolidBrush(Gdiplus::Color(0,0,0)));
#endif
}

static bool check_colour_marks(const char * src, unsigned int len = -1)
{
	const char * ptr = src;
	while(*ptr && (unsigned)(ptr-src) < len)
	{
		if (*ptr==3)
		{
			return true;
		}
		ptr++;
	}
	return false;
}

void remove_color_marks(const char * src,pfc::string8 & out, t_size len = pfc_infinite)//helper
{
	out.reset();
	const char * ptr = src;
	while(*src && (unsigned)(src-ptr) < len)
	{
		if (*src==3)
		{
			src++;
			while(*src && *src!=3) src++;
			if (*src==3) src++;
		}
		else out.add_byte(*src++);
	}
}

void remove_color_marks(const char * src,pfc::string8_fast_aggressive & out, t_size len = pfc_infinite)//helper
{
	out.reset();
	const char * ptr = src;
	while(*src && (unsigned)(src-ptr) < len)
	{
		if (*src==3)
		{
			src++;
			while(*src && *src!=3) src++;
			if (*src==3) src++;
		}
		else out.add_byte(*src++);
	}
}
unsigned get_trunc_len(const char * src, unsigned len)
{
	unsigned rv = len;

	const char * ptr = src;
	ptr += len-1;
	while (ptr>src && (*ptr == ' ' || *ptr == '.')) {ptr --;rv--;}
	return rv;
}

BOOL uGetTextExtentExPoint_helper::run(HDC dc, const char * text, int length, int max_width, LPINT max_chars, LPINT width_array, LPSIZE sz, unsigned * width_out, bool trunc)
	{
		const char * src = text;

		if (check_colour_marks(text, length))
		{ 
			remove_color_marks(text, temp, length);
			src = temp;
			length = temp.length();
		}

		text_w.convert(src, length);
#if 1
		ScriptString_instance p_ScriptString;
		p_ScriptString.analyse(dc, text_w.get_ptr(), text_w.length(), max_width, true);

		{
			int _max_chars = p_ScriptString.get_output_character_count();
			if (width_array)
				p_ScriptString.get_character_logical_extents(width_array);
			if (max_chars) *max_chars = _max_chars;
#else
		if (GetTextExtentExPointW(dc, text_w.get_ptr(), text_w.length(), max_width, max_chars, width_array, sz))
		{
#endif
			if (trunc || width_out)
			{
				w_utf8.convert(text_w.get_ptr(), *max_chars);
				*max_chars = trunc ? get_trunc_len(w_utf8, w_utf8.length()) : w_utf8.length();
				if (width_out)
					*width_out = get_text_width_color(dc, w_utf8, *max_chars);
			}
			return TRUE;
		}
		return FALSE;
	}

BOOL uGetTextExtentExPoint2(HDC dc, const char * text, int length, int max_width, LPINT max_chars, LPINT width_array, LPSIZE sz, unsigned * width_out, bool trunc)
{
	const char * src = text;
	pfc::string8 temp;

	if (check_colour_marks(text, length))
	{ 
		remove_color_marks(text, temp, length);
		src = temp;
		length = temp.length();
	}

	pfc::stringcvt::string_wide_from_utf8_fast text_w(src, length);
	if (GetTextExtentExPointW(dc, text_w.get_ptr(), text_w.length(), max_width, max_chars, width_array, sz))
	{
		pfc::stringcvt::string_utf8_from_wide w_utf8(text_w.get_ptr(), *max_chars);
		*max_chars = trunc ? get_trunc_len(w_utf8, w_utf8.length()) : w_utf8.length();
		if (width_out)
			*width_out = get_text_width_color(dc, w_utf8, *max_chars);
		return TRUE;
	}
	return FALSE;
}


// here we depend on valid pointer anyway, no point using param_ stuff (or rather cant be bothered to make utf16/ansi => utf8 ones)
BOOL uGetTextExtentExPoint(HDC dc, const char * text, int length, int max_width, LPINT max_chars, LPINT width_array, LPSIZE sz, unsigned & width_out, bool trunc)
{
	const char * src = text;
	pfc::string8 temp;

	if (check_colour_marks(text, length))
	{ 
		remove_color_marks(text, temp, length);
		src = temp;
		length = temp.length();
	}

	if (IsUnicode())
	{
		pfc::stringcvt::string_wide_from_utf8 text_w(src, length);
		if (GetTextExtentExPointW(dc, text_w.get_ptr(), text_w.length(), max_width, max_chars, width_array, sz))
		{
			pfc::stringcvt::string_utf8_from_wide w_utf8(text_w.get_ptr(), *max_chars);
			*max_chars = trunc ? get_trunc_len(w_utf8, w_utf8.length()) : w_utf8.length();
			width_out = get_text_width_color(dc, w_utf8, *max_chars);
			return TRUE;
		}
		return FALSE;
	}
	else 
	{
		pfc::stringcvt::string_ansi_from_utf8 text_a(src, length);
		if (GetTextExtentExPointA(dc, text_a.get_ptr(), text_a.length(), max_width, max_chars, width_array, sz))
		{
			pfc::stringcvt::string_utf8_from_ansi a_utf8(text_a.get_ptr(), *max_chars);
			*max_chars = trunc ? get_trunc_len(a_utf8, a_utf8.length()) : a_utf8.length();
			width_out = get_text_width_color(dc, a_utf8, *max_chars);
			return TRUE;
		}
		return FALSE;
	}
}
	bool is_rect_null(const RECT * r)
	{
		return r->right <= r->left || r->bottom <= r->top;
	}

	void get_text_size(HDC dc,const char * src,int len, SIZE & sz)
	{
		sz.cx = 0;
		sz.cy = 0;
		if (!dc) return;
		if (len<=0) return;
		else
		{
			//uGetTextExtentPoint32(dc,src,len,&sz);
			pfc::stringcvt::string_wide_from_utf8 wstr(src, len);
			t_size wlen = wstr.length();

#if 1
			ScriptString_instance p_ScriptString(dc, wstr, wlen, NULL, false);
			p_ScriptString.get_output_size(sz);

#elif 0
			//BOOL ret2 = GetTextExtentPoint32(dc,wstr,wlen,&sz);
			RECT rc = {0};
			/*DRAWTEXTPARAMS dtp;
			memset(&dtp, 0, sizeof(dtp));
			dtp.cbSize = sizeof(dtp);*/
			DrawTextEx(dc, const_cast<wchar_t*>(wstr.get_ptr()), wlen, &rc, DT_CALCRECT|DT_SINGLELINE|DT_TOP|DT_NOPREFIX, NULL/*&dtp*/);
			sz.cx = rc.right;
			sz.cy = rc.bottom;
#elif 0
			pfc::array_staticsize_t<INT> widths(wlen);
			pfc::array_staticsize_t<char> classes(wlen);
			/*GetTextExtentExPoint(dc, wstr, wlen, NULL, NULL, widths.get_ptr(), &sz);
			if (wlen) sz.cx = widths[wlen-1];*/
			GCP_RESULTS gcp;
			memset(&gcp, 0, sizeof(gcp));
			gcp.lStructSize = sizeof(gcp);
			gcp.lpDx = widths.get_ptr();
			gcp.lpClass = classes.get_ptr();
			gcp.nGlyphs = wlen;
			BOOL b_ret = GetCharacterPlacement (dc, wstr, wlen, NULL, &gcp, GCP_DIACRITIC|GCP_GLYPHSHAPE|GCP_DISPLAYZWG);
			DWORD err = GetLastError();
			int cx = HIWORD(b_ret), cy = LOWORD(b_ret);
			if (wlen) sz.cx = LOWORD(b_ret);//widths[wlen-1];
#endif
		}
	}

	void get_text_size_legacy(HDC dc,const char * src,int len, SIZE & sz)
	{
		sz.cx = 0;
		sz.cy = 0;
		if (!dc) return;
		if (len<=0) return;
		else
		{
			uGetTextExtentPoint32(dc,src,len,&sz);
		}
	}

	int get_text_width(HDC dc,const char * src,int len)
	{
		SIZE sz;
		get_text_size(dc, src, len, sz);
		return sz.cx;
	}

	int get_text_width_legacy(HDC dc,const char * src,int len)
	{
		SIZE sz;
		get_text_size_legacy(dc, src, len, sz);
		return sz.cx;
	}

	//GetTextExtentPoint32 wrapper, removes color marks
	int get_text_width_color(HDC dc,const char * src,int len, bool b_ignore_tabs)
	{
		if (!dc) return 0;
		int ptr = 0;
		int start = 0;
		int rv = 0;
		if (len<0) len = strlen(src);
		while(ptr<len)
		{
			if (src[ptr]==3)
			{
				rv += get_text_width(dc,src+start,ptr-start);
				ptr++;
				while(ptr<len && src[ptr]!=3) ptr++;
				if (ptr<len) ptr++;
				start = ptr;
			}
			else if (b_ignore_tabs && src[ptr]=='\t')
			{
				rv += get_text_width(dc,src+start,ptr-start);
				while (ptr<len && src[ptr]=='\t') ptr++;
			}
			else ptr++;
		}
		rv += get_text_width(dc,src+start,ptr-start);
		return rv;
	}

	int get_text_width_color_legacy(HDC dc,const char * src,int len, bool b_ignore_tabs)
	{
		if (!dc) return 0;
		int ptr = 0;
		int start = 0;
		int rv = 0;
		if (len<0) len = strlen(src);
		while(ptr<len)
		{
			if (src[ptr]==3)
			{
				rv += get_text_width_legacy(dc,src+start,ptr-start);
				ptr++;
				while(ptr<len && src[ptr]!=3) ptr++;
				if (ptr<len) ptr++;
				start = ptr;
			}
			else if (b_ignore_tabs && src[ptr]=='\t')
			{
				rv += get_text_width_legacy(dc,src+start,ptr-start);
				while (ptr<len && src[ptr]=='\t') ptr++;
			}
			else ptr++;
		}
		rv += get_text_width_legacy(dc,src+start,ptr-start);
		return rv;
	}

	/*int get_text_max_chars(HDC dc,const char * src,int len, unsigned width, unsigned & new_width)
	{
	if (!dc) return 0;
	int ptr = 0;
	int start = 0;
	int rv = 0;
	if (len<0) len = strlen(src);
	int used = 0;
	while(ptr<len && used<width)
	{
	if (src[ptr]==3)
	{
	//		rv += get_text_width(dc,src+start,ptr-start);
	ptr++;
	while(ptr<len && src[ptr]!=3) ptr++;
	if (ptr<len) ptr++;
	//	start = ptr;
	}
	int cw = get_text_width(dc,src+ptr,1);
	if (used+cw < width) {rv++;used+=cw;}
	ptr++;
	}
	new_width = used;
	//	rv += get_text_width(dc,src+start,ptr-start);

	return (rv != len ? rv : -1);
	}*/

	int strlen_utf8_colour(const char * src,int len=-1)
	{
		int ptr = 0;
		int start = 0;
		int rv = 0;
		if (len<0) len = strlen(src);
		while(ptr<len)
		{
			if (src[ptr]==3)
			{
				rv += pfc::strlen_utf8(src+start,ptr-start);
				ptr++;
				while(ptr<len && src[ptr]!=3) ptr++;
				if (ptr<len) ptr++;
				start = ptr;
			}
			else ptr++;
		}
		rv += pfc::strlen_utf8(src+start,ptr-start);
		return rv;
	}


	BOOL text_out_colours_ellipsis(HDC dc,const char * src_c,int src_c_len,int x_offset,int pos_y,const RECT * base_clip,bool selected,bool show_ellipsis,DWORD default_color,alignment align, unsigned * p_width, bool b_set_default_colours, unsigned * p_position)
	{
		class colour_change_data
		{
		public:
			t_size index, length;
			COLORREF colour;
		};

		pfc::stringcvt::string_wide_from_utf8 wstr(src_c, src_c_len);

		pfc::array_t<wchar_t> src_s;
		//src_s.append_fromptr(wstr.get_ptr(), wstr.length());
		const wchar_t * src = wstr.get_ptr();

		t_size wLen = wstr.length();

		if (!base_clip) return FALSE;

		if (base_clip)
		{
			if (is_rect_null(base_clip) || base_clip->right<=base_clip->left+x_offset || base_clip->bottom<=pos_y) return TRUE;
		}


		pfc::list_t<colour_change_data, pfc::alloc_fast_aggressive> colourChanges;

		bool b_lastColourUsed = true;
		COLORREF cr_last = NULL;

		{
			const wchar_t * p_start = src;
			const wchar_t * p_block_start = src;
			const wchar_t * p_current = src;

			COLORREF cr_current = b_set_default_colours ? default_color : GetTextColor(dc);

			while ((p_current - p_start) < wLen)
			{
				p_block_start = p_current;
				while ( (p_current - p_start) < wLen && p_current[0] != 3 )
				{
					p_current++;
				}
				if (p_current > p_block_start)
				{
					colour_change_data temp;
					temp.index = src_s.get_size();
						//p_block_start - src;
					temp.length = p_current-p_block_start;
					temp.colour = cr_current;
					colourChanges.add_item(temp);
					src_s.append_fromptr(p_block_start, p_current-p_block_start);
					b_lastColourUsed = true;
				}
				else if ((p_current - p_start) >= wLen)
					break; 
				if ((p_current - p_start) + 1 < wLen) //need another at least char for valid colour code operation
				{
					if (p_current[0] == 3)
					{
						COLORREF new_colour;
						COLORREF new_colour_selected;

						bool have_selected = false;

						if (p_current[1]==3) 
						{
							new_colour=new_colour_selected=default_color;
							have_selected=true;
							p_current+=2;
						}
						else
						{
							p_current++;
							new_colour = strtoul_t(p_current,min(6,wLen-(p_current-p_start)), 0x10);
							while((p_current-p_start)<wLen && p_current[0]!=3 && p_current[0]!='|')
							{
								p_current++;
							}
							if ((p_current-p_start)<wLen && p_current[0]=='|')
							{
								if ((p_current-p_start)+1<wLen)
								{
									p_current++;
									new_colour_selected = strtoul_t(p_current,min(6,wLen-(p_current-p_start)), 0x10);
									have_selected = true;
								}
								else
									break; //unexpected eos, malformed string

								while((p_current-p_start)<wLen && p_current[0]!=3) p_current++;
							}
							p_current++;
						}
						if (selected) new_colour = have_selected ? new_colour_selected : 0xFFFFFF - new_colour;
						
						b_lastColourUsed = false;
						cr_last = (cr_current = new_colour);
					}
				}
				else
					break; //unexpected eos, malformed string
			}

		}

		RECT clip = *base_clip, ellipsis = {0,0,0,0};

		t_size i, colourChangeCount = colourChanges.get_count();
		int widthTotal = 0, availableWidth = clip.right - clip.left - x_offset;

		src = src_s.get_ptr();
		wLen = src_s.get_size();

		pfc::array_staticsize_t<ScriptString_instance> scriptStrings(colourChangeCount);
		pfc::array_staticsize_t<int> characterExtents (wLen);

		for (i=0; i<colourChangeCount; i++)
		{
			t_size thisWLen = colourChanges[i].length;
			if (thisWLen)
			{
				const wchar_t * thisWStr = &src[colourChanges[i].index];
				scriptStrings[i].analyse(dc, thisWStr, thisWLen, /*availableWidth - widthTotal*/ -1, false);
				scriptStrings[i].get_character_logical_extents(&characterExtents[colourChanges[i].index], i && colourChanges[i].index ? characterExtents[colourChanges[i].index-1] : 0);
				widthTotal += scriptStrings[i].get_output_width();
				if (widthTotal > availableWidth)
				{
					colourChangeCount = i+1;
					break;
				}
			}
		}


		COLORREF cr_old = GetTextColor(dc);

		SetTextAlign(dc,TA_LEFT);
		SetBkMode(dc,TRANSPARENT);
		//if (b_set_default_colours)
		//	SetTextColor(dc, default_color);

		int height = uGetTextHeight(dc);

		int position = clip.left;//item.left+BORDER;

#if 1
		//ScriptString_instance pScriptString(dc, src_c, len, -1, false);
#endif

		{

#if 1
			if (show_ellipsis)
			{
				if (widthTotal > availableWidth) 
				{
					int ellipsis_width = get_text_width(dc, ELLIPSIS, ELLIPSIS_LEN)+2;
					if (ellipsis_width <= clip.right - clip.left - x_offset)
					{
						for (i=wLen; i; i--)
						{
							if (colourChangeCount && colourChanges[colourChangeCount-1].length == 0)
							{
								colourChangeCount--;
							}
							else if ( (i > 2 && characterExtents[i-2] + ellipsis_width <= availableWidth) || i == 1)
							{
								src_s[i-1] = 0x2026;
								src_s.set_size(i);
								src = src_s.get_ptr();
								wLen = i;
								if (colourChangeCount)
								{
									widthTotal -= scriptStrings[colourChangeCount-1].get_output_width();
									scriptStrings[colourChangeCount-1].analyse(dc, &src[colourChanges[colourChangeCount-1].index], colourChanges[colourChangeCount-1].length, -1, false);
									widthTotal += scriptStrings[colourChangeCount-1].get_output_width();
								}
								break;
							}
							else if (colourChangeCount && colourChanges[colourChangeCount-1].index == i-1)
							{
								colourChangeCount--;
								widthTotal -= scriptStrings[colourChangeCount].get_output_width();
							}
							else if (colourChangeCount)
							{
								colourChanges[colourChangeCount-1].length = i - colourChanges[colourChangeCount-1].index - 1;
							}
						}
					}
				}
			}
#endif
		}
		//	if (!need_ellipsis)
		{
			if (align == ALIGN_CENTRE)
			{
				position += (clip.right - clip.left - widthTotal)/2;
			}
			else if (align == ALIGN_RIGHT)
			{
				position += (clip.right - clip.left - widthTotal - x_offset);
			}
			else 
			{ 
				position+=x_offset;
			}
		}

		if (p_width) *p_width = NULL;
		for (i=0; i<colourChangeCount; i++)
		{
			SetTextColor(dc, colourChanges[i].colour);
			scriptStrings[i].text_out(position,pos_y,ETO_CLIPPED,&clip);
			int thisWidth = scriptStrings[i].get_output_width();
			position += thisWidth;
			if (p_width) *p_width += thisWidth;
		}

		if (p_position) *p_position = position;


		/*	if (need_ellipsis)
		{
		SetTextColor(dc,default_color);
		uExtTextOut(dc,ellipsis.left+1,pos_y,ETO_CLIPPED,&ellipsis,ELLIPSIS,ELLIPSIS_LEN,0);
		}*/

		if (b_set_default_colours)
			SetTextColor(dc, cr_old);
		else if (!b_lastColourUsed)
			SetTextColor(dc, cr_last);

		return TRUE;
	}

	BOOL text_out_colours_tab(HDC dc,const char * display,int display_len,int left_offset,int border,const RECT * base_clip,bool selected,DWORD default_color,bool columns,bool use_tab,bool show_ellipsis,alignment align, unsigned * p_width, bool b_set_default_colours, bool b_vertical_align_centre, unsigned * p_position)
	{

		if (!base_clip) return FALSE;

		RECT clip = *base_clip;

		if (is_rect_null(&clip)) return TRUE;

		int extra = (clip.bottom-clip.top - uGetTextHeight(dc));

		int pos_y = clip.top + (b_vertical_align_centre ? (extra/2) : extra);

		int n_tabs = 0;

		display_len = pfc::strlen_max(display, display_len);
		

		columns = use_tab; //always called equal

		if (use_tab)
		{
			int start = 0;
			int n;
			for(n=0;n<display_len;n++)
			{
				if (display[n]=='\t')
				{
					start = n+1;
					n_tabs++;
				}
			}
		}

		if (n_tabs == 0)
		{
			clip.left += border;
			clip.right -= border;
			return text_out_colours_ellipsis(dc, display, display_len, left_offset,pos_y,&clip,selected,show_ellipsis,default_color,align, p_width, b_set_default_colours, p_position);
		}

		if (p_width)
			*p_width = clip.right;
		if (p_position)
			*p_position = clip.right;

		clip.left += left_offset;
		int tab_total = clip.right - clip.left;
		
		int ptr = display_len;
		int tab_ptr = 0;
		int written = 0;
		int clip_x = clip.right;
		RECT t_clip = clip;

		do
		{
			int ptr_end = ptr;
			while(ptr>0 && display[ptr-1]!='\t') ptr--;
			const char * t_string = display + ptr;
			int t_length = ptr_end - ptr;
			if (t_length>0)
			{
				t_clip.right -= border;

				//int t_width = get_text_width_color(dc,t_string,t_length) + border*2 + left_offset;

				if (tab_ptr)
					t_clip.left = clip.right - MulDiv(tab_ptr,tab_total,n_tabs) + border;
				

				unsigned p_written = NULL;
				text_out_colours_ellipsis(dc,t_string,t_length,0,pos_y,&t_clip,selected,show_ellipsis,default_color,tab_ptr ? ALIGN_LEFT : ALIGN_RIGHT, &p_written, b_set_default_colours);

				if (tab_ptr == 0)
					t_clip.left = t_clip.right - p_written;

				t_clip.right = t_clip.left - border;
			}

			if (ptr>0)
			{
				ptr--;//tab char
				tab_ptr++;
			}
		}
		while(ptr>0);

		return TRUE;
	}



#if 0
	BOOL text_out_colours_tab_v1(HDC dc,const char * display,int display_len,int left_offset,int border,const RECT * base_clip,bool selected,DWORD default_color,bool columns,bool use_tab,bool show_ellipsis,alignment align, unsigned * p_width, bool b_set_default_colours, bool b_vertical_align_centre)
	{

		if (!base_clip) return FALSE;

		RECT clip = *base_clip;

		if (is_rect_null(&clip)) return TRUE;

		int extra = (clip.bottom-clip.top - uGetTextHeight(dc));

		int pos_y = clip.top + (b_vertical_align_centre ? (extra/2) : extra);

		int n_tabs = 0;
		int total_width = 0;

		if (use_tab)
		{
			int start = 0;
			int n;
			for(n=0;n<display_len;n++)
			{
				if (display[n]=='\t')
				{
					if (start<n) total_width += get_text_width_color(dc,display+start,n-start) + 2*border;
					start = n+1;
					n_tabs++;
				}
			}
			if (start<display_len)
			{
				//add width of rest of text after last tab
				total_width += get_text_width_color(dc,display+start,display_len-start) + 2*border;
			}
		}

		if (n_tabs == 0)
		{
			clip.left += border;
			clip.right -= border;
			return text_out_colours_ellipsis(dc, display, display_len, left_offset,pos_y,&clip,selected,show_ellipsis,default_color,align, p_width, b_set_default_colours);
		}

		if (p_width)
			*p_width = clip.right;


		int tab_total = clip.right - clip.left;
		if (!columns) tab_total -= total_width;
		int ptr = display_len;
		int tab_ptr = 0;
		int written = 0;
		int clip_x = clip.right;
		do
		{
			int ptr_end = ptr;
			while(ptr>0 && display[ptr-1]!='\t') ptr--;
			const char * t_string = display + ptr;
			int t_length = ptr_end - ptr;
			if (t_length>0)
			{
				int t_width = get_text_width_color(dc,t_string,t_length) + border*2 + left_offset;

				int pos_x;
				int pos_x_right;

				if (!columns)
				{
					pos_x_right = clip.right - MulDiv(tab_ptr,tab_total,n_tabs) - written/* - left_offset*/;
				}
				else
				{
					if (tab_ptr==0) pos_x_right = clip.right/* - left_offset*/;
					else pos_x_right = clip.right - MulDiv(tab_ptr,tab_total,n_tabs) + t_width/* - left_offset*/;
				}

				if (ptr==0) 
				{
					pos_x = left_offset + border;
				}
				else
				{			
					pos_x = pos_x_right - t_width - clip.left + left_offset + border;
					//			if (pos_x<0) pos_x = clip.left+left_offset;
				}

				RECT t_clip = clip;

				//console::printf(" tab %u, right %i   clip %i",tab_ptr,t_clip.right,clip_x);

				if (t_clip.right > clip_x/* + clip.left*/) 
					t_clip.right = clip_x/* + clip.left*/;

				text_out_colours_ellipsis(dc,t_string,t_length,pos_x,pos_y,&t_clip,selected,show_ellipsis,default_color,ALIGN_LEFT, 0, b_set_default_colours);

				if (clip_x> (pos_x + clip.left - left_offset-border*2))
					clip_x = pos_x + clip.left - left_offset-border*2;

				written += t_width;
			}

			if (ptr>0)
			{
				ptr--;//tab char
				tab_ptr++;
			}
		}
		while(ptr>0);

		return TRUE;
	}
#endif


};