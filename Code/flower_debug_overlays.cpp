INTERNAL_FUNCTION void ProfileShowGraph(char* Name, float* Values, char* Overlay = 0)
{
    
#if 0    
    ImVec2 Size(0.0f, Global_Debug->Menus.GraphsSizeY);
    
    ImGui::PlotLines(Name, Values, 
                     GetCountFramesForGraph(),
                     GetValuesOffsetForGraph(Global_Debug),
                     Overlay, 
                     0.0f, FLT_MAX, Size);
#endif
    
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


INTERNAL_FUNCTION void DEBUGShowTopClocks()
{
    debug_state* State = Global_Debug;
    
    debug_thread_frame* MainFrame = GetThreadFrameByIndex(State->MainThread,
                                                          State->ViewFrameIndex);
    debug_thread_frame* Frame = GetThreadFrameByIndex(State->WatchThread, 
                                                      State->ViewFrameIndex);
    debug_common_frame* FrameCommon = GetFrameByIndex(State, State->ViewFrameIndex);
    
#if 0
    ImGui::Checkbox("IncludingChildren", &Global_Debug->Menus.IncludingChildren);
    
    if(ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        if(Global_Debug->Menus.IncludingChildren)
        {
            ImGui::TextUnformatted("Use that to see clocks usage without children. :)");
        }
        else
        {
            ImGui::TextUnformatted("Use that to see total clocks usage. :)");
        }
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
#endif
    
#if 1
    if(MainFrame->FrameUpdateNode)
    {
        FillAndSortStats(State, Frame, Global_Debug->Menus.IncludingChildren);
        
        u64 FrameClocksElapsed_ = MainFrame->FrameUpdateNode->TimingSnapshot.ClocksElapsed;
        f32 FrameClocksElapsed = (f32)FrameClocksElapsed_;
        
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
            
            char StatBuf[256];
            stbsp_sprintf(StatBuf, "%11lluc %8.2f%%  %5.2f ms  %8uh %-30s",
                          ToShowClocks,
                          CoveragePercentage,
                          ApproxInFrameTime * 1000.0f,
                          Stat->Stat.HitCount,
                          StatName);
            
            b32 IsSelectedStat = false;
            if(State->Menus.SelectedStatGUID)
            {
                IsSelectedStat = StringsAreEqual(State->Menus.SelectedStatGUID, Stat->UniqueName);
            }
            
            // NOTE(Dima): Text
            ShowTextUnformatted(StatBuf);
            
#if 0
            // TODO(Dima): If clicked on this stat then execute this code
            if(IsSelectedStat)
            {
                State->Menus.SelectedStatGUID = 0;
            }
            else
            {
                State->Menus.SelectedStatGUID = Stat->UniqueName;
            }
#endif
        }
    }
#endif
    
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
}
#endif
}

INTERNAL_FUNCTION void DEBUGShowFPSGraph()
{
    debug_state* State = Global_Debug;
    
    float* FrameTimes = State->Menus.FPSGraph_FrameTimes;;
    float* FPSValues = State->Menus.FPSGraph_FPSValues;
    
    // NOTE(Dima): Setting Values
    for(int FrameIndex = 0; 
        FrameIndex < DEBUG_PROFILED_FRAMES_COUNT;
        FrameIndex++)
    {
        debug_thread_frame* Frame = GetThreadFrameByIndex(State->MainThread, 
                                                          FrameIndex);
        
        debug_common_frame* CommonFrame = GetFrameByIndex(State, FrameIndex);
        
        float FrameTime = CommonFrame->FrameTime;
        FrameTimes[FrameIndex] = FrameTime * 1000.0f;
        FPSValues[FrameIndex] = 0.0f;
        
        if(FrameTime > 0.00001f)
        {
            FPSValues[FrameIndex] = 1.0f / FrameTime;
        }
    }
    
    
    ProfileShowGraph("FrameTimes(ms)", FrameTimes, 0);
    ProfileShowGraph("FPS", FPSValues, 0);
}

INTERNAL_FUNCTION void DEBUGShowOverlays()
{
    FUNCTION_TIMING();
    
    debug_state* Deb = Global_Debug;
    
    if(BeginLayout("MainLayout"))
    {
        ShowText("Last Frame: %.2fms", Global_Time->DeltaTime * 1000.0f);
        SameLine();
        ShowText("Time: %.2fs", Global_Time->Time);
        SameLine();
        ShowText("FPS: %.0f", 1.0f / Global_Time->DeltaTime);
        
        // NOTE(Dima): Showing graphs
        if (Button("REC"))
        {
            Deb->IsRecording = !Deb->IsRecording;
            Deb->RecordingChangeRequested = true;
        }
        
#if 0
        ImGui::SliderFloat("Graph height (Pixels)", &Deb->Menus.GraphsSizeY, 60, 150);
#endif
        
        //DEBUGShowFPSGraph();
        DEBUGShowTopClocks();
        
        EndLayout();
    }
}