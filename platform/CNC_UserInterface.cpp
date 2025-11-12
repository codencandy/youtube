#include "../libs/imgui/imgui.h"
#include "CNC_UserInterface.h"

UserInterface* CreateUserinterface()
{
    UserInterface* ui = (UserInterface*)malloc( sizeof( UserInterface ) );

    ui->m_clockApp     = false;
    ui->m_christmasApp = false;
    ui->m_shapeApp     = false;
    ui->m_pongApp      = true;
    ui->m_values       = NULL;
    ui->m_debugValues  = NULL;

    return ui;
}

void SetupImGui( MainRenderer* renderer )
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup ImGui style
    ImGui::StyleColorsDark();
    io.Fonts->AddFontFromFileTTF( "res/blender_thin.ttf", 16.0f );
    
    // Setup Renderer backend
    ImGui_ImplMetal_Init( renderer->m_gpu );
    ImGui_ImplOSX_Init( renderer->m_view );

    io.DisplaySize.x = renderer->m_view.bounds.size.width;
    io.DisplaySize.y = renderer->m_view.bounds.size.height;

    CGFloat framebufferScale   = renderer->m_view.window.screen.backingScaleFactor ?: NSScreen.mainScreen.backingScaleFactor;
    io.DisplayFramebufferScale = ImVec2( framebufferScale, framebufferScale );

    ImGuiStyle& myStyle = ImGui::GetStyle();
    
    // padding & spacing
    myStyle.WindowPadding     = ImVec2( 12.0f, 12.0f );
    myStyle.FramePadding      = ImVec2( 12.0f, 8.0f );
    myStyle.ItemSpacing       = ImVec2( 12.0f, 12.0f );
    myStyle.ItemInnerSpacing  = ImVec2( 4.0f, 4.0f );
    myStyle.IndentSpacing     = 21.0f;

    // borders
    myStyle.FrameBorderSize         = 0.0f;
    myStyle.WindowBorderSize        = 0.0f;
    myStyle.SeparatorTextBorderSize = 1.0f;

    // rounding
    myStyle.WindowRounding    = 6.0f;
    myStyle.FrameRounding     = 3.0f;
    myStyle.ScrollbarRounding = 9.0f;
    myStyle.GrabRounding      = 2.0f;

    ImVec4* colors = ImGui::GetStyle().Colors;
    colors[ImGuiCol_Text]                   = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_Text]                   = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_Text]                   = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_TextDisabled]           = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    colors[ImGuiCol_WindowBg]               = ImVec4(0.13f, 0.13f, 0.13f, 0.70f);
    colors[ImGuiCol_ChildBg]                = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_PopupBg]                = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
    colors[ImGuiCol_Border]                 = ImVec4(0.35f, 0.79f, 0.81f, 0.50f);
    colors[ImGuiCol_BorderShadow]           = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_FrameBg]                = ImVec4(0.09f, 0.10f, 0.11f, 0.54f);
    colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.17f, 0.18f, 0.20f, 0.40f);
    colors[ImGuiCol_FrameBgActive]          = ImVec4(0.45f, 0.45f, 0.45f, 0.67f);
    colors[ImGuiCol_TitleBg]                = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
    colors[ImGuiCol_TitleBgActive]          = ImVec4(0.09f, 0.10f, 0.11f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
    colors[ImGuiCol_MenuBarBg]              = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
    colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
    colors[ImGuiCol_CheckMark]              = ImVec4(0.23f, 0.82f, 0.72f, 1.00f);
    colors[ImGuiCol_SliderGrab]             = ImVec4(0.88f, 0.77f, 0.24f, 1.00f);
    colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.88f, 0.77f, 0.24f, 1.00f);
    colors[ImGuiCol_Button]                 = ImVec4(0.38f, 0.39f, 0.38f, 0.40f);
    colors[ImGuiCol_ButtonHovered]          = ImVec4(0.59f, 0.59f, 0.59f, 1.00f);
    colors[ImGuiCol_ButtonActive]           = ImVec4(0.27f, 0.28f, 0.30f, 1.00f);
    colors[ImGuiCol_Header]                 = ImVec4(0.80f, 0.80f, 0.80f, 0.31f);
    colors[ImGuiCol_HeaderHovered]          = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
    colors[ImGuiCol_HeaderActive]           = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_Separator]              = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
    colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.10f, 0.40f, 0.75f, 0.78f);
    colors[ImGuiCol_SeparatorActive]        = ImVec4(0.10f, 0.40f, 0.75f, 1.00f);
    colors[ImGuiCol_ResizeGrip]             = ImVec4(0.26f, 0.59f, 0.98f, 0.20f);
    colors[ImGuiCol_ResizeGripHovered]      = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
    colors[ImGuiCol_ResizeGripActive]       = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
    colors[ImGuiCol_Tab]                    = ImVec4(0.23f, 0.28f, 0.35f, 0.86f);
    colors[ImGuiCol_TabHovered]             = ImVec4(0.41f, 0.44f, 0.47f, 0.80f);
    colors[ImGuiCol_TabActive]              = ImVec4(0.39f, 0.45f, 0.52f, 1.00f);
    colors[ImGuiCol_TabUnfocused]           = ImVec4(0.07f, 0.10f, 0.15f, 0.97f);
    colors[ImGuiCol_TabUnfocusedActive]     = ImVec4(0.14f, 0.26f, 0.42f, 1.00f);
    colors[ImGuiCol_PlotLines]              = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered]       = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
    colors[ImGuiCol_PlotHistogram]          = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered]   = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    colors[ImGuiCol_TableHeaderBg]          = ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
    colors[ImGuiCol_TableBorderStrong]      = ImVec4(0.31f, 0.31f, 0.35f, 1.00f);
    colors[ImGuiCol_TableBorderLight]       = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);
    colors[ImGuiCol_TableRowBg]             = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_TableRowBgAlt]          = ImVec4(1.00f, 1.00f, 1.00f, 0.9f);
    colors[ImGuiCol_TextSelectedBg]         = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
    colors[ImGuiCol_DragDropTarget]         = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
    colors[ImGuiCol_NavHighlight]           = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_NavWindowingHighlight]  = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    colors[ImGuiCol_NavWindowingDimBg]      = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
    colors[ImGuiCol_ModalWindowDimBg]       = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
}

void ShowUserInterface( UserInterface* ui )
{
    ImVec2 buttonSize = ImVec2( 150, 30 );

    ImGui::Begin( "codencandy demo" );
    ImGui::BeginTabBar( "apps" ); 

    if( ImGui::BeginTabItem("clock") ) 
    {
        if( ImGui::Button( "clock app", buttonSize ) )
        {
            ui->m_clockApp = !ui->m_clockApp;
        }

        ImGui::EndTabItem();
    }

    if( ImGui::BeginTabItem( "christmas" ) )
    {
        if( ImGui::Button( "christmas app", buttonSize ) )
        {
            ui->m_christmasApp = !ui->m_christmasApp;
        }

        ImGui::EndTabItem();
    }

    if( ImGui::BeginTabItem( "shapes" ) )
    {
        if( ImGui::Button( "shape app", buttonSize ) )
        {
            ui->m_shapeApp = !ui->m_shapeApp;
        }

        ImGui::EndTabItem();
    }

    if( ImGui::BeginTabItem( "pong" ) )
    {
        if( ImGui::Button( "pong app", buttonSize ) )
        {
            ui->m_pongApp = !ui->m_pongApp;
        }

        u32 numValues = arrlen( ui->m_values );
        for( u32 i=0; i<numValues; ++i )
        {
            ControlValue* value = ui->m_values[i];
    
            switch( value->m_controlType )
            {
                case INPUT_FIELD: ImGui::InputFloat(  value->m_label, (f32*)value->m_value ); break;
                case SLIDER:      ImGui::SliderFloat( value->m_label, (f32*)value->m_value, 0.0f, 20.0f ); break;
                default: break;
            }
        }
    
        u32 numDbgValues = arrlen( ui->m_debugValues );
        for( u32 i=0; i<numDbgValues; ++i )
        {
            DebugValue* value = ui->m_debugValues[i];
            switch( value->m_controlType )
            {
                case TEXT_BOX: ImGui::InputFloat( value->m_label, &value->m_value ); break;
                default: break;
            }
        }

        ImGui::EndTabItem();
    }

    ImGui::EndTabBar();
    ImGui::End();

    arrfree( ui->m_debugValues );
}

void PlatformAddCtrlValue( void* userInterface, ControlValue*  value )
{
    UserInterface* ui = (UserInterface*)userInterface;
    arrput( ui->m_values, value );
}

void PlatformShowDebugValue( void* userInterface, DebugValue* value )
{
    UserInterface* ui = (UserInterface*)userInterface;
    arrput( ui->m_debugValues, value );
}
