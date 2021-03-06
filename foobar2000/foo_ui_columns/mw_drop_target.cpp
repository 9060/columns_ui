#include "stdafx.h"

bool g_last_rmb = false;

drop_handler_interface::drop_handler_interface() : drop_ref_count(0)
{
	last_over.x = 0; last_over.y = 0;
	m_DropTargetHelper.instantiate(CLSID_DragDropHelper, NULL, CLSCTX_INPROC_SERVER);
}

HRESULT STDMETHODCALLTYPE drop_handler_interface::Drop(IDataObject *pDataObj, DWORD grfKeyState, POINTL ptl, DWORD *pdwEffect)
{
	POINT pt = { ptl.x, ptl.y };
	if (m_DropTargetHelper.is_valid()) m_DropTargetHelper->Drop(pDataObj, &pt, *pdwEffect);

	static_api_ptr_t<playlist_manager> playlist_api;
	static_api_ptr_t<playlist_incoming_item_filter> incoming_api;

	HWND wnd = WindowFromPoint(pt);
	bool b_fuck_death = wnd != g_main_window && !IsChild(g_main_window, wnd);

	*pdwEffect = b_fuck_death ? DROPEFFECT_NONE : DROPEFFECT_COPY;

	bool process = !ui_drop_item_callback::g_on_drop(pDataObj) && !b_fuck_death;

	if (process && g_last_rmb)
	{
		process = false;
		enum { ID_DROP = 1, ID_CANCEL };

		HMENU menu = CreatePopupMenu();

		uAppendMenu(menu, (MF_STRING), ID_DROP, "&Add files here");
		uAppendMenu(menu, MF_SEPARATOR, 0, 0);
		uAppendMenu(menu, MF_STRING, ID_CANCEL, "&Cancel");

		int cmd = TrackPopupMenu(menu, TPM_RIGHTBUTTON | TPM_NONOTIFY | TPM_RETURNCMD, pt.x, pt.y, 0, g_main_window, 0);
		DestroyMenu(menu);

		if (cmd)
		{
			switch (cmd)
			{
			case ID_DROP:
				process = true;
				break;
			}
		}
	}

	if (process)
	{
		metadb_handle_list data;

		incoming_api->process_dropped_files(pDataObj, data, true, g_main_window);

		bool send_new_playlist = false;

		int idx = -1;

		playlist_api->activeplaylist_undo_backup();
		playlist_api->activeplaylist_clear_selection();
		playlist_api->activeplaylist_insert_items(idx, data, bit_array_true());

		data.remove_all();
	}


	if (m_DropTargetHelper.is_valid()) m_DropTargetHelper->DragLeave();
	mmh::ole::SetDropDescription(m_DataObject.get_ptr(), DROPIMAGE_INVALID, "", "");

	m_DataObject.release();

	return S_OK;
}

HRESULT STDMETHODCALLTYPE drop_handler_interface::DragLeave(void)
{
	if (m_DropTargetHelper.is_valid()) m_DropTargetHelper->DragLeave();
	mmh::ole::SetDropDescription(m_DataObject.get_ptr(), DROPIMAGE_INVALID, "", "");

	last_over.x = 0;
	last_over.y = 0;
	m_DataObject.release();
	return S_OK;
}

HRESULT STDMETHODCALLTYPE drop_handler_interface::DragOver(DWORD grfKeyState, POINTL ptl, DWORD *pdwEffect)
{
	POINT pt = { ptl.x, ptl.y };
	if (m_DropTargetHelper.is_valid()) m_DropTargetHelper->DragOver(&pt, *pdwEffect);

	g_last_rmb = ((grfKeyState & MK_RBUTTON) != 0);

	HWND wnd = WindowFromPoint(pt);
	bool b_fuck_death = wnd != g_main_window && !IsChild(g_main_window, wnd);

	//if (last_over.x != pt.x || last_over.y != pt.y)
	if (ui_drop_item_callback::g_is_accepted_type(m_DataObject, pdwEffect) || (!b_fuck_death && static_api_ptr_t<playlist_incoming_item_filter>()->process_dropped_files_check(m_DataObject)))
	{
		*pdwEffect = DROPEFFECT_COPY;

		pfc::string8 name;
		static_api_ptr_t<playlist_manager>()->activeplaylist_get_name(name);
		mmh::ole::SetDropDescription(m_DataObject, DROPIMAGE_COPY, "Add to %1", name);
	}
	else
	{
		*pdwEffect = DROPEFFECT_NONE;
		mmh::ole::SetDropDescription(m_DataObject.get_ptr(), DROPIMAGE_INVALID, "", "");
	}

	last_over = ptl;


	return S_OK;
}

HRESULT STDMETHODCALLTYPE drop_handler_interface::DragEnter(IDataObject *pDataObj, DWORD grfKeyState, POINTL ptl, DWORD *pdwEffect)
{
	POINT pt = { ptl.x, ptl.y };
	if (m_DropTargetHelper.is_valid()) m_DropTargetHelper->DragEnter(g_main_window, pDataObj, &pt, *pdwEffect);

	m_DataObject = pDataObj;

	last_over.x = 0;
	last_over.y = 0;
	g_last_rmb = ((grfKeyState & MK_RBUTTON) != 0);

	HWND wnd = WindowFromPoint(pt);
	bool b_fuck_death = wnd != g_main_window && !IsChild(g_main_window, wnd);

	if (ui_drop_item_callback::g_is_accepted_type(pDataObj, pdwEffect) || (!b_fuck_death && static_api_ptr_t<playlist_incoming_item_filter>()->process_dropped_files_check(pDataObj)))
	{
		*pdwEffect = DROPEFFECT_COPY;
		pfc::string8 name;
		static_api_ptr_t<playlist_manager>()->activeplaylist_get_name(name);
		mmh::ole::SetDropDescription(pDataObj, DROPIMAGE_COPY, "Add to %1", name);
		return S_OK;
	}
	else
	{
		*pdwEffect = DROPEFFECT_NONE;
		mmh::ole::SetDropDescription(m_DataObject.get_ptr(), DROPIMAGE_INVALID, "", "");
	}
	return S_OK; //??
}

ULONG STDMETHODCALLTYPE drop_handler_interface::Release()
{
	LONG rv = InterlockedDecrement(&drop_ref_count);
	if (!rv)
	{
#ifdef _DEBUG
		OutputDebugString(_T("deleting drop_handler_interface"));
#endif
		delete this;
	}
	return rv;
}

ULONG STDMETHODCALLTYPE drop_handler_interface::AddRef()
{
	return InterlockedIncrement(&drop_ref_count);
}

HRESULT STDMETHODCALLTYPE drop_handler_interface::QueryInterface(REFIID riid, LPVOID FAR *ppvObject)
{
	if (ppvObject == NULL) return E_INVALIDARG;
	*ppvObject = NULL;
	if (riid == IID_IUnknown) { AddRef(); *ppvObject = (IUnknown*)this; return S_OK; }
	else if (riid == IID_IDropTarget) { AddRef(); *ppvObject = (IDropTarget*)this; return S_OK; }
	else return E_NOINTERFACE;
}

