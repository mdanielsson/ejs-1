/*
 * Copyright (c) All Rights Reserved. See details at the end of the file.
 */
package com.embedthis.ejs.ide.core;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.ParserConfigurationException;
import javax.xml.transform.OutputKeys;
import javax.xml.transform.Result;
import javax.xml.transform.Source;
import javax.xml.transform.Transformer;
import javax.xml.transform.TransformerConfigurationException;
import javax.xml.transform.TransformerException;
import javax.xml.transform.TransformerFactory;
import javax.xml.transform.dom.DOMSource;
import javax.xml.transform.stream.StreamResult;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IProgressMonitor;
import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.NamedNodeMap;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;
import org.xml.sax.SAXException;

/**
 * Object representation of the .bc file. Contains the logic for opening and
 * closing the file, as well as parsing the contents.
 * 
 * @author Gavin
 * 
 */
public class BCFile {

	protected static final String ATTRIBUTE_NAME = "name";
	protected static final String COMPILER_OPTIONS_NODE = "compilerOptions";
	protected static final String BUILD_CONFIGURATION_NODE = "buildConfiguration";
	protected static final String BUILD_TYPE_NODE = "buildType";
	protected static final String BUILD_TYPE_FILE_BY_FILE = "enabled";
	protected static final String BUILD_TYPE_NORMAL = "disabled";
	
	protected static final String RESOURCE_LIST_NODE = "resource";
	protected static final String RESOURCE_ITEM_NODE = "resourceItem";

	private Document doc = null;
	private IFile bcFile = null;
	
	// Console output
//	private EJScriptConsoleDisplayMgr console = 
//				EJScriptConsoleDisplayMgr.getDefault();

//	private ArrayList<EJSBuildConfiguration> buildConfigurations = 
//		new ArrayList<EJSBuildConfiguration>();
	
	private EJSBuildConfiguration bc;
	
	// project reference for confirming resource existence.
	private IProject project = null;

	/**
	 * Constructor opens the given file and parses the content.
	 * 
	 * @param file
	 */
	public BCFile(IFile file) {
		if (file.exists()) {
			bcFile = file;
			project = file.getProject();
			doc = getDoc(new File(bcFile.getLocation().toOSString()));
			if (doc != null) {
				parse();
			}
		}
	}

	public String getBCName() {
		return bcFile.getProjectRelativePath().toPortableString();
	}

	public IProject getProject() {
		return project;
	}

	/**
	 * Generate a Document either parsed from a file, or a new empty Document
	 * for populating.
	 * 
	 * @param localFile
	 *            optional File to parse into Document
	 * @return new Document
	 */
	public Document getDoc(File localFile) {
		DocumentBuilderFactory factory = DocumentBuilderFactory.newInstance();

		try {
			DocumentBuilder builder = factory.newDocumentBuilder();
			if (builder == null) {
				EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG,
						EJScriptTrace.COMPILER_TRACE, "builder == null");
				doc = null;
			} else {
				if (localFile != null) {
					doc = builder.parse(localFile);
				} else {
					doc = builder.newDocument();
				}
			}
		} catch (ParserConfigurationException e) {
			EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG,
					EJScriptTrace.COMPILER_TRACE,
					"ParserConfigurationException: " + e.getMessage());
			doc = null;
		} catch (SAXException e) {
			EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG,
					EJScriptTrace.COMPILER_TRACE, "SAXException: "
							+ e.getMessage());
			doc = null;
		} catch (IOException e) {
			EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG,
					EJScriptTrace.COMPILER_TRACE, "IOException: "
							+ e.getMessage());
			doc = null;
		}

		return doc;
	}

	/**
	 * Retrieve the current set of build configurations in this bc file.
	 * 
	 * @return current build configurations in this file
	 */
	public EJSBuildConfiguration getBuildConfig() {
		return bc;
	}

	/**
	 * Add a new configuration to this bc file.
	 * 
	 * @param bc
	 */
//	public void addBuildConfig(EJSBuildConfiguration bc) {
//		buildConfigurations.add(bc);
//	}

	/**
	 * Save document model back to bc file in file system.
	 * 
	 * Changes to this function must be done carefully as the save of this file
	 * causes the builder to fire.  If done improperly, the builder will not fire
	 * and changes will not go into effect.
	 * 
	 * @param monitor
	 */
	public void save(IProgressMonitor monitor) {
		boolean isSuccessful = false;
		updateDoc();
		FileOutputStream outputStream = null;
		if (doc != null) {
			try {
				Source src = new DOMSource(doc);
				outputStream = new FileOutputStream(bcFile.getLocation().toFile());
				Result result = new StreamResult(outputStream);
				Transformer xformer = TransformerFactory.newInstance()
						.newTransformer();
				xformer.setOutputProperty(OutputKeys.INDENT, "yes");
				xformer.setOutputProperty(OutputKeys.METHOD, "xml");
				xformer.transform(src, result);
				isSuccessful = true;
			} catch (TransformerConfigurationException e) {
				EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG,
						EJScriptTrace.COMPILER_TRACE,
						"TransformerConfigurationException: " + e.getMessage());
			} catch (TransformerException e) {
				EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG,
						EJScriptTrace.COMPILER_TRACE,
						"TransformerException: " + e.getMessage());
			} catch (FileNotFoundException e) { 
				EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG,
						EJScriptTrace.COMPILER_TRACE,
						"FileNotFoundException: " + e.getMessage());
			} finally {
				if (outputStream != null) {
					try {
						EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG,
								EJScriptTrace.COMPILER_TRACE,
								"finally closing file");
						outputStream.close();
					} catch (IOException e) {
						
					}
				}
			}
			try {
				bcFile.refreshLocal(IResource.DEPTH_ZERO, null);
			} catch (CoreException e) {
				EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG,
						EJScriptTrace.COMPILER_TRACE,
						"CoreException: " + e.getMessage());
			}
			if (isSuccessful) {
				EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG,
					EJScriptTrace.COMPILER_TRACE,
					"successfully saved file");
			} else {
				EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG,
					EJScriptTrace.COMPILER_TRACE, "save file failed");
			}
		}
		if (monitor != null) {
			monitor.worked(1);
		}
		
	}

	/**
	 * Generate BuildConfig object(s) from file.
	 */
	private void parse() {
		EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG,
				EJScriptTrace.COMPILER_TRACE, "+BCFile:parse");
		NodeList buildConfigs = doc
				.getElementsByTagName(BUILD_CONFIGURATION_NODE);

		if (buildConfigs != null) {
			if (buildConfigs.getLength() == 0) {
				EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG,
						EJScriptTrace.COMPILER_TRACE, bcFile.getName()
								+ " is an empty file");
			} else if (buildConfigs.getLength() > 1) {
				EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG,
						EJScriptTrace.COMPILER_TRACE, bcFile.getName()
								+ " contains more than one BC");
			} else {

				Node node = buildConfigs.item(0);
				if (node == null) {
					EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG,
							EJScriptTrace.COMPILER_TRACE, "Error parsing BC file: node == null");
					return;
				}
				NamedNodeMap attributes = node.getAttributes();
				if (attributes == null) {
					EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG,
							EJScriptTrace.COMPILER_TRACE, "Error parsing BC file: attributes == null");
					return;
				}
				Node attrNode = attributes.getNamedItem(ATTRIBUTE_NAME);
				if (attrNode == null) {
					EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG,
							EJScriptTrace.COMPILER_TRACE, "Error parsing BC file: attrNode == null");
					return;
				}
				String name = attrNode.getNodeValue();
				if (name == null) {
					EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG,
							EJScriptTrace.COMPILER_TRACE, "Error parsing BC file: name == null");
					return;
				}
//			EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG,
//					EJScriptTrace.COMPILER_TRACE, "FOUND: " + name);
				convertNodeToBC(name, node);
			}
		}
		EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG,
				EJScriptTrace.COMPILER_TRACE, "-BCFile:parse");
	}

	/**
	 * Create a build configuration from the file and add it to the local Map
	 * 
	 * @param name
	 * @param buildConfigNode
	 */
	private void convertNodeToBC(String name, Node buildConfigNode) {
		EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG,
				EJScriptTrace.COMPILER_TRACE, "+addBuildConfigToMap: " 
				+ "BCName is " + name + " bcFileName is " + this.getBCName());
		bc = new EJSBuildConfiguration(name,
				getBCName());
		NodeList memberNodes = buildConfigNode.getChildNodes();

		for (int i = 0; i < memberNodes.getLength(); i++) {
			Node node = memberNodes.item(i);
			if (node == null) {
				continue;
			}
			if (node.getNodeName() == RESOURCE_LIST_NODE) {
				EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG,
						EJScriptTrace.COMPILER_TRACE, "Adding resource: "
								+ node.getTextContent());
				if (node.getTextContent().equals("ALL")) {
// TODO GMK!!!					buildConfig.includeAllResources(true);
				} else {
					bc.addResource(node.getTextContent());
				}
			}
			if (node.getNodeName() == COMPILER_OPTIONS_NODE) {
//				EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG,
//						EJScriptTrace.COMPILER_TRACE, "compilerOptions: "
//								+ node.getTextContent());
				bc.setCommandArgs(node.getTextContent());
			}
			if (node.getNodeName() == BUILD_TYPE_NODE) {
				if (node.getTextContent().equals(BUILD_TYPE_FILE_BY_FILE)) {
					bc.setBuildType(true);
				} else {
					bc.setBuildType(false);
				}
			}
		}
		EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG,
				EJScriptTrace.COMPILER_TRACE, "-addBuildConfigToMap");
	}

	/**
	 * Update the internal document model before writing to the system file.
	 */
	private void updateDoc() {
		EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG,
				EJScriptTrace.COMPILER_TRACE, "+updateDoc");
		Document newDoc = getDoc(null);

		Element e = newDoc.createElement("bc");

		bc.addToDoc(newDoc, e);
		EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG,
				EJScriptTrace.COMPILER_TRACE, "+addded build config");
		doc.appendChild(e);
		doc = newDoc;

		EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG,
				EJScriptTrace.COMPILER_TRACE, "-addToDoc");
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
