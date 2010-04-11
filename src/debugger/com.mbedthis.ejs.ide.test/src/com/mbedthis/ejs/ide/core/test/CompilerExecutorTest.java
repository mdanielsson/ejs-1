package com.mbedthis.ejs.ide.core.test;

import java.util.logging.Logger;

import junit.framework.TestCase;
import java.util.List;

import com.mbedthis.ejs.ide.core.CompilerExecutor;
import com.mbedthis.ejs.ide.core.CompilerOutput;

public class CompilerExecutorTest extends TestCase {

	private static Logger logger = Logger.getLogger("com.mbedthis.jscripteditor");
	private static String CURRENT_PATH_TO_COMPILER = "C:/dev/mob/juice/dummyCompiler/bin";
	
	
	
	protected void setUp() throws Exception {
		super.setUp();
	}

	protected void tearDown() throws Exception {
		super.tearDown();
	}

	public void testBasic() {
		logger.fine("Starting Test");
		CompilerExecutor ce = new CompilerExecutor(
				CURRENT_PATH_TO_COMPILER + "/dummy.exe",
				new String[] {"4", "test"});
		logger.fine("CompilerExecuter instantiated");
		List<CompilerOutput> list = ce.compile();
		logger.severe("CompilerExecuter compiled list size is " + list.size());
		assertTrue(list.size() == 4);
	}

	public void testEmpty() {
		CompilerExecutor ce = new CompilerExecutor(
				CURRENT_PATH_TO_COMPILER + "/notThere.exe",
				new String[] {"6"});
		List<CompilerOutput> list = ce.compile();
		assertTrue(list == null);
	}
	
	
}
