#include "libs/imgui/imgui.h"
#include "CNC_UserInterface.h"

UserInterface* CreateUserinterface()
{
    UserInterface* ui = (UserInterface*)malloc( sizeof( UserInterface ) );

    ui->m_clockApp     = false;
    ui->m_christmasApp = false;
    ui->m_shapeApp     = false;
    ui->m_pongApp      = true;

    return ui;
}

void ShowUserInterface( UserInterface* ui )
{
    ImVec2 buttonSize = ImVec2( 150, 30 );

    ImGui::Begin( "codencandy demo" );

    if( ImGui::Button( "clock app", buttonSize ) )
    {
        ui->m_clockApp = !ui->m_clockApp;
    }

    if( ImGui::Button( "christmas app", buttonSize ) )
    {
        ui->m_christmasApp = !ui->m_christmasApp;
    }

    if( ImGui::Button( "shape app", buttonSize ) )
    {
        ui->m_shapeApp = !ui->m_shapeApp;
    }

    if( ImGui::Button( "pong app", buttonSize ) )
    {
        ui->m_pongApp = !ui->m_pongApp;
    }

    ImGui::End();
}