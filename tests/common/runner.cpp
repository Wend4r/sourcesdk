#include "common/runner.h"

#include <stdio.h>

static TestRegistration *&GetTestList()
{
	static TestRegistration *s_pTests = nullptr;
	return s_pTests;
}

void RegisterTest( const char *pName, TestFunction pFunction )
{
	new TestRegistration( pName, pFunction );
}

TestRegistration::TestRegistration( const char *pName, TestFunction pFunction )
 : m_pName( pName ),
   m_pFunction( pFunction ),
   m_pNext( GetTestList() )
{
	GetTestList() = this;
}

void ReportTestFailure( const char *pFile, int nLine, const char *pExpression, const char *pExpected, const char *pActual )
{
	printf( "Expected: %s\n", pExpression );
	if ( pExpected && pActual )
	{
		printf( "Actual:   %s != %s\n", pExpected, pActual );
	}
	printf( "File:     %s:%d\n", pFile, nLine );
}

int RunAllTests()
{
	int nPassed = 0;
	int nFailed = 0;

	for ( TestRegistration *pTest = GetTestList(); pTest; pTest = pTest->m_pNext )
	{
		printf( "[ RUN      ] %s\n", pTest->m_pName );
		fflush( stdout );

		try
		{
			pTest->m_pFunction();
			++nPassed;
			printf( "[       OK ] %s\n\n", pTest->m_pName );
			fflush( stdout );
		}
		catch ( const TestFailure & )
		{
			++nFailed;
			printf( "[  FAILED  ] %s\n\n", pTest->m_pName );
			fflush( stdout );
		}
		catch ( ... )
		{
			++nFailed;
			printf( "Unexpected exception\n" );
			printf( "[  FAILED  ] %s\n\n", pTest->m_pName );
		}
	}

	printf( "Tests passed: %d\n", nPassed );
	printf( "Tests failed: %d\n", nFailed );
	fflush( stdout );

	return nFailed == 0 ? 0 : 1;
}
