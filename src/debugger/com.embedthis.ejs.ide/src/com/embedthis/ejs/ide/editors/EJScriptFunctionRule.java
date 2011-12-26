/*
 * Copyright (c) All Rights Reserved. See details at the end of the file.
 */
package com.embedthis.ejs.ide.editors;

import org.eclipse.core.runtime.Assert;
import org.eclipse.jface.text.rules.ICharacterScanner;
import org.eclipse.jface.text.rules.IRule;
import org.eclipse.jface.text.rules.IToken;
import org.eclipse.jface.text.rules.IWordDetector;
import org.eclipse.jface.text.rules.Token;

/**
 * @author Gavin
 *
 */
public class EJScriptFunctionRule implements IRule {

	/** The word detector used by this rule */
	protected IWordDetector fDetector;
	/** The default token to be returned on success and if nothing else has been specified. */
	protected IToken fDefaultToken;
	
	/**
	 * Creates a rule which, with the help of a word detector, will return the token
	 * associated with the detected word. If no token has been associated, the
	 * specified default token will be returned.
	 *
	 * @param detector the word detector to be used by this rule, may not be <code>null</code>
	 * @param defaultToken the default token to be returned on success
	 *		if nothing else is specified, may not be <code>null</code>
	 *
	 */
	public EJScriptFunctionRule(IWordDetector detector, IToken defaultToken) {

		Assert.isNotNull(detector);
		Assert.isNotNull(defaultToken);

		fDetector= detector;
		fDefaultToken= defaultToken;
	}
	
	/* (non-Javadoc)
	 * @see org.eclipse.jface.text.rules.IRule#evaluate(org.eclipse.jface.text.rules.ICharacterScanner)
	 */
	public IToken evaluate(ICharacterScanner scanner) {
		int count = 0;
		int c= scanner.read();
		StringBuffer fBuffer = new StringBuffer();
		
		if (fDetector.isWordStart((char) c)) {

			do {
				fBuffer.append((char)c);
				c= scanner.read();
				count++;
			} while (c != ICharacterScanner.EOF && fDetector.isWordPart((char) c));
/*			EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG,
				EJScriptTrace.GENERAL_TRACE,
				"out of do/while c is " + (char)c);
 */
			while ((char)c == ' ') {
				c = scanner.read();
			}
			if ((char)c != '(') {
				scanner.unread();
				return Token.UNDEFINED;
			}
			// Atempting to unread scanner and only read back function name
			scanner.unread();
/*			EJScriptTrace.trace(EJScriptTrace.TRACE_VERBOSE,
				EJScriptTrace.GENERAL_TRACE,
				"count is " + count + " fBuffer is " + fBuffer.toString());
 */
			for (int i = 0; i < count; i++) {
				scanner.read();
			}
			return fDefaultToken;
		}

		scanner.unread();
		return Token.UNDEFINED;
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
