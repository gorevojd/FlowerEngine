INTERNAL_FUNCTION inline rc2 GetBarRectHorz(rc2 TotalRect, int BarsCount, int BarIndex){
    float WidthPerOneBar = GetWidth(TotalRect) / (float)BarsCount;
    
    float TargetX = TotalRect.Min.x + (float)BarIndex * WidthPerOneBar;
    
    rc2 Result = RectMinDim(V2(TargetX, TotalRect.Min.y), 
                            V2(WidthPerOneBar, GetHeight(TotalRect)));
    
    return(Result);
}

INTERNAL_FUNCTION void PrintWebForBaredGraph(int BarsCount,
                                             rc2 TotalRect,
                                             v4 Color)
{
    float WidthPerOneBar = GetDim(TotalRect).x / (float)BarsCount;
    
    // NOTE(Dima): Printing black web
    PushRectOutline(TotalRect, 2, Color);
    PushRect(TotalRect, UIGetColor(UIColor_GraphBackground));
    
    for(int Index = 1; Index < BarsCount; Index++)
    {
        float Target = TotalRect.Min.x + (float)Index * WidthPerOneBar;
        
        PushLine2D(V2(Target, TotalRect.Min.y), V2(Target, TotalRect.Max.y), 1, Color);
    }
    
}

INTERNAL_FUNCTION void ShowFramesSlider()
{
    // NOTE(Dima): Showing graphs
    if (Button("REC"))
    {
        Global_Debug->IsRecording = !Global_Debug->IsRecording;
        Global_Debug->RecordingChangeRequested = true;
    }
    
    SameLine();
    
    if(Button("New"))
    {
        Global_Debug->ViewFrameIndex = Global_Debug->NewestFrameIndex;
    }
    
    SameLine();
    
    if(Button("Old"))
    {
        Global_Debug->ViewFrameIndex = Global_Debug->OldestFrameIndex;
    }
    
    ui_element* Elem = UIBeginElement("FramesSlider", UIElement_Cached);
    ui_layout* Layout = GetCurrentLayout();
    
    if(UIElementIsOpenedInTree(Elem))
    {
        PreAdvance();
        f32 DimX = UVToScreenPoint(1.0f, 0.0f).x - Layout->At.x;
        
        float ScaledAsc = GetLineBase();
        v2 At = Layout->At - V2(0.0f, ScaledAsc);
        rc2 SliderRectInit = RectMinDim(At, V2(DimX, ScaledAsc * 4.0f));
        rc2 SliderRect = GrowRectByPixels(SliderRectInit, -5);
        
        ui_interaction Interaction = CreateInteraction(Elem, InteractionPriority_Avg);
        
        ProcessMouseKeyInteractionInRect(&Interaction, KeyMouse_Left, SliderRect);
        
        for(int BarIndex = 0;
            BarIndex < DEBUG_PROFILED_FRAMES_COUNT;
            BarIndex++)
        {
            rc2 BarRc = GetBarRectHorz(SliderRect, DEBUG_PROFILED_FRAMES_COUNT,
                                       BarIndex);
            
            f32 MousePx = GetMouseP().x;
            MousePx = Clamp(MousePx, SliderRect.Min.x, SliderRect.Max.x);
            
            if(MousePx >= BarRc.Min.x && MousePx <= BarRc.Max.x)
            {
                if(InteractionIsActive(&Interaction))
                {
                    if((BarIndex != Global_Debug->CollationFrameIndex))
                    {
                        Global_Debug->ViewFrameIndex = BarIndex;
                    }
                }
            }
        }
        
        // NOTE(Dima): Printing black web
        PrintWebForBaredGraph(DEBUG_PROFILED_FRAMES_COUNT,
                              SliderRect, UIGetColor(UIColor_Borders));
        
        // NOTE(Dima): Printing collation frame bar
        PushRect(GetBarRectHorz(SliderRect, 
                                DEBUG_PROFILED_FRAMES_COUNT, 
                                Global_Debug->CollationFrameIndex),
                 UIGetColor(UIColor_GraphFrameCollation));
        
        // NOTE(Dima): Printing newest frame bar
        PushRect(GetBarRectHorz(SliderRect, 
                                DEBUG_PROFILED_FRAMES_COUNT, 
                                Global_Debug->NewestFrameIndex),
                 UIGetColor(UIColor_GraphFrameNew));
        
        // NOTE(Dima): Printing oldest frame bar
        PushRect(GetBarRectHorz(SliderRect, 
                                DEBUG_PROFILED_FRAMES_COUNT, 
                                Global_Debug->OldestFrameIndex),
                 UIGetColor(UIColor_GraphFrameOld));
        
        // NOTE(Dima): Printing viewing frame bar
        PushRect(GetBarRectHorz(SliderRect, 
                                DEBUG_PROFILED_FRAMES_COUNT, 
                                Global_Debug->ViewFrameIndex),
                 UIGetColor(UIColor_GraphFrameView));
        
        DescribeElement(SliderRectInit);
    }
    
    UIEndElement(UIElement_Cached);
    
    // NOTE(Dima): Viewing frame time
    debug_common_frame* FrameCommon = GetFrameByIndex(Global_Debug->ViewFrameIndex);
    ShowText("View frame time: %.2fms", FrameCommon->FrameTime * 1000.0f);
    SameLine();
    ShowText("FPS: %.0f", 1.0f / FrameCommon->FrameTime);
}

INTERNAL_FUNCTION void 
FillAndSortStats(debug_state* State, 
                 debug_thread_frame* Frame, 
                 b32 IncludingChildren)
{
    // NOTE(Dima): Filling to sort table
    debug_timing_stat* Stat = Frame->StatUse.Next;
    
    Frame->ToSortStatsCount = 0;
    
    for(int StatIndex = 0;
        StatIndex < DEBUG_STATS_TO_SORT_SIZE;
        StatIndex++)
    {
        if(Stat == &Frame->StatUse){
            break;
        }
        
        ++Frame->ToSortStatsCount;
        
        Frame->ToSortStats[StatIndex] = Stat;
        
        Stat = Stat->Next;
    }
    
    // NOTE(Dima): Sorting ToSort table by selection sort
    for(int i = 0; i < Frame->ToSortStatsCount - 1; i++){
        u64 MaxValue = GetClocksFromStat(Frame->ToSortStats[i], IncludingChildren);
        int MaxIndex = i;
        
        for(int j = i + 1; j < Frame->ToSortStatsCount; j++){
            u64 CurClocks = GetClocksFromStat(Frame->ToSortStats[j], IncludingChildren);
            if(CurClocks > MaxValue){
                MaxValue = CurClocks;
                MaxIndex = j;
            }
        }
        
        if(MaxIndex != i){
            debug_timing_stat* Temp = Frame->ToSortStats[i];
            Frame->ToSortStats[i] = Frame->ToSortStats[MaxIndex];
            Frame->ToSortStats[MaxIndex] = Temp;
        }
    }
}

inline f32 GetFrameAncientAgeOld(int FrameIndex)
{
    int AgeInteger = 0;
    int ColFrame = Global_Debug->CollationFrameIndex;
    
    if(FrameIndex > ColFrame)
    {
        AgeInteger = FrameIndex - ColFrame;
    }
    else
    {
        AgeInteger = FrameIndex + DEBUG_PROFILED_FRAMES_COUNT - ColFrame;
    }
    
    AgeInteger = DEBUG_PROFILED_FRAMES_COUNT - AgeInteger;
    
    f32 Result = Clamp01((f32)AgeInteger / (f32)DEBUG_PROFILED_FRAMES_COUNT);
    
    return(Result);
}

INTERNAL_FUNCTION void ProfileShowGraph(char* Name, float* Values, float MaxValue,
                                        v4 NewColor = UIGetColor(UIColor_GraphFrameNew),
                                        v4 OldColor = UIGetColor(UIColor_GraphFrameOld))
{
    FUNCTION_TIMING();
    
    // NOTE(Dima): Frames slider
    ui_element* Elem = UIBeginElement("FramesSlider", UIElement_Cached);
    ui_layout* Layout = GetCurrentLayout();
    
    if(UIElementIsOpenedInTree(Elem))
    {
        PreAdvance();
        
        float ScaledAsc = GetLineBase();
        
        v2 At = Layout->At - V2(0.0f, ScaledAsc);
        v2 Dim = V2(ScaledAsc * UI_GRAPH_WIDTH_UNITS, 
                    ScaledAsc * UI_GRAPH_HEIGHT_SMALL_UNITS);
        rc2 GraphRect = RectMinDim(At, Dim);
        
        PushRect(GraphRect, UIGetColor(UIColor_GraphBackground));
        
        f32 OneFramePixels = Dim.x / (f32)DEBUG_PROFILED_FRAMES_COUNT;
        f32 ValueToPixelsScale = Dim.y / MaxValue;
        b32 LineJustStarted = true;
        v2 LastPoint = V2(0.0f, 0.0f);
        b32 LastWasGreaterThanMax = false;
        
        for(int FrameIndex = 0;
            FrameIndex < DEBUG_PROFILED_FRAMES_COUNT;
            FrameIndex++)
        {
            
            if(FrameIndex != Global_Debug->CollationFrameIndex)
            {
                f32 CurrentValue = Values[FrameIndex];
                
                b32 CurGreaterThanMax = false;
                if(CurrentValue > MaxValue)
                {
                    CurrentValue = MaxValue;
                    CurGreaterThanMax = true;
                }
                
                f32 LinePointX = At.x + (f32)FrameIndex * OneFramePixels;
                f32 LinePointY = GraphRect.Max.y - CurrentValue * ValueToPixelsScale;
                
                f32 Age = GetFrameAncientAgeOld(FrameIndex);
                v4 LineColor = Lerp(NewColor, OldColor, Age);
                
                v2 CurrentP = V2(LinePointX, LinePointY);
                
                b32 LineIsAboveGraph = ((CurGreaterThanMax == LastWasGreaterThanMax) && CurGreaterThanMax);
                
                if(!LineJustStarted && !LineIsAboveGraph)
                {
                    PushLine2D(CurrentP, LastPoint, Lerp(4, 2, Age), LineColor);
                }
                else{
                    LineJustStarted = false;
                }
                
                LastWasGreaterThanMax = CurGreaterThanMax;
                
                LastPoint = CurrentP;
            }
            else
            {
                v2 ViewFrameCenter = V2(At.x + ((f32)Global_Debug->ViewFrameIndex + 0.5f) * OneFramePixels,
                                        GetCenter(GraphRect).y);
                v2 ViewFrameDim = V2(OneFramePixels, Dim.y);
                rc2 ViewFrameRect = RectCenterDim(ViewFrameCenter, ViewFrameDim);
                
                PushRect(ViewFrameRect, V4(UIGetColor(UIColor_GraphFrameView).rgb, 0.6f));
                
                LastWasGreaterThanMax = false;
                LineJustStarted = true;
            }
        }
        
        PrintTextAligned(Name, At, 
                         TextAlign_Left,
                         TextAlign_Top,
                         UIGetColor(UIColor_Text));
        
        PushRectOutline(GraphRect, 2, UIGetColor(UIColor_Borders));
        
        DescribeElement(GraphRect);
    }
    
    UIEndElement(UIElement_Cached);
}

INTERNAL_FUNCTION void ListTopClocks()
{
    FUNCTION_TIMING();
    
    debug_state* State = Global_Debug;
    
    debug_thread_frame* MainFrame = GetThreadFrameByIndex(State->MainThread,
                                                          State->ViewFrameIndex);
    debug_thread_frame* Frame = GetThreadFrameByIndex(State->WatchThread, 
                                                      State->ViewFrameIndex);
    debug_common_frame* FrameCommon = GetFrameByIndex(State->ViewFrameIndex);
    
    ui_element* Elem = UIBeginElement("ListTopClocks", UIElement_Cached);
    ui_layout* Layout = GetCurrentLayout();
    
    if(MainFrame->FrameUpdateNode)
    {
        if(UIElementIsOpenedInTree(Elem))
        {
            PreAdvance();
            
            f32 ScaledAsc = GetLineBase();
            v2 At = Layout->At - V2(0.0f, ScaledAsc);
            v2 Dim = V2(ScaledAsc * UI_GRAPH_WIDTH_UNITS, 
                        ScaledAsc * UI_GRAPH_HEIGHT_BIG_UNITS);
            rc2 GraphRect = RectMinDim(At, Dim);
            
            PushRect(GraphRect, UIGetColor(UIColor_GraphBackground));
            
            {
                FillAndSortStats(State, Frame, Global_Debug->Menus.IncludingChildren);
                
                u64 FrameClocksElapsed_ = MainFrame->FrameUpdateNode->TimingSnapshot.ClocksElapsed;
                f32 FrameClocksElapsed = (f32)FrameClocksElapsed_;
                
                
                UIPushScale(0.75f);
                v2 TextAt = At + V2(0.0f, GetLineBase());
                for(int StatIndex = 0; 
                    StatIndex < Frame->ToSortStatsCount; 
                    StatIndex++)
                {
                    debug_timing_stat* Stat = Frame->ToSortStats[StatIndex];
                    
                    u64 ToShowClocks = GetClocksFromStat(Stat, Global_Debug->Menus.IncludingChildren);
                    
                    f32 CoveragePercentage = 100.0f * (f32)ToShowClocks / FrameClocksElapsed;
                    
                    f32 ApproxInFrameTime = CoveragePercentage * FrameCommon->FrameTime * 0.01f;
                    
                    char StatName[256];
                    DEBUGParseNameFromUnique(StatName, 256, Stat->UniqueName);
                    int StatID = StringHashFNV(StatName);
                    
                    char StatBuf[256];
                    stbsp_sprintf(StatBuf, "%11lluc %8.2f%%  %5.2f ms  %8uh %-30s",
                                  ToShowClocks,
                                  CoveragePercentage,
                                  ApproxInFrameTime * 1000.0f,
                                  Stat->Stat.HitCount,
                                  StatName);
                    
                    rc2 ThisTextRect = GetTextRect(StatBuf, TextAt);
                    
                    // NOTE(Dima): Processing interaction
                    ui_interaction Interaction = CreateInteraction(Elem, InteractionPriority_Avg);
                    
                    ProcessMouseKeyInteractionInRect(&Interaction, KeyMouse_Left, ThisTextRect);
                    
                    v4 TextColor = UIGetColor(UIColor_Text);
                    if(Interaction.WasHotInInteraction)
                    {
                        TextColor = UIGetColor(UIColor_TextHot);
                        
                        if(Interaction.WasActiveInInteraction)
                        {
                            if(Global_Debug->Menus.TopClocks_SelectedStatID == StatID)
                            {
                                Global_Debug->Menus.TopClocks_SelectedStatID = -1;
                            }
                            else
                            {
                                Global_Debug->Menus.TopClocks_SelectedStatID = StatID;
                            }
                        }
                    }
                    
                    // NOTE(Dima): If stat is selected
                    b32 IsSelectedStat = StatID == Global_Debug->Menus.TopClocks_SelectedStatID;
                    if(IsSelectedStat)
                    {
                        rc2 LineRect = RectMinDim(ThisTextRect.Min, 
                                                  V2(Dim.x, GetDim(ThisTextRect).y));
                        
                        PushRect(LineRect, UIGetColor(UIColor_ButtonBackground));
                    }
                    
                    PrintText(StatBuf, TextAt, TextColor);
                    TextAt.y += GetLineAdvance();
                    
                    if(TextAt.y - GetLineBase() > GraphRect.Max.y)
                    {
                        break;
                    }
                }
                
                UIPopScale();
            }
            
            PushRectOutline(GraphRect, 2, UIGetColor(UIColor_Borders));
            
            DescribeElement(GraphRect);
        }
        
        
    }
    
    UIEndElement(UIElement_Cached);
}

INTERNAL_FUNCTION void ShowTopFunctionsGraph()
{
    
}

INTERNAL_FUNCTION void ShowTopClocks()
{
    FUNCTION_TIMING();
    
    debug_state* State = Global_Debug;
    
    debug_thread_frame* MainFrame = GetThreadFrameByIndex(State->MainThread,
                                                          State->ViewFrameIndex);
    debug_thread_frame* Frame = GetThreadFrameByIndex(State->WatchThread, 
                                                      State->ViewFrameIndex);
    debug_common_frame* FrameCommon = GetFrameByIndex(State->ViewFrameIndex);
    
    
    StepLittleY();
    BoolButton("Including Children", &Global_Debug->Menus.IncludingChildren);
    
    ListTopClocks();
    
    if(State->Menus.TopClocks_SelectedStatID != -1)
    {
#if 0
        ProfileShowGraph("Frame (ms)", FrameTimes, 60);
        SameColumn();
        ProfileShowGraph("FPS", FPSValues, 400);
#endif
    }
    else
    {
        ShowTopFunctionsGraph();
    }
    
#if 0        
    
    if(State->Menus.SelectedStatGUID)
    {
        if(ImGui::TreeNode("SelectedFunction"))
        {
            char* GUID = State->Menus.SelectedStatGUID;
            debug_timing_stat* Stat = CreateOrFindStatForUniqueName(State, Frame, GUID, false);
            
            char StatName[256];
            DEBUGParseNameFromUnique(StatName, 256, 
                                     GUID);
            
            {
                u64 IncludeChildrenClocks = GetClocksFromStat(Stat, true);
                u64 ExcludeChildrenClocks = GetClocksFromStat(Stat, false);
                
                f32 IncludeChildrenPercent = 100.0f * (f32)IncludeChildrenClocks / FrameClocksElapsed;
                f32 ExcludeChildrenPercent = 100.0f * (f32)ExcludeChildrenClocks / FrameClocksElapsed;
                
                f32 IncludeChildrenTime = IncludeChildrenPercent * FrameCommon->FrameTime * 0.01f * 1000.0f;
                f32 ExcludeChildrenTime = ExcludeChildrenPercent * FrameCommon->FrameTime * 0.01f * 1000.0f;
                
                // NOTE(Dima): Outputing
                Text("Hit count: %d", Stat->Stat.HitCount);
                Text("Include children: %5.2fms, %5.2f%%", 
                     IncludeChildrenTime,
                     IncludeChildrenPercent);
                Text("Exclude children: %5.2fms, %5.2f%%", 
                     ExcludeChildrenTime,
                     ExcludeChildrenPercent);
            }
            
            {
                
                float* ValuesIncl = State->Menus.SelectedFunFloatsIncl;
                float* ValuesExcl = State->Menus.SelectedFunFloatsExcl;
                
                // NOTE(Dima): Filling graph
                for(int FrameIndex = 0;
                    FrameIndex < DEBUG_PROFILED_FRAMES_COUNT;
                    FrameIndex++)
                {
                    debug_thread_frame* CurFrame = GetThreadFrameByIndex(State->WatchThread, FrameIndex);
                    
                    debug_timing_stat* Stat = CreateOrFindStatForUniqueName(State, CurFrame, GUID, false);
                    
                    if(Stat)
                    {
                        f32 InclPerc = (f32)GetClocksFromStat(Stat, true) / FrameClocksElapsed;
                        f32 ExclPerc = (f32)GetClocksFromStat(Stat, false) / FrameClocksElapsed;
                        
                        f32 InclTime = InclPerc * FrameCommon->FrameTime * 1000.0f;
                        f32 ExclTime = ExclPerc * FrameCommon->FrameTime * 1000.0f;
                        
                        ValuesIncl[FrameIndex] = InclTime;
                        ValuesExcl[FrameIndex] = ExclTime;
                    }
                    else
                    {
                        ValuesIncl[FrameIndex] = 0.0f;
                        ValuesExcl[FrameIndex] = 0.0f;
                    }
                }
                
                int ValuesOffset = GetValuesOffsetForGraph(State);
                
                ProfileShowGraph("Time(ms) Incl", ValuesIncl, StatName);
                ProfileShowGraph("Time(ms) Excl", ValuesExcl, StatName);
            }
            
            ImGui::TreePop();
        }
    }
#endif
}

INTERNAL_FUNCTION void ShowFPSGraph()
{
    FUNCTION_TIMING();
    
    debug_state* State = Global_Debug;
    
    float* FrameTimes = State->Menus.FPSGraph_FrameTimes;
    float* FPSValues = State->Menus.FPSGraph_FPSValues;
    
    // NOTE(Dima): Setting Values
    for(int FrameIndex = 0; 
        FrameIndex < DEBUG_PROFILED_FRAMES_COUNT;
        FrameIndex++)
    {
        debug_thread_frame* Frame = GetThreadFrameByIndex(State->MainThread, 
                                                          FrameIndex);
        
        debug_common_frame* CommonFrame = GetFrameByIndex(FrameIndex);
        
        float FrameTime = CommonFrame->FrameTime;
        float FpsValue = 0.0f;
        
        if(FrameTime > 0.00001f)
        {
            FpsValue = 1.0f / FrameTime;
        }
        
        if(FrameIndex == Global_Debug->CollationFrameIndex)
        {
            FrameTime = 0.0f;
            FpsValue = 0.0f;
        }
        
        FrameTimes[FrameIndex] = FrameTime * 1000.0f;
        FPSValues[FrameIndex] = FpsValue;
    }
    
    ProfileShowGraph("FrameTimes(ms)", FrameTimes, 60);
    
    SameLine();
    
    ProfileShowGraph("FPS", FPSValues, 400);
}

INTERNAL_FUNCTION void DEBUGShowOverlays()
{
    FUNCTION_TIMING();
    
    debug_state* Deb = Global_Debug;
    
    if(BeginLayout("MainLayout"))
    {
        char Buf[256];
        stbsp_sprintf(Buf, "Last frame: %.2fms, %.0fhz##Profile",
                      Global_Time->DeltaTime * 1000.0f,
                      1.0f / Global_Time->DeltaTime);
        
        if(TreeNode(Buf))
        {
            // NOTE(Dima): Frame slider
            ShowFramesSlider();
            
            ShowFPSGraph();
            
            ShowTopClocks();
            
            TreePop();
        }
        
        EndLayout();
    }
}