#ifndef SOURCESDK_TESTS_COMMON_RUNNER_H
#define SOURCESDK_TESTS_COMMON_RUNNER_H

using TestFunction = void ( * )();

void RegisterTest( const char *pName, TestFunction pFunction );
int RunAllTests();

class TestRegistration
{
public:
	TestRegistration( const char *pName, TestFunction pFunction );

	const char *m_pName;
	TestFunction m_pFunction;
	TestRegistration *m_pNext;
};

class TestFailure
{
};

void ReportTestFailure( const char *pFile, int nLine, const char *pExpression, const char *pExpected, const char *pActual );

#endif // SOURCESDK_TESTS_COMMON_RUNNER_H
