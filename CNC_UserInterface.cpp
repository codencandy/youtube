#include "libs/imgui/imgui.h"
#include "CNC_UserInterface.h"

UserInterface* CreateUserinterface()
{
    UserInterface* ui = (UserInterface*)malloc( sizeof( UserInterface ) );

    ui->m_clockApp     = true;
    ui->m_christmasApp = true;

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

    ImGui::End();
}