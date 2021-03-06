#include "stdafx.h"

cui::fcl::group_impl_factory fclgroupcolumns(cui::fcl::groups::titles_playlist_view, "Columns Playlist Scripts", "Columns Playlist Scripts", cui::fcl::groups::title_scripts);
cui::fcl::group_impl_factory fclgroupcommon(cui::fcl::groups::titles_common, "Common Scripts", "Common Scripts", cui::fcl::groups::title_scripts);

class export_columns : public cui::fcl::dataset
{
	enum t_identifiers
	{
		identifier_column,
	};
	enum t_columns
	{
		identifier_name,
		identifier_display,
		identifier_sort,
		identifier_use_sort,
		identifier_style,
		identifier_use_style,
		identifier_width,
		identifier_alignment,
		identifier_filter_type,
		identifier_filter,
		identifier_resize,
		identifier_show,
		identifier_edit_field,

	};
	virtual void get_name (pfc::string_base & p_out) const
	{
		p_out = "Columns";
	}
	virtual const GUID & get_group () const
	{
		return cui::fcl::groups::titles_playlist_view;
	}
	virtual const GUID & get_guid () const
	{
		// {2415AAE7-7F5E-4ad8-94E2-1E730A2139EF}
		static const GUID guid = 
		{ 0x2415aae7, 0x7f5e, 0x4ad8, { 0x94, 0xe2, 0x1e, 0x73, 0xa, 0x21, 0x39, 0xef } };
		return guid;
	}
	virtual void get_data (stream_writer * p_writer, t_uint32 type, cui::fcl::t_export_feedback & feedback, abort_callback & p_abort) const
	{
		fcl::writer out(p_writer, p_abort);
		t_size count = g_columns.get_count(), i;
		pfc::string8 temp;
		out.write_raw(count);

		for (i=0; i<count; i++)
		{
			stream_writer_memblock sw;
			fcl::writer w(&sw, p_abort);
			w.write_item(identifier_name, g_columns[i]->name);
			w.write_item(identifier_display, g_columns[i]->spec);
			w.write_item(identifier_style, g_columns[i]->colour_spec);
			w.write_item(identifier_edit_field, g_columns[i]->edit_field);
			w.write_item(identifier_filter, g_columns[i]->filter);
			w.write_item(identifier_sort, g_columns[i]->sort_spec);
			w.write_item(identifier_use_style, g_columns[i]->use_custom_colour);
			w.write_item(identifier_use_sort, g_columns[i]->use_custom_sort);
			w.write_item(identifier_show, g_columns[i]->show);
			w.write_item(identifier_filter_type, (t_uint32)g_columns[i]->filter_type);
			w.write_item(identifier_alignment, (t_uint32)g_columns[i]->align);
			w.write_item(identifier_resize, (t_uint32)g_columns[i]->parts);
			w.write_item(identifier_width, (t_uint32)g_columns[i]->width);

			out.write_item(identifier_column, sw.m_data.get_ptr(), sw.m_data.get_size());
		}
	}
	virtual void set_data (stream_reader * p_reader, t_size stream_size, t_uint32 type, cui::fcl::t_import_feedback & feedback, abort_callback & p_abort)
	{
		fcl::reader reader(p_reader, stream_size, p_abort);
		t_size i, count;
		reader.read_item(count);
		column_list_t newcolumns;
		for (i=0; i<count; i++)
		{
			t_uint32 column_id;
			t_uint32 column_size;

			reader.read_item(column_id);
			reader.read_item(column_size);

			column_t::ptr item = new column_t;

			fcl::reader reader2(p_reader, column_size, p_abort);

			t_uint32 element_id;
			t_uint32 element_size;

			while (reader2.get_remaining())
			{
				reader2.read_item(element_id);
				reader2.read_item(element_size);

				switch (element_id)
				{
				case identifier_name:
					reader2.read_item(item->name, element_size);
					break;
				case identifier_filter:
					reader2.read_item(item->filter, element_size);
					break;
				case identifier_sort:
					reader2.read_item(item->sort_spec, element_size);
					break;
				case identifier_display:
					reader2.read_item(item->spec, element_size);
					break;
				case identifier_edit_field:
					reader2.read_item(item->edit_field, element_size);
					break;
				case identifier_style:
					reader2.read_item(item->colour_spec, element_size);
					break;
				case identifier_resize:
					{
						reader2.read_item(item->parts);
					}
					break;
				case identifier_width:
					{
						reader2.read_item(item->width);
					}
					break;
				case identifier_alignment:
					{
						t_uint32 temp;
						reader2.read_item(temp);
						item->align = ((alignment)temp);
					}
					break;
				case identifier_filter_type:
					{
						t_uint32 temp;
						reader2.read_item(temp);
						item->filter_type = ((playlist_filter_type)temp);
					}
					break;
				case identifier_use_sort:
					{
						reader2.read_item(item->use_custom_sort);
					}
					break;
				case identifier_use_style:
					{
						reader2.read_item(item->use_custom_colour);
					}
					break;
				case identifier_show:
					{
						reader2.read_item(item->show);
					}
					break;
				default:
					reader2.skip(element_size);
					break;
				};
			}
			newcolumns.add_item(item);
		}

		g_columns.set_entries_ref(newcolumns);
		refresh_all_playlist_views();	
		pvt::ng_playlist_view_t::g_on_columns_change();
	}
};

cui::fcl::dataset_factory<export_columns> g_export_coloumns_t;

class export_pview : public cui::fcl::dataset
{
	enum t_identifiers
	{
		identifier_global_script,
		identifier_style_script,
		identifier_show_header,
		identifier_autosize_columns,
		identifier_use_globals_for_sorting,
		identifier_use_dates,
		identifier_use_globals,
	};
	virtual void get_name (pfc::string_base & p_out) const
	{
		p_out = "Colours";
	}
	virtual const GUID & get_group () const
	{
		return cui::fcl::groups::titles_playlist_view;
	}
	virtual const GUID & get_guid () const
	{
		// {190F4811-899A-4366-A181-FF5161FC1C77}
		static const GUID guid = 
		{ 0x190f4811, 0x899a, 0x4366, { 0xa1, 0x81, 0xff, 0x51, 0x61, 0xfc, 0x1c, 0x77 } };
		return guid;
	}
	virtual void get_data (stream_writer * p_writer, t_uint32 type, cui::fcl::t_export_feedback & feedback, abort_callback & p_abort) const
	{
		fcl::writer out(p_writer, p_abort);

		out.write_item(identifier_global_script, cfg_globalstring);
		out.write_item(identifier_style_script, cfg_colour);
		out.write_item(identifier_show_header, cfg_header);
		out.write_item(identifier_autosize_columns, cfg_nohscroll);
		out.write_item(identifier_use_globals_for_sorting, cfg_global_sort);
		out.write_item(identifier_use_dates, cfg_playlist_date);
		out.write_item(identifier_use_globals, cfg_global);
	}
	virtual void set_data (stream_reader * p_reader, t_size stream_size, t_uint32 type, cui::fcl::t_import_feedback & feedback, abort_callback & p_abort)
	{
		fcl::reader reader(p_reader, stream_size, p_abort);
		t_uint32 element_id;
		t_uint32 element_size;

		while (reader.get_remaining())
		{
			reader.read_item(element_id);
			reader.read_item(element_size);

			switch (element_id)
			{
			case identifier_global_script:
				reader.read_item(cfg_globalstring, element_size);
				break;
			case identifier_style_script:
				reader.read_item(cfg_colour, element_size);
				break;
			case identifier_show_header:
				reader.read_item(cfg_header);
				break;
			case identifier_autosize_columns:
				reader.read_item(cfg_nohscroll);
				break;
			case identifier_use_globals_for_sorting:
				reader.read_item(cfg_global_sort);
				break;
			case identifier_use_dates:
				reader.read_item(cfg_playlist_date);
				break;
			case identifier_use_globals:
				reader.read_item(cfg_global);
				break;
			default:
				reader.skip(element_size);
				break;
			};
		}

		refresh_all_playlist_views();	
		pvt::ng_playlist_view_t::g_on_autosize_change();
		pvt::ng_playlist_view_t::g_on_vertical_item_padding_change();
		pvt::ng_playlist_view_t::g_on_show_header_change();
		pvt::ng_playlist_view_t::g_update_all_items();
	}
};

cui::fcl::dataset_factory<export_pview> g_export_pview_t;

class export_titles : public cui::fcl::dataset
{
	enum t_identifiers
	{
		identifier_main_window_title,
		identifier_status_bar,
		identifier_notification_icon_tooltip,
		identifier_copy_command,
		identifier_playlist,
	};
	virtual void get_name (pfc::string_base & p_out) const
	{
		p_out = "Titles";
	}
	virtual const GUID & get_group () const
	{
		return cui::fcl::groups::titles_common;
	}
	virtual const GUID & get_guid () const
	{
		// {9AF6A28B-3EFF-4d1a-AD81-FA1759F1D66C}
		static const GUID guid = 
		{ 0x9af6a28b, 0x3eff, 0x4d1a, { 0xad, 0x81, 0xfa, 0x17, 0x59, 0xf1, 0xd6, 0x6c } };
		return guid;
	}
	virtual void get_data (stream_writer * p_writer, t_uint32 type, cui::fcl::t_export_feedback & feedback, abort_callback & p_abort) const
	{
		fcl::writer out(p_writer, p_abort);

		out.write_item(identifier_status_bar, main_window::config_status_bar_script.get());
		out.write_item(identifier_notification_icon_tooltip, main_window::config_notification_icon_script.get());
		out.write_item(identifier_main_window_title, main_window::config_main_window_title_script.get());
	}
	virtual void set_data (stream_reader * p_reader, t_size stream_size, t_uint32 type, cui::fcl::t_import_feedback & feedback, abort_callback & p_abort)
	{
		fcl::reader reader(p_reader, stream_size, p_abort);
		t_uint32 element_id;
		t_uint32 element_size;

		while (reader.get_remaining())
		{
			reader.read_item(element_id);
			reader.read_item(element_size);

			pfc::string8 buffer;

			switch (element_id)
			{
			case identifier_main_window_title:
				reader.read_item(buffer, element_size);
				main_window::config_main_window_title_script.set(buffer);
				break;
			case identifier_notification_icon_tooltip:
				reader.read_item(buffer, element_size);
				main_window::config_notification_icon_script.set(buffer);
				break;
			case identifier_status_bar:
				reader.read_item(buffer, element_size);
				main_window::config_status_bar_script.set(buffer);
				break;
			default:
				reader.skip(element_size);
				break;
			};
		}
	}
};

cui::fcl::dataset_factory<export_titles> g_export_titles_t;