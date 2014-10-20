#include "..\CxUsart_1_drv.h"
#include "Utils.h"

//------------------------------------------------------------------------------
CxUsart_1_drv &u1_drv = CxUsart_1_drv::getInstance();   
//------------------------------------------------------------------------------

CxUsart_1_drv::CxUsart_1_drv( ):
   IxDriver         ( "U1_DRV" )
  ,comPortHandle    ( NULL     )
  ,timeOut          ( 10   ) 
  ,lCounter         ( 0    )    
  ,inputBufferSize  ( SizeOfInputBuffer  )
  ,outputBufferSize ( SizeOfoutputBuffer ) 
  ,pInputBuffer     ( NULL )
  ,pOutputBuffer    ( NULL )  
  ,InputLength      ( 0    )  
{
  mod_memset( reinterpret_cast<char*>(&IO_UART), 0, sizeof(TIO_UART), sizeof(TIO_UART) );   // preset buffer
} 

CxUsart_1_drv::~CxUsart_1_drv( )
{

}

CxUsart_1_drv &CxUsart_1_drv::getInstance( )
{
  static CxUsart_1_drv theInstance;
  return theInstance;
}

//------------------------------------------------------------------------------

void CxUsart_1_drv::reinit( )
{
  DCB dcb;   
  GetCommState( &dcb );   

  comPortHandle = xU1_SerialPortInitMinimal( dcb.BaudRate, SizeOfInputBuffer );
}

void CxUsart_1_drv::LoadOutputBuffer( char *buf, const unsigned short buf_size )
{
  if( buf_size != 0 && buf != NULL )
  {
    mod_memcpy( reinterpret_cast<char*>(IO_UART.aOPack), buf, buf_size, GetSizeOfOutputBuffer() );    
    IO_UART.sOPack_size = buf_size;
    IO_UART.lOPack_numb++;
  }
  else
  {
    // !!!
    /*ASSERTION(e_sin);*/
  }   
}

unsigned short CxUsart_1_drv::ReadFromInputBuffer( char *buf, const unsigned short buf_limit )
{
  if( IO_UART.sIPack_size > 0 )
  {
    if( buf != NULL && buf_limit != 0 )
    {
      unsigned short t_size = IO_UART.sIPack_size;      
      mod_memcpy( buf, reinterpret_cast<char*>(IO_UART.aIPack), IO_UART.sIPack_size, buf_limit );
      IO_UART.sIPack_size = 0;
      return( t_size );        
    }
    else
    {
      // !!!
      /*ASSERTION(e_sin);*/
    }   
  }
  return(0);                                                              
}

void CxUsart_1_drv::FlushInputBuffers( )
{
  if( IO_UART.cIPack_rec  != 0 )
  {
    IO_UART.cIPack_rec = false; 
    IO_UART.sIPack_size = 0;
  }
}

void CxUsart_1_drv::FlushOutputBuffers( )
{
  if( IO_UART.cOPack_trans  != 0 )
  {
    IO_UART.cOPack_trans = false; 
    IO_UART.sOPack_size = 0;
  }
}
     
bool CxUsart_1_drv::IsTransitionCompleted( )
{
  bool result = false;  
  if( true != IO_UART.cOPack_trans ) result = true;  
  return( result );  
}

bool CxUsart_1_drv::IsReceiveCompleted()
{
  bool result = IO_UART.cIPack_rec;  
  if( result == true ) IO_UART.cIPack_rec = 0;  
  return( result );
}

unsigned short CxUsart_1_drv::GetSizeOfInputBuffer( )const
{
  return( sizeof(IO_UART.aIPack) );
}

unsigned short CxUsart_1_drv::GetSizeOfOutputBuffer( )const
{
  return( sizeof(IO_UART.aOPack) );
} 

bool CxUsart_1_drv::SetCommState( pDCB pointerOnDCB )
{
  bool result = SetInternalCommState( pointerOnDCB );
  
  if( result == true ) reinit();
  
  return result;
}

//------------------------------------------------------------------------------

void CxUsart_1_drv::CommandProcessor( TCommand &Command )
{ 
  // command process
  switch ( Command.ComID )
  {
   case C_SET_IN_BUFFER :
      {
        pInputBuffer = *( reinterpret_cast<char**>(Command.Container) );  
        break;
      }  
   case C_SET_OUT_BUFFER :
      {
        pOutputBuffer = *( reinterpret_cast<char**>(Command.Container) );
        break;
      }
   case C_SET_DCB  : 
      {
        pDCB ptrDCB = reinterpret_cast<pDCB>( Command.Container );
        SetCommState( ptrDCB );        
        break;
      }        
    case C_SEND_DATA : 
      {
        if( NULL != pOutputBuffer )
        {  
           unsigned short dataLength = *(reinterpret_cast<unsigned short*>( Command.Container ));
           LoadOutputBuffer( pOutputBuffer, dataLength );
           StartTxD();
        }
        else
        {
          // !!!
        }  
        break;
      }   
    case C_GET_IO_SIZE : 
      {
        TCommand ComIOsize  = { Command.ConsumerID, Command.SenderID, request, C_GET_IO_SIZE,  reinterpret_cast<void*>(&outputBufferSize) };
        SendCommand( &ComIOsize );
        break;
      }     
    case C_STOP : 
      {
        StopTxD();
        StopRxD();
        break;
      }          
      
    default :  {   }    // unknown command 
  }     
}

void CxUsart_1_drv::ThreadProcessor( )
{
   // at here we get information from driver and put data in driver
   if( comPortHandle != NULL )
   {
     //---tx uart processor---
     if( IO_UART.cOPack_trans != false && IO_UART.sOPack_size != 0 )
     {
       if( IO_UART.sOPack_size < GetSizeOfOutputBuffer() )
       {  
         vU1_SerialPutString( comPortHandle, reinterpret_cast<char*>(IO_UART.aOPack), IO_UART.sOPack_size );
       }  
       else 
       {
         vU1_SerialPutString( comPortHandle, reinterpret_cast<char*>(IO_UART.aOPack), GetSizeOfOutputBuffer() );
       }         
       IO_UART.sOPack_size = 0;
     } 
     
     //---tx finish event generation---
     if( true == vU1_isTransmissionFinished( comPortHandle ) )
     {         
         TCommand ComIOsize  = { DrvID, ConsumerID, response, C_SEND_DATA_END, NULL };
         SendCommand( &ComIOsize );     
         IO_UART.cOPack_trans = false; 
     }  
     
     //---rx uart processor---
     char symbol = 0;     
     bool result = xU1_SerialGetChar( comPortHandle, &symbol, (portTickType)0 );
     
     if( true == result )
     {
        if( IO_UART.sIPack_size < GetSizeOfInputBuffer() )
        {  
          IO_UART.aIPack[IO_UART.sIPack_size++] = symbol;
        }  
        lCounter = (timeOut >> 1) + get_sys_tick();      
     }
     else
     {
       if( (lCounter != 0) && (get_sys_tick() > lCounter) )
       {
         IO_UART.lIPack_numb++;
         IO_UART.cIPack_rec = true;       
         lCounter = 0;
       }  
     }                          
   } 
   else // if( comPortHandle != NULL )..
   {
      // !!! 
   }  

   //---send command for acceptor---
   if( NULL != pInputBuffer )
   {
      if( true == IsReceiveCompleted() )
      {  
        InputLength = ReadFromInputBuffer( pInputBuffer, inputBufferSize );     
        if( InputLength > 0)
        {
           TCommand ComIOsize  = { DrvID, ConsumerID, request, C_NEED_DATA_PROCESS, &InputLength };
           SendCommand( &ComIOsize );
        }
      }
   }
   else
   {
      // !!!
   }      
}

//------------------------------------------------------------------------------
