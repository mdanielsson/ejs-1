/**
 * Copyright (c) All Rights Reserved. See details at the end of the file.
 */
package com.embedthis.ejs.ide;

import org.eclipse.core.resources.ISaveContext;
import org.eclipse.core.resources.ISaveParticipant;
import org.eclipse.core.runtime.CoreException;

public class EJScriptSaveParticipant implements ISaveParticipant {

	/**
	 * Called when the save state process is complete (clean up).
	 * 
	 * delete the old saved state that is no longer necessary.
	 */
	public void doneSaving(ISaveContext context) {
		// TODO Auto-generated method stub
		
	}

	/**
	 * Called as a prelude to the task of saving state information.
	 */
	public void prepareToSave(ISaveContext context) throws CoreException {
		// TODO Auto-generated method stub

	}

	/**
	 * Called when a save has to be undone.
	 * 
	 * delete the saved state that was just written
	 */
	public void rollback(ISaveContext context) {
		// TODO Auto-generated method stub

	}

	/**
	 * Called when it is time to save state information.
	 */
	public void saving(ISaveContext context) throws CoreException {
		// TODO Auto-generated method stub
		
		context.needDelta();
		
		switch (context.getKind()) {
		case ISaveContext.FULL_SAVE:
			break;
		case ISaveContext.PROJECT_SAVE:
			break;
		case ISaveContext.SNAPSHOT:
			break;
		}
	}

}
/*
 *	@copy	default
 *	
 *	Copyright (c) Embedthis Software LLC, 2003-2011. All Rights Reserved.
 *	Copyright (c) Michael O'Brien, 1993-2011. All Rights Reserved.
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
    vim: sw=8 ts=8 expandtab

    @end
 */
