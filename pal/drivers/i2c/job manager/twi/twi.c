
//------------------------------------------------------------------------------
//         Headers
//------------------------------------------------------------------------------

#include "FreeRTOS.h"
#include "lib_AT91SAM7S.h"
#include "twi.h"


//------------------------------------------------------------------------------
/// AT91F_TWI_Open
/// nitializes TWI device
//------------------------------------------------------------------------------
void AT91F_TWI_Open( int TwiClock )
{
    // Configure TWI PIOs 
    AT91F_TWI_CfgPIO( );

    // Configure PMC by enabling TWI clock 
    AT91F_TWI_CfgPMC( );
    
    // Configure TWI in master mode 
    AT91F_TWI_Configure( AT91C_BASE_TWI );
    
    // Set TWI Clock Waveform Generator Register 
    TWI_Configure( i2cCOM, TwiClock, configCPU_CLOCK_HZ );
}


//------------------------------------------------------------------------------
/// Computes and returns x power of y.
/// \param x  Value.
/// \param y  Power.
//------------------------------------------------------------------------------
unsigned int power(unsigned int x, unsigned int y)
{
    unsigned int result = 1;
    while (y > 0) {

        result *= x;
        y--;
    }

    return result;
}

//------------------------------------------------------------------------------
/// Configures a TWI peripheral to operate in master mode, at the given
/// frequency (in Hz). The duty cycle of the TWI clock is set to 50%.
/// \param pTwi  Pointer to an AT91S_TWI instance.
/// \param twck  Desired TWI clock frequency.
/// \param mck  Master clock frequency.
//------------------------------------------------------------------------------

void TWI_Configure(AT91S_TWI *pTwi, unsigned int twck, unsigned int mck)
{
    unsigned int ckdiv = 0;
    unsigned int cldiv;
    unsigned char ok = 0;

    // Configure clock
    while( !ok ) 
    {        
      cldiv = ((mck / (2 * twck)) - 3) / power(2, ckdiv);
      if (cldiv <= 255) 
      {
          ok = 1;
      }
      else 
      {
          ckdiv++;
      }
    }

    pTwi->TWI_CWGR = (ckdiv << 16) | (cldiv << 8) | cldiv;
}

//------------------------------------------------------------------------------
/// Sends a STOP condition on the TWI.
/// \param pTwi  Pointer to an AT91S_TWI instance.
//------------------------------------------------------------------------------
void TWI_Stop(AT91S_TWI *pTwi)
{
    pTwi->TWI_CR = AT91C_TWI_STOP;
}

//------------------------------------------------------------------------------
/// Starts a read operation on the TWI bus with the specified slave, and returns
/// immediately. Data must then be read using TWI_ReadByte() whenever a byte is
/// available (poll using TWI_ByteReceived()).
/// \param pTwi  Pointer to an AT91S_TWI instance.
/// \param address  Slave address on the bus.
/// \param iaddress  Optional internal address bytes.
/// \param isize  Number of internal address bytes.
//-----------------------------------------------------------------------------
void TWI_StartRead(
    AT91S_TWI *pTwi,
    unsigned char address,
    unsigned int iaddress,
    unsigned char isize)
{
    // Set slave address and number of internal address bytes
    pTwi->TWI_MMR = (isize << 8) | AT91C_TWI_MREAD | (address << 16);

    // Set internal address bytes
    pTwi->TWI_IADR = iaddress;

    // Send START condition
    pTwi->TWI_CR = AT91C_TWI_START;
}

//-----------------------------------------------------------------------------
/// Reads a byte from the TWI bus. The read operation must have been started
/// using TWI_StartRead() and a byte must be available (check with
/// TWI_ByteReceived()).
/// Returns the byte read.
/// \param pTwi  Pointer to an AT91S_TWI instance.
//-----------------------------------------------------------------------------
unsigned char TWI_ReadByte(AT91S_TWI *pTwi)
{
    return pTwi->TWI_RHR;
}

//-----------------------------------------------------------------------------
/// Sends a byte of data to one of the TWI slaves on the bus. This function
/// must be called once before TWI_StartWrite() with the first byte of data
/// to send, then it shall be called repeatedly after that to send the
/// remaining bytes.
/// \param pTwi  Pointer to an AT91S_TWI instance.
/// \param byte  Byte to send.
//-----------------------------------------------------------------------------
void TWI_WriteByte(AT91S_TWI *pTwi, unsigned char byte)
{
    pTwi->TWI_THR = byte;
}

//-----------------------------------------------------------------------------
/// Starts a write operation on the TWI to access the selected slave, then
/// returns immediately. A byte of data must be provided to start the write;
/// other bytes are written next.
/// \param pTwi  Pointer to an AT91S_TWI instance.
/// \param address  Address of slave to acccess on the bus.
/// \param iaddress  Optional slave internal address.
/// \param isize  Number of internal address bytes.
/// \param byte  First byte to send.
//-----------------------------------------------------------------------------
void TWI_StartWrite( AT91S_TWI *pTwi, unsigned char address, unsigned int iaddress, unsigned char isize )
{
    // Set slave address and number of internal address bytes
    pTwi->TWI_MMR = (isize << 8) | (address << 16);

    // Set internal address bytes
    pTwi->TWI_IADR = iaddress;
}

//-----------------------------------------------------------------------------
/// Returns 1 if a byte has been received and can be read on the given TWI
/// peripheral; otherwise, returns 0. This function resets the status register
/// of the TWI.
/// \param pTwi  Pointer to an AT91S_TWI instance.
//-----------------------------------------------------------------------------
unsigned char TWI_ByteReceived(AT91S_TWI *pTwi)
{
    return ((pTwi->TWI_SR & AT91C_TWI_RXRDY) == AT91C_TWI_RXRDY);
}

//-----------------------------------------------------------------------------
/// Returns 1 if a byte has been sent, so another one can be stored for
/// transmission; otherwise returns 0. This function clears the status register
/// of the TWI.
/// \param pTwi  Pointer to an AT91S_TWI instance.
//-----------------------------------------------------------------------------
unsigned char TWI_ByteSent(AT91S_TWI *pTwi)
{
    return ((pTwi->TWI_SR & AT91C_TWI_TXRDY) == AT91C_TWI_TXRDY);
}

//-----------------------------------------------------------------------------
/// Returns 1 if the current transmission is complete (the STOP has been sent);
/// otherwise returns 0.
/// \param pTwi  Pointer to an AT91S_TWI instance.
//-----------------------------------------------------------------------------
unsigned char TWI_TransferComplete(AT91S_TWI *pTwi)
{
    return ((pTwi->TWI_SR & AT91C_TWI_TXCOMP) == AT91C_TWI_TXCOMP);
}

//-----------------------------------------------------------------------------
/// Enables the selected interrupts sources on a TWI peripheral.
/// \param pTwi  Pointer to an AT91S_TWI instance.
/// \param sources  Bitwise OR of selected interrupt sources.
//-----------------------------------------------------------------------------
void TWI_EnableIt(AT91S_TWI *pTwi, unsigned int sources)
{
    pTwi->TWI_IER = sources;
}

//-----------------------------------------------------------------------------
/// Disables the selected interrupts sources on a TWI peripheral.
/// \param pTwi  Pointer to an AT91S_TWI instance.
/// \param sources  Bitwise OR of selected interrupt sources.
//-----------------------------------------------------------------------------
void TWI_DisableIt(AT91S_TWI *pTwi, unsigned int sources)
{
    pTwi->TWI_IDR = sources;
}

//-----------------------------------------------------------------------------
/// Returns the current status register of the given TWI peripheral. This
/// resets the internal value of the status register, so further read may yield
/// different values.
/// \param pTwi  Pointer to an AT91S_TWI instance.
//-----------------------------------------------------------------------------
unsigned int TWI_GetStatus(AT91S_TWI *pTwi)
{
    return pTwi->TWI_SR;
}

//-----------------------------------------------------------------------------
/// Returns the current status register of the given TWI peripheral, but
/// masking interrupt sources which are not currently enabled.
/// This resets the internal value of the status register, so further read may
/// yield different values.
/// \param pTwi  Pointer to an AT91S_TWI instance.
//-----------------------------------------------------------------------------
unsigned int TWI_GetMaskedStatus(AT91S_TWI *pTwi)
{
    unsigned int status;

    status = pTwi->TWI_SR;
    status &= pTwi->TWI_IMR;

    return status;
}


