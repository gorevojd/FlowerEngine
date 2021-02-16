INTERNAL_FUNCTION void AddCommandToCube(rubiks_cube* Cube,
                                        int Axis,
                                        int FirstFaceIndex,
                                        int LastFaceIndex,
                                        int IsClockwise)
{
    rubiks_command* NewCommand = &Cube->Commands[Cube->AddIndex];
    
    NewCommand->Axis = Axis;
    NewCommand->FirstFaceIndex = FirstFaceIndex;
    NewCommand->LastFaceIndex = LastFaceIndex;
    NewCommand->IsClockwise = IsClockwise;
    NewCommand->Type = RubiksCommand_Rotation;
    
    Cube->AddIndex = (Cube->AddIndex + 1) % Cube->CommandsCount;
    
    Assert(Cube->AddIndex != Cube->DoIndex);
}

INTERNAL_FUNCTION void AddCommandToCube(rubiks_cube* Cube,
                                        int Axis,
                                        int FaceIndex,
                                        int IsClockwise)
{
    AddCommandToCube(Cube,
                     Axis,
                     FaceIndex,
                     FaceIndex,
                     IsClockwise);
}

INTERNAL_FUNCTION void AddFinishStateCommand(rubiks_cube* Cube,
                                             u8 FinalState)
{
    rubiks_command* NewCommand = &Cube->Commands[Cube->AddIndex];
    
    NewCommand->Type = RubiksCommand_ChangeState;
    NewCommand->FinalState = FinalState;
    Cube->ExecutingSolvingNow = true;
    
    Cube->AddIndex = (Cube->AddIndex + 1) % Cube->CommandsCount;
    Assert(Cube->AddIndex != Cube->DoIndex);
}

INTERNAL_FUNCTION inline b32 CanStartSolvingState(rubiks_cube* Cube, u32 SolvingState)
{
    b32 Result = !Cube->ExecutingSolvingNow && (SolvingState == Cube->SolvingState);
    
    return(Result);
}

INTERNAL_FUNCTION inline void ChangeSolvingState(rubiks_cube* Cube, u32 TargetState)
{
    Cube->SolvingState = TargetState;
    Cube->ExecutingSolvingNow = false;
}

// NOTE(Dima): Standard commands like R, L, U, D, F, B
INTERNAL_FUNCTION inline void RubCom_R(rubiks_cube* Cube, 
                                       b32 CounterClockwise = false)
{
    AddCommandToCube(Cube, RubiksAxis_X, 0, CounterClockwise);
}

INTERNAL_FUNCTION inline void RubCom_L(rubiks_cube* Cube,
                                       b32 CounterClockwise = false)
{
    AddCommandToCube(Cube, RubiksAxis_X, Cube->Dim - 1, !CounterClockwise);
}

INTERNAL_FUNCTION inline void RubCom_U(rubiks_cube* Cube,
                                       b32 CounterClockwise = false)
{
    AddCommandToCube(Cube, RubiksAxis_Y, Cube->Dim - 1, !CounterClockwise);
}

INTERNAL_FUNCTION inline void RubCom_D(rubiks_cube* Cube, 
                                       b32 CounterClockwise = false)
{
    AddCommandToCube(Cube, RubiksAxis_Y, 0, CounterClockwise);
}

INTERNAL_FUNCTION inline void RubCom_F(rubiks_cube* Cube,
                                       b32 CounterClockwise = false)
{
    AddCommandToCube(Cube, RubiksAxis_Z, 0, CounterClockwise);
}

INTERNAL_FUNCTION inline void RubCom_B(rubiks_cube* Cube,
                                       b32 CounterClockwise = false)
{
    AddCommandToCube(Cube, RubiksAxis_Z, Cube->Dim - 1, !CounterClockwise);
}

// NOTE(Dima): M, E, S
/*

M - Center on X axis. Rotation same as from L
E - Center on Y axis. Rotation same as from D
S - Center on Z axis. Rotation same as from F

*/

INTERNAL_FUNCTION inline void RubCom_M(rubiks_cube* Cube, 
                                       b32 CounterClockwise = false)
{
    AddCommandToCube(Cube, RubiksAxis_X, 
                     1, Cube->Dim - 2,
                     !CounterClockwise);
}

INTERNAL_FUNCTION inline void RubCom_E(rubiks_cube* Cube,
                                       b32 CounterClockwise = false)
{
    AddCommandToCube(Cube, RubiksAxis_Y, 
                     1, Cube->Dim - 2, 
                     CounterClockwise);
}

INTERNAL_FUNCTION inline void RubCom_S(rubiks_cube* Cube,
                                       b32 CounterClockwise = false)
{
    AddCommandToCube(Cube, RubiksAxis_Z,
                     1, Cube->Dim - 2,
                     CounterClockwise);
}

// NOTE(Dima): Commands for doubly rotation. Rotation of a side and center
INTERNAL_FUNCTION inline void RubCom_RR(rubiks_cube* Cube, 
                                        b32 CounterClockwise = false)
{
    AddCommandToCube(Cube, RubiksAxis_X, 0, Cube->Dim - 2, CounterClockwise);
}

INTERNAL_FUNCTION inline void RubCom_LL(rubiks_cube* Cube,
                                        b32 CounterClockwise = false)
{
    AddCommandToCube(Cube, RubiksAxis_X, 1, Cube->Dim - 1, !CounterClockwise);
}

INTERNAL_FUNCTION inline void RubCom_UU(rubiks_cube* Cube,
                                        b32 CounterClockwise = false)
{
    AddCommandToCube(Cube, RubiksAxis_Y, 1, Cube->Dim - 1, !CounterClockwise);
}

INTERNAL_FUNCTION inline void RubCom_DD(rubiks_cube* Cube, 
                                        b32 CounterClockwise = false)
{
    AddCommandToCube(Cube, RubiksAxis_Y, 0, Cube->Dim - 2, CounterClockwise);
}

INTERNAL_FUNCTION inline void RubCom_FF(rubiks_cube* Cube,
                                        b32 CounterClockwise = false)
{
    AddCommandToCube(Cube, RubiksAxis_Z, 0, Cube->Dim - 2, CounterClockwise);
}

INTERNAL_FUNCTION inline void RubCom_BB(rubiks_cube* Cube,
                                        b32 CounterClockwise = false)
{
    AddCommandToCube(Cube, RubiksAxis_Z, 1, Cube->Dim - 1, !CounterClockwise);
}

// NOTE(Dima): Commands for rotating whole cube
INTERNAL_FUNCTION inline void RubCom_X(rubiks_cube* Cube,
                                       b32 CounterClockwise = false)
{
    AddCommandToCube(Cube, RubiksAxis_X, 0, Cube->Dim - 1, CounterClockwise);
}

INTERNAL_FUNCTION inline void RubCom_Y(rubiks_cube* Cube,
                                       b32 CounterClockwise = false)
{
    AddCommandToCube(Cube, RubiksAxis_Y, 0, Cube->Dim - 1, !CounterClockwise);
}

INTERNAL_FUNCTION inline void RubCom_Z(rubiks_cube* Cube,
                                       b32 CounterClockwise = false)
{
    AddCommandToCube(Cube, RubiksAxis_Z, 0, Cube->Dim - 1, CounterClockwise);
}

// NOTE(Dima): Other command processing
INTERNAL_FUNCTION void FinishCommandExecution(rubiks_cube* Cube)
{
    Cube->DoIndex = (Cube->DoIndex + 1) % Cube->CommandsCount;
}

INTERNAL_FUNCTION b32 CanExecuteCommand(rubiks_cube* Cube)
{
    b32 Result = (Cube->AddIndex != Cube->DoIndex) && !Cube->IsRotatingNow;
    
    return(Result);
}

// NOTE(Dima): Solving
INTERNAL_FUNCTION inline b32 SideIsSolved(rubiks_cube* Cube, u32 SideIndex)
{
    u8* Side = Cube->Sides[SideIndex];
    
    u32 Result = true;
    if(Cube->Dim >= 3)
    {
        u32 TargetColor = Side[0];
        
        for(int y = 0; y < Cube->Dim; y++)
        {
            for(int x = 0; x < Cube->Dim; x++)
            {
                if(Side[y * Cube->Dim + x] != TargetColor)
                {
                    Result = false;
                    break;
                }
            }
        }
    }
    
    return(Result);
}

INTERNAL_FUNCTION inline b32 CubeIsSolved(rubiks_cube* Cube)
{
    b32 Result = true;
    
    for(int i = 0; i < 6; i++)
    {
        if(!SideIsSolved(Cube, i))
        {
            Result = false;
            break;
        }
    }
    
    return(Result);
}

inline u8 GetSideCenterFirstColor(rubiks_cube* Cube, u32 SideIndex)
{
    Assert(Cube->Dim >= 3);
    
    u8* Side = Cube->Sides[SideIndex];
    
    u8 Result = Side[1 * Cube->Dim + 1];
    
    return(Result);
}

inline u8 GetEdgeCenterNColor(rubiks_cube* Cube, 
                              u32 SideIndex, 
                              u32 SideEdge,
                              int N)
{
    u8* Side = Cube->Sides[SideIndex];
    
    u8 Result = Side[0];
    
    if(Cube->Dim >= 3)
    {
        switch(SideEdge)
        {
            case RubSideEdge_Top:
            {
                Result = Side[1 + N];
            }break;
            
            case RubSideEdge_Right:
            {
                Result = Side[Cube->Dim * (1 + N) + (Cube->Dim - 1)];
            }break;
            
            case RubSideEdge_Bottom:
            {
                Result = Side[Cube->Dim * (Cube->Dim - 1) + 1 + N];
            }break;
            
            case RubSideEdge_Left:
            {
                Result = Side[Cube->Dim * (1 + N)];
            }break;
        }
    }
    
    return(Result);
}

inline u8 GetEdgeCenterFirstColor(rubiks_cube* Cube, u32 SideIndex, u32 SideEdge)
{
    u8 Result = GetEdgeCenterNColor(Cube, SideIndex, SideEdge, 0);
    
    return(Result);
}

inline u8 GetSideCornerColor(rubiks_cube* Cube, u32 SideIndex, u32 GetMode)
{
    u8* Side = Cube->Sides[SideIndex];
    
    u8 Result = Side[0];
    
    if(Cube->Dim >= 2)
    {
        switch(GetMode)
        {
            case RubSideGetCorn_TopLeft:
            {
                Result = Side[0];
            }break;
            
            case RubSideGetCorn_TopRight:
            {
                Result = Side[Cube->Dim - 1];
            }break;
            
            case RubSideGetCorn_BotLeft:
            {
                Result = Side[Cube->Dim * (Cube->Dim - 1)];
            }break;
            
            case RubSideGetCorn_BotRight:
            {
                Result = Side[Cube->Dim * Cube->Dim - 1];
            }break;
        }
    }
    
    return(Result);
}

INTERNAL_FUNCTION int FindEdgeIndexForColors(rubiks_cube* Cube,
                                             u8 Color1,
                                             u8 Color2,
                                             b32* IsInverted)
{
    Assert(Cube->Dim >= 3);
    Assert(IsInverted);
    
    u16 ColorCombInit = Color1 | ((u16)Color2 << 8);
    
    b32 Inverted = false;
    int Result = -1;
    for(int i = 0; i < 12; i++)
    {
        int* EdgesSides = RubiksEdgesSides[i];
        
        u8 Side1 = EdgesSides[0];
        u8 Side2 = EdgesSides[1];
        
        u8 GetMethod1 = EdgesSides[2];
        u8 GetMethod2 = EdgesSides[3];
        
        u8 EdgeColor1 = GetEdgeCenterFirstColor(Cube, Side1, GetMethod1);
        u8 EdgeColor2 = GetEdgeCenterFirstColor(Cube, Side2, GetMethod2);
        
        u16 ColorCombination1 = EdgeColor1 | ((u16)EdgeColor2 << 8);
        u16 ColorCombination2 = EdgeColor2 | ((u16)EdgeColor1 << 8);
        
        b32 Found = false;
        if(ColorCombInit == ColorCombination1)
        {
            Found = true;
            Inverted = false;
        }
        else if(ColorCombInit == ColorCombination2)
        {
            Found = true;
            Inverted = true;
        }
        
        if(Found)
        {
            Result = i;
            
            break;
        }
    }
    
    // NOTE(Dima): The edge should be found!
    Assert(Result != -1);
    *IsInverted = Inverted;
    
    return(Result);
}

inline u32 RubMakeColorComb(u8 Color1, u8 Color2, u8 Color3)
{
    u32 Result = Color1 | ((u32)Color2 << 8) | ((u32)Color3 << 16);
    
    return(Result);
}



INTERNAL_FUNCTION int FindCornerIndexForColors(rubiks_cube* Cube,
                                               u8 Color1,
                                               u8 Color2,
                                               u8 Color3,
                                               int* RotationResult)
{
    Assert(Cube->Dim >= 2);
    Assert(RotationResult);
    
    u32 ColorCombInit = RubMakeColorComb(Color1, Color2, Color3);
    
    int Rotation = 0;
    int Result = -1;
    for(int i = 0; i < 8; i++)
    {
        int* CornersPrep = RubiksCornersPrep[i];
        
        u8 CornCol1 = GetSideCornerColor(Cube, CornersPrep[0], CornersPrep[3]);
        u8 CornCol2 = GetSideCornerColor(Cube, CornersPrep[1], CornersPrep[4]);
        u8 CornCol3 = GetSideCornerColor(Cube, CornersPrep[2], CornersPrep[5]);
        
        u32 CurrentCombination1 = RubMakeColorComb(CornCol1, CornCol2, CornCol3);
        u32 CurrentCombination2 = RubMakeColorComb(CornCol2, CornCol3, CornCol1);
        u32 CurrentCombination3 = RubMakeColorComb(CornCol3, CornCol1, CornCol2);
        
        b32 Found = false;
        if(ColorCombInit == CurrentCombination1)
        {
            Rotation = RubCornRot_Correct;
            Found = true;
        }
        else if(ColorCombInit == CurrentCombination2)
        {
            Rotation = RubCornRot_SecondAtFirst;
            Found = true;
        }
        else if(ColorCombInit == CurrentCombination3)
        {
            Rotation = RubCornRot_ThirdAtFirst;
            Found = true;
        }
        
        if(Found)
        {
            Result = i;
            break;
        }
    }
    
    // NOTE(Dima): The edge should be found!
    Assert(Result != -1);
    *RotationResult = Rotation;
    
    return(Result);
}

INTERNAL_FUNCTION inline b32 SideCenterOnRightPlace(rubiks_cube* Cube, u32 SideIndex)
{
    Assert(Cube->Dim >= 3);
    
    u32 TargetColor = SideIndex;
    
    u32 Result = true;
    if(Cube->Dim >= 3)
    {
        u32 CenterColor = GetSideCenterFirstColor(Cube, SideIndex);
        
        Result = (TargetColor == CenterColor);
    }
    
    return(Result);
}

INTERNAL_FUNCTION inline b32 EdgeCenterIsSolved(rubiks_cube* Cube, u32 EdgeIndex)
{
    b32 Result = true;
    
    if(Cube->Dim >= 4)
    {
        int* EdgesSides = RubiksEdgesSides[EdgeIndex];
        
        u8 Side1 = EdgesSides[0];
        u8 Side2 = EdgesSides[1];
        
        u8 GetMethod1 = EdgesSides[2];
        u8 GetMethod2 = EdgesSides[3];
        
        u8 EdgeColor1 = GetEdgeCenterFirstColor(Cube, Side1, GetMethod1);
        u8 EdgeColor2 = GetEdgeCenterFirstColor(Cube, Side2, GetMethod2);
        
        u16 InitComb = (u16)EdgeColor1 | ((u16)EdgeColor2 << 8);
        
        for(int i = 0; i < Cube->Dim - 2; i++)
        {
            u8 Col1 = GetEdgeCenterNColor(Cube, Side1, GetMethod1, i);
            u8 Col2 = GetEdgeCenterNColor(Cube, Side2, GetMethod2, i);
            
            u16 ColorComb = (u16)Col1 | ((u16)Col2 << 8);
            
            if(ColorComb != InitComb)
            {
                Result = false;
                break;
            }
        }
    }
    
    return(Result);
}

inline b32 SideCenterIsSolved(rubiks_cube* Cube, int SideIndex)
{
    b32 Result = true;
    
    if(Cube->Dim >= 4)
    {
        u8 InitColor = GetSideCenterFirstColor(Cube, SideIndex);
        
        u8* Side = Cube->Sides[SideIndex];
        
        for(int y = 1; y < Cube->Dim - 1; y++)
        {
            for(int x = 1; x < Cube->Dim - 1; x++)
            {
                if(Side[y * Cube->Dim + x] != InitColor)
                {
                    Result = false;
                    break;
                }
            }
        }
    }
    
    return(Result);
}

INTERNAL_FUNCTION int FindFirstSideWithUnsolvedCenter(rubiks_cube* Cube)
{
    int Result = -1;
    
    int Sides[6] = 
    {
        RubSide_Up,
        RubSide_Down,
        RubSide_Front,
        RubSide_Right,
        RubSide_Back,
        RubSide_Left,
    };
    
    for(int SideIndex = 0;
        SideIndex < RubSide_Count;
        SideIndex++)
    {
        if(!SideCenterIsSolved(Cube, Sides[SideIndex]))
        {
            Result = Sides[SideIndex];
            
            break;
        }
    }
    
    return(Result);
}

INTERNAL_FUNCTION int FindFirstWallWithUnsolvedCenter(rubiks_cube* Cube)
{
    int Result = -1;
    
    int Sides[4] = 
    {
        RubSide_Front,
        RubSide_Right,
        RubSide_Back,
        RubSide_Left,
    };
    
    for(int SideIndex = 0;
        SideIndex < 4;
        SideIndex++)
    {
        if(!SideCenterIsSolved(Cube, Sides[SideIndex]))
        {
            Result = Sides[SideIndex];
            
            break;
        }
    }
    
    return(Result);
}

INTERNAL_FUNCTION int FindFirstEdgeWithUnsolvedCenter(rubiks_cube* Cube)
{
    int Result = -1;
    
    for(int EdgeIndex = 0;
        EdgeIndex < RubEdge_Count;
        EdgeIndex++)
    {
        if(!EdgeCenterIsSolved(Cube, EdgeIndex))
        {
            Result = EdgeIndex;
            
            break;
        }
    }
    
    return(Result);
}

INTERNAL_FUNCTION inline b32 CrossEdgeCorrectlyPlaced(rubiks_cube* Cube, 
                                                      u32 TopCenterColor, 
                                                      u32 WallColor)
{
    b32 Result = true;
    
    if(Cube->Dim >= 3)
    {
        Assert(TopCenterColor == RubiksColor_White);
        
        u8 EdgeOnTopColor = GetEdgeCenterFirstColor(Cube, RubSide_Up, RubSideEdge_Bottom);
        u8 EdgeOnFronColor = GetEdgeCenterFirstColor(Cube, RubSide_Front, RubSideEdge_Top);
        
        b32 TopColorCorrect = (EdgeOnTopColor == TopCenterColor);
        b32 FrontColorCorrect = (EdgeOnFronColor == WallColor);
        
        if(!TopColorCorrect || !FrontColorCorrect)
        {
            Result = false;
        }
    }
    
    return(Result);
}

INTERNAL_FUNCTION inline int FindSideWithCenterColor(rubiks_cube* Cube, u8 Color)
{
    int Result = -1;
    
    for(int SideIndex = 0; 
        SideIndex < 6;
        SideIndex++)
    {
        u8 CurrentColor = GetSideCenterFirstColor(Cube, SideIndex);
        if((Cube->Dim % 2 == 1) &&
           (Cube->Dim >= 4))
        {
            u8* Side = Cube->Sides[SideIndex];
            
            int HalfDim = Cube->Dim / 2;
            
            CurrentColor = Side[Cube->Dim * HalfDim + HalfDim];
        }
        
        if(CurrentColor == Color)
        {
            Result = SideIndex;
            
            break;
        }
    }
    
    return(Result);
}

INTERNAL_FUNCTION void MoveSideCentersToRightPlace(rubiks_cube* Cube)
{
    // NOTE(Dima): We'll reach this point only if cube is >= 3 dimensions
    if(CanStartSolvingState(Cube, RubState_MoveCenters_Green))
    {
        u8 SideWithGreen = FindSideWithCenterColor(Cube, RubiksColor_Green);
        switch(SideWithGreen)
        {
            // NOTE(Dima): Front
            case RubiksDirection_Front:
            {
                // NOTE(Dima): Change the state to next
                //Cube->SolvingState = RubState_MoveCenters_White;
            }break;
            
            // NOTE(Dima): Back
            case RubiksDirection_Back:
            {
                RubCom_Y(Cube, false);
                RubCom_Y(Cube, false);
            }break;
            
            // NOTE(Dima): Up
            case RubiksDirection_Up:
            {
                RubCom_X(Cube, true);
            }break;
            
            
            // NOTE(Dima): Down
            case RubiksDirection_Down:
            {
                RubCom_X(Cube, false);
            }break;
            
            
            // NOTE(Dima): Left
            case RubiksDirection_Left:
            {
                RubCom_Y(Cube, true);
            }break;
            
            
            // NOTE(Dima): Right
            case RubiksDirection_Right:
            {
                RubCom_Y(Cube, false);
            }break;
        }
        
        // NOTE(Dima): Adding command to transition to next state
        AddFinishStateCommand(Cube, RubState_MoveCenters_White);
    }
    
    if(CanStartSolvingState(Cube, RubState_MoveCenters_White))
    {
        u8 SideWithWhite = FindSideWithCenterColor(Cube, RubiksColor_White);
        switch(SideWithWhite)
        {
            // NOTE(Dima): Up
            case RubiksDirection_Up:
            {
                // NOTE(Dima): Change state to next
                //Cube->SolvingState = RubState_MakeWhiteCross;
            }break;
            
            // NOTE(Dima): Down
            case RubiksDirection_Down:
            {
                RubCom_Z(Cube, false);
                RubCom_Z(Cube, false);
            }break;
            
            
            // NOTE(Dima): Left
            case RubiksDirection_Left:
            {
                RubCom_Z(Cube, false);
            }break;
            
            // NOTE(Dima): Right
            case RubiksDirection_Right:
            {
                RubCom_Z(Cube, true);
            }break;
            
            default:
            {
                // NOTE(Dima): It's impossible to get here since Front and Back should be already solved
                InvalidCodePath;
            }break;
        }
        
        AddFinishStateCommand(Cube, RubState_MakeWhiteCross);
    }
}

INTERNAL_FUNCTION void MoveEdgeCentersToRightPlace(rubiks_cube* Cube)
{
    if(CanStartSolvingState(Cube, RubState_MakeWhiteCross))
    {
        int SideColors[4] = 
        {
            GetSideCenterFirstColor(Cube, RubSide_Right),
            GetSideCenterFirstColor(Cube, RubSide_Back),
            GetSideCenterFirstColor(Cube, RubSide_Left),
            GetSideCenterFirstColor(Cube, RubSide_Front),
        };
        
        int CompareResults[4] = 
        {
            RubEdge_UpRight,
            RubEdge_UpBack,
            RubEdge_UpLeft,
            RubEdge_UpFront,
        };
        
        b32 AllCorrectlyPlaced = true;
        for(int i = 0; i < 4; i++)
        {
            b32 TestInverted;
            int TestFoundEdge = FindEdgeIndexForColors(Cube,
                                                       RubiksColor_White,
                                                       SideColors[i],
                                                       &TestInverted);
            
            b32 Fits = (CompareResults[i] == TestFoundEdge);
            if(Fits && TestInverted)
            {
                Fits = false;
            }
            
            if(!Fits)
            {
                AllCorrectlyPlaced = false;
                break;
            }
        }
        
        if(AllCorrectlyPlaced)
        {
            AddFinishStateCommand(Cube, RubState_FlipToYellowAfterCross);
        }
        else
        {
            u8 CurrentWallColor = GetSideCenterFirstColor(Cube, RubiksDirection_Front);
            
            b32 Inverted;
            int FoundEdge = FindEdgeIndexForColors(Cube,
                                                   RubiksColor_White, 
                                                   CurrentWallColor,
                                                   &Inverted);
            
            switch(FoundEdge)
            {
                case RubEdge_UpFront:
                {
                    if(!Inverted)
                    {
                        // NOTE(Dima): Edge is already solved so move to next one
                        
                        RubCom_Y(Cube, false);
                    }
                    else
                    {
                        RubCom_F(Cube, false);
                        RubCom_U(Cube, true);
                        RubCom_R(Cube, false);
                        RubCom_U(Cube, false);
                    }
                }break;
                
                case RubEdge_UpRight:
                {
                    if(!Inverted)
                    {
                        RubCom_U(Cube);
                    }
                    else
                    {
                        RubCom_R(Cube, true);
                        RubCom_F(Cube, true);
                    }
                }break;
                
                case RubEdge_UpBack:
                {
                    if(!Inverted)
                    {
                        RubCom_U(Cube);
                        RubCom_U(Cube);
                    }
                    else
                    {
                        RubCom_B(Cube, true);
                        RubCom_U(Cube, true);
                        RubCom_R(Cube, true);
                        RubCom_U(Cube);
                    }
                }break;
                
                case RubEdge_UpLeft:
                {
                    if(!Inverted)
                    {
                        RubCom_U(Cube, true);
                    }
                    else
                    {
                        RubCom_L(Cube);
                        RubCom_F(Cube);
                    }
                }break;
                
                case RubEdge_DownFront:
                {
                    if(!Inverted)
                    {
                        RubCom_F(Cube);
                        RubCom_F(Cube);
                    }
                    else
                    {
                        // NOTE(Dima): Bringing to DownRight case
                        RubCom_D(Cube);
                    }
                }break;
                
                case RubEdge_DownRight:
                {
                    if(!Inverted)
                    {
                        // NOTE(Dima): To Down-Front
                        RubCom_D(Cube, true);
                    }
                    else
                    {
                        RubCom_R(Cube);
                        RubCom_F(Cube, true);
                        RubCom_R(Cube, true);
                    }
                }break;
                
                case RubEdge_DownBack:
                {
                    if(!Inverted)
                    {
                        // NOTE(Dima): Bringing to DownFront case
                        RubCom_D(Cube);
                        RubCom_D(Cube);
                    }
                    else
                    {
                        // NOTE(Dima): Bringing to DownRight case
                        RubCom_D(Cube, true);
                    }
                }break;
                
                case RubEdge_DownLeft:
                {
                    if(!Inverted)
                    {
                        // NOTE(Dima): Bringing to DownFront case
                        RubCom_D(Cube);
                    }
                    else
                    {
                        // NOTE(Dima): Bringing to DownRight case 
                        RubCom_D(Cube, true);
                        RubCom_D(Cube, true);
                    }
                }break;
                
                case RubEdge_RightBack:
                {
                    if(!Inverted)
                    {
                        RubCom_U(Cube);
                        RubCom_U(Cube);
                        RubCom_B(Cube);
                        RubCom_U(Cube);
                        RubCom_U(Cube);
                    }
                    else
                    {
                        RubCom_U(Cube, true);
                        RubCom_R(Cube, true);
                        RubCom_U(Cube);
                    }
                }break;
                
                case RubEdge_BackLeft:
                {
                    if(!Inverted)
                    {
                        RubCom_U(Cube);
                        RubCom_L(Cube);
                        RubCom_U(Cube, true);
                    }
                    else
                    {
                        RubCom_U(Cube);
                        RubCom_U(Cube);
                        RubCom_B(Cube, true);
                        RubCom_U(Cube);
                        RubCom_U(Cube);
                    }
                }break;
                
                case RubEdge_LeftFront:
                {
                    if(!Inverted)
                    {
                        RubCom_F(Cube);
                    }
                    else
                    {
                        RubCom_U(Cube);
                        RubCom_L(Cube, true);
                        RubCom_U(Cube, true);
                    }
                }break;
                
                case RubEdge_FrontRight:
                {
                    if(!Inverted)
                    {
                        RubCom_U(Cube, true);
                        RubCom_R(Cube);
                        RubCom_U(Cube);
                    }
                    else
                    {
                        RubCom_F(Cube, true);
                    }
                }break;
            }
            
            
            AddFinishStateCommand(Cube, RubState_MakeWhiteCross);
        }
    }
    
    if(CanStartSolvingState(Cube, RubState_FlipToYellowAfterCross))
    {
        AddFinishStateCommand(Cube, RubState_SolveFirstLayer);
    }
}

INTERNAL_FUNCTION b32 FirstLayerIsSolved(rubiks_cube* Cube)
{
    // NOTE(Dima): Checking if first layer is OK
    int SideColors[4] = 
    {
        GetSideCenterFirstColor(Cube, RubSide_Front),
        GetSideCenterFirstColor(Cube, RubSide_Right),
        GetSideCenterFirstColor(Cube, RubSide_Back),
        GetSideCenterFirstColor(Cube, RubSide_Left),
    };
    
    int CompareResults[4] = 
    {
        RubCorn_UpFrontRight,
        RubCorn_UpRightBack,
        RubCorn_UpBackLeft,
        RubCorn_UpLeftFront,
    };
    
    b32 FirstLayerCorrect = true;
    for(int i = 0; i < 4; i++)
    {
        int CornRotation;
        int TestFoundEdge = FindCornerIndexForColors(Cube,
                                                     RubiksColor_White,
                                                     SideColors[i],
                                                     SideColors[(i + 1) % 4],
                                                     &CornRotation);
        
        b32 ThisCorrect = (CompareResults[i] == TestFoundEdge);
        if(ThisCorrect && (CornRotation != RubCornRot_Correct))
        {
            ThisCorrect = false;
        }
        
        if(!ThisCorrect)
        {
            FirstLayerCorrect = false;
            break;
        }
    }
    
    return(FirstLayerCorrect);
}

INTERNAL_FUNCTION void SolveFirstLayer(rubiks_cube* Cube)
{
    if(CanStartSolvingState(Cube, RubState_SolveFirstLayer))
    {
        b32 FirstLayerCorrect = FirstLayerIsSolved(Cube);
        
        if(FirstLayerCorrect)
        {
            RubCom_Z(Cube);
            RubCom_Z(Cube);
            
            AddFinishStateCommand(Cube, RubState_SolveSecondLayer);
        }
        else
        {
            u8 CurrentWallColor = GetSideCenterFirstColor(Cube, RubiksDirection_Front);
            u8 RightWallColor = GetSideCenterFirstColor(Cube, RubiksDirection_Right);
            
            int CornerRotation;
            int CornerIndex = FindCornerIndexForColors(Cube, 
                                                       RubiksColor_White,
                                                       CurrentWallColor,
                                                       RightWallColor,
                                                       &CornerRotation);
            
            switch(CornerIndex)
            {
                case RubCorn_UpFrontRight:
                {
                    if(CornerRotation == RubCornRot_Correct)
                    {
                        // NOTE(Dima): Corner is already solved so move to next one
                        RubCom_Y(Cube, false);
                    }
                    else if(CornerRotation == RubCornRot_SecondAtFirst)
                    {
                        RubCom_R(Cube, true);
                        RubCom_D(Cube, true);
                        RubCom_R(Cube);
                    }
                    else if(CornerRotation == RubCornRot_ThirdAtFirst)
                    {
                        RubCom_F(Cube);
                        RubCom_D(Cube);
                        RubCom_F(Cube, true);
                        RubCom_D(Cube);
                        RubCom_D(Cube);
                    }
                }break;
                
                case RubCorn_UpRightBack:
                {
                    if(CornerRotation == RubCornRot_Correct)
                    {
                        RubCom_B(Cube, true);
                        RubCom_D(Cube, true);
                        RubCom_B(Cube);
                        RubCom_D(Cube, true);
                    }
                    else if(CornerRotation == RubCornRot_SecondAtFirst)
                    {
                        RubCom_R(Cube);
                        RubCom_D(Cube);
                        RubCom_D(Cube);
                        RubCom_R(Cube, true);
                        RubCom_R(Cube, true);
                        RubCom_D(Cube);
                        RubCom_R(Cube);
                    }
                    else if(CornerRotation == RubCornRot_ThirdAtFirst)
                    {
                        RubCom_R(Cube);
                        RubCom_D(Cube);
                        RubCom_R(Cube, true);
                        RubCom_D(Cube);
                    }
                }break;
                
                case RubCorn_UpBackLeft:
                {
                    if(CornerRotation == RubCornRot_Correct)
                    {
                        RubCom_B(Cube);
                        RubCom_D(Cube);
                        RubCom_B(Cube, true);
                        RubCom_D(Cube);
                        RubCom_D(Cube);
                    }
                    else if(CornerRotation == RubCornRot_SecondAtFirst)
                    {
                        RubCom_L(Cube, true);
                        RubCom_D(Cube, true);
                        RubCom_L(Cube);
                    }
                    else if(CornerRotation == RubCornRot_ThirdAtFirst)
                    {
                        RubCom_B(Cube);
                        RubCom_D(Cube);
                        RubCom_B(Cube, true);
                    }
                }break;
                
                case RubCorn_UpLeftFront:
                {
                    if(CornerRotation == RubCornRot_Correct)
                    {
                        RubCom_L(Cube);
                        RubCom_D(Cube);
                        RubCom_D(Cube);
                        RubCom_L(Cube, true);
                    }
                    else if(CornerRotation == RubCornRot_SecondAtFirst)
                    {
                        RubCom_F(Cube, true);
                        RubCom_D(Cube);
                        RubCom_D(Cube);
                        RubCom_F(Cube);
                    }
                    else if(CornerRotation == RubCornRot_ThirdAtFirst)
                    {
                        RubCom_L(Cube);
                        RubCom_D(Cube);
                        RubCom_L(Cube, true);
                        RubCom_D(Cube, true);
                    }
                }break;
                
                case RubCorn_DownRightFront:
                {
                    if(CornerRotation == RubCornRot_Correct)
                    {
                        RubCom_D(Cube);
                    }
                    else if(CornerRotation == RubCornRot_SecondAtFirst)
                    {
                        RubCom_D(Cube);
                    }
                    else if(CornerRotation == RubCornRot_ThirdAtFirst)
                    {
                        RubCom_D(Cube, true);
                    }
                }break;
                
                case RubCorn_DownBackRight:
                {
                    if(CornerRotation == RubCornRot_Correct)
                    {
                        RubCom_R(Cube);
                        RubCom_D(Cube, true);
                        RubCom_R(Cube, true);
                    }
                    else if(CornerRotation == RubCornRot_SecondAtFirst)
                    {
                        RubCom_F(Cube);
                        RubCom_D(Cube, true);
                        RubCom_F(Cube, true);
                    }
                    else if(CornerRotation == RubCornRot_ThirdAtFirst)
                    {
                        RubCom_D(Cube);
                        RubCom_D(Cube);
                    }
                }break;
                
                case RubCorn_DownLeftBack:
                {
                    if(CornerRotation == RubCornRot_Correct)
                    {
                        RubCom_D(Cube, true);
                    }
                    else if(CornerRotation == RubCornRot_SecondAtFirst)
                    {
                        RubCom_D(Cube, true);
                    }
                    else if(CornerRotation == RubCornRot_ThirdAtFirst)
                    {
                        RubCom_D(Cube);
                    }
                }break;
                
                case RubCorn_DownFrontLeft:
                {
                    if(CornerRotation == RubCornRot_Correct)
                    {
                        RubCom_D(Cube);
                        RubCom_D(Cube);
                    }
                    else if(CornerRotation == RubCornRot_SecondAtFirst)
                    {
                        RubCom_D(Cube);
                        RubCom_D(Cube);
                    }
                    else if(CornerRotation == RubCornRot_ThirdAtFirst)
                    {
                        RubCom_R(Cube, true);
                        RubCom_D(Cube);
                        RubCom_R(Cube);
                    }
                }break;
            }
            
            AddFinishStateCommand(Cube, RubState_SolveFirstLayer);
        }
    }
}

INTERNAL_FUNCTION void SolveSecondLayer(rubiks_cube* Cube)
{
    if(CanStartSolvingState(Cube, RubState_SolveSecondLayer))
    {
        
        int SideColors[4] = 
        {
            GetSideCenterFirstColor(Cube, RubSide_Right),
            GetSideCenterFirstColor(Cube, RubSide_Back),
            GetSideCenterFirstColor(Cube, RubSide_Left),
            GetSideCenterFirstColor(Cube, RubSide_Front),
        };
        
        int CompareResults[4] = 
        {
            RubEdge_RightBack,
            RubEdge_BackLeft,
            RubEdge_LeftFront,
            RubEdge_FrontRight,
        };
        
        b32 AllCorrectlyPlaced = true;
        for(int i = 0; i < 4; i++)
        {
            b32 TestInverted;
            int TestFoundEdge = FindEdgeIndexForColors(Cube,
                                                       SideColors[i],
                                                       SideColors[(i + 1) % 4],
                                                       &TestInverted);
            
            b32 Fits = (CompareResults[i] == TestFoundEdge);
            if(Fits && TestInverted)
            {
                Fits = false;
            }
            
            if(!Fits)
            {
                AllCorrectlyPlaced = false;
                break;
            }
        }
        
        
        if(AllCorrectlyPlaced)
        {
            AddFinishStateCommand(Cube, RubState_SolveYellowCross);
        }
        else
        {
            u8 CurrentWallColor = GetSideCenterFirstColor(Cube, RubSide_Front);
            u8 NextWallColor = GetSideCenterFirstColor(Cube, RubSide_Right);
            
            b32 Inverted;
            int Test = FindEdgeIndexForColors(Cube,
                                              CurrentWallColor,
                                              NextWallColor,
                                              &Inverted);
            
            switch(Test)
            {
                case RubEdge_UpFront:
                {
                    if(!Inverted)
                    {
                        RubCom_U(Cube, true);
                    }
                    else
                    {
                        RubCom_U(Cube);
                        RubCom_R(Cube);
                        RubCom_U(Cube, true);
                        RubCom_R(Cube, true);
                        RubCom_U(Cube, true);
                        RubCom_F(Cube, true);
                        RubCom_U(Cube);
                        RubCom_F(Cube);
                    }
                }break;
                
                case RubEdge_UpRight:
                {
                    if(!Inverted)
                    {
                        RubCom_R(Cube, true);
                        RubCom_F(Cube, true);
                        RubCom_R(Cube);
                        RubCom_U(Cube);
                        RubCom_R(Cube);
                        RubCom_U(Cube, true);
                        RubCom_R(Cube, true);
                        RubCom_F(Cube);
                    }
                    else
                    {
                        RubCom_U(Cube);
                    }
                }break;
                
                case RubEdge_UpBack:
                {
                    if(!Inverted)
                    {
                        RubCom_U(Cube);
                    }
                    else
                    {
                        RubCom_U(Cube);
                        RubCom_U(Cube);
                    }
                }break;
                
                case RubEdge_UpLeft:
                {
                    if(!Inverted)
                    {
                        RubCom_U(Cube);
                        RubCom_U(Cube);
                    }
                    else
                    {
                        RubCom_U(Cube, true);
                    }
                }break;
                
                case RubEdge_FrontRight:
                {
                    if(!Inverted)
                    {
                        RubCom_Y(Cube);
                    }
                    else
                    {
                        RubCom_F(Cube, true);
                        RubCom_U(Cube, true);
                        RubCom_F(Cube);
                        RubCom_U(Cube);
                        
                        RubCom_R(Cube);
                        RubCom_U(Cube, true);
                        RubCom_R(Cube, true);
                        RubCom_U(Cube);
                        
                        RubCom_R(Cube);
                        RubCom_U(Cube, true);
                        RubCom_R(Cube, true);
                        RubCom_U(Cube);
                        
                        RubCom_U(Cube);
                        RubCom_R(Cube);
                        RubCom_U(Cube, true);
                        RubCom_R(Cube, true);
                    }
                }break;
                
                
                case RubEdge_RightBack:
                {
                    if(!Inverted)
                    {
                        RubCom_B(Cube);
                        RubCom_U(Cube);
                        RubCom_B(Cube, true);
                        RubCom_U(Cube, true);
                        
                        RubCom_F(Cube, true);
                        RubCom_U(Cube);
                        RubCom_F(Cube);
                        
                        RubCom_U(Cube);
                        RubCom_R(Cube);
                        RubCom_U(Cube, true);
                        RubCom_R(Cube, true);
                        
                        RubCom_B(Cube);
                        RubCom_U(Cube, true);
                        RubCom_B(Cube, true);
                    }
                    else
                    {
                        RubCom_R(Cube);
                        RubCom_R(Cube);
                        RubCom_U(Cube);
                        RubCom_U(Cube);
                        RubCom_R(Cube);
                        RubCom_R(Cube);
                        RubCom_U(Cube);
                        RubCom_U(Cube);
                        RubCom_R(Cube);
                        RubCom_R(Cube);
                    }
                }break;
                
                case RubEdge_BackLeft:
                {
                    if(!Inverted)
                    {
                        RubCom_B(Cube, true);
                        RubCom_U(Cube);
                        RubCom_B(Cube);
                        
                        RubCom_U(Cube);
                        RubCom_L(Cube);
                        RubCom_U(Cube, true);
                        RubCom_L(Cube, true);
                        
                        RubCom_U(Cube, true);
                    }
                    else
                    {
                        RubCom_B(Cube, true);
                        RubCom_U(Cube, true);
                        RubCom_B(Cube);
                        
                        RubCom_F(Cube, true);
                        RubCom_U(Cube);
                        RubCom_F(Cube);
                        
                        RubCom_B(Cube, true);
                        RubCom_U(Cube);
                        RubCom_B(Cube);
                        
                        RubCom_U(Cube);
                        RubCom_U(Cube);
                        RubCom_R(Cube);
                        RubCom_U(Cube);
                        RubCom_U(Cube);
                        RubCom_R(Cube, true);
                        RubCom_U(Cube);
                        RubCom_U(Cube);
                        RubCom_R(Cube);
                        RubCom_U(Cube, true);
                        RubCom_R(Cube, true);
                    }
                }break;
                
                
                case RubEdge_LeftFront:
                {
                    if(!Inverted)
                    {
                        RubCom_L(Cube, true);
                        RubCom_U(Cube);
                        RubCom_L(Cube);
                        RubCom_U(Cube);
                        RubCom_L(Cube, true);
                        RubCom_U(Cube);
                        RubCom_L(Cube);
                        RubCom_F(Cube);
                        RubCom_U(Cube, true);
                        RubCom_F(Cube, true);
                    }
                    else
                    {
                        RubCom_F(Cube);
                        RubCom_F(Cube);
                        RubCom_U(Cube);
                        RubCom_U(Cube);
                        RubCom_F(Cube);
                        RubCom_F(Cube);
                        RubCom_U(Cube);
                        RubCom_U(Cube);
                        RubCom_F(Cube);
                        RubCom_F(Cube);
                    }
                }break;
                
            }
            
            AddFinishStateCommand(Cube, RubState_SolveSecondLayer);
        }
    }
}

INTERNAL_FUNCTION void SolveYellowCross(rubiks_cube* Cube)
{
    if(CanStartSolvingState(Cube, RubState_SolveYellowCross))
    {
        b32 YellowCrossIsSolved = true;
        
        int SideFaceGetEdgeMethod[4] = 
        {
            RubSideEdge_Bottom,
            RubSideEdge_Right,
            RubSideEdge_Top,
            RubSideEdge_Left,
        };
        
        u8 IsYellow[4];
        
        for(int i = 0; i < 4; i++)
        {
            u8 EdgeColor = GetEdgeCenterFirstColor(Cube, RubSide_Up, SideFaceGetEdgeMethod[i]);
            IsYellow[i] = (EdgeColor == RubiksColor_Yellow);
        }
        
        for(int i = 0; i < 4; i++)
        {
            if(!IsYellow[i])
            {
                YellowCrossIsSolved = false;
                break;
            }
        }
        
        if(YellowCrossIsSolved)
        {
            AddFinishStateCommand(Cube, RubState_SolveYellowEdges_Match);
        }
        else
        {
            if(IsYellow[0] &&
               IsYellow[2])
            {
                // NOTE(Dima): Vertical bar
                RubCom_U(Cube);
            }
            else if(IsYellow[1] &&
                    IsYellow[3])
            {
                // NOTE(Dima): Horizontal bar
                RubCom_F(Cube);
                
                RubCom_R(Cube);
                RubCom_U(Cube);
                RubCom_R(Cube, true);
                RubCom_U(Cube, true);
                
                RubCom_F(Cube, true);
            }
            else if(IsYellow[3] && IsYellow[2])
            {
                // NOTE(Dima): TopLeft angle
                RubCom_F(Cube);
                
                RubCom_U(Cube);
                RubCom_R(Cube);
                RubCom_U(Cube, true);
                RubCom_R(Cube, true);
                
                RubCom_F(Cube, true);
            }
            else if(IsYellow[1] && IsYellow[2])
            {
                // NOTE(Dima): TopRight angle
                RubCom_U(Cube, true);
            }
            else if(IsYellow[1] && IsYellow[0])
            {
                // NOTE(Dima): RightBot angle
                RubCom_U(Cube);
                RubCom_U(Cube);
            }
            else if(IsYellow[0] && IsYellow[3])
            {
                RubCom_U(Cube);
            }
            else if(!IsYellow[0] &&
                    !IsYellow[1] &&
                    !IsYellow[2] &&
                    !IsYellow[3])
            {
                // NOTE(Dima): Horizontal bar
                RubCom_F(Cube);
                
                RubCom_R(Cube);
                RubCom_U(Cube);
                RubCom_R(Cube, true);
                RubCom_U(Cube, true);
                
                RubCom_F(Cube, true);
            }
            
            AddFinishStateCommand(Cube, RubState_SolveYellowCross);
        }
    }
}

INTERNAL_FUNCTION void SolveYellowEdgesMatch(rubiks_cube* Cube, u32 MatchState, u32 FinishState)
{
    if(CanStartSolvingState(Cube, MatchState))
    {
        u8 FrontCenterColor = GetSideCenterFirstColor(Cube, RubSide_Front);
        
        int SideWalls[4] = 
        {
            RubSide_Front,
            RubSide_Right,
            RubSide_Back,
            RubSide_Left,
        };
        
        int FoundSide = -1;
        for(int WallIndex = 0; WallIndex < 4; WallIndex++)
        {
            u8 EdgeColor = GetEdgeCenterFirstColor(Cube, SideWalls[WallIndex], RubSideEdge_Top);
            
            if(FrontCenterColor == EdgeColor)
            {
                FoundSide = WallIndex;
                break;
            }
        }
        
        Assert(FoundSide != -1);
        
        switch(FoundSide)
        {
            case 0:
            {
                // NOTE(Dima): Nothing
            }break;
            
            case 1:
            {
                RubCom_U(Cube);
            }break;
            
            case 2:
            {
                RubCom_U(Cube);
                RubCom_U(Cube);
            }break;
            
            case 3:
            {
                RubCom_U(Cube, true);
            }break;
        }
        
        AddFinishStateCommand(Cube, FinishState);
    }
}

INTERNAL_FUNCTION void SolveYellowEdges(rubiks_cube* Cube)
{
    SolveYellowEdgesMatch(Cube, 
                          RubState_SolveYellowEdges_Match,
                          RubState_SolveYellowEdges);
    
    
    
    if(CanStartSolvingState(Cube, RubState_SolveYellowEdges))
    {
        int SideWalls[4] = 
        {
            RubSide_Front,
            RubSide_Right,
            RubSide_Back,
            RubSide_Left,
        };
        
        // NOTE(Dima): Getting wall colors
        u8 WallCentersColors[4];
        for(int WallIndex = 0; WallIndex < 4; WallIndex++)
        {
            WallCentersColors[WallIndex] = GetSideCenterFirstColor(Cube, SideWalls[WallIndex]);
        }
        
        // NOTE(Dima): Walking and checking if all color are correctly matched 
        b32 AllEdgesCorrect = true;
        for(int WallIndex = 0; WallIndex < 4; WallIndex++)
        {
            u8 EdgeColor = GetEdgeCenterFirstColor(Cube, SideWalls[WallIndex], RubSideEdge_Top);
            
            if(WallCentersColors[WallIndex] != EdgeColor)
            {
                AllEdgesCorrect = false;
                break;
            }
        }
        
        if(AllEdgesCorrect)
        {
            AddFinishStateCommand(Cube, RubState_PlaceYellowCorners);
        }
        else
        {
            AddFinishStateCommand(Cube, RubState_SolveYellowEdges_PrepareAndExecute);
            
        }
    }
    
    if(CanStartSolvingState(Cube, RubState_SolveYellowEdges_PrepareAndExecute))
    {
        u8 LeftUpEdgeColor = GetEdgeCenterFirstColor(Cube, RubSide_Left, RubSideEdge_Top);
        u8 LeftCenterColor = GetSideCenterFirstColor(Cube, RubSide_Left);
        
        if(LeftCenterColor == LeftUpEdgeColor)
        {
            RubCom_Y(Cube);
            RubCom_Y(Cube);
        }
        else
        {
            RubCom_Y(Cube, true);
        }
        
        RubCom_R(Cube);
        RubCom_U(Cube);
        RubCom_R(Cube, true);
        RubCom_U(Cube);
        RubCom_R(Cube);
        RubCom_U(Cube);
        RubCom_U(Cube);
        RubCom_R(Cube, true);
        RubCom_U(Cube);
        
        AddFinishStateCommand(Cube, RubState_SolveYellowEdges_Match);
    }
}

INTERNAL_FUNCTION void PlaceYellowCorners(rubiks_cube* Cube)
{
    if(CanStartSolvingState(Cube, RubState_PlaceYellowCorners))
    {
        int CompareResults[4] = 
        {
            RubCorn_UpFrontRight,
            RubCorn_UpRightBack,
            RubCorn_UpBackLeft,
            RubCorn_UpLeftFront,
        };
        b32 Results[4];
        
        int SideWalls[4] = 
        {
            RubSide_Front,
            RubSide_Right,
            RubSide_Back,
            RubSide_Left,
        };
        
        // NOTE(Dima): Getting wall colors
        u8 WallCentersColors[4];
        for(int WallIndex = 0; WallIndex < 4; WallIndex++)
        {
            WallCentersColors[WallIndex] = GetSideCenterFirstColor(Cube, SideWalls[WallIndex]);
        }
        
        int FirstCorrectIndex = -1;
        int CountCorrect = 0;
        for(int i = 0;
            i < 4;
            i++)
        {
            int Rotation;
            int FindResult = FindCornerIndexForColors(Cube, 
                                                      RubiksColor_Yellow,
                                                      WallCentersColors[i],
                                                      WallCentersColors[(i + 1) % 4],
                                                      &Rotation);
            
            Results[i] = (FindResult == CompareResults[i]);
            
            if(Results[i])
            {
                if(FirstCorrectIndex == -1)
                {
                    FirstCorrectIndex = i;
                }
                
                CountCorrect++;
            }
        }
        
        if(CountCorrect == 4)
        {
            AddFinishStateCommand(Cube, RubState_OrientYellowCorners_Prepare);
        }
        else
        {
            if(FirstCorrectIndex != -1)
            {
                switch(FirstCorrectIndex)
                {
                    case 0:
                    {
                        // NOTE(Dima): Nothing to do
                    }break;
                    
                    case 1:
                    {
                        RubCom_Y(Cube);
                    }break;
                    
                    case 2:
                    {
                        RubCom_Y(Cube);
                        RubCom_Y(Cube);
                    }break;
                    
                    case 3:
                    {
                        RubCom_Y(Cube, true);
                    }break;
                }
            }
            
            RubCom_U(Cube);
            RubCom_R(Cube);
            RubCom_U(Cube, true);
            RubCom_L(Cube, true);
            RubCom_U(Cube);
            RubCom_R(Cube, true);
            RubCom_U(Cube, true);
            RubCom_L(Cube);
            
            AddFinishStateCommand(Cube, RubState_PlaceYellowCorners);
        }
    }
}

INTERNAL_FUNCTION int OrientFindPlace(rubiks_cube* Cube,
                                      b32* AllCorrect)
{
    
#if 1    
    int CompareResults[4] = 
    {
        RubCorn_UpFrontRight,
        RubCorn_UpRightBack,
        RubCorn_UpBackLeft,
        RubCorn_UpLeftFront,
    };
#endif
    
    int SideWalls[4] = 
    {
        RubSide_Front,
        RubSide_Right,
        RubSide_Back,
        RubSide_Left,
    };
    
    // NOTE(Dima): Getting wall colors
    u8 WallColors[4];
    for(int WallIndex = 0; WallIndex < 4; WallIndex++)
    {
        WallColors[WallIndex] = GetEdgeCenterFirstColor(Cube, SideWalls[WallIndex], RubSideEdge_Top);
    }
    
    int FirstIncorrectIndex = -1;
    
    *AllCorrect = true;
    for(int i = 0;
        i < 4;
        i++)
    {
        int Rotation;
        int FindResult = FindCornerIndexForColors(Cube, 
                                                  RubiksColor_Yellow,
                                                  WallColors[i],
                                                  WallColors[(i + 1) % 4],
                                                  &Rotation);
        
        Assert(FindResult == CompareResults[i]);
        
        b32 IsCorrect = (Rotation == 0);
        
        if(!IsCorrect)
        {
            *AllCorrect = false;
            
            if(FirstIncorrectIndex == -1)
            {
                FirstIncorrectIndex = i;
            }
            
            break;
        }
    }
    
    return(FirstIncorrectIndex);
}

INTERNAL_FUNCTION void OrientYellowCorners(rubiks_cube* Cube)
{
    if(CanStartSolvingState(Cube, RubState_OrientYellowCorners_Prepare))
    {
        b32 AllCorrect;
        int FirstIncorrectIndex = OrientFindPlace(Cube, &AllCorrect);
        
        if(AllCorrect)
        {
            AddFinishStateCommand(Cube, RubState_Solved);
        }
        else
        {
            Assert(FirstIncorrectIndex != -1);
            
            switch(FirstIncorrectIndex)
            {
                case 0:
                {
                    // NOTE(Dima): Nothing to do
                }break;
                
                case 1:
                {
                    RubCom_Y(Cube);
                }break;
                
                case 2:
                {
                    RubCom_Y(Cube);
                    RubCom_Y(Cube);
                }break;
                
                case 3:
                {
                    RubCom_Y(Cube, true);
                }break;
            }
            
            RubCom_R(Cube, true);
            RubCom_D(Cube, true);
            RubCom_R(Cube);
            RubCom_D(Cube);
            
            RubCom_R(Cube, true);
            RubCom_D(Cube, true);
            RubCom_R(Cube);
            RubCom_D(Cube);
            
            AddFinishStateCommand(Cube, RubState_OrientYellowCorners);
        }
    }
    
    if(CanStartSolvingState(Cube, RubState_OrientYellowCorners))
    {
        b32 AllCorrect;
        int FirstIncorrectIndex = OrientFindPlace(Cube, &AllCorrect);
        
        if(AllCorrect)
        {
            AddFinishStateCommand(Cube, RubState_OrientYellowCorners_MatchInTheEnd);
        }
        else
        {
            switch(FirstIncorrectIndex)
            {
                case 0:
                {
                    // NOTE(Dima): Nothing to do
                }break;
                
                case 1:
                {
                    RubCom_U(Cube);
                }break;
                
                case 2:
                {
                    RubCom_U(Cube);
                    RubCom_U(Cube);
                }break;
                
                case 3:
                {
                    RubCom_U(Cube, true);
                }break;
            }
            
            RubCom_R(Cube, true);
            RubCom_D(Cube, true);
            RubCom_R(Cube);
            RubCom_D(Cube);
            
            RubCom_R(Cube, true);
            RubCom_D(Cube, true);
            RubCom_R(Cube);
            RubCom_D(Cube);
            
            AddFinishStateCommand(Cube, RubState_OrientYellowCorners);
        }
    }
    
    SolveYellowEdgesMatch(Cube, 
                          RubState_OrientYellowCorners_MatchInTheEnd,
                          RubState_Solved);
}


INTERNAL_FUNCTION b32 SideCenterWithColorIsSolved(rubiks_cube* Cube, u8 Color)
{
    b32 Result = true;
    
    int SideIndex = -1;
    
    for(int SideIndex = 0;
        SideIndex < 6;
        SideIndex++)
    {
        u8* Side = Cube->Sides[SideIndex];
        
        if(Side[0] == Color)
        {
            // NOTE(Dima): Found side with this color.
            SideIndex = SideIndex;
            
            break;
        }
    }
    
    if(SideIndex == -1)
    {
        Result = false;
    }
    else
    {
        Result = SideCenterIsSolved(Cube, SideIndex);
    }
    
    return(Result);
}

INTERNAL_FUNCTION void SolveSideCenter(rubiks_cube* Cube,
                                       u32 SolveState, 
                                       u32 SolveToUpState,
                                       u32 HelperToFrontState,
                                       u32 FuncState,
                                       u32 ExitState,
                                       u8 CenterColor,
                                       u8 HelperCenterColor)
{
    // NOTE(Dima): Checking if need to solve
    if(CanStartSolvingState(Cube, SolveState))
    {
        if(SideCenterWithColorIsSolved(Cube, CenterColor))
        {
            // NOTE(Dima): This side is solved. Go ahead and solve other sides
            AddFinishStateCommand(Cube, ExitState);
        }
        else
        {
            AddFinishStateCommand(Cube, SolveToUpState);
        }
    }
    
    // NOTE(Dima): Placing unsolved side on Top
    if(CanStartSolvingState(Cube, SolveToUpState))
    {
        int UnsolvedSide = -1;
        
        if((Cube->Dim % 2 == 1) &&
           (Cube->Dim >= 4))
        {
            // NOTE(Dima): We can move white side to up. But first - find it.
            UnsolvedSide = FindSideWithCenterColor(Cube, CenterColor);
        }
        else
        {
            // NOTE(Dima): We can move any side to up. But first - find it.
            UnsolvedSide = FindFirstSideWithUnsolvedCenter(Cube);
        }
        
        // NOTE(Dima): Need to solve sides. First - rotate unsolved side to the Up
        switch(UnsolvedSide)
        {
            case RubSide_Front:
            {
                RubCom_X(Cube);
            }break;
            
            case RubSide_Back:
            {
                RubCom_X(Cube, true);
            }break;
            
            case RubSide_Right:
            {
                RubCom_Z(Cube, true);
            }break;
            
            case RubSide_Left:
            {
                RubCom_Z(Cube);
            }break;
            
            case RubSide_Up:
            {
                // NOTE(Dima): No need to rotate as unsolved side is already on Up
            }break;
            
            case RubSide_Down:
            {
                RubCom_Z(Cube);
                RubCom_Z(Cube);
            }break;
        }
        
        AddFinishStateCommand(Cube, HelperToFrontState);
    }
    
    // NOTE(Dima): Placing helper side on front
    if(CanStartSolvingState(Cube, HelperToFrontState))
    {
        int UnsolvedWall;
        if((Cube->Dim % 2 == 1) &&
           (Cube->Dim >= 4))
        {
            UnsolvedWall = FindSideWithCenterColor(Cube, HelperCenterColor);
        }
        else
        {
            UnsolvedWall = FindFirstWallWithUnsolvedCenter(Cube);
        }
        
        switch(UnsolvedWall)
        {
            case RubSide_Front:
            {
                // NOTE(Dima): No need to rotate
            }break;
            
            case RubSide_Back:
            {
                RubCom_Y(Cube);
                RubCom_Y(Cube);
            }break;
            
            case RubSide_Right:
            {
                RubCom_Y(Cube);
            }break;
            
            case RubSide_Left:
            {
                RubCom_Y(Cube, true);
            }break;
            
            default:
            {
                // NOTE(Dima): Helper side should always be adjasent to what we try to solve now.
                InvalidCodePath;
            }break;
        }
        
        AddFinishStateCommand(Cube, FuncState);
    }
    
    // NOTE(Dima): Actual solving
    if(CanStartSolvingState(Cube, FuncState))
    {
        b32 Solved = SideCenterIsSolved(Cube, RubSide_Up);
        
        if(Solved)
        {
            AddFinishStateCommand(Cube, ExitState);
        }
        else
        {
            AddFinishStateCommand(Cube, FuncState);
        }
    }
}

INTERNAL_FUNCTION void SolveSidesCenters(rubiks_cube* Cube)
{
    if(CanStartSolvingState(Cube, RubState_SolvingCenters))
    {
        int UnsolvedSide = FindFirstSideWithUnsolvedCenter(Cube);
        
        if(UnsolvedSide == -1)
        {
            // NOTE(Dima): All sides are solved
            AddFinishStateCommand(Cube, RubState_SolvingEdgesCenters);
        }
        else
        {
            AddFinishStateCommand(Cube, RubState_SolvingCenters_SolveWhite);
        }
    }
    
    // NOTE(Dima): White
    SolveSideCenter(Cube,
                    RubState_SolvingCenters_SolveWhite,
                    RubState_SolvingCenters_SolveWhite_ToUp,
                    RubState_SolvingCenters_SolveWhite_GreenToFront,
                    RubState_SolvingCenters_SolveWhite_Func,
                    RubState_SolvingCenters_SolveYellow,
                    RubiksColor_White,
                    RubiksColor_Green);
    
    // NOTE(Dima): Yellow
    SolveSideCenter(Cube,
                    RubState_SolvingCenters_SolveYellow,
                    RubState_SolvingCenters_SolveYellow_ToUp,
                    RubState_SolvingCenters_SolveYellow_GreenToFront,
                    RubState_SolvingCenters_SolveYellow_Func,
                    RubState_SolvingCenters_SolveGreen,
                    RubiksColor_Yellow,
                    RubiksColor_Green);
    
    // NOTE(Dima): Green
    SolveSideCenter(Cube,
                    RubState_SolvingCenters_SolveGreen,
                    RubState_SolvingCenters_SolveGreen_ToUp,
                    RubState_SolvingCenters_SolveGreen_OrangeToFront,
                    RubState_SolvingCenters_SolveGreen_Func,
                    RubState_SolvingCenters_SolveOrange,
                    RubiksColor_Green,
                    RubiksColor_Orange);
    
    // NOTE(Dima): Orange
    SolveSideCenter(Cube,
                    RubState_SolvingCenters_SolveOrange,
                    RubState_SolvingCenters_SolveOrange_ToUp,
                    RubState_SolvingCenters_SolveOrange_BlueToFront,
                    RubState_SolvingCenters_SolveOrange_Func,
                    RubState_SolvingCenters_SolveBlue,
                    RubiksColor_Orange,
                    RubiksColor_Blue);
    
    // NOTE(Dima): Blue
    SolveSideCenter(Cube,
                    RubState_SolvingCenters_SolveBlue,
                    RubState_SolvingCenters_SolveBlue_ToUp,
                    RubState_SolvingCenters_SolveBlue_RedToFront,
                    RubState_SolvingCenters_SolveBlue_Func,
                    RubState_SolvingCenters,
                    RubiksColor_Blue,
                    RubiksColor_Red);
    
}

INTERNAL_FUNCTION void SolveEdgeCenters(rubiks_cube* Cube)
{
    if(CanStartSolvingState(Cube, RubState_SolvingEdgesCenters))
    {
        int UnsolvedEdge = FindFirstEdgeWithUnsolvedCenter(Cube);
        
        if(UnsolvedEdge == -1)
        {
            // NOTE(Dima): No unsolved edges are left - Go agead to solve rest of the cube
            AddFinishStateCommand(Cube, RubState_MoveCenters_Green);
        }
        else
        {
            // NOTE(Dima): Need to solve edges
            switch(UnsolvedEdge)
            {
                case RubEdge_UpFront:
                {
                    
                }break;
                
                case RubEdge_UpRight:
                {
                    
                }break;
                
                case RubEdge_UpBack:
                {
                    
                }break;
                
                case RubEdge_UpLeft:
                {
                    
                }break;
                
                case RubEdge_DownFront:
                {
                    
                }break;
                
                case RubEdge_DownRight:
                {
                    
                }break;
                
                case RubEdge_DownBack:
                {
                    
                }break;
                
                case RubEdge_DownLeft:
                {
                    
                }break;
                
                case RubEdge_FrontRight:
                {
                    
                }break;
                
                case RubEdge_RightBack:
                {
                    
                }break;
                
                case RubEdge_BackLeft:
                {
                    
                }break;
                
                case RubEdge_LeftFront:
                {
                    
                }break;
            }
            
            AddFinishStateCommand(Cube, RubState_SolvingEdgesCenters_Func);
        }
    }
}

INTERNAL_FUNCTION void SolveCube(rubiks_cube* Cube)
{
    if(Cube->SolvingState != RubState_Solved)
    {
        SolveSidesCenters(Cube);
        
        SolveEdgeCenters(Cube);
        
        MoveSideCentersToRightPlace(Cube);
        
        MoveEdgeCentersToRightPlace(Cube);
        
        SolveFirstLayer(Cube);
        
        SolveSecondLayer(Cube);
        
        SolveYellowCross(Cube);
        
        SolveYellowEdges(Cube);
        
        PlaceYellowCorners(Cube);
        
        OrientYellowCorners(Cube);
    }
}