#include "..\CxUartBase.h"

#include <stdlib.h>
#include "utils.h"
//#include "CxAssertions.h"


//-------------------------------BASE UART class--------------------------------


CxUartBase::CxUartBase()
{
  mod_memset( reinterpret_cast<char*>(&IO_UART), 0, sizeof(TIO_UART), sizeof(TIO_UART) );   // preset buffer
}

