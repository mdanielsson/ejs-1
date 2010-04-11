/**
 * 
 */
package com.embedthis.ejs.ide.wizards;

import org.eclipse.jface.preference.IPreferenceStore;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.wizard.WizardPage;
import org.eclipse.swt.events.SelectionListener;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.layout.RowLayout;
import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.FileDialog;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Text;

import com.embedthis.ejs.ide.EJScriptPlugin;
import com.embedthis.ejs.ide.preferences.PreferenceConstants;


/**
 * @author Gavin
 *
 */
public class EJScriptProjectPreferencesWizardPage extends WizardPage {

	Text text;
	Text fileslist;
	
	public EJScriptProjectPreferencesWizardPage(String pageName) {
		super(pageName);
	}
	
	/* (non-Javadoc)
	 * @see org.eclipse.jface.dialogs.IDialogPage#createControl(org.eclipse.swt.widgets.Composite)
	 */
	public void createControl(Composite parent) {
		
		/* TODO GMK Instead of setting path to compiler, need to
		 * allow user to add to an existing build configuration
		 * or create a new one for this file.
		 */
		IPreferenceStore store = EJScriptPlugin.getDefault().getPreferenceStore();;
		String compiler = store.getString(PreferenceConstants.PATH_TO_EJS_COMPILER);
		
		Composite composite = new Composite(parent, SWT.NONE);
		RowLayout rowLayout = new RowLayout();
		rowLayout.justify = true;
		rowLayout.marginLeft = 5;
		rowLayout.marginRight = 5;
		rowLayout.spacing = 5;
		composite.setLayout(rowLayout);
		
		Label label = new Label(composite, SWT.NONE);
		label.setText("Select the EJS compiler");
		text = new Text(composite, SWT.SINGLE);
		text.setSize(100, 12);
		text.setText(compiler);
		Button button = new Button(composite, SWT.PUSH);
		button.setText("Browse");
		button.addSelectionListener(new SelectionListener() {
			public void widgetSelected(SelectionEvent e) {
				handleBrowse(PreferenceConstants.PATH_TO_EJS_COMPILER);
			}

			public void widgetDefaultSelected(SelectionEvent e) {
				handleBrowse(PreferenceConstants.PATH_TO_EJS_COMPILER);
			}
		});
		
		Label fileslistLabel = new Label(composite, SWT.NONE);
		fileslistLabel.setText("List of Files");
		fileslist = new Text(composite, SWT.SINGLE);
		text.setSize(100, 12);
		text.setText(compiler);
		Button filesListBrowse = new Button(composite, SWT.PUSH);
		filesListBrowse.setText("Browse");
		filesListBrowse.addSelectionListener(new SelectionListener() {
			public void widgetSelected(SelectionEvent e) {
				handleBrowse(PreferenceConstants.PATH_TO_EJS_COMPILER);
			}

			public void widgetDefaultSelected(SelectionEvent e) {
				handleBrowse(PreferenceConstants.PATH_TO_EJS_COMPILER);
			}
		});
		
		
		setControl(composite);
		return;
	}
	
	/**
	 * Handles the behavior when the Browse button is selected.
	 *
	 */
	private void handleBrowse(String key)
	{
		String compiler = null;
		try {
			FileDialog compilerLocation = new FileDialog(this.getShell());
			compiler = compilerLocation.open();
		} catch (IllegalArgumentException iaE) {
			
		}
		finally {
			if (compiler != null) {
				IPreferenceStore store = EJScriptPlugin.getDefault().getPreferenceStore();
				store.setValue(key, compiler);
				if (text != null) {
					text.setText(compiler);
				}
			}
		}
	}

}
