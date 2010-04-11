/*
 * Copyright (c) All Rights Reserved. See details at the end of the file.
 */
package com.embedthis.ejs.ide.editors;

import org.eclipse.jface.text.rules.IRule;
import org.eclipse.jface.text.rules.RuleBasedScanner;
import org.eclipse.jface.text.rules.SingleLineRule;
import org.eclipse.jface.text.rules.IToken;
import org.eclipse.jface.text.rules.Token;
import org.eclipse.jface.text.rules.WhitespaceRule;
import org.eclipse.jface.text.rules.WordRule;
import org.eclipse.jface.text.*;

import com.embedthis.ejs.ide.editors.EJScriptKeywordConstants;


public class EJScriptScanner extends RuleBasedScanner {

	public EJScriptScanner(ColorManager manager) {
		
		IToken stringInstr = 
			new Token (
				new TextAttribute(
					manager.getColor(EJScriptColorConstants.STRING)));
		IToken commentInstr = 
			new Token (
				new TextAttribute(
					manager.getColor(EJScriptColorConstants.COMMENT)));
		
		IRule[] rules = new IRule[3];
		//Add rule for processing instructions
		rules[0] = new SingleLineRule("\"", "\"", stringInstr);
		//Add rule for processing single line comments '//'
		rules[1] = new SingleLineRule("//", null, commentInstr);
		// Add generic whitespace rule.
		rules[2] = new WhitespaceRule(new EJScriptWhitespaceDetector());
		// Add keyword rules
		rules = addKeywordRules(rules, manager);
		setRules(rules);
	}
	
	private IRule[] addKeywordRules(IRule[] rules, ColorManager manager) {
		/*
		 * Copy rules array to a new array plus one for the Keyword Detector 
		 */
		IRule[] newRules = new IRule[rules.length + 1];
		IToken keywordInstr = 
			new Token (new TextAttribute(
					manager.getColor(EJScriptColorConstants.KEYWORDS)));
		
		IToken unimplementedKeywordInstr = 
			new Token (new TextAttribute(
					manager.getColor(EJScriptColorConstants.UNIMPLEMENTED_KEYWORDS)));
		
		IToken futureKeywordInstr = 
			new Token (new TextAttribute(
					manager.getColor(EJScriptColorConstants.FUTURE_KEYWORDS)));
		
		// copy incoming rules
		for (int i = 0; i < rules.length; i++) {
			newRules[i] = rules[i];
		}
		int i = rules.length;
		
		WordRule wordRules = new WordRule(new EJScriptKeywordDetector());
		for (int index = 0; index < EJScriptKeywordConstants.RESERVED_WORDS.length; index++) {
			wordRules.addWord(EJScriptKeywordConstants.RESERVED_WORDS[index], keywordInstr);
		}
		for (int index = 0; index < EJScriptKeywordConstants.UNIMPLEMENTED_RESERVED_WORDS.length; index++) {
			wordRules.addWord(EJScriptKeywordConstants.UNIMPLEMENTED_RESERVED_WORDS[index], unimplementedKeywordInstr);
		}
		for (int index = 0; index < EJScriptKeywordConstants.FUTURE_RESERVED_WORDS.length; index++) {
			wordRules.addWord(EJScriptKeywordConstants.FUTURE_RESERVED_WORDS[index], futureKeywordInstr);
		}
		newRules[i++] = wordRules;

		/*
		 * Add Rule for parsing Function names.
		 */
/*		TextAttribute attr2 = new TextAttribute(
				manager.getColor(EJScriptColorConstants.FUNCTIONS));
		IToken functionInstr = 
			new Token (attr2);
		
		EJScriptFunctionRule functionRule = 
			new EJScriptFunctionRule(new EJScriptFunctionDetector(), 
					functionInstr);
		newRules[i++] = functionRule;
 */
  	
 		return newRules;
	}
}

/*
 *	@copy	default
 *	
 *	Copyright (c) Embedthis Software LLC, 2003-2010. All Rights Reserved.
 *	Copyright (c) Michael O'Brien, 1993-2010. All Rights Reserved.
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
 *	@end
 */
