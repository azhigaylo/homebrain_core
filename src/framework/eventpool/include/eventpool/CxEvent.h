#ifndef _CX_EVENTIX
#define _CX_EVENTIX

//------------------------------------------------------------------------------
//----------------base event calss----------------------------------------------
//------------------------------------------------------------------------------

#pragma pack ( 1 )
struct TEvent
{
   unsigned short eventType;
   unsigned short  dataSize;
   void *eventData;
}; typedef TEvent *pTEvent;
#pragma pack ( )

//------------------------------------------------------------------------------

class CxEvent
{
  public:

    CxEvent();
    virtual ~CxEvent(){}

  protected:
     TEvent Event;

}; typedef CxEvent *pCxEvent;

#endif // _CX_EVENTIX

