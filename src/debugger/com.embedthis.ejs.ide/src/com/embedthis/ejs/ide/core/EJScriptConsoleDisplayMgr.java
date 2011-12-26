/*
 * Copyright (c) All Rights Reserved. See details at the end of the file.
 */
package com.embedthis.ejs.ide.core;



import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.text.IDocument;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.widgets.Display;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.console.ConsolePlugin;
import org.eclipse.ui.console.IConsole;
import org.eclipse.ui.console.IConsoleConstants;
import org.eclipse.ui.console.MessageConsole;
import org.eclipse.ui.console.MessageConsoleStream;

import com.embedthis.ejs.ide.EJScriptPlugin;

/**
 * @author Gavin
 *
 */
public class EJScriptConsoleDisplayMgr {

	private static EJScriptConsoleDisplayMgr fDefault = null;
	private String fTitle = null;
	private MessageConsole fMessageConsole = null;
	
	public static final int MSG_INFORMATION = 1;
	public static final int MSG_ERROR = 2;
	public static final int MSG_WARNING = 3;
	
	public EJScriptConsoleDisplayMgr(String messageTitle)
	{
		EJScriptTrace.trace(EJScriptTrace.TRACE_VERBOSE,
				EJScriptTrace.COMPILER_TRACE,
				"+EJScriptConsoleDisplayMgr");
		fDefault = this;
		fTitle = messageTitle;
	}
	
	public static EJScriptConsoleDisplayMgr getDefault() {
		EJScriptTrace.trace(EJScriptTrace.TRACE_VERBOSE,
				EJScriptTrace.COMPILER_TRACE,
				"+EJScriptConsoleDisplayMgr getDefault");
		if (fDefault == null) {
			fDefault = new EJScriptConsoleDisplayMgr("EJScript");
		}
		return fDefault;
	}
	
	public void println(String msg)
	{
		EJScriptTrace.trace(EJScriptTrace.TRACE_VERBOSE,
				EJScriptTrace.COMPILER_TRACE,
				"+EJScriptConsoleDisplayMgr println");
		
		println(msg, MSG_INFORMATION);
		fMessageConsole.activate();
		EJScriptTrace.trace(EJScriptTrace.TRACE_VERBOSE,
				EJScriptTrace.COMPILER_TRACE,
				"-EJScriptConsoleDisplayMgr println");
	}
	
	public void println(String msg, int msgKind)
	{
		EJScriptTrace.trace(EJScriptTrace.TRACE_VERBOSE,
				EJScriptTrace.COMPILER_TRACE,
				"+EJScriptConsoleDisplayMgr println(String, int)");
		if (msg == null) 
		{
			return;
		}
		
		if (!displayConsoleView())
		{
			MessageDialog.openError(PlatformUI.getWorkbench().getActiveWorkbenchWindow().getShell(), "Error", msg);
			return;
		}
		getNewMessageConsoleStream(msgKind).println(msg);
	}
	
	public void clear()
	{
		EJScriptTrace.trace(EJScriptTrace.TRACE_VERBOSE,
				EJScriptTrace.COMPILER_TRACE,
				"+EJScriptConsoleDisplayMgr clear");
		IDocument document = getMessageConsole().getDocument();
		if (document != null)
		{
			document.set("");
		}
	}
	
	private boolean displayConsoleView()
	{
		EJScriptTrace.trace(EJScriptTrace.TRACE_VERBOSE,
				EJScriptTrace.COMPILER_TRACE,
				"+EJScriptConsoleDisplayMgr displayConsoleView");
		try
		{
			IWorkbenchWindow activeWorkbenchWindow = PlatformUI.getWorkbench().getActiveWorkbenchWindow();
			if (activeWorkbenchWindow != null)
			{
				IWorkbenchPage activePage = activeWorkbenchWindow.getActivePage();
				if (activePage != null)
				{
					activePage.showView(IConsoleConstants.ID_CONSOLE_VIEW, null, IWorkbenchPage.VIEW_VISIBLE);
				}
			}
		} catch (PartInitException partEx)
		{
			return false;
		}
		return true;
	}
	
	/*
	 * TODO GMK - future enhancement - add color to specific types of trace messages -
	 * the problem was that I could not get a handle to a display to retrieve the color.
	 */  
	
	private MessageConsoleStream getNewMessageConsoleStream(int msgKind)
	{
		EJScriptTrace.trace(EJScriptTrace.TRACE_VERBOSE,
				EJScriptTrace.COMPILER_TRACE,
				"+EJScriptConsoleDisplayMgr getNewMessageConsoleStream");
		int swtColorId = SWT.COLOR_BLACK;
		
		switch (msgKind)
		{
			case MSG_INFORMATION:
				swtColorId = SWT.COLOR_DARK_GREEN;
				break;
			case MSG_ERROR:
				swtColorId = SWT.COLOR_DARK_MAGENTA;
				break;
			case MSG_WARNING:
				swtColorId = SWT.COLOR_DARK_BLUE;
				break;
			default:
		}

		MessageConsoleStream msgConsoleStream = getMessageConsole().newMessageStream();
/*		Color newColor = null;
		Display display = EJScriptPlugin.getDefault().getWorkbench().getDisplay();
		try {
			 newColor = display.getSystemColor(swtColorId);
		} catch (Exception e) {
			System.out.println(e.getMessage());
		}
		msgConsoleStream.setColor(newColor);
 */
		return msgConsoleStream;
	}
	
	private MessageConsole getMessageConsole()
	{
		EJScriptTrace.trace(EJScriptTrace.TRACE_VERBOSE,
				EJScriptTrace.COMPILER_TRACE,
				"+EJScriptConsoleDisplayMgr getMessageConsole");
		if (fMessageConsole == null) 
		{
			createMessageConsoleStream(fTitle);
		}
		
		return fMessageConsole;
	}
	
	private void createMessageConsoleStream(String title)
	{
		EJScriptTrace.trace(EJScriptTrace.TRACE_VERBOSE,
				EJScriptTrace.COMPILER_TRACE,
				"+EJScriptConsoleDisplayMgr createMessageConsoleStream");
		fMessageConsole = new MessageConsole(title, null);
		ConsolePlugin.getDefault().getConsoleManager().addConsoles(new IConsole[]{ fMessageConsole});
	}
}

/*
 *	@copy	default
 *	
 *	Copyright (c) Embedthis Software LLC, 2003-2012. All Rights Reserved.
 *	Copyright (c) Michael O'Brien, 1993-2012. All Rights Reserved.
 *	
 *	This software is distributed under commercial and open source licenses.
 *	You may use the GPL open source license described below or you may acquire 
 *	a commercial license from Embedthis Software. You agree to be fully bound 
 *	by the terms of either license. Consult the LICENSE.TXT distributed with 
 *	this software for full details.
 *	
 *	This software is open source; you can redistribute it and/or modify it 
 *	under the terms of the GNU General Public License as published by the 
 *	Free Software Foundation; either version 2 of the License, or (at your 
 *	option) any later version. See the GNU General Public License for more 
 *	details at: http://www.embedthis.com/downloads/gplLicense.html
 *	
 *	This program is distributed WITHOUT ANY WARRANTY; without even the 
 *	implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
 *	
 *	This GPL license does NOT permit incorporating this software into 
 *	proprietary programs. If you are unable to comply with the GPL, you must
 *	acquire a commercial license to use this software. Commercial licenses 
 *	for this software and support services are available from Embedthis 
 *	Software at http://www.embedthis.com 
 *	
 *	Local variables:
 *	tab-width: 4
 *	c-basic-offset: 4
 *	End:
 *	vim: sw=4 ts=4 
 *
 *	Local variables:
    tab-width: 4
    c-basic-offset: 4
    End:
    vim: sw=4 ts=4 expandtab

    @end
 */
