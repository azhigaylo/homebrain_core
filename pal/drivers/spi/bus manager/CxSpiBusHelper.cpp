//------------------------------------------------------------------------------

#include "CxSysTimer.h"
#include "CxSpiBusHelper.h"

//------------------------------------------------------------------------------

#define SPI_SCBR_MIN  2

#define USE_DMA

//------------------------------------------------------------------------------
static volatile unsigned char spiControlTimer;			                // 100Hz decrement timer 

void spi_timerproc ()
{
   unsigned char n;
   n = spiControlTimer;						             
   if (n) spiControlTimer = --n;
}

CxSysVirtualTimer SpiControlTimer( spi_timerproc, 10, CxSysVirtualTimer::cycle, CxSysVirtualTimer::on );

//------------------------------------------------------------------------------

CxSpiBusHelper::CxSpiBusHelper( ):
   helperBusy ( false ) 
{    

}

//------------------------------------------------------------------------------

unsigned char CxSpiBusHelper::xmit_spi(unsigned char dat)
{
    AT91PS_SPI pSPI      = AT91C_BASE_SPI;
  
    while( !( pSPI->SPI_SR & AT91C_SPI_TDRE ) );                                // transfer compl. wait
    pSPI->SPI_TDR = dat;

    while( !( pSPI->SPI_SR & AT91C_SPI_RDRF ) );                                // wait for char
    return (unsigned char)( pSPI->SPI_RDR );                                    // it's important to read RDR here!
}


unsigned char CxSpiBusHelper::rcvr_spi()
{
    unsigned char dat;
    AT91PS_SPI pSPI      = AT91C_BASE_SPI;
  
    while( !( pSPI->SPI_SR & AT91C_SPI_TDRE ) );                                // transfer compl. wait
    pSPI->SPI_TDR = 0xFF;

    while( !( pSPI->SPI_SR & AT91C_SPI_RDRF ) );                                // wait for char
    dat = (unsigned char)( pSPI->SPI_RDR );

    return dat;
}

void CxSpiBusHelper::rcvr_spi_m(unsigned char *dest)
{
    unsigned char dat;
    AT91PS_SPI pSPI      = AT91C_BASE_SPI;
  
    while( !( pSPI->SPI_SR & AT91C_SPI_TDRE ) );                                // transfer compl. wait
    pSPI->SPI_TDR = 0xFF;
  
    while( !( pSPI->SPI_SR & AT91C_SPI_RDRF ) );                                // wait for char
    dat = (unsigned char)( pSPI->SPI_RDR );
  
    *dest = dat;
}

void CxSpiBusHelper::init_spi()
{
    AT91PS_SPI pSPI      = AT91C_BASE_SPI;
    AT91PS_PMC pPMC      = AT91C_BASE_PMC;
    AT91PS_PIO pPIOA     = AT91C_BASE_PIOA;

    // disable PIO from controlling MOSI, MISO, SCK (=hand over to SPI)
    // keep CS untouched - used as GPIO pin during init
    pPIOA->PIO_PDR = AT91C_PA12_MISO | AT91C_PA13_MOSI | AT91C_PA14_SPCK; 
    // set pin-functions in PIO Controller
    pPIOA->PIO_ASR = AT91C_PA12_MISO | AT91C_PA13_MOSI | AT91C_PA14_SPCK; 
    
    // enable peripheral clock for SPI ( PID Bit 5 )
    pPMC->PMC_PCER = ( (unsigned short) 1 << AT91C_ID_SPI ); // n.b. IDs are just bit-numbers
    
    pSPI->SPI_CR = AT91C_SPI_SWRST;

    // SPI mode: master, fixed periph. sel., FDIV=0, fault detection disabled
    pSPI->SPI_MR  = AT91C_SPI_MSTR | AT91C_SPI_PS_FIXED | AT91C_SPI_MODFDIS;

    // set PCS for fixed select
    pSPI->SPI_MR &= 0xFFF0FFFF; // clear old PCS - redundant (AT91lib)

    // SPI enable and reset
    pSPI->SPI_CR = AT91C_SPI_SPIEN;
}


void CxSpiBusHelper::setSpeed( char chipNumber, unsigned char speed)
{
   unsigned long reg;
   AT91PS_SPI pSPI      = AT91C_BASE_SPI;

   if ( speed < SPI_SCBR_MIN ) speed = SPI_SCBR_MIN;
   if ( speed > 1 ) speed &= 0xFE;

   reg = pSPI->SPI_CSR[chipNumber];
   reg = ( reg & ~(AT91C_SPI_SCBR) ) | ( (unsigned long)speed << 8 );
   pSPI->SPI_CSR[chipNumber] = reg;
}

// set chip-select-register
void CxSpiBusHelper::setSpiPropertis( char chipNumber, unsigned int csRegister )
{
   AT91PS_SPI pSPI      = AT91C_BASE_SPI;
   pSPI->SPI_CSR[chipNumber] = csRegister;
}

// set chip-select-register
void CxSpiBusHelper::chipSelect( char chipNumber )
{
   AT91PS_SPI pSPI  = AT91C_BASE_SPI;
   AT91PS_PIO pPIOA = AT91C_BASE_PIOA;
   
   // set spi chip number
   pSPI->SPI_MR &= ~AT91C_SPI_PCS;
   pSPI->SPI_MR |= (~(0x0001 << chipNumber)<<16) & AT91C_SPI_PCS; // set PCS
   
   // set pin direction
   switch( chipNumber ) 
   {
     case 0  : pPIOA->PIO_PDR = (unsigned int)AT91C_PA11_NPCS0; break;
     case 1  : pPIOA->PIO_PDR = (unsigned int)AT91C_PA31_NPCS1; break;
     case 2  : pPIOA->PIO_PDR = (unsigned int)AT91C_PA10_NPCS2; break;
     case 3  : pPIOA->PIO_PDR = (unsigned int)AT91C_PA22_NPCS3; break;     
     default : break; 
   }   
}

// Receive a data packet from SPI device
// buff - Data buffer to store received data 
// wc - Word count (0 means 256 words) 
bool CxSpiBusHelper::rcvr_datablock( unsigned char *buff, unsigned char wc )
{
   unsigned char token;

#ifdef USE_DMA
   // TODO: deuglyfy 
   static const unsigned char dummy_ff_block[512] ={0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
          0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
          0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
          0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
          0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
          0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
          0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
          0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
          0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
          0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
          0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
          0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
          0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
          0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
          0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
          0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
          0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
          0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
          0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
          0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
          0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
          0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
          0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
          0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
          0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
          0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
          0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
          0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
          0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
#endif

   spiControlTimer = 10;
   do 
   {							                        // Wait for data packet in timeout of 100ms 
          token = rcvr_spi();
   } while ((token == 0xFF) && spiControlTimer);
   if(token != 0xFE) return false;	                                        // If not valid data token, retutn with error 

#ifdef USE_DMA
   // enable DMA transfer
   *AT91C_SPI_RPR = (unsigned long)buff;
   *AT91C_SPI_RCR = 2 * (wc == 0 ? 256 : wc);
   *AT91C_SPI_TPR = (unsigned long)dummy_ff_block;
   *AT91C_SPI_TCR = 2 * (wc == 0 ? 256 : wc);
   *AT91C_SPI_PTCR = AT91C_PDC_RXTEN;
   *AT91C_SPI_PTCR = AT91C_PDC_TXTEN;
  
   while(! (*AT91C_SPI_SR & AT91C_SPI_ENDRX));	
   *AT91C_SPI_PTCR = AT91C_PDC_RXTDIS;
   *AT91C_SPI_PTCR = AT91C_PDC_TXTDIS;
#else
   do 
   {							                        // Receive the data block into buffer
          rcvr_spi_m(buff++);
          rcvr_spi_m(buff++);
   } while (--wc);
#endif
  
   rcvr_spi();						                        // Discard CRC 
   rcvr_spi();

   return true;					                                // Return with success 
}
   
//--------------------------------------
// Send a data packet to SPI 
// 512 byte data block to be transmitted 
// Data/Stop token 

bool CxSpiBusHelper::xmit_datablock( const unsigned char *buff, unsigned char token )
{
	AT91PS_SPI pSPI      = AT91C_BASE_SPI;
	unsigned char resp, wc = 0;
        
	xmit_spi(token);				                        //Xmit data token 
	if (token != 0xFD) {	                                                // Is data token 
		
#ifdef USE_DMA
		wc = wc;      // serve for prevent Pe177 warning
          
                // enable DMA transfer
		*AT91C_SPI_TPR = reinterpret_cast<unsigned int>(buff);
		*AT91C_SPI_TCR = 512;
		*AT91C_SPI_PTCR = AT91C_PDC_TXTEN;

		while(! (*AT91C_SPI_SR & AT91C_SPI_ENDTX));	
		*AT91C_SPI_PTCR = AT91C_PDC_TXTDIS;
		(unsigned char)( pSPI->SPI_RDR );                               // it's important to read RDR here!
#else
		do {							        // Xmit the 512 byte data block to SPI
			xmit_spi(*buff++);
			xmit_spi(*buff++);
		} while (--wc);
#endif
		
		xmit_spi(0xFF);			                                // CRC (Dummy) 
		xmit_spi(0xFF);
		resp = rcvr_spi();			                        // Reveive data response 
		if ((resp & 0x1F) != 0x05)		                        // If not accepted, return with error
			return false;
	}

	return true;
}
