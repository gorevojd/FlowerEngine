
enum tic_tac_cell_value
{
    TicTacCell_None,
    TicTacCell_Cross,
    TicTacCell_Zero,
};

struct tic_tac_quad
{
    v3 CenterPUnit;
    v2 CenterP;
    
    f32 Width;
    f32 VisibleWidth;
    
    rc2 Rect;
    
    u32 CellValue;
};

enum tic_tac_state_of_game
{
    TicTacState_FadeToGame,
    TicTacState_Game,
    TicTacState_GameFadeOut,
    TicTacState_WinAnim,
    TicTacState_WaitForNewPlay,
};

struct tic_tac_state
{
    f32 QuadWidth;
    
    tic_tac_quad Quads[9];
    
    image* Cross;
    image* Zero;
    
    batch_rect_buffer RectBuffer;
    
    u32 CurPlayerCellValue;
    
    v2 WinBeginP;
    v2 WinEndP;
    f32 WinTime;
    f32 BitmapsAndLineTransparency;
    b32 BothLose;
    
    f32 FadeToGameStartTime;
    f32 FadeoutStartTime;
    
    u32 State;
};

inline v2 ReprojectToWindowCoords(v2 Point, 
                                  const m44& ProjMatrix,
                                  f32 WindowWidth,
                                  f32 WindowHeight)
{
    v4 TempP = V4(Point.x, 
                  Point.y,
                  0.0f,
                  1.0f);
    
    TempP = TempP * ProjMatrix;
    TempP /= TempP.w;
    
    v2 PointNDC = TempP.xy;
    v2 Point01 = PointNDC * 0.5f + V2(0.5f);
    
    // TODO(Dima): Maybe add an option to inver Y at this line of code
    Point01.y = 1.0f - Point01.y;
    
    v2 Result = V2(WindowWidth * Point01.x, WindowHeight * Point01.y);
    
    return Result;
}

void ResetAllCellValues(tic_tac_state* State)
{
    for (int CellIndex = 0;
         CellIndex < 9;
         CellIndex++)
    {
        tic_tac_quad* Quad = &State->Quads[CellIndex];
        
        Quad->CellValue = TicTacCell_None;
    }
    
    State->State = TicTacState_FadeToGame;
    State->FadeToGameStartTime = Global_Time->Time;
    State->CurPlayerCellValue = TicTacCell_Cross;
    State->BitmapsAndLineTransparency = 0.0f;
    State->BothLose = false;
}

SCENE_INIT(TicTacToe)
{
    tic_tac_state* State = GetSceneState(tic_tac_state);
    
    InitRectBuffer(&State->RectBuffer, 1000); 
    
    State->QuadWidth = 1.0f;
    
    for (int i = 0; i < 9; i++)
    {
        int QuadX = i % 3;
        int QuadY = i / 3;
        
        tic_tac_quad* Quad = &State->Quads[i];
        
        Quad->CellValue = TicTacCell_None;
        Quad->CenterPUnit = State->QuadWidth * V3(-1 + QuadX, 0, -1 + QuadY);
        Quad->Width = State->QuadWidth;
        Quad->VisibleWidth = State->QuadWidth * 0.95f;
        
        // NOTE(Dima): Reprojecting rect to window coords
        m44 Projection = OrthographicProjectionUnit(Global_RenderCommands->WindowDimensions.Width,
                                                    Global_RenderCommands->WindowDimensions.Height);
        
        rc2 RectUnit = RectCenterDim(V2(Quad->CenterPUnit.x, 
                                        Quad->CenterPUnit.z), 
                                     V2(Quad->VisibleWidth));
        v2 NewMin = ReprojectToWindowCoords(V2(RectUnit.Min.x,
                                               RectUnit.Max.y),
                                            Projection, 
                                            Global_RenderCommands->WindowDimensions.Width,
                                            Global_RenderCommands->WindowDimensions.Height);
        v2 NewMax = ReprojectToWindowCoords(V2(RectUnit.Max.x,
                                               RectUnit.Min.y),
                                            Projection, 
                                            Global_RenderCommands->WindowDimensions.Width,
                                            Global_RenderCommands->WindowDimensions.Height);
        
        Quad->Rect = RectMinMax(NewMin, NewMax);
        Quad->CenterP = GetCenter(Quad->Rect);
    }
    
    ResetAllCellValues(State);
    
    loading_params Params = LoadingParams_Image();
    Params.Image.Align = V2(0.5f);
    State->Zero = LoadImageFile("../Data/ForGame/TicTacToe/zero.png", Params);
    State->Cross = LoadImageFile("../Data/ForGame/TicTacToe/cross.png", Params);
}

inline b32 TicTac_CoordIsOnDiagonal(int RowIndex, int ColIndex)
{
    b32 Result = (RowIndex == ColIndex) || (2 - RowIndex == ColIndex);
    
    return Result;
}

b32 CheckIfPlayerWin(tic_tac_state* State, int CellIndex, 
                     int* FirstWinCellIndex, 
                     int* LastWinCellIndex)
{
    int Row = CellIndex / 3;
    int Col = CellIndex % 3;
    
    
    // NOTE(Dima): Checking if row was completed
    b32 RowIsComplete = true;
    int WinRowIndex = Row;
    for (int ColIndex = 0;
         ColIndex < 3;
         ColIndex++)
    {
        tic_tac_quad* CurQuad = &State->Quads[Row * 3 + ColIndex];
        
        if (CurQuad->CellValue != State->CurPlayerCellValue)
        {
            RowIsComplete = false;
            break;
        }
    }
    
    // NOTE(Dima): Checking if column was completed
    b32 ColIsComplete = true;
    int WinColIndex = Col;
    for (int RowIndex = 0;
         RowIndex < 3;
         RowIndex++)
    {
        tic_tac_quad* CurQuad = &State->Quads[RowIndex * 3 + Col];
        
        if (CurQuad->CellValue != State->CurPlayerCellValue)
        {
            ColIsComplete = false;
            break;
        }
    }
    
    // NOTE(Dima): Checking if diagonal is complete
    b32 DiagonalIsComplete = true;
    int WinDiagonal = -1;
    if(TicTac_CoordIsOnDiagonal(Row, Col))
    {
        // NOTE(Dima): Check first diagonal
        b32 FirstDiagonalIsComplete = true;
        for (int Index = 0;
             Index < 3;
             Index++)
        {
            tic_tac_quad* CurQuad = &State->Quads[Index * 3 + Index];
            
            if (CurQuad->CellValue != State->CurPlayerCellValue)
            {
                FirstDiagonalIsComplete = false;
                break;
            }
        }
        
        if(FirstDiagonalIsComplete)
        {
            WinDiagonal = 0;
        }
        
        // NOTE(Dima): Check second diagonal
        b32 SecondDiagonalIsComplete = true;
        for (int Index = 0;
             Index < 3;
             Index++)
        {
            tic_tac_quad* CurQuad = &State->Quads[Index * 3 + (2 - Index)];
            
            if (CurQuad->CellValue != State->CurPlayerCellValue)
            {
                SecondDiagonalIsComplete = false;
                break;
            }
        }
        
        if(SecondDiagonalIsComplete)
        {
            WinDiagonal = 1;
        }
        
        DiagonalIsComplete = FirstDiagonalIsComplete || SecondDiagonalIsComplete;
    }
    else
    {
        DiagonalIsComplete = false;
    }
    
    b32 Result = false;
    if (RowIsComplete)
    {
        Result = true;
        
        *FirstWinCellIndex = WinRowIndex * 3;
        *LastWinCellIndex = WinRowIndex * 3 + 2;
    }
    else if(ColIsComplete)
    {
        Result = true;
        
        *FirstWinCellIndex = WinColIndex;
        *LastWinCellIndex = 2 * 3 + WinColIndex;
    }
    else if(DiagonalIsComplete)
    {
        Result = true;
        
        if(WinDiagonal == 0)
        {
            *FirstWinCellIndex = 0;
            *LastWinCellIndex = 8;
        }
        else if (WinDiagonal == 1)
        {
            *FirstWinCellIndex = 6;
            *LastWinCellIndex = 2;
        }
    }
    
    return Result;
}

b32 CheckIfAllLoose(tic_tac_state* State)
{
    b32 AllAreFilled = true;
    
    for (int CellIndex = 0;
         CellIndex < 9;
         CellIndex++)
    {
        tic_tac_quad* Quad = &State->Quads[CellIndex];
        
        if (Quad->CellValue == TicTacCell_None)
        {
            AllAreFilled = false;
            break;
        }
    }
    
    return AllAreFilled;
}

b32 TicTac_ButtonWithText(tic_tac_state* State, char* ButtonText, f32 TextHeight = 200.0f)
{
    rc2 ButtonRect = RectCenterDim(UVToScreenPoint(0.5f, 0.5f),
                                   UVToScreenPoint(0.65f, 0.25f));
    
    v4 ButtonColor = V4(0.09f, 0.08f, 0.4f, 1.0f);
    v4 TextColor = V4(0.85f, 0.4f, 0.0f, 1.0f);
    
    b32 Result = false;
    if (MouseInRect(ButtonRect))
    {
        ButtonColor = V4(0.11f, 0.11f, 0.45f, 1.0f);
        TextColor = V4(1.0f, 0.6f, 0.0f, 1.0f);
        
        if (GetKeyDown(KeyMouse_Left))
        {
            Result = true;
        }
    }
    
    asset_id DimboFontID = GetAssetID("Font_Dimbo");
    font* Dimbo = G_GetAssetDataByID(DimboFontID, font);
    
#if 1        
    PrintTextWithFontAligned(Dimbo, 
                             ButtonText,
                             UVToScreenPoint(0.5f, 0.5f),
                             TextHeight,
                             TextAlign_Center,
                             TextAlign_Center,
                             TextColor);
#else
    PrintTextWithFont(Dimbo,
                      ButtonText,
                      UVToScreenPoint(0.2f, 0.5f),
                      200.0f,
                      TextColor);
#endif
    
    PushRect(&State->RectBuffer,
             ButtonRect,
             ButtonColor);
    PushRectOutline(Global_RenderCommands->DEBUG_Rects2D_Window,
                    ButtonRect, 
                    10.0f);
    
    return Result;
}

void TicTac2D(tic_tac_state* State)
{
    ResetRectBuffer(&State->RectBuffer);
    
    //PushClear(V3(0.23, 0.44f, 0.01f));
    PushClear(V3(0.7f, 0.75f, 0.95f));
    
    if(GetKeyDown(Key_R))
    {
        ResetAllCellValues(State);
    }
    
    // NOTE(Dima): Updating and Rendering cells
    for (int CellIndex = 0;
         CellIndex < 9;
         CellIndex++)
    {
        tic_tac_quad* Quad = &State->Quads[CellIndex];
        
        int Row = CellIndex / 3;
        int Col = CellIndex % 3;
        
        v4 QuadColor = ColorWhite();
        if(State->State == TicTacState_Game)
        {
            if (MouseInRect(Quad->Rect))
            {
                if (GetKeyDown(KeyMouse_Left))
                {
                    if (Quad->CellValue == TicTacCell_None)
                    {
                        Quad->CellValue = State->CurPlayerCellValue;
                        
                        // NOTE(Dima): Check for win
                        int FirstCellIndex;
                        int LastCellIndex;
                        b32 Win = CheckIfPlayerWin(State, 
                                                   CellIndex, 
                                                   &FirstCellIndex, 
                                                   &LastCellIndex);
                        
                        // NOTE(Dima): Change player if previous player didn't win
                        if (!Win)
                        {
                            if (State->CurPlayerCellValue == TicTacCell_Cross)
                            {
                                State->CurPlayerCellValue = TicTacCell_Zero;
                            }
                            else if(State->CurPlayerCellValue == TicTacCell_Zero)
                            {
                                State->CurPlayerCellValue = TicTacCell_Cross;
                            }
                            
                            b32 AllLoose = CheckIfAllLoose(State);
                            
                            if (AllLoose)
                            {
                                State->State = TicTacState_GameFadeOut;
                                State->FadeoutStartTime = Global_Time->Time;
                                State->BitmapsAndLineTransparency = 1.0f;
                                State->BothLose = true;
                            }
                        }
                        else
                        {
                            State->State = TicTacState_WinAnim;
                            
                            tic_tac_quad* FirstQuad = &State->Quads[FirstCellIndex];
                            tic_tac_quad* LastQuad = &State->Quads[LastCellIndex];
                            
                            v2 WinBeginP = FirstQuad->CenterP;
                            v2 WinEndP = LastQuad->CenterP;
                            
                            v2 LineDir = WinEndP - WinBeginP;
                            
                            v2 AdditionalOffset = Normalize(LineDir) * 200.0f;
                            
                            State->WinBeginP = WinBeginP - AdditionalOffset;
                            State->WinEndP = WinEndP + AdditionalOffset;
                            State->WinTime = Global_Time->Time;
                        }
                    }
                }
                
                QuadColor = V4(1.0f, 1.0f, 0.8f, 1.0f);
            }
        }
        
        PushRect(&State->RectBuffer,
                 Quad->Rect,
                 V4(QuadColor.rgb, 1.0f - State->BitmapsAndLineTransparency));
        
        PushRectOutline(&State->RectBuffer,
                        Quad->Rect,
                        5.0f,
                        V4(ColorBlack().rgb, 1.0f - State->BitmapsAndLineTransparency));
    }
    
    
    if(State->State == TicTacState_WinAnim)
    {
#define TIC_TAC_TIME_FOR_WIN_ANIM 1.0f
        f32 AnimT = (Global_Time->Time - State->WinTime) / TIC_TAC_TIME_FOR_WIN_ANIM;
        
        if (AnimT > 2.0f)
        {
            State->State = TicTacState_GameFadeOut;
            State->FadeoutStartTime = Global_Time->Time;
        }
        else
        {
            v2 LineDir = State->WinEndP - State->WinBeginP;
            
            if(!State->BothLose)
            {
                PushLine2D(Global_RenderCommands->DEBUG_Rects2D_Window, 
                           State->WinBeginP, 
                           State->WinBeginP + LineDir * FlowerMin(AnimT, 1.0f),
                           40,
                           V4(ColorRed().rgb, 1.0f - State->BitmapsAndLineTransparency));
            }
        }
    }
    else if (State->State == TicTacState_GameFadeOut)
    {
#define TIC_TAC_TIME_FOR_FADEOUT_ANIM 1.0f
        f32 AnimT = (Global_Time->Time - State->FadeoutStartTime) / TIC_TAC_TIME_FOR_FADEOUT_ANIM;
        
        if(AnimT > 2.0f)
        {
            State->State = TicTacState_WaitForNewPlay;
        }
        else
        {
            State->BitmapsAndLineTransparency = FlowerMin(AnimT, 1.0f);
            
            if(!State->BothLose)
            {
                PushLine2D(Global_RenderCommands->DEBUG_Rects2D_Window, 
                           State->WinBeginP, 
                           State->WinEndP,
                           40,
                           V4(ColorRed().rgb, 1.0f - State->BitmapsAndLineTransparency));
            }
        }
    }
    else if(State->State == TicTacState_WaitForNewPlay)
    {
        char* ButtonText = "LET'S TRY AGAIN !!!";
        f32 TextSize = 200.0f;
        if (State->BothLose)
        {
            ButtonText = "DRAW.. TO BATTLE AGAIN !!!";
            TextSize = 150.0f;
        }
        
        if(TicTac_ButtonWithText(State, ButtonText, TextSize))
        {
            ResetAllCellValues(State);
        }
    }
    else if(State->State == TicTacState_FadeToGame)
    {
#define TIC_TAC_FADE_TO_GAME_TIME 1.0f
        f32 FadeT = (Global_Time->Time - State->FadeToGameStartTime) / TIC_TAC_FADE_TO_GAME_TIME;
        
        if (FadeT > 1.0f)
        {
            State->State = TicTacState_Game;
        }
        else
        {
            State->BitmapsAndLineTransparency = 1.0f - FadeT;
        }
    }
    
    RectBufferSetMatrices(&State->RectBuffer,
                          IdentityMatrix4(),
                          OrthographicProjectionWindow(Global_RenderCommands->WindowDimensions.Width,
                                                       Global_RenderCommands->WindowDimensions.Height));
    
    PushRectBuffer(&State->RectBuffer);
    
    
    // NOTE(Dima): Rendering images
    for(int CellIndex = 0;
        CellIndex < 9;
        CellIndex++)
    {
        tic_tac_quad* Quad = &State->Quads[CellIndex];
        
        image* CellImage = 0;
        switch(Quad->CellValue)
        {
            case TicTacCell_Zero:
            {
                CellImage = State->Zero;
            }break;
            
            case TicTacCell_Cross:
            {
                CellImage = State->Cross;
            }break;
        }
        
        if (CellImage)
        {
            PushImage(CellImage, 
                      Quad->CenterP,
                      GetHeight(Quad->Rect) * 0.95f,
                      CellImage->Align,
                      V4(ColorBlack().rgb, 1.0f - State->BitmapsAndLineTransparency));
        }
    }
}

SCENE_UPDATE(TicTacToe)
{
    tic_tac_state* State = GetSceneState(tic_tac_state);
    
    TicTac2D(State);
}