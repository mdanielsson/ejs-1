/*
 * Copyright (c) All Rights Reserved. See details at the end of the file.
 */
package com.embedthis.ejs.ide.dialogs;

import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.jface.dialogs.Dialog;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.events.SelectionListener;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Combo;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.FileDialog;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.swt.widgets.Text;
import org.eclipse.ui.forms.widgets.Form;
import org.eclipse.ui.forms.widgets.FormToolkit;

import com.embedthis.ejs.ide.core.EJScriptCompilerOptions;
import com.embedthis.ejs.ide.core.EJScriptTrace;
import com.embedthis.ejs.ide.properties.PropertyConstants;

public class CompilerOptionsDialog extends Dialog {
	
	private EJScriptCompilerOptions compilerOption;
	
	private static final String DEBUG_TEXT = "Enable Debug";
	private static final String OPTIMIZE_TEXT = "Optimization Level";
	private static final String OUT_TEXT = "Output File";
	private static final String SEARCH_PATH_TEXT = "Seach Path";
	private static final String STANDARD_TEXT = "Standard";
	private static final String STRICT_TEXT = "Strict";
	private static final String VERBOSE_TEXT = "Enable Verbose Output";
	private static final String WARN_TEXT = "Warning level";
	
	
	private Button debugCheckBox;
	private Button optimizeCheckBox;
	private Combo combo;
	private Button outCheckBox;
	private Text outFile;
	private Button outBrowseButton;
	private FileDialog outDialog;
	private Button searchPathCheckBox;
	private Text ejsPath;
	private Button standardCheckBox;
	private Button strictCheckBox;
	private Button verboseCheckBox;
	private Button warnCheckBox;
	private Combo warnCombo;
	private Button extraOptionCheckBox;
	private Text extraOption;

	public CompilerOptionsDialog(Shell parent, EJScriptCompilerOptions co) {
		super(parent);
		compilerOption = co;
	}
	
	public EJScriptCompilerOptions getCompilerOptions() {
		return compilerOption;
	}
	
	protected void okPressed() {
		EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG,
				EJScriptTrace.COMPILER_TRACE,
				"+okPressed");
		compilerOption.enableDebug(debugCheckBox.getSelection());
		compilerOption.enableOptimize(optimizeCheckBox.getSelection(), 
				combo.getText());
		compilerOption.enableOut(outCheckBox.getSelection(), outFile.getText());
		compilerOption.enableSearchPath(searchPathCheckBox.getSelection(),
				ejsPath.getText());
		compilerOption.enableStandard(standardCheckBox.getSelection());
		compilerOption.enableStrict(strictCheckBox.getSelection());
		compilerOption.enableVerbose(verboseCheckBox.getSelection());
		compilerOption.enableWarn(warnCheckBox.getSelection(), warnCombo.getText());
		compilerOption.enableExtraOption(extraOptionCheckBox.getSelection(), extraOption.getText());
		EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG,
				EJScriptTrace.COMPILER_TRACE,
				"-okPressed");
		super.okPressed();
	}
	
	protected Control createDialogArea(Composite parent) {
		FormToolkit toolkit = new FormToolkit(parent.getDisplay());
		Form form = toolkit.createForm(parent);
		form.setText("Compiler Options");
		toolkit.decorateFormHeading(form);
		
		final Shell shell = parent.getShell();
		
		GridLayout layout = new GridLayout();
		layout.numColumns = 3;
		form.getBody().setLayout(layout);
//		Composite composite = new Composite(form.getBody(), SWT.NONE);
		Composite composite = form.getBody();
		String optimizeOptions[] = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9"};
		String warnOptions[] = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9"};
	
		//Label for path field
//		Label pathLabel = new Label(composite, SWT.BORDER | SWT.WRAP);
//		pathLabel.setText("Use the fields below to construct the compiler options.");
		
		Color white = new Color(null, 255, 255, 255);
		
		/*
		 * --debug
		 */
		debugCheckBox = new Button(composite, SWT.CHECK);
		debugCheckBox.setText(DEBUG_TEXT);
		debugCheckBox.setToolTipText(PropertyConstants.DEBUG_DESCRIPTION);
		debugCheckBox.addSelectionListener(new PropertiesCheckBoxListener());
		debugCheckBox.setSelection(compilerOption.isDebugEnabled());
		GridData tripleWideGD = new GridData();
		tripleWideGD.horizontalSpan = 3;
		debugCheckBox.setLayoutData(tripleWideGD);
		debugCheckBox.setBackground(white);
		
		/*
		 *  --optimize
		 */
		optimizeCheckBox = new Button(composite, SWT.CHECK);
		optimizeCheckBox.setText(OPTIMIZE_TEXT); 
		optimizeCheckBox.setToolTipText(PropertyConstants.OPTIMIZE_DESCRIPTION);
		optimizeCheckBox.addSelectionListener(new PropertiesCheckBoxListener());
		optimizeCheckBox.setSelection(compilerOption.isOptimizeEnabled());
		GridData singleWideGD = new GridData();
		singleWideGD.horizontalSpan = 1;
		optimizeCheckBox.setLayoutData(singleWideGD);
		optimizeCheckBox.setBackground(white);
		
		/*
		 * --optimize combo box
		 */
		combo = new Combo(composite, SWT.DROP_DOWN);
		combo.setItems(optimizeOptions);
		combo.setVisibleItemCount(5);
		combo.addSelectionListener(new PropertiesComboListener(optimizeCheckBox));
		if (compilerOption.isOptimizeEnabled() == true) {
			combo.setText(compilerOption.getOptimizeValue());
		}
		GridData doubleWideGD = new GridData();
		doubleWideGD.horizontalSpan = 2;
		combo.setLayoutData(doubleWideGD);
		
		/*
		 * --out
		 */
		outCheckBox = new Button(composite, SWT.CHECK); 
		outCheckBox.setText(OUT_TEXT);
		outCheckBox.setToolTipText(PropertyConstants.OUT_DESCRIPTION);
		outCheckBox.addSelectionListener(new PropertiesCheckBoxListener());
		outCheckBox.setSelection(compilerOption.isOutEnabled());
		singleWideGD = new GridData();
		singleWideGD.horizontalSpan = 1;
		outCheckBox.setLayoutData(singleWideGD);
		outCheckBox.setBackground(white);

		/*
		 * --out outFile
		 */
		outFile = new Text(composite, SWT.SINGLE | SWT.BORDER);
		outFile.setEditable(true);
//		outFile.setSize(512, 12);
		if (compilerOption.isOutEnabled() == true) {
			outFile.setText(compilerOption.getOutValue());
		}
		singleWideGD = new GridData();
		singleWideGD.horizontalSpan = 1;
		outFile.setLayoutData(singleWideGD);
		
		/*
		 * --out browse
		 */
		outBrowseButton = new Button(composite, SWT.NONE); 
		outBrowseButton.setText("Browse...");
		outBrowseButton.addSelectionListener(new PropertiesCheckBoxListener());
		outBrowseButton.setSelection(compilerOption.isOutEnabled());
		singleWideGD = new GridData();
		singleWideGD.horizontalSpan = 1;
		outBrowseButton.setLayoutData(singleWideGD);
		outBrowseButton.setBackground(white);
		outBrowseButton.addSelectionListener(new SelectionListener() {
			public void widgetSelected(SelectionEvent event) {
				outDialog = new FileDialog(shell);
				outDialog.setText("BuildConfiguration Out file location");
				outDialog.setFilterPath(ResourcesPlugin.getWorkspace().getRoot().toString());
				String userFile = outDialog.open();
				if (outFile != null) {
					outFile.setText(userFile);
					outCheckBox.setSelection(true);
				}
			}
			
			public void widgetDefaultSelected(SelectionEvent event) {
				widgetSelected(event);
			}
		});
				
		/*
		 *  --searchpath
		 */
		searchPathCheckBox = new Button(composite, SWT.CHECK); 
		searchPathCheckBox.setText(SEARCH_PATH_TEXT + "(use '" + System.getProperty("path.separator") + 
				"' as the path separator)");
		searchPathCheckBox.setToolTipText(PropertyConstants.SEARCH_PATH_DESCRIPTION);
		searchPathCheckBox.addSelectionListener(new PropertiesCheckBoxListener());
		searchPathCheckBox.setSelection(compilerOption.isSearchPathEnabled());
		singleWideGD = new GridData();
		singleWideGD.horizontalSpan = 1;
		searchPathCheckBox.setLayoutData(singleWideGD);
		searchPathCheckBox.setBackground(white);
		
		/*
		 * --searchpath ejsPath
		 * TODO GMK search path option could be added similar to email attachments,
		 * Break search path into each item, edit/remove for each and add new
		 * at the end.
		 */
		ejsPath = new Text(composite, SWT.SINGLE | SWT.BORDER);
		ejsPath.setEditable(true);
//		ejsPath.setSize(256, 12);
		if (compilerOption.isSearchPathEnabled() == true) {
			ejsPath.setText(compilerOption.getSearchPathValue());
		}
		singleWideGD = new GridData();
		singleWideGD.horizontalSpan = 1;
		ejsPath.setLayoutData(singleWideGD);
		
		// TODO GMK add group to make these radio buttons look better.

		/*
		 * --standard
		 */
		standardCheckBox = new Button(composite, SWT.RADIO);
		standardCheckBox.setText(STANDARD_TEXT);
		standardCheckBox.setToolTipText(PropertyConstants.STANDARD_DESCRIPTION);
		standardCheckBox.addSelectionListener(new PropertiesCheckBoxListener());
		standardCheckBox.setSelection(compilerOption.isStandardEnabled());
		tripleWideGD = new GridData();
		tripleWideGD.horizontalSpan = 3;
		standardCheckBox.setLayoutData(tripleWideGD);
		standardCheckBox.setBackground(white);
		
		/*
		 * --strict
		 */
		strictCheckBox = new Button(composite, SWT.RADIO);
		strictCheckBox.setText(STRICT_TEXT);
		strictCheckBox.setToolTipText(PropertyConstants.STRICT_DESCRIPTION);
		strictCheckBox.addSelectionListener(new PropertiesCheckBoxListener());
		strictCheckBox.setSelection(compilerOption.isStrictEnabled());
		tripleWideGD = new GridData();
		tripleWideGD.horizontalSpan = 3;
		strictCheckBox.setLayoutData(tripleWideGD);
		strictCheckBox.setBackground(white);
		
		/*
		 * --verbose
		 */
		verboseCheckBox = new Button(composite, SWT.CHECK);
		verboseCheckBox.setText(VERBOSE_TEXT);
		verboseCheckBox.setToolTipText(PropertyConstants.VERBOSE_DESCRIPTION);
		verboseCheckBox.addSelectionListener(new PropertiesCheckBoxListener());
		verboseCheckBox.setSelection(compilerOption.isVerboseEnabled());
		tripleWideGD = new GridData();
		tripleWideGD.horizontalSpan = 3;
		verboseCheckBox.setLayoutData(tripleWideGD);
		verboseCheckBox.setBackground(white);
		
		/*
		 *  --warn
		 */
		warnCheckBox = new Button(composite, SWT.CHECK);
		warnCheckBox.setText(WARN_TEXT); 
		warnCheckBox.setToolTipText(PropertyConstants.WARN_DESCRIPTION);
		warnCheckBox.addSelectionListener(new PropertiesCheckBoxListener());
		warnCheckBox.setSelection(compilerOption.isOptimizeEnabled());
		singleWideGD = new GridData();
		singleWideGD.horizontalSpan = 1;
		warnCheckBox.setLayoutData(singleWideGD);
		warnCheckBox.setBackground(white);
		
		/*
		 * --warn combo box
		 */
		warnCombo = new Combo(composite, SWT.DROP_DOWN);
		warnCombo.setItems(warnOptions);
		warnCombo.setVisibleItemCount(5);
		warnCombo.addSelectionListener(new PropertiesComboListener(warnCheckBox));
		if (compilerOption.isWarnEnabled() == true) {
			warnCombo.setText(compilerOption.getWarnValue());
		}
		doubleWideGD = new GridData();
		doubleWideGD.horizontalSpan = 2;
		warnCombo.setLayoutData(doubleWideGD);
		
		Label label = new Label(composite, SWT.NONE);
		label.setText("An optional space delineated list of extra command arguments.\n" +
				"Each item separated by a space is passed in as its own argument.");
		
		tripleWideGD = new GridData();
		tripleWideGD.horizontalSpan = 3;
		label.setLayoutData(tripleWideGD);
		
		/*
		 *  Extra Options
		 */
		extraOptionCheckBox = new Button(composite, SWT.CHECK); 
		extraOptionCheckBox.setText("Extra Options");
		extraOptionCheckBox.setToolTipText(PropertyConstants.EXTRA_OPTION_DESCRIPTION);
		extraOptionCheckBox.addSelectionListener(new PropertiesCheckBoxListener());
		extraOptionCheckBox.setSelection(compilerOption.isExtraOptionEnabled());
		singleWideGD = new GridData();
		singleWideGD.horizontalSpan = 1;
		extraOptionCheckBox.setLayoutData(singleWideGD);
		extraOptionCheckBox.setBackground(white);
		
		/*
		 * This option will be appended to the compile options verbatim... no validation done.
		 */
		extraOption = new Text(composite, SWT.SINGLE | SWT.BORDER | SWT.FILL);
		extraOption.setEditable(true);
		if (compilerOption.isExtraOptionEnabled() == true) {
			extraOption.setText(compilerOption.getExtraOption());
		}
		doubleWideGD = new GridData(SWT.FILL);
		doubleWideGD.horizontalSpan = 2;
		
		extraOption.setLayoutData(doubleWideGD);
		
		composite.setLayoutData(new GridData(GridData.FILL_HORIZONTAL));
		
		return composite;
	}

	private class PropertiesCheckBoxListener implements SelectionListener {

		PropertiesCheckBoxListener() {
		}
		
		public void widgetSelected(SelectionEvent e) {
		}
		
		public void widgetDefaultSelected(SelectionEvent e) {
		}
	}
	
	private class PropertiesComboListener implements SelectionListener {
		Button comboButton;
		PropertiesComboListener(Button button) {
			comboButton = button;
		}

		public void widgetSelected(SelectionEvent e) {
			comboButton.setSelection(true);
		}

		public void widgetDefaultSelected(SelectionEvent e) {
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
