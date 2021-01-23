#ifndef FLOWER_UI_H
#define FLOWER_UI_H

enum text_align_type
{
    TextAlign_Left,
    TextAlign_Right,
    
    TextAlign_Top,
    TextAlign_Bottom,
    
    TextAlign_Center,
};


// NOTE(Dima): Bounding boxes
struct ui_element_bb
{
    // NOTE(Dima): This is for example for button Area
    rc2 Active;
    
    // NOTE(Dima): This is for total element rect (like button and it's text, line,  etc)
    rc2 Total;
};

// NOTE(Dima): Layouts
struct ui_layout
{
    const char* Name;
    v2 At;
    b32 JustStarted;
    b32 StayOnSameLine;
    ui_element_bb LastBB;
    
    ui_layout* Next;
};

struct ui_params
{
    // NOTE(Dima): Ui params
    render_commands* Commands;
    font* Font;
    f32 Scale;
    
    f32 ScaleStack[64];
    int ScaleStackIndex;
    
    window_dimensions* WindowDims;
};

struct ui_graph
{
    v2 Origin;
    v2 UnitMin;
    v2 UnitMax;
    f32 PixelsPerUnit;
    b32 ShowIntLabels;
    
    v2 AxisX;
    v2 AxisY;
    f32 AxisThickness;
    
    v4 AxisXColor;
    v4 AxisYColor;
};

struct ui_slider_graph
{
    rc2 Rect;
    v2 Dim;
    
    f32 MinValue;
    f32 MaxValue;
    
    b32 ValueScreenPCalculated;
    v2 ValueScreenP;
};

// NOTE(Dima): Colors
enum ui_color_type
{
    UIColor_Text,
    UIColor_TextActive,
    
    UIColor_Count,
};

struct ui_colors
{
    v4 Colors[UIColor_Count];
};

struct ui_state
{
    memory_arena* Arena;
    
    ui_layout* CurrentLayout;
    
    ui_graph Graph;
    ui_graph* CurrentGraph;
    
    ui_params Params;
    
    // NOTE(Dima): Immediate mode stuff
    ui_colors Colors;
    
    char StringFormatBuffer[2048];
    
    ui_layout* FirstLayout;
};

#endif //FLOWER_UI_H
