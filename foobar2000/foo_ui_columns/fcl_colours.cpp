#include "stdafx.h"

class export_colours : public cui::fcl::dataset
{
	enum t_colour_pview_identifiers
	{
		colours_pview_mode,
		colours_pview_background,
		colours_pview_selection_background,
		colours_pview_inactive_selection_background,
		colours_pview_text,
		colours_pview_selection_text,
		colours_pview_inactive_selection_text,
		colours_pview_header_font,
		colours_pview_list_font,
		colours_pview_use_system_focus_frame,
		identifier_item_height,
	};
	virtual void get_name (pfc::string_base & p_out) const
	{
		p_out = "Colours";
	}
	virtual const GUID & get_group () const
	{
		return cui::fcl::groups::colours_and_fonts;
	}
	virtual const GUID & get_guid () const
	{
		// {1D5291B1-392D-4469-B905-91202B80EB7B}
		static const GUID guid = 
		{ 0x1d5291b1, 0x392d, 0x4469, { 0xb9, 0x5, 0x91, 0x20, 0x2b, 0x80, 0xeb, 0x7b } };
		return guid;
	}
	virtual void get_data (stream_writer * p_writer, t_uint32 type, cui::fcl::t_export_feedback & feedback, abort_callback & p_abort) const
	{
		fcl::writer out(p_writer, p_abort);
		/*out.write_item(colours_pview_mode, cfg_pv_use_custom_colours);
		out.write_item(colours_pview_background, cfg_back);
		out.write_item(colours_pview_selection_background, cfg_pv_selected_back);
		out.write_item(colours_pview_inactive_selection_background, cfg_pv_selected_text_no_focus);
		out.write_item(colours_pview_text, cfg_pv_text_colour);
		out.write_item(colours_pview_selection_text, cfg_pv_selected_text_colour);
		out.write_item(colours_pview_inactive_selection_text, cfg_pv_selected_text_no_focus);
		out.write_item(colours_pview_list_font, cfg_font);
		out.write_item(colours_pview_header_font, cfg_header_font);*/
		//out.write_item(colours_pview_use_system_focus_frame, cfg_pv_use_system_frame);
		out.write_item(identifier_item_height, cfg_height);
	}
	virtual void set_data (stream_reader * p_reader, t_size stream_size, t_uint32 type, cui::fcl::t_import_feedback & feedback, abort_callback & p_abort)
	{
		fcl::reader reader(p_reader, stream_size, p_abort);
		t_uint32 element_id;
		t_uint32 element_size;
		bool b_font_read = false, b_colour_read=false;

		while (reader.get_remaining())
		{
			reader.read_item(element_id);
			reader.read_item(element_size);

			switch (element_id)
			{
			case identifier_item_height:
				reader.read_item(cfg_height);
				break;
			case colours_pview_mode:
				reader.read_item(cfg_pv_use_custom_colours);
				break;
			case colours_pview_use_system_focus_frame:
				reader.read_item(cfg_pv_use_system_frame);
				break;
			case colours_pview_background:
				b_colour_read=true;
				reader.read_item(cfg_back);
				break;
			case colours_pview_selection_background:
				reader.read_item(cfg_pv_selected_back);
				break;
			case colours_pview_inactive_selection_background:
				reader.read_item(cfg_pv_selected_text_no_focus);
				break;
			case colours_pview_text:
				reader.read_item(cfg_pv_text_colour);
				break;
			case colours_pview_selection_text:
				reader.read_item(cfg_pv_selected_text_colour);
				break;
			case colours_pview_inactive_selection_text:
				reader.read_item(cfg_pv_selected_text_no_focus);
				break;
			case colours_pview_header_font:
				reader.read_item(cfg_header_font);
				break;
			case colours_pview_list_font:
				b_font_read = true;
				reader.read_item(cfg_font);
				break;
			default:
				reader.skip(element_size);
				break;
			};
		}

		//on_header_font_change();
		//on_playlist_font_change();
		//pvt::ng_playlist_view_t::g_on_font_change();
		//pvt::ng_playlist_view_t::g_on_header_font_change();
		if (b_font_read)
			g_import_fonts_to_unified(true, false, false);
		if (b_colour_read)
			g_import_pv_colours_to_unified_global();
		//refresh_all_playlist_views();	
		//pvt::ng_playlist_view_t::g_update_all_items();
	}
};

cui::fcl::dataset_factory<export_colours> g_export_colours_t;

class export_colours_switcher : public cui::fcl::dataset
{
	enum t_colour_switcher_identifiers
	{
		colours_switcher_mode, //not used
		colours_switcher_background,
		colours_switcher_selection_background,
		colours_switcher_inactive_selection_background,
		colours_switcher_text,
		colours_switcher_selection_text,
		colours_switcher_inactive_selection_text,
		colours_switcher_font_tabs,
		colours_switcher_font_list,
		identifier_item_height,
	};
	virtual void get_name (pfc::string_base & p_out) const
	{
		p_out = "Colours";
	}
	virtual const GUID & get_group () const
	{
		return cui::fcl::groups::colours_and_fonts;
	}
	virtual const GUID & get_guid () const
	{
		// {1DE0CF38-5E8E-439c-8F01-B8999975AC0D}
		static const GUID guid = 
		{ 0x1de0cf38, 0x5e8e, 0x439c, { 0x8f, 0x1, 0xb8, 0x99, 0x99, 0x75, 0xac, 0xd } };
		return guid;
	}
	virtual void get_data (stream_writer * p_writer, t_uint32 type, cui::fcl::t_export_feedback & feedback, abort_callback & p_abort) const
	{
		fcl::writer out(p_writer, p_abort);
		/*out.write_item(colours_switcher_background, cfg_plist_bk);
		out.write_item(colours_switcher_selection_background, cfg_plist_selected_back);
		out.write_item(colours_switcher_inactive_selection_background, cfg_plist_selected_back_no_focus);
		out.write_item(colours_switcher_text, cfg_plist_text);
		out.write_item(colours_switcher_selection_text, cfg_plist_selected_text);
		out.write_item_config(colours_switcher_inactive_selection_text, playlist_switcher::colours::config_inactive_selection_text);
		out.write_item(colours_switcher_font_tabs, cfg_tab_font);
		out.write_item(colours_switcher_font_list, cfg_plist_font);*/
		out.write_item(identifier_item_height, cfg_plheight);
	}
	virtual void set_data (stream_reader * p_reader, t_size stream_size, t_uint32 type, cui::fcl::t_import_feedback & feedback, abort_callback & p_abort)
	{
		fcl::reader reader(p_reader, stream_size, p_abort);
		t_uint32 element_id;
		t_uint32 element_size;
		bool b_font_read = false;

		while (reader.get_remaining())
		{
			reader.read_item(element_id);
			reader.read_item(element_size);

			switch (element_id)
			{
			case identifier_item_height:
				reader.read_item(cfg_plheight);
				break;
			case colours_switcher_background:
				reader.read_item(cfg_plist_bk);
				break;
			case colours_switcher_selection_background:
				reader.read_item(cfg_plist_selected_back);
				break;
			case colours_switcher_inactive_selection_background:
				reader.read_item(cfg_plist_selected_back_no_focus);
				break;
			case colours_switcher_text:
				reader.read_item(cfg_plist_text);
				break;
			case colours_switcher_selection_text:
				reader.read_item(cfg_plist_selected_text);
				break;
			case colours_switcher_inactive_selection_text:
				reader.read_item_config(playlist_switcher::colours::config_inactive_selection_text);
				break;
			case colours_switcher_font_list:
				b_font_read = true;
				reader.read_item(cfg_plist_font);
				break;
			case colours_switcher_font_tabs:
				reader.read_item(cfg_tab_font);
				break;
			default:
				reader.skip(element_size);
				break;
			};

			if (b_font_read)
				g_import_fonts_to_unified(false, true, false);
		}

		//update_playlist_switcher_panels();
		//on_switcher_font_change();
		//g_on_tabs_font_change();
	}
};

cui::fcl::dataset_factory<export_colours_switcher> g_export_colours_switcher_t;

class export_misc_fonts : public cui::fcl::dataset
{
	enum t_colour_pview_identifiers
	{
		font_status,
	};
	virtual void get_name (pfc::string_base & p_out) const
	{
		p_out = "Misc fonts";
	}
	virtual const GUID & get_group () const
	{
		return cui::fcl::groups::colours_and_fonts;
	}
	virtual const GUID & get_guid () const
	{
		// {0A297BE7-DE43-49da-8D8E-C8D888CF1014}
		static const GUID guid = 
		{ 0xa297be7, 0xde43, 0x49da, { 0x8d, 0x8e, 0xc8, 0xd8, 0x88, 0xcf, 0x10, 0x14 } };
		return guid;
	}
	virtual void get_data (stream_writer * p_writer, t_uint32 type, cui::fcl::t_export_feedback & feedback, abort_callback & p_abort) const
	{
		fcl::writer out(p_writer, p_abort);
		//out.write_item(font_status, cfg_status_font);
	}
	virtual void set_data (stream_reader * p_reader, t_size stream_size, t_uint32 type, cui::fcl::t_import_feedback & feedback, abort_callback & p_abort)
	{
		fcl::reader reader(p_reader, stream_size, p_abort);
		t_uint32 element_id;
		t_uint32 element_size;
		bool b_font_read = false;

		while (reader.get_remaining())
		{
			reader.read_item(element_id);
			reader.read_item(element_size);

			switch (element_id)
			{
			case font_status:
				b_font_read = true;
				reader.read_item(cfg_status_font);
				break;
			default:
				reader.skip(element_size);
				break;
			};
		}

		if (b_font_read)
			g_import_fonts_to_unified(false, false, true);
		//on_status_font_change();
	}
};

cui::fcl::dataset_factory<export_misc_fonts> g_export_misc_fonts_t;
