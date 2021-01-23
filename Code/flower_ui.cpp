INTERNAL_FUNCTION inline ui_params* UIGetParams()
{
    ui_params* Params = &Global_UI->Params;
    
    return(Params);
}

INTERNAL_FUNCTION inline f32 GetLineBase()
{
    ui_params* Params = UIGetParams();
    
    f32 Result = Params->Font->Ascent * Params->Scale;
    
    return(Result);
}

INTERNAL_FUNCTION inline f32 GetLineAdvance()
{
    ui_params* Params = UIGetParams();
    
    f32 Result = Params->Font->LineAdvance * Params->Scale;
    
    return(Result);
}


INTERNAL_FUNCTION inline f32 GetKerning(font* Font, u32 CodepointFirst, u32 CodepointSecond)
{
    int GlyphIndex = CodepointFirst - ' ';
    int NextGlyphIndex = -1;
    
    if(CodepointSecond != 0)
    {
        NextGlyphIndex = CodepointSecond - ' ';
    }
    
    f32 Kerning = 0.0f;
    if(NextGlyphIndex != -1)
    {
        Kerning = Font->KerningPairs[GlyphIndex * Font->GlyphCount + NextGlyphIndex];
        
        if(Kerning > 0.0001f)
        {
            int a = 1;
        }
    }
    
    return(Kerning);
}

enum print_text_flags
{
    PrintText_3D = (1 << 0),
};

INTERNAL_FUNCTION rc2 PrintText_(font* Font, 
                                 char* Text, 
                                 v3 Left, v3 Up,
                                 v3 P, 
                                 v2 Offset, 
                                 u32 Flags,
                                 f32 Scale = 1.0f, 
                                 b32 IsGetSizePass = false,
                                 v4 C = V4(1.0f, 1.0f, 1.0f, 1.0f))
{
    char* At = Text;
    
    v2 AtP = V2(P.x, P.y);
    
    rect_buffer* Buffer = &Global_RenderCommands->Rects2D;
    
    int IndexToTransformMatrix = Buffer->IdentityMatrixIndex;
    
    b32 Is3D = (Flags & PrintText_3D) != 0;
    if(Is3D)
    {
        Buffer = &Global_RenderCommands->Rects3D;
        AtP = {};
        
        Up = -Up;
        Left = -Left;
        
        Scale *= 1.0f / (f32)Font->Size;
        
        m44 TextTransform = Matrix4FromRows(V4(Left, 0.0f), 
                                            V4(Up, 0.0f), 
                                            V4(NOZ(Cross(Left, Up)), 0.0f),
                                            V4(P.x, P.y, P.z, 1.0f));
        IndexToTransformMatrix = PushRectTransform(Buffer, &TextTransform);
    }
    
    rc2 Bounds;
    Bounds.Min.x = AtP.x;
    Bounds.Min.y = AtP.y - Font->Ascent * Scale;
    Bounds.Max.y = AtP.y - Font->Descent * Scale;
    
    image* GlyphImages = Font->GlyphImages;
    
    while(*At)
    {
        int GlyphIndex = *At - ' ';
        
        glyph* Glyph = &Font->Glyphs[GlyphIndex];
        
        if(!IsGetSizePass)
        {
            image* Image = &GlyphImages[Glyph->ImageIndex];
            
            f32 TargetHeight = (f32)Image->Height * Scale;
            
            v2 ImageP = AtP + V2(Glyph->XOffset, Glyph->YOffset) * Scale + Offset;
            PushGlyph(Buffer, Glyph, ImageP, 
                      TargetHeight, 
                      IndexToTransformMatrix, C);
        }
        
        f32 Kerning = GetKerning(Font, *At, *(At + 1)); 
        
        //AtP.x += Glyph->Advance * Scale;
        AtP.x += (Glyph->Advance + Kerning) * Scale;
        
        At++;
    }
    
    Bounds.Max.x = AtP.x;
    
    return(Bounds);
}

INTERNAL_FUNCTION rc2 PrintText(font* Font, 
                                char* Text, 
                                v2 P, 
                                f32 Scale = 1.0f, 
                                v4 C = V4(1.0f, 1.0f, 1.0f, 1.0f), 
                                b32 WithShadow = true)
{
    if(WithShadow)
    {
        PrintText_(Font, Text, 
                   V3_Left(), V3_Up(), 
                   V3(P, 0.0f), V2(1.0f, 1.0f), 
                   0, Scale, false, 
                   V4(0.0f, 0.0f, 0.0f, 1.0f));
    }
    rc2 Result = PrintText_( Font, Text, 
                            V3_Left(), V3_Up(), 
                            V3(P, 0.0f), V2(0.0f, 0.0f), 
                            0, Scale, false,  C);
    
    return(Result);
}

INTERNAL_FUNCTION void PrintText3D(font* Font, 
                                   char* Text,
                                   v3 Left, v3 Up,
                                   v3 P, 
                                   v4 C = V4(1.0f, 1.0f, 1.0f, 1.0f), 
                                   f32 Scale = 1.0f, 
                                   b32 WithShadow = true)
{
    
#if 0    
    if(WithShadow)
    {
        PrintText_(Font, Text, 
                   Left, Up, P, 
                   V2(1.0f, 1.0f), 
                   PrintText_3D, 
                   Scale, 
                   V4(0.0f, 0.0f, 0.0f, 1.0f));
    }
#endif
    
    PrintText_(Font, Text, 
               Left, Up, P, 
               V2(0.0f, 0.0f), 
               PrintText_3D, 
               Scale, 
               false, C);
}

INTERNAL_FUNCTION inline rc2 GetTextRect(char* Text, v2 P)
{
    ui_params* Params = UIGetParams();
    
    font* Font = Params->Font;
    
    rc2 Result = PrintText_(Font, 
                            Text, 
                            V3_Left(), V3_Up(), 
                            V3(P.x, P.y, 0.0f), 
                            V2(0.0f, 0.0f), 
                            0, Params->Scale, true);
    
    return(Result);
}

INTERNAL_FUNCTION inline v2 GetTextSize(char* Text)
{
    rc2 TextRect = GetTextRect(Text, V2(0.0f, 0.0f));
    
    v2 Result = GetDim(TextRect);
    
    return(Result);
}

INTERNAL_FUNCTION inline f32 GetPrintHorizontalPosition(f32 Min, f32 Max, 
                                                        f32 TextDimX, u32 Align,
                                                        f32 TextScale)
{
    f32 Result = Min;
    
    switch(Align)
    {
        case TextAlign_Right:
        {
            Result = Max - TextDimX;
        }break;
        
        case TextAlign_Center:
        {
            Result = Min + (Max - Min) * 0.5f - TextDimX * 0.5f;
        }break;
        
        case TextAlign_Left:
        {
            Result = Min;
        }break;
        
        default:
        {
            InvalidCodePath;
        }break;
    }
    
    return(Result);
}

INTERNAL_FUNCTION inline f32 GetPrintVerticalPosition(f32 Min, f32 Max, 
                                                      f32 TextDimY, u32 Align, 
                                                      f32 TextScale)
{
    f32 Result = Min;
    
    font* Font = Global_UI->Params.Font;
    
    switch(Align)
    {
        case TextAlign_Top:
        {
            Result = Min + Font->Ascent * TextScale;
        }break;
        
        case TextAlign_Bottom:
        {
            Result = Max + Font->Descent * TextScale;
        }break;
        
        case TextAlign_Center:
        {
            f32 DimY = (Font->Ascent - Font->Descent) * TextScale;
            
            f32 CenterY = Min + (Max - Min) * 0.5f;
            
            Result = CenterY - DimY * 0.5f + Font->Ascent * TextScale;
        }break;
        
        default:
        {
            InvalidCodePath;
        }break;
    }
    
    return(Result);
}

INTERNAL_FUNCTION inline v2 GetPrintPositionInRect(rc2 Rect,
                                                   v2 TextDim,
                                                   u32 AlignX,
                                                   u32 AlignY)
{
    v2 Result;
    
    ui_params* Params = UIGetParams();
    
    Result.x = GetPrintHorizontalPosition(Rect.Min.x, Rect.Max.x,
                                          TextDim.x, AlignX, Params->Scale);
    Result.y = GetPrintVerticalPosition(Rect.Min.y, Rect.Max.y,
                                        TextDim.y, AlignY, Params->Scale);
    
    return(Result);
}

INTERNAL_FUNCTION rc2 PrintText(char* Text,
                                v2 P,
                                v4 C = V4(1.0f, 1.0f, 1.0f, 1.0f),
                                b32 WithShadow = true)
{
    ui_params* Params = UIGetParams();
    font* Font = Params->Font;
    
    rc2 Result = PrintText(Font,
                           Text,
                           P,
                           Params->Scale, 
                           C,
                           WithShadow);
    
    return(Result);
}

INTERNAL_FUNCTION rc2 PrintTextAligned(char* Text, 
                                       rc2 Rect,
                                       u32 AlignX = TextAlign_Center,
                                       u32 AlignY = TextAlign_Center,
                                       v4 C = V4(1.0f, 1.0f, 1.0f, 1.0f),
                                       b32 WithShadow = true)
{
    v2 TextSize = GetTextSize(Text);
    
    v2 PrintP = GetPrintPositionInRect(Rect, TextSize,
                                       AlignX, AlignY);
    
    rc2 Result = PrintText(Text, PrintP, C, WithShadow);
    
    return(Result);
}


INTERNAL_FUNCTION rc2 PrintTextAligned(char* Text, 
                                       v2 Point,
                                       u32 AlignX = TextAlign_Center,
                                       u32 AlignY = TextAlign_Center,
                                       v4 C = V4(1.0f, 1.0f, 1.0f, 1.0f),
                                       b32 WithShadow = true)
{
    rc2 Result = PrintTextAligned(Text, RectMinMax(Point, Point), 
                                  AlignX, AlignY,
                                  C, WithShadow);
    
    return(Result);
}

// NOTE(Dima): Get pixel point on screen from top left corner
INTERNAL_FUNCTION inline v2 UVToScreenPoint(float x, float y)
{
    v2 Result;
    
    Result.x = x * Global_UI->Params.WindowDims->Width;
    Result.y = y * Global_UI->Params.WindowDims->Height;
    
    return(Result);
}


INTERNAL_FUNCTION inline v2 UVToScreenPoint(v2 UV)
{
    v2 Result = UVToScreenPoint(UV.x, UV.y);
    
    return(Result);
}

INTERNAL_FUNCTION inline v4 UIGetColor(u32 Color)
{
    v4 Result = Global_UI->Colors.Colors[Color];
    
    return(Result);
}

INTERNAL_FUNCTION void InitUIColors()
{
    ui_colors* Colors_ =  &Global_UI->Colors;
    
    v4* Colors = Colors_->Colors;
    
    Colors[UIColor_Text] = ColorWhite();
    Colors[UIColor_TextActive] = ColorYellow();
}

INTERNAL_FUNCTION void InitUI(memory_arena* Arena)
{
    Global_UI = PushStruct(Arena, ui_state);
    
    Global_UI->Arena = Arena;
    
    InitUIColors();
    
    Global_UI->FirstLayout = 0;
}

// NOTE(Dima): UI parameters
INTERNAL_FUNCTION inline void UISetParams(ui_params Params)
{
    Global_UI->Params = Params;
}

inline void UIPushScale(f32 Scale)
{
    ui_params* Params = UIGetParams();
    
    Assert(Params->ScaleStackIndex < ArrayCount(Params->ScaleStack));
    
    Params->ScaleStack[Params->ScaleStackIndex++] = Scale;
    Params->Scale = Scale;
}

inline void UIPopScale()
{
    ui_params* Params = UIGetParams();
    
    Assert(Params->ScaleStackIndex > 0);
    
    Params->ScaleStack[Params->ScaleStackIndex] = 0.0f;
    Params->Scale = Params->ScaleStack[--Params->ScaleStackIndex];
}

INTERNAL_FUNCTION void UIBeginFrame()
{
    window_dimensions* WndDims = &Global_RenderCommands->WindowDimensions;
    
    ui_params ParamsUI = {};
    ParamsUI.Commands = Global_RenderCommands;
    ParamsUI.Font = &Global_Assets->LiberationMono;
    ParamsUI.WindowDims = WndDims;
    
    UISetParams(ParamsUI);
    
    // NOTE(Dima): Init font scale stack
    ParamsUI.ScaleStackIndex = 0;
    UIPushScale(1.0f);
    
    // NOTE(Dima): Initializing layouts
    ui_layout* LayoutAt = Global_UI->FirstLayout;
    
    while(LayoutAt)
    {
        LayoutAt->At = V2(0.0f);
        LayoutAt->JustStarted = true;
        LayoutAt->StayOnSameLine = false;
        
        LayoutAt = LayoutAt->Next;
    }
}

// NOTE(Dima): Layouts stuff
INTERNAL_FUNCTION b32 BeginLayout(const char* Name)
{
    ui_layout* Found = 0;
    
    // NOTE(Dima): FInding layout
    if(Global_UI->FirstLayout)
    {
        ui_layout* At = Global_UI->FirstLayout;
        
        while(At != 0)
        {
            if(StringsAreEqual((char*)Name, (char*)At->Name))
            {
                Found = At;
                break;
            }
            
            At = At->Next;
        }
    }
    
    // NOTE(Dima): If not found - then allocate
    if(!Found)
    {
        Found = PushStruct(Global_UI->Arena, ui_layout);
        
        Found->Name = Name;
        Found->Next = Global_UI->FirstLayout;
        
        Global_UI->FirstLayout = Found;
    }
    
    b32 Result = true;
    
    Assert(Global_UI->CurrentLayout == 0);
    Global_UI->CurrentLayout = Found;
    
    return(Result);
}

INTERNAL_FUNCTION inline ui_layout* GetCurrentLayout()
{
    ui_layout* Layout = Global_UI->CurrentLayout;
    
    return(Layout);
}

INTERNAL_FUNCTION void EndLayout()
{
    Assert(Global_UI->CurrentLayout);
    
    Global_UI->CurrentLayout = 0;
}

INTERNAL_FUNCTION void SameLine()
{
    ui_layout* Layout = GetCurrentLayout();
    
    Layout->StayOnSameLine = true;
}

INTERNAL_FUNCTION inline void PreAdvance()
{
    ui_layout* Layout = GetCurrentLayout();
    
    if(Layout->JustStarted)
    {
        Layout->JustStarted = false;
        
        Layout->At.y += GetLineBase();
    }
    else
    {
        f32 VerticalAdvance = GetLineAdvance();
        f32 HorizontalP = 0.0f;
        
        if(Layout->StayOnSameLine)
        {
            Layout->StayOnSameLine = false;
            
            VerticalAdvance = 0.0f;
            HorizontalP = Layout->LastBB.Total.Max.x + GetLineBase();
        }
        
        Layout->At.y += VerticalAdvance;
        Layout->At.x = HorizontalP;
    }
}

INTERNAL_FUNCTION inline void DescribeElement(rc2 Active, rc2 Total)
{
    ui_layout* Layout = GetCurrentLayout();
    
    Layout->LastBB.Active = Active;
    Layout->LastBB.Total = Total;
}

// NOTE(Dima): Elements
INTERNAL_FUNCTION void ShowTextUnformatted(char* Text)
{
    ui_params* Params = UIGetParams();
    
    PreAdvance();
    
    rc2 Bounds = PrintText(Params->Font, 
                           Text, 
                           Global_UI->CurrentLayout->At, 
                           Params->Scale, 
                           UIGetColor(UIColor_Text));
    
    DescribeElement(Bounds, Bounds);
}

INTERNAL_FUNCTION int ShowText(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    int Result = stbsp_vsnprintf(Global_UI->StringFormatBuffer, 
                                 ArrayCount(Global_UI->StringFormatBuffer), 
                                 fmt, args);
    va_end(args);
    
    ShowTextUnformatted(Global_UI->StringFormatBuffer);
    
    return(Result);
}

INTERNAL_FUNCTION b32 Button(const char* Name)
{
    ui_params* Params = UIGetParams();
    
    PreAdvance();
    
    v2 TextPrintP = Global_UI->CurrentLayout->At;
    rc2 Bounds = GetTextRect((char*)Name, TextPrintP);
    
    b32 Pressed = false;
    v4 TextC = UIGetColor(UIColor_Text);
    if(MouseInRect(Bounds))
    {
        TextC = UIGetColor(UIColor_TextActive);
        
        if(GetKeyDown(KeyMouse_Left))
        {
            Pressed = true;
        }
    }
    
    PushRect(Bounds, ColorRed());
    
    PrintText((char*)Name, 
              TextPrintP, 
              TextC);
    
    DescribeElement(Bounds, Bounds);
    
    return(Pressed);
}
