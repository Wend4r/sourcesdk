#include "common/source2_main.h"

int main( int argc, char **argv )
{
	const int nExitCode = Source2Main( argc, argv );

	Source2TestExit( nExitCode );
}
