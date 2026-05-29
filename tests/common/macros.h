#ifndef SOURCESDK_TESTS_COMMON_MACROS_H
#define SOURCESDK_TESTS_COMMON_MACROS_H

#include "runner.h"

#define REGISTER_TEST( name ) \
	static void name(); \
	static TestRegistration name##_registration( #name, &name ); \
	static void name()

#define REGISTER_NAMED_TEST( test_name, function_name ) \
	static void function_name(); \
	static TestRegistration function_name##_registration( test_name, &function_name ); \
	static void function_name()

#endif // SOURCESDK_TESTS_COMMON_MACROS_H
