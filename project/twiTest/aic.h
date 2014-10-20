//------------------------------------------------------------------------------
/// \dir
/// !Purpose
/// 
/// Methods and definitions for configuring interrupts using the Advanced
/// Interrupt Controller (AIC).
/// 
/// !Usage
/// -# Configure an interrupt source using AIC_ConfigureIT
/// -# Enable or disable interrupt generation of a particular source with
///    AIC_EnableIT and AIC_DisableIT.
//------------------------------------------------------------------------------

#ifndef AIC_H
#define AIC_H

//------------------------------------------------------------------------------
//         Headers
//------------------------------------------------------------------------------

#include <board.h>

//------------------------------------------------------------------------------
//         Definitions
//------------------------------------------------------------------------------

#ifndef AT91C_AIC_SRCTYPE_INT_HIGH_LEVEL
    /// Redefinition of missing constant.
    #define AT91C_AIC_SRCTYPE_INT_HIGH_LEVEL AT91C_AIC_SRCTYPE_INT_LEVEL_SENSITIVE
#endif

//------------------------------------------------------------------------------
//         Global functions
//------------------------------------------------------------------------------

extern void AIC_ConfigureIT(unsigned int source,
                                   unsigned int mode,
                                   void (*handler)( void ));

extern void AIC_EnableIT(unsigned int source);

extern void AIC_DisableIT(unsigned int source);

#endif //#ifndef AIC_H

