#ifndef CNC_USERINTERFACE_H
#define CNC_USERINTERFACE_H

typedef struct UserInterface
{
    bool m_clockApp;
    bool m_christmasApp;
    bool m_shapeApp;
    bool m_pongApp;

} UserInterface;

UserInterface* CreateUserinterface();
void           ShowUserInterface( UserInterface* ui );

#endif//CNC_USERINTERFACE_H
