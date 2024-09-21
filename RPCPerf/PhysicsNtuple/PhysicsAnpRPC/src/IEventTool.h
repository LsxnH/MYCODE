#ifndef ATH_IEVENTTOOL_H
#define ATH_IEVENTTOOL_H

/**********************************************************************************
 * @Package: PhysicsAnpRPC
 * @Class  : IEventTool
 * @Author : Rustem Ospanov
 *
 * @Brief  :
 * 
 * Tool base class for processing event data
 * 
 **********************************************************************************/

// ROOT
#include "TStopwatch.h"

// Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

namespace Ath
{
  class Event;

  static const InterfaceID IID_IEventTool ( "Ath::IEventTool", 1, 0 );
  
  class IEventTool : virtual public IAlgTool
  {
  public:
    
    IEventTool() {}
    virtual ~IEventTool() {}
    
    static const InterfaceID& interfaceID() { return IID_IEventTool; }
    
    virtual void ProcEvent(Ath::Event &) = 0;

    TStopwatch& GetTimer() { return m_timer; }

  protected:

    TStopwatch                     m_timer;
  };
}

#endif
