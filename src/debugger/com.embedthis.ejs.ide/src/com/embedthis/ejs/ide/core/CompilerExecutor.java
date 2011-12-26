/*
 * Copyright (c) All Rights Reserved. See details at the end of the file.
 */
package com.embedthis.ejs.ide.core;

// Java Imports
import java.io.IOException;
import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.io.File;
import java.util.ArrayList;
import java.util.List;
import java.util.StringTokenizer;

// Eclipse Imports
import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IResource;

// EJScript Plug-in Imports
import com.embedthis.ejs.ide.core.EJScriptConsoleDisplayMgr;
import com.embedthis.ejs.ide.core.EJScriptTrace;

/**
 * CompilerExecutor implements an instance of the EJScript compiler.
 * Execute the compiler and capture the output.
 * @author Gavin Kavanaugh
 *
 */
public class CompilerExecutor extends Thread implements Runnable {

	private File exe;
	private Runtime runtime;
	private String[] args;
	private String compilerOptsString;
	private IResource[] files;

	/**
	 * Used to output information to the EJScript console.
	 */
	private EJScriptConsoleDisplayMgr console = 
		EJScriptConsoleDisplayMgr.getDefault();

	/**
	 * Initialize the CompilerExecutor with the path to the compiler, the list of files
	 * and the compilerOptions.
	 * @param pathToExe - String path to the compiler
	 * @param files - list of files to be compiled together
	 * @param options - EJSCriptCompilerOptions for this build
	 */
	public CompilerExecutor(String pathToExe, IResource[] files, String options) {
		exe = new File(pathToExe);
		this.compilerOptsString = options;
		runtime = Runtime.getRuntime();
		this.files = files;
		EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG,
				EJScriptTrace.COMPILER_TRACE,
				"files length is " + files.length);
		for (int i = 0; i < files.length; i++) {
			EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG,
					EJScriptTrace.COMPILER_TRACE,
					"file[" + i + "] is " + files[i]);
		}
	}
	
	/**
	 * Entry method for runnable instance.
	 */
	public void run() {
		
	}

	
	/**
	 * Execute the compile - capture results and convert to CompilerOutput objects.
	 * @return - a List of CompilerOutput objects representing the output of the compiler.
	 */
	public List<CompilerOutput> compile() {
		EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG,
				EJScriptTrace.COMPILER_TRACE,
				"+compile");
		Process process = null;
		BufferedReader in = null;
		BufferedReader error =  null;
		String line;
		List<String> outputList = new ArrayList<String>();
		List<CompilerOutput> list = null;
		EJScriptTrace.trace(EJScriptTrace.TRACE_VERBOSE,
				EJScriptTrace.COMPILER_TRACE, 
				"compile: path to es.exe is " + exe.getAbsolutePath());
		try {
			if (exe.exists() && files != null && files.length > 0) {
				args = constructArguments();
				traceCompilerArgs(args);

				File dir = new File(files[0].getRawLocation().removeLastSegments(1).toOSString());
				EJScriptTrace.trace(EJScriptTrace.TRACE_VERBOSE,
						EJScriptTrace.COMPILER_TRACE, 
						"compile: dir is " + dir);
				process = runtime.exec(args, null, dir);
				EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG, EJScriptTrace.COMPILER_TRACE,
						"process started");
				in = new BufferedReader(new InputStreamReader(process.getInputStream()));
				error = new BufferedReader(new InputStreamReader(process.getErrorStream()));
				list = new ArrayList<CompilerOutput>();
				EJScriptTrace.trace(EJScriptTrace.TRACE_VERBOSE,
						EJScriptTrace.COMPILER_TRACE,
						"\n\nBegin Compiler output:\n");
/*
 *  			Assuming compiler returns on stdout
 */
				while ((line = in.readLine()) != null) {
					outputList.add(line);
				}
  
				/*
				 * Assuming compiler output on stderr
				 */
				while ((line = error.readLine()) != null) {
					outputList.add(line);
				}
				EJScriptTrace.trace(EJScriptTrace.TRACE_VERBOSE,
						EJScriptTrace.COMPILER_TRACE,
						"Waiting for it to complete");
				process.waitFor();
				EJScriptTrace.trace(EJScriptTrace.TRACE_VERBOSE,
						EJScriptTrace.COMPILER_TRACE,
						"End Compiler output:\n\n");
				
				/*
				 * Convert the output from the compiler into Markers in case of errors or warnings.
				 * Ignore any output that does not match the expected error/warning string.
				 * 
				 * In the cases where the error or warning is followed by the location of
				 * the error/warning, attempt to decode and set marker to that location.
				 */
				if (outputList.size() > 0) {
					console.println("");
				}
				for (int i = 0; i < outputList.size(); i++) {
					CompilerOutput output = new CompilerOutput(outputList.get(i));
					if (output.isValid()) {
						output.setFile(getFileByName(output.getFileName()));
						list.add(output);
					}
					console.println(outputList.get(i));
				}
				if (outputList.size() > 0) {
					console.println("");
				}
				
				EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG,
						EJScriptTrace.COMPILER_TRACE,
						"compiler completed");
				int rc = process.exitValue();
				EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG,
						EJScriptTrace.COMPILER_TRACE,
						exe.getAbsolutePath() + 
						" exited with exitValue " + rc + " and list length is " + list.size());
				if (rc != 0) {
					console.println(exe.getName() + ": exited with value: " + rc + " (0x" + Integer.toHexString(rc).toUpperCase() + ")");
				}
				for (int x = 0; x < list.size(); x++) {
					EJScriptTrace.trace(EJScriptTrace.TRACE_VERBOSE,
							EJScriptTrace.COMPILER_TRACE,
							"element " + x + " is " + 
							list.get(x).toString());
				}
				in.close();
			} else {
				EJScriptTrace.traceError(EJScriptTrace.COMPILER_TRACE,
						exe.getAbsolutePath() + " Not Found");
			}
			EJScriptTrace.trace(EJScriptTrace.TRACE_DEBUG,
					EJScriptTrace.COMPILER_TRACE,
					"finished");
		
		} catch (IOException e) {
			EJScriptTrace.traceError("GMK226: " + EJScriptTrace.COMPILER_TRACE, e);
			console.println(e.getMessage());
		} catch (InterruptedException e) {
			EJScriptTrace.traceError("GMK229: " + EJScriptTrace.COMPILER_TRACE, e);
			console.println(e.getMessage());
		} catch (IllegalMonitorStateException e) {
			EJScriptTrace.traceError("GMK232: " + EJScriptTrace.COMPILER_TRACE, e);
			console.println(e.getMessage());
		} catch (Exception e) {
			EJScriptTrace.traceError("GMK244: " + EJScriptTrace.COMPILER_TRACE, e);
			EJScriptTrace.trace(EJScriptTrace.TRACE_VERBOSE,
					EJScriptTrace.COMPILER_TRACE,
					e.getMessage());
			console.println(e.getMessage());
		} finally {
			try {
				EJScriptTrace.trace(EJScriptTrace.TRACE_VERBOSE,
						EJScriptTrace.COMPILER_TRACE,
						"Inside compile finally block");
				if (error != null) {
					while ((line = error.readLine()) != null) {
						System.out.println("Error: " + line);
						EJScriptTrace.trace(EJScriptTrace.TRACE_VERBOSE,
								EJScriptTrace.COMPILER_TRACE,
								"Error stream: " +line);
					}
					error.close();
				}
				EJScriptTrace.trace(EJScriptTrace.TRACE_VERBOSE,
						EJScriptTrace.COMPILER_TRACE,
						"Leaving compile finally block");
			} catch (IOException e) {
				
				EJScriptTrace.traceError("250: " + EJScriptTrace.COMPILER_TRACE, e);
			}
		}
		EJScriptTrace.trace(EJScriptTrace.TRACE_VERBOSE,
				EJScriptTrace.COMPILER_TRACE,
				"End Compile");
		
		return list;
	}
	
	/**
	 * Build the arguments list.
	 * arg[0] is always the compiler
	 * arg[1] - arg[n] are the compiler arguments
	 * arg[n+1] - arg[m] are the files to be compiled together
	 * @return String[] of arguments
	 */
	private String[] constructArguments() {
		EJScriptTrace.trace(EJScriptTrace.TRACE_VERBOSE,
				EJScriptTrace.COMPILER_TRACE,
				"+constructArguments");
		int filesLength;
		for (filesLength = files.length; filesLength > 0; filesLength--) {
			if (files[filesLength - 1] != null) {
				break;
			}
		}
		StringTokenizer tok = new StringTokenizer(compilerOptsString, " ");
		String[] args = new String[1 + tok.countTokens() + filesLength];
		
		int i = 0;
		args[i++] = exe.getAbsolutePath();
		while (tok.hasMoreElements()) {
			args[i++] = tok.nextToken();
		}
		for (int j = 0; j < filesLength && files[j] != null; j++, i++) {
			if (files[j] == null){
				EJScriptTrace.trace(EJScriptTrace.TRACE_VERBOSE,
						EJScriptTrace.COMPILER_TRACE,
						"files[" + j + "] is null  ");
			}
			if (files[j].getRawLocation() == null) {
				EJScriptTrace.trace(EJScriptTrace.TRACE_VERBOSE,
						EJScriptTrace.COMPILER_TRACE,
						"files[" + j + "].getRawLocation is null for " + files[j]);
			}
			EJScriptTrace.trace(EJScriptTrace.TRACE_VERBOSE,
					EJScriptTrace.COMPILER_TRACE,
					"files[j] is " + files[j] + " files[j].getRawLocation is " + 
					files[j].getRawLocation());
			
			args[i] = files[j].getRawLocation().toOSString();
		}
		EJScriptTrace.trace(EJScriptTrace.TRACE_VERBOSE,
				EJScriptTrace.COMPILER_TRACE,
				"constructArguments done");
		return args;
	}
	
	/**
	 * Looks up the IFile reference based on the error output string of the 
	 * file name.
	 */
	private IFile getFileByName(String fileName) {
		IFile file;
		for (int i = 0; i < files.length; i++) {
			if (files[i] instanceof IFile) {
				file = (IFile)files[i];
				if (file.getName().equals(fileName) || 
						(fileName != null && fileName.equals(file.getRawLocation().toOSString()))) {
					return file;
				}
				
			}
		}
		return null;
	}
	
	/**
	 * Trace the compiler arguments to the user through the console.
	 * @param args The built argument array.
	 */
	private void traceCompilerArgs(String[] args)
	{
		String executionString = new String("");
		
		for (int i = 0; i < args.length; i++)
		{
			if (args[i] != null) {
				executionString = executionString.concat(args[i]);
				if (i < args.length - 1)
				{
					executionString = executionString.concat(" ");
				}
			}
		}
		console.println(executionString);
		EJScriptTrace.trace(EJScriptTrace.TRACE_CRITICAL,
				EJScriptTrace.COMPILER_TRACE, executionString);
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
