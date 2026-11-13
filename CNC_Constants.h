#ifndef CNC_CONSTANTS_H
#define CNC_CONSTANTS_H

#include "CNC_Types.h"

#define CNC_WINDOW_WIDTH  1000
#define CNC_WINDOW_HEIGHT  500

static color WHITE      = rgba( 1.0f, 1.0f, 1.0f, 1.0f );
static color RED        = rgba( 1.0f, 0.0f, 0.0f, 1.0f );
static color GREY       = rgba( 0.5f, 0.5f, 0.5f, 1.0f );
static color LIGHT_GREY = rgba( 0.3f, 0.3f, 0.3f, 1.0f );
static color LIGHT_BLUE = rgba( 0.3f, 0.7f, 0.6f, 1.0f );
static color YELLOW     = rgba( 0.9f, 0.8f, 0.0f, 1.0f );
static color GREEN      = rgba( 0.0f, 1.0f, 0.0f, 1.0f );

static color WHITE_50   = rgba( 1.0f, 1.0f, 1.0f, 0.5f );
static color WHITE_20   = rgba( 1.0f, 1.0f, 1.0f, 0.2f );

// Sanzo Wada
struct Sanzo_Wada_296
{
    color sulpurYellow;
    color ochraceousSalmon;
    color paleRawUmber;
    color slateColor;
};

static Sanzo_Wada_296 sanzo1 = { rgb(245, 236, 194),
                                 rgb(216, 163, 123),
                                 rgb(113, 80, 47),
                                 rgb(52, 69, 76) };

#endif//CNC_CONSTANTS_H
