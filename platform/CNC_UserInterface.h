#ifndef CNC_USERINTERFACE_H
#define CNC_USERINTERFACE_H

#include "../engine/CNC_Types.h"
typedef struct UserInterface
{
    bool m_clockApp;
    bool m_christmasApp;
    bool m_shapeApp;
    bool m_pongApp;

    ControlValue** m_values;
    DebugValue**   m_debugValues;

} UserInterface;

UserInterface* CreateUserinterface();
void           ShowUserInterface  ( UserInterface* ui );

#endif//CNC_USERINTERFACE_H
