#include "prj_minecraft.h"

#define STB_PERLIN_IMPLEMENTATION
#include "stb_perlin.h"

inline minc_block_texture_indices CreateBlockAll(int Index)
{
    minc_block_texture_indices Result = {};
    
    Result.Front = Index;
    Result.Back = Index;
    Result.Left = Index;
    Result.Right = Index;
    Result.Up = Index;
    Result.Down = Index;
    
    return(Result);
}

inline minc_block_texture_indices CreateBlockSide(int SideIndex, int UpdownIndex)
{
    minc_block_texture_indices Result = {};
    
    Result.Front = SideIndex;
    Result.Back = SideIndex;
    Result.Left = SideIndex;
    Result.Right = SideIndex;
    Result.Up = UpdownIndex;
    Result.Down = UpdownIndex;
    
    return(Result);
}

inline minc_block_texture_indices CreateBlockUpDown(int SideIndex, int Up, int Down)
{
    minc_block_texture_indices Result = {};
    
    Result.Front = SideIndex;
    Result.Back = SideIndex;
    Result.Left = SideIndex;
    Result.Right = SideIndex;
    Result.Up = Up;
    Result.Down = Down;
    
    return(Result);
}

inline minc_block_texture_indices CreateBlock(int Front,
                                              int Back,
                                              int Left,
                                              int Right,
                                              int Up,
                                              int Down)
{
    minc_block_texture_indices Result = {};
    
    Result.Front = Front;
    Result.Back = Back;
    Result.Left = Left;
    Result.Right = Right;
    Result.Up = Up;
    Result.Down = Down;
    
    return(Result);
}

INTERNAL_FUNCTION void InitMinecraftBlockTextures(minecraft* Mine)
{
    // NOTE(Dima): Init blocks texture indices
    Mine->BlocksTextureIndices[MincBlock_Ground] = CreateBlockAll(MincTexture_Ground);
    Mine->BlocksTextureIndices[MincBlock_GroundGrass] = CreateBlockUpDown(MincTexture_GroundSide,
                                                                          MincTexture_GroundGrass,
                                                                          MincTexture_Ground);
    Mine->BlocksTextureIndices[MincBlock_Stone] = CreateBlockAll(MincTexture_Stone);
    Mine->BlocksTextureIndices[MincBlock_StoneWeak] = CreateBlockAll(MincTexture_StoneWeak);
    Mine->BlocksTextureIndices[MincBlock_Sand] = CreateBlockAll(MincTexture_Sand);
    Mine->BlocksTextureIndices[MincBlock_Brick] = CreateBlockAll(MincTexture_Brick);
    Mine->BlocksTextureIndices[MincBlock_Bomb] = CreateBlockUpDown(MincTexture_BombSide,
                                                                   MincTexture_BombUp,
                                                                   MincTexture_BombDown);
    
    Mine->BlocksTextureIndices[MincBlock_BlockIron] = CreateBlockAll(MincTexture_BlockIron);
    Mine->BlocksTextureIndices[MincBlock_BlockGold] = CreateBlockAll(MincTexture_BlockGold);
    Mine->BlocksTextureIndices[MincBlock_BlockDiamond] = CreateBlockAll(MincTexture_BlockDiam);
    
    Mine->BlocksTextureIndices[MincBlock_Snow] = CreateBlockAll(MincTexture_GroundSnow);
    Mine->BlocksTextureIndices[MincBlock_SnowGround] = CreateBlockUpDown(MincTexture_GroundSnowSide,
                                                                         MincTexture_GroundSnow,
                                                                         MincTexture_Ground);
    
    Mine->BlocksTextureIndices[MincBlock_TreeWood] = CreateBlockSide(MincTexture_WoodTreeSide,
                                                                     MincTexture_WoodTreeUp);
    Mine->BlocksTextureIndices[MincBlock_TreeWoodBirch] = CreateBlockSide(MincTexture_WoodTreeBirch,
                                                                          MincTexture_WoodTreeUp);
    Mine->BlocksTextureIndices[MincBlock_TreeWoodDark] = CreateBlockSide(MincTexture_WoodTreeDark,
                                                                         MincTexture_WoodTreeUp);
    Mine->BlocksTextureIndices[MincBlock_TreeWoodGrass] = CreateBlockSide(MincTexture_WoodTreeGrass,
                                                                          MincTexture_WoodTreeUp);
    Mine->BlocksTextureIndices[MincBlock_TreeWoodGrassSnow] = CreateBlockSide(MincTexture_WoodTreeGrassSnow,
                                                                              MincTexture_WoodTreeUp);
    
    Mine->BlocksTextureIndices[MincBlock_TreeLeaves] = CreateBlockAll(MincTexture_Leaves);
    Mine->BlocksTextureIndices[MincBlock_TreeLeavesSnow] = CreateBlockAll(MincTexture_LeavesSnow);
}

INTERNAL_FUNCTION inline void MincFaceOffsetOnCube(minecraft* Minecraft,
                                                   int Normal, 
                                                   int A, int B, int C, int D)
{
    minc_offsets_to_vertex Cube[8] =
    {
        {1, 1, 0},
        {0, 1, 0},
        {0, 1, 1},
        {1, 1, 1},
        {1, 0, 0},
        {0, 0, 0},
        {0, 0, 1},
        {1, 0, 1},
    };
    
    minc_face_offsets_to_vertex* Face = &Minecraft->OffsetsToV[Normal];
    
    Face->VertexOffsets[0] = Cube[A];
    Face->VertexOffsets[1] = Cube[B];
    Face->VertexOffsets[2] = Cube[C];
    Face->VertexOffsets[3] = Cube[A];
    Face->VertexOffsets[4] = Cube[C];
    Face->VertexOffsets[5] = Cube[D];
}

INTERNAL_FUNCTION void InitMinecraftTextureOffsets(minecraft* Mine)
{
    MincFaceOffsetOnCube(Mine,
                         MincFaceNormal_Front,
                         2, 3, 7, 6);
    
    MincFaceOffsetOnCube(Mine,
                         MincFaceNormal_Back,
                         0, 1, 5, 4);
    
    MincFaceOffsetOnCube(Mine,
                         MincFaceNormal_Left,
                         3, 0, 4, 7);
    
    MincFaceOffsetOnCube(Mine,
                         MincFaceNormal_Right,
                         1, 2, 6, 5);
    
    MincFaceOffsetOnCube(Mine,
                         MincFaceNormal_Up,
                         3, 2, 1, 0);
    
    MincFaceOffsetOnCube(Mine,
                         MincFaceNormal_Down,
                         4, 5, 6, 7);
}

INTERNAL_FUNCTION inline minc_biome* CreateDefaultBiome(minecraft* Mine, 
                                                        u32 Type, 
                                                        f32 Weight)
{
    minc_biome* Result = &Mine->Biomes[Type];
    
    Result->LayerBlocks[0] = MincBlock_GroundGrass;
    Result->LayerBlocks[1] = MincBlock_Ground;
    Result->LayerBlocks[2] = MincBlock_Stone;
    Result->LayerBlocks[3] = MincBlock_Stone;
    
    Result->StartWeight = Mine->BiomesTotalWeight;
    Result->EndWeight = Mine->BiomesTotalWeight + Weight;
    
    Mine->BiomesTotalWeight += Weight;
    Result->BaseHeight = 64.0f;
    Result->NoiseFrequency = 256.0f;
    Result->NoiseScale = 20.0f;
    
    return(Result);
}

INTERNAL_FUNCTION void InitMinecraftBiomes(minecraft* Mine)
{
    Mine->BiomesTotalWeight = 0.0f;
    
    minc_biome* Desert = CreateDefaultBiome(Mine, MincBiome_Desert, 10);
    Desert->NoiseFrequency = 1024.0f;
    Desert->NoiseScale = 10.0f;
    Desert->BaseHeight = 50.0f;
    Desert->LayerBlocks[0] = MincBlock_Sand;
    Desert->LayerBlocks[1] = MincBlock_Sand;
    Desert->LayerBlocks[2] = MincBlock_Ground;
    Desert->LayerBlocks[3] = MincBlock_Stone;
    
    minc_biome* Standard = CreateDefaultBiome(Mine, MincBiome_Standard, 20);
    Standard->NoiseFrequency = 700.0f;
    Standard->NoiseScale = 20.0f;
    Standard->BaseHeight = 50.0f;
    
    minc_biome* SnowTaiga = CreateDefaultBiome(Mine, MincBiome_SnowTaiga, 20);
    SnowTaiga->NoiseFrequency = 256.0f;
    SnowTaiga->NoiseScale = 40.0f;
    SnowTaiga->BaseHeight = 60.0f;
    SnowTaiga->LayerBlocks[0] = MincBlock_SnowGround;
    
    // NOTE(Dima): Backpropagate biome weights
    f32 OneOverTotalWeight = 1.0f / Mine->BiomesTotalWeight;
    for(int BiomeIndex = 0;
        BiomeIndex < MincBiome_Count;
        BiomeIndex++)
    {
        minc_biome* Biome = Mine->Biomes + BiomeIndex;
        
        Biome->StartWeight *= OneOverTotalWeight;
        Biome->EndWeight *= OneOverTotalWeight;
    }
}

#define GET_BLOCK_INDEX_IN_CHUNK(x, y, z) (y * (MINC_CHUNK_WIDTH * MINC_CHUNK_WIDTH) + z * MINC_CHUNK_WIDTH + x)

INTERNAL_FUNCTION inline int MincGetBlockIndex(int X, int Y, int Z)
{
    int Result = GET_BLOCK_INDEX_IN_CHUNK(X, Y, Z);
    
    return(Result);
}

INTERNAL_FUNCTION inline u8 GetBlockInChunk(minc_chunk* Chunk,
                                            int X, int Y, int Z)
{
    int Index = GET_BLOCK_INDEX_IN_CHUNK(X, Y, Z);
    
    u8 Result = Chunk->Blocks[Index];
    
    return(Result);
}

struct minc_generate_chunks
{
    minc_chunk* Dst;
    
    minc_chunk* SideChunks[MincFaceNormal_Count];
};

INTERNAL_FUNCTION void GenerateChunkMesh(minecraft* Minecraft,
                                         minc_temp_mesh* Mesh,
                                         minc_generate_chunks Chunks)
{
    minc_chunk* Chunk = Chunks.Dst;
    
    Mesh->VerticesCount = 0;
    Mesh->FaceCount = 0;
    
    for(int y = 0; y < MINC_CHUNK_HEIGHT; y++)
    {
        for(int z = 0; z < MINC_CHUNK_WIDTH; z++)
        {
            for(int x = 0; x < MINC_CHUNK_WIDTH; x++)
            {
                int BlockIndex = GET_BLOCK_INDEX_IN_CHUNK(x, y, z);
                
                u8 BlockType = Chunk->Blocks[BlockIndex];
                
                b32 IsOuterX = (x == 0) || (x == MINC_CHUNK_WIDTH - 1);
                b32 IsOuterY = (y == 0) || (y == MINC_CHUNK_HEIGHT - 1);
                b32 IsOuterZ = (z == 0) || (z == MINC_CHUNK_WIDTH - 1);
                
                b32 BlockIsOuter = IsOuterX || IsOuterY || IsOuterZ;
                
                if(BlockType != MincBlock_Empty)
                {
                    for(int NormalDir = 0;
                        NormalDir < MincFaceNormal_Count;
                        NormalDir++)
                    {
                        u8 NeighbourBlock = MincBlock_Empty;
                        
                        minc_chunk* SideChunk = Chunks.SideChunks[NormalDir];
                        
                        switch(NormalDir)
                        {
                            case MincFaceNormal_Left:
                            {
                                if(x + 1 < MINC_CHUNK_WIDTH)
                                {
                                    NeighbourBlock = GetBlockInChunk(Chunk, x + 1, y, z);
                                }
                                else
                                {
                                    if(SideChunk)
                                    {
                                        NeighbourBlock = GetBlockInChunk(SideChunk, 0, y, z);
                                    }
                                }
                            }break;
                            
                            case MincFaceNormal_Right:
                            {
                                if(x - 1 >= 0)
                                {
                                    NeighbourBlock = GetBlockInChunk(Chunk, x - 1, y, z);
                                }
                                else
                                {
                                    if(SideChunk)
                                    {
                                        NeighbourBlock = GetBlockInChunk(SideChunk, MINC_CHUNK_WIDTH - 1, y, z);
                                    }
                                }
                            }break;
                            
                            case MincFaceNormal_Front:
                            {
                                if(z + 1 < MINC_CHUNK_WIDTH)
                                {
                                    NeighbourBlock = GetBlockInChunk(Chunk, x, y, z + 1);
                                }
                                else
                                {
                                    if(SideChunk)
                                    {
                                        NeighbourBlock = GetBlockInChunk(SideChunk, x, y, 0);
                                    }
                                }
                            }break;
                            
                            case MincFaceNormal_Back:
                            {
                                if(z - 1 >= 0)
                                {
                                    NeighbourBlock = GetBlockInChunk(Chunk, x, y, z - 1);
                                }
                                else
                                {
                                    if(SideChunk)
                                    {
                                        NeighbourBlock = GetBlockInChunk(SideChunk, x, y, MINC_CHUNK_WIDTH - 1);
                                    }
                                }
                            }break;
                            
                            case MincFaceNormal_Up:
                            {
                                if(y + 1 < MINC_CHUNK_HEIGHT)
                                {
                                    NeighbourBlock = GetBlockInChunk(Chunk, x, y + 1, z);
                                }
                                else
                                {
                                    if(SideChunk)
                                    {
                                        NeighbourBlock = GetBlockInChunk(SideChunk, x, 0, z);
                                    }
                                }
                            }break;
                            
                            case MincFaceNormal_Down:
                            {
                                if(y - 1 >= 0)
                                {
                                    NeighbourBlock = GetBlockInChunk(Chunk, x, y - 1, z);
                                }
                                else
                                {
                                    if(SideChunk)
                                    {
                                        NeighbourBlock = GetBlockInChunk(SideChunk, x, MINC_CHUNK_HEIGHT, z);
                                    }
                                }
                            }break;
                        }
                        
                        if(NeighbourBlock == MincBlock_Empty)
                        {
                            // NOTE(Dima): Generating and setting vertices
                            minc_block_texture_indices* TexIndices = &Minecraft->BlocksTextureIndices[BlockType];
                            int TexIndex = TexIndices->Sides[NormalDir];
                            
                            minc_face_offsets_to_vertex* OffsetToV = &Minecraft->OffsetsToV[NormalDir];
                            
                            for(int VertexIndex = 0;
                                VertexIndex < 6;
                                VertexIndex++)
                            {
                                minc_offsets_to_vertex* Offset = &OffsetToV->VertexOffsets[VertexIndex];
                                
                                u32 Vertex = 0;
                                
                                int VertexX = x + Offset->x;
                                int VertexY = y + Offset->y;
                                int VertexZ = z + Offset->z;
                                
                                Vertex |= (VertexX & 63);
                                Vertex |= (VertexZ & 63) << 6;
                                Vertex |= (VertexY & 255) << 12;
                                
                                Mesh->Vertices[Mesh->VerticesCount++] = Vertex;
                            }
                            
                            // NOTE(Dima): Generating and Setting per face data
                            u32 PerFace = NormalDir;
                            PerFace |= ((TexIndex & 255) << 3);
                            
                            Mesh->PerFaceData[Mesh->FaceCount++] = PerFace;
                        }
                    }
                }
                
            } // Loop x
        } // Loop z
    } // Loop y
    
}

INTERNAL_FUNCTION void MincCopyTempMeshToMesh(voxel_mesh* Mesh, minc_temp_mesh* TempMesh)
{
    Mesh->VerticesCount = TempMesh->VerticesCount;
    Mesh->FaceCount = TempMesh->FaceCount;
    
    mi DataSize = TempMesh->VerticesCount * sizeof(u32) + TempMesh->FaceCount * sizeof(u32);
    if(DataSize > 0)
    {
        
        Mesh->Free = malloc(DataSize);
        
        // NOTE(Dima): Copy vertices;
        memcpy(Mesh->Free, 
               TempMesh->Vertices, 
               TempMesh->VerticesCount * sizeof(u32));
        
        // NOTE(Dima): Copy per face data
        memcpy((u8*)Mesh->Free + Mesh->VerticesCount * sizeof(u32),
               TempMesh->PerFaceData,
               TempMesh->FaceCount * sizeof(u32));
        
        // NOTE(Dima): Reset pointers
        Mesh->Vertices = (u32*)Mesh->Free;
        Mesh->PerFaceData = (u32*)((u8*)Mesh->Free + Mesh->VerticesCount * sizeof(u32));
    }
    else
    {
        Mesh->Vertices = 0;
        Mesh->PerFaceData = 0;
        
        Mesh->Free = 0;
    }
}

INTERNAL_FUNCTION inline void MincSetColumn(minc_chunk* Blocks,
                                            int MinIndex,
                                            int MaxIndex,
                                            int X, int Z,
                                            u8 Block)
{
    int Min = Clamp(MinIndex, 0, MINC_CHUNK_HEIGHT);
    int Max = Clamp(MaxIndex, 0, MINC_CHUNK_HEIGHT);
    
    for(int Index = Min;
        Index <= Max;
        Index++)
    {
        Blocks->Blocks[MincGetBlockIndex(X, Index, Z)] = Block;
    }
}

inline u64 MincGetKey(int X, int Y, int Z)
{
    u64 Key = (u32)X | ((u64)((u32)Z) << 32);
    //u64 Key = (u64)X | ((u64)Z << 32);
    
    return(Key);
}

INTERNAL_FUNCTION inline minc_chunk_meta_slot* MincFindSlot(minecraft* Mine, 
                                                            int X, int Z)
{
    u64 Key = MincGetKey(X, 0, Z);
    u32 KeyHash = Hash32Slow(Key);
    int Index = KeyHash % MINC_META_TABLE_SIZE;
    
    minc_chunk_meta_slot* Result = 0;
    
    minc_chunk_meta_slot* At = Mine->MetaTable[Index];
    while(At)
    {
        if(At->Key == Key)
        {
            Result = At;
            break;
        }
        
        At = At->NextInHash;
    }
    
    return(Result);
}

INTERNAL_FUNCTION inline minc_chunk_meta* MincFindChunkMeta(minecraft* Mine, 
                                                            int X, int Z)
{
    minc_chunk_meta* Meta = 0;
    
    minc_chunk_meta_slot* Slot = MincFindSlot(Mine, X, Z);
    if(Slot)
    {
        Meta = Slot->Meta;
    }
    
    return(Meta);
}

INTERNAL_FUNCTION void CopyChunkMeta(minc_chunk_meta* Dst,
                                     minc_chunk_meta* Src)
{
    for(int i = 0; i < MINC_CHUNK_WIDTH * MINC_CHUNK_WIDTH; i++)
    {
        Dst->BiomeMap[i] = Src->BiomeMap[i];
        Dst->NoiseMap[i] = Src->NoiseMap[i];
        Dst->HeightMap[i] = Src->HeightMap[i];
    }
}

struct minc_generate_maps_work
{
    minc_chunk_meta* Meta;
    minecraft* Mine;
    task_memory* Task;
    
    int X;
    int Z;
};

INTERNAL_FUNCTION PLATFORM_CALLBACK(MincGenerateMapsWork)
{
    minc_generate_maps_work* Work = (minc_generate_maps_work*)Data;
    
    minc_chunk_meta* Meta = Work->Meta;
    minecraft* Mine = Work->Mine;
    
    u8* BiomeMap = Meta->BiomeMap;
    f32* NoiseMap = Meta->NoiseMap;
    u16* HeightMap = Meta->HeightMap;
    
    v3 ChunkWorldP = V3(Work->X * MINC_CHUNK_WIDTH,
                        0.0f,
                        Work->Z * MINC_CHUNK_WIDTH);
    
    for(int z = 0; z < MINC_CHUNK_WIDTH; z++)
    {
        for(int x = 0; x < MINC_CHUNK_WIDTH; x++)
        {
            int TargetIndex = z * MINC_CHUNK_WIDTH + x;
            
            // NOTE(Dima): Generating and finding biome
            f32 BiomeNoiseInit = stb_perlin_noise3((ChunkWorldP.x + x) / 256.0f, 
                                                   0.0f, 
                                                   (ChunkWorldP.z + z) / 256.0f,
                                                   0, 0, 0);
            f32 BiomeNoise = BiomeNoiseInit * 0.5f + 0.5f;
            
            // NOTE(Dima): Finding corresponding biome
            int Biome = -1;
            for(int BiomeIndex = 0;
                BiomeIndex < MincBiome_Count;
                BiomeIndex++)
            {
                minc_biome* CurBiome = &Mine->Biomes[BiomeIndex];
                
                if((BiomeNoise >= CurBiome->StartWeight) && 
                   (BiomeNoise <= CurBiome->EndWeight))
                {
                    // NOTE(Dima): Biome found
                    Biome = BiomeIndex;
                    break;
                }
            }
            
            Assert(Biome != -1);
            
            // NOTE(Dima): Set in biome map
            BiomeMap[TargetIndex] = Biome;
            
            // NOTE(Dima): Generating & Setting noise in the noise map
            minc_biome* CurBiome = Mine->Biomes + Biome;
            f32 TempX = (ChunkWorldP.x + x) / CurBiome->NoiseFrequency;
            f32 TempZ = (ChunkWorldP.z + z) / CurBiome->NoiseFrequency;
            
            f32 Noise = stb_perlin_fbm_noise3(TempX, 0.0f, TempZ, 2.0f, 0.5f, 6);
            NoiseMap[TargetIndex] = Noise;
            
            // NOTE(Dima): Generating height and setting it in height map
            f32 ScaledNoise = Noise * CurBiome->NoiseScale;
            int Height = CurBiome->BaseHeight + (int)ScaledNoise;
            int CurrentHeight = ClampFloat(Height, 0, MINC_CHUNK_HEIGHT - 1);
            HeightMap[TargetIndex] = CurrentHeight;
        }
    }
    
    Meta->State.store(MincChunk_MapsGenerated);
    FreeTaskMemory(Mine->TaskPool, Work->Task);
}

struct minc_fix_gaps_work
{
    minc_chunk_meta* Meta;
    minecraft* Mine;
    task_memory* Task;
    
    int X;
    int Z;
    
    minc_chunk_meta* LookupChunks[9];
};

INTERNAL_FUNCTION PLATFORM_CALLBACK(MincFixBiomeGapsWork)
{
    minc_fix_gaps_work* Work = (minc_fix_gaps_work*)Data;
    
    v3 ChunkWorldP = V3(Work->X * MINC_CHUNK_WIDTH,
                        0.0f,
                        Work->Z * MINC_CHUNK_WIDTH);
    
    minc_chunk_meta* Meta = Work->Meta;
    minecraft* Mine = Work->Mine;
    
    int X = Work->X;
    int Z = Work->Z;
    
    u8* BiomeMap = Meta->BiomeMap;
    f32* NoiseMap = Meta->NoiseMap;
    u16* HeightMap = Meta->HeightMap;
    
    f32 AvgNearHeights[MINC_CHUNK_WIDTH * MINC_CHUNK_WIDTH];
    f32 BiomeNearPerc[MINC_CHUNK_WIDTH * MINC_CHUNK_WIDTH];
    
    // NOTE(Dima): Fixing biome gaps. First - calc avg biome nearby percentage and avg height
    for(int z = 0; z < MINC_CHUNK_WIDTH; z++)
    {
        for(int x = 0; x < MINC_CHUNK_WIDTH; x++)
        {
            int IndexInMap = z * MINC_CHUNK_WIDTH + x;
            
            // NOTE(Dima): Lookup into neighbour cells
            int TotalHeightNear = 0.0f;
            f32 TotalCurBiome = 0.0f;
            
            u8 CurBiome = BiomeMap[IndexInMap];
            
            int CountCheckedNear = 0;
            
            int r = 5;
            for(int a = -r; a <= r; a++)
            {
                int LookupZ = a + z;
                int LookupChunkZ = 1;
                if(LookupZ < 0)
                {
                    LookupZ = MINC_CHUNK_WIDTH + LookupZ;
                    LookupChunkZ--;
                }
                else if(LookupZ >= MINC_CHUNK_WIDTH)
                {
                    LookupZ = LookupZ - MINC_CHUNK_WIDTH;
                    LookupChunkZ++;
                }
                
                for(int b = -r; b <= r; b++)
                {
                    int LookupX = b + x;
                    int LookupChunkX = 1;
                    if(LookupX < 0)
                    {
                        LookupX = MINC_CHUNK_WIDTH + LookupX;
                        LookupChunkX--;
                    }
                    else if(LookupX >= MINC_CHUNK_WIDTH)
                    {
                        LookupX = LookupX - MINC_CHUNK_WIDTH;
                        LookupChunkX++;
                    }
                    
                    minc_chunk_meta* Lookup = Work->LookupChunks[LookupChunkZ * 3 + LookupChunkX];
                    
                    if(Lookup)
                    {
                        int LookupIndexInMap = LookupZ * MINC_CHUNK_WIDTH + LookupX;
                        
                        if(CurBiome == Lookup->BiomeMap[LookupIndexInMap])
                        {
                            TotalCurBiome += 1.0f;
                        }
                        
                        TotalHeightNear += Lookup->HeightMap[LookupIndexInMap];
                        
                        CountCheckedNear++;
                    }
                }
            }
            
            BiomeNearPerc[IndexInMap] = TotalCurBiome / (f32)CountCheckedNear;
            AvgNearHeights[IndexInMap] = (f32)TotalHeightNear / (f32)CountCheckedNear;
        }
    }
    
    for(int z = 0; z < MINC_CHUNK_WIDTH; z++)
    {
        for(int x = 0; x < MINC_CHUNK_WIDTH; x++)
        {
            int IndexInMap = z * MINC_CHUNK_WIDTH + x;
            
            int TargetHeight = HeightMap[IndexInMap];
            if(BiomeNearPerc[IndexInMap] < 0.99f)
            {
                TargetHeight = AvgNearHeights[IndexInMap];
            }
            
            HeightMap[IndexInMap] = TargetHeight;
        }
    }
    
    Meta->State.store(MincChunk_ReadyToGenerateChunk);
    FreeTaskMemory(Mine->TaskPool, Work->Task);
}

struct minc_generate_chunk_work
{
    minecraft* Minecraft;
    task_memory* TaskMemory;
    minc_chunk* Chunk;
    minc_chunk_meta* Meta;
};

INTERNAL_FUNCTION PLATFORM_CALLBACK(MincGenerateChunkWork)
{
    minc_generate_chunk_work* Work = (minc_generate_chunk_work*)Data;
    
    minecraft* Minecraft = Work->Minecraft;
    minc_chunk_meta* Meta = Work->Meta;
    minc_chunk* Chunk = Work->Chunk;
    task_memory* TaskMemory = Work->TaskMemory;
    
    for(int BlockIndex = 0;
        BlockIndex < MINC_CHUNK_COUNT;
        BlockIndex++)
    {
        Chunk->Blocks[BlockIndex] = MincBlock_Empty;
    }
    
    // NOTE(Dima): Heightmaps and all other stuff should be generated
    Assert(Meta);
    
    for(int z = 0; z < MINC_CHUNK_WIDTH; z++)
    {
        for(int x = 0; x < MINC_CHUNK_WIDTH; x++)
        {
            int IndexInMap = z * MINC_CHUNK_WIDTH + x;
            
            minc_biome* Biome = Minecraft->Biomes + Meta->BiomeMap[IndexInMap];
            
            f32 Noise = Meta->NoiseMap[IndexInMap];
            
            // NOTE(Dima): Setting block type at height
            int CurrentHeight = Meta->HeightMap[IndexInMap];
            Chunk->Blocks[MincGetBlockIndex(x, CurrentHeight, z)] = Biome->LayerBlocks[0];
            
            // NOTE(Dima): 
            --CurrentHeight;
            int GroundLayerHeight = std::max(4 + (int)(Noise * 8), 2);
            MincSetColumn(Chunk, 
                          CurrentHeight - (GroundLayerHeight - 1), 
                          CurrentHeight,
                          x, z,
                          Biome->LayerBlocks[1]);
            
            // NOTE(Dima): Setting stones
            CurrentHeight -= GroundLayerHeight;
            MincSetColumn(Chunk,
                          0, CurrentHeight,
                          x, z,
                          Biome->LayerBlocks[3]);
        }
    }
    
    Meta->State = MincChunk_ReadyToGenerateMesh;
    
    FreeTaskMemory(Minecraft->TaskPool, TaskMemory);
}

struct minc_generate_mesh_work
{
    minecraft* Minecraft;
    minc_generate_chunks GenerationChunks;
    task_memory* TaskMemory;
    minc_chunk* Chunk;
    minc_chunk_meta* Meta;
    
    minc_temp_mesh TempMesh;
};

INTERNAL_FUNCTION PLATFORM_CALLBACK(MincGenerateMeshWork)
{
    minc_generate_mesh_work* Work = (minc_generate_mesh_work*)Data;
    
    minecraft* Minecraft = Work->Minecraft;
    task_memory* TaskMemory = Work->TaskMemory;
    minc_chunk* Chunk = Work->Chunk;
    minc_chunk_meta* Meta = Work->Meta;
    
    GenerateChunkMesh(Minecraft, 
                      &Work->TempMesh,
                      Work->GenerationChunks);
    
    MincCopyTempMeshToMesh(&Chunk->Mesh, &Work->TempMesh);
    
    Meta->State.store(MincChunk_MeshGenerated);
    
    FreeTaskMemory(Minecraft->TaskPool, TaskMemory);
}

INTERNAL_FUNCTION inline minc_chunk* MincGetGeneratedChunk(minecraft* Mine, int X, int Z)
{
    minc_chunk_meta_slot* Slot = MincFindSlot(Mine, X, Z);
    
    minc_chunk* Result = 0;
    if(Slot)
    {
        if(Slot->Chunk)
        {
            if(Slot->Meta->State > MincChunk_GeneratingChunk)
            {
                Result = Slot->Chunk;
            }
        }
    }
    
    return(Result);
}

INTERNAL_FUNCTION void UpdateChunkAtIndex(minecraft* Minecraft,
                                          int X, int Z)
{
    minc_chunk_meta_slot* ChunkMetaSlot = MincFindSlot(Minecraft, X, Z);
    
    v3 ChunkWorldP = V3(X * MINC_CHUNK_WIDTH,
                        0.0f,
                        Z * MINC_CHUNK_WIDTH);
    
    if(ChunkMetaSlot)
    {
        minc_chunk* Chunk = ChunkMetaSlot->Chunk;
        minc_chunk_meta* Meta = ChunkMetaSlot->Meta;
        
        switch(Meta->State)
        {
            case MincChunk_Unloaded:
            {
                Assert(Chunk == 0);
                
                // NOTE(Dima): Start generating maps work
                task_memory* Task = GetTaskMemoryForUse(Minecraft->TaskPool, sizeof(minc_generate_maps_work));
                Assert(Task);
                
                minc_generate_maps_work* Work = PushStruct(&Task->Arena, minc_generate_maps_work);
                
                Work->Meta = Meta;
                Work->Mine = Minecraft;
                Work->Task = Task;
                Work->X = X;
                Work->Z = Z;
                
                Meta->State.store(MincChunk_GeneratingMaps);
                KickJob(MincGenerateMapsWork, Work, JobPriority_High);
            }break;
            
            case MincChunk_MapsGenerated:
            {
                b32 CanFix = true;
                
                for(int j = -1; j <= 1; j++)
                {
                    for(int i = -1; i <= 1; i++)
                    {
                        if(!MincFindChunkMeta(Minecraft, X + i, Z + j))
                        {
                            CanFix = false;
                            break;
                        }
                    }
                }
                
                if(CanFix)
                {
                    task_memory* Task = GetTaskMemoryForUse(Minecraft->TaskPool, sizeof(minc_fix_gaps_work));
                    Assert(Task);
                    
                    minc_fix_gaps_work* Work = PushStruct(&Task->Arena, minc_fix_gaps_work);
                    
                    Work->Meta = Meta;
                    Work->Mine = Minecraft;
                    Work->Task = Task;
                    Work->X = X;
                    Work->Z = Z;
                    
                    for(int j = -1; j <= 1; j++)
                    {
                        for(int i = -1; i <= 1; i++)
                        {
                            int LookupIndex = (j + 1) * 3 + (i + 1);
                            Work->LookupChunks[LookupIndex] = 0;
                            
                            minc_chunk_meta* LookupChunkMeta = MincFindChunkMeta(Minecraft, X + i, Z + j);
                            if(LookupChunkMeta)
                            {
                                if(LookupChunkMeta->State >= MincChunk_MapsGenerated)
                                {
                                    Work->LookupChunks[LookupIndex] = LookupChunkMeta;
                                }
                            }
                        }
                    }
                    
                    Meta->State.store(MincChunk_FixingBiomeGaps, std::memory_order_relaxed);
                    KickJob(MincFixBiomeGapsWork, Work, JobPriority_High);
                }
            }break;
            
            case MincChunk_ReadyToGenerateChunk:
            {
                Meta->State.store(MincChunk_GeneratingChunk, std::memory_order_relaxed);
                
                // NOTE(Dima): Allocating chunk
                ChunkMetaSlot->Chunk = (minc_chunk*)malloc(sizeof(minc_chunk));
                Chunk = ChunkMetaSlot->Chunk;
                memset(Chunk->Blocks, 0, MINC_CHUNK_COUNT);
                
                // NOTE(Dima): Setting chunk
                Chunk->CoordX = X;
                Chunk->CoordZ = Z;
                
                // NOTE(Dima): Starting work to generate chunk
                task_memory* Task = GetTaskMemoryForUse(Minecraft->TaskPool, sizeof(minc_generate_chunk_work));
                Assert(Task);
                
                minc_generate_chunk_work* Work = PushStruct(&Task->Arena, minc_generate_chunk_work);
                
                Work->Minecraft = Minecraft;
                Work->TaskMemory = Task;
                Work->Chunk = Chunk;
                Work->Meta = Meta;
                
                KickJob(MincGenerateChunkWork, Work, JobPriority_High);
            }break;
            
            case MincChunk_ReadyToGenerateMesh:
            {
                Meta->State.store(MincChunk_GeneratingMesh, std::memory_order_relaxed);
                
                task_memory* Task = GetTaskMemoryForUse(Minecraft->TaskPool, sizeof(minc_generate_mesh_work));
                Assert(Task);
                
                minc_generate_mesh_work* Work = PushStruct(&Task->Arena, minc_generate_mesh_work);
                
                Work->GenerationChunks = {};
                Work->GenerationChunks.Dst = Chunk;
                Work->GenerationChunks.SideChunks[MincFaceNormal_Left] = MincGetGeneratedChunk(Minecraft, X + 1, Z);
                Work->GenerationChunks.SideChunks[MincFaceNormal_Right] = MincGetGeneratedChunk(Minecraft, X - 1, Z);
                Work->GenerationChunks.SideChunks[MincFaceNormal_Front] = MincGetGeneratedChunk(Minecraft, X, Z + 1);
                Work->GenerationChunks.SideChunks[MincFaceNormal_Back] = MincGetGeneratedChunk(Minecraft, X, Z - 1);
                
                Work->Chunk = Chunk;
                Work->Minecraft = Minecraft;
                Work->TaskMemory = Task;
                Work->Meta = Meta;
                
                KickJob(MincGenerateMeshWork, Work, JobPriority_High);
            }break;
            
            case MincChunk_MeshGenerated:
            {
                PushVoxelChunkMesh(&Chunk->Mesh, ChunkWorldP);
            }break;
        }
    }
    else
    {
        // NOTE(Dima): Insert meta to table
        u64 Key = MincGetKey(X, 0, Z);
        u32 KeyHash = Hash32Slow(Key);
        int Index = KeyHash % MINC_META_TABLE_SIZE;
        
        minc_chunk_meta_slot* At = Minecraft->MetaTable[Index];
        
        while(At)
        {
            // NOTE(Dima): It can not exist here. Otherwise it should have been found previously
            Assert(At->Key != Key);
            
            At = At->NextInHash;
        }
        
        // NOTE(Dima): Allocating meta
        minc_chunk_meta_slot* NewMetaSlot = PushStruct(Minecraft->Arena, minc_chunk_meta_slot);
        NewMetaSlot->NextInHash = Minecraft->MetaTable[Index];
        NewMetaSlot->Key = Key;
        
        Minecraft->MetaTable[Index] = NewMetaSlot;
        
        // NOTE(Dima): Allocating chunk meta
        minc_chunk_meta* ChunkMeta = PushStructSafe(Minecraft->Arena, minc_chunk_meta);
        ChunkMeta->CoordX = X;
        ChunkMeta->CoordZ = Z;
        
        // NOTE(Dima): Setting data
        NewMetaSlot->Meta = ChunkMeta;
        NewMetaSlot->Chunk = 0;
        
        // NOTE(Dima): Starting generating meta for chunk
        ChunkMeta->State.store(MincChunk_Unloaded, std::memory_order_relaxed);
    }
}

INTERNAL_FUNCTION void CreateMinecraft(memory_arena* Arena, minecraft* Mine)
{
    Mine->Arena = Arena;
    Mine->ChunksViewDistance = 20;
    
    InitMinecraftBlockTextures(Mine);
    InitMinecraftTextureOffsets(Mine);
    InitMinecraftBiomes(Mine);
    
    Mine->TaskPool = CreateTaskMemoryPoolDynamic(Arena);
    
    // NOTE(Dima): Init meta table
    for(int MetaIndex = 0;
        MetaIndex < MINC_META_TABLE_SIZE;
        MetaIndex++)
    {
        Mine->MetaTable[MetaIndex] = 0;
    }
}

INTERNAL_FUNCTION void UpdateMinecraft(minecraft* Mine, v3 PlayerP)
{
    int vd = Mine->ChunksViewDistance;
    
    int CurChunkX = Floor(PlayerP.x / (f32)MINC_CHUNK_WIDTH);
    int CurChunkZ = Floor(PlayerP.z / (f32)MINC_CHUNK_WIDTH);
    
    {
        BLOCK_TIMING("Minc: Remove Unseen");
        
        // NOTE(Dima): Iterating to remove unseen
        for(int i = 0; i < MINC_META_TABLE_SIZE; i++)
        {
            minc_chunk_meta_slot* At = Mine->MetaTable[i];
            
            while(At)
            {
                minc_chunk* Chunk = At->Chunk;
                
                if(Chunk)
                {
                    int DiffX = Chunk->CoordX - CurChunkX;
                    int DiffZ = Chunk->CoordZ - CurChunkZ;
                    
                    int DistToChunk = Sqrt(DiffX * DiffX + 
                                           DiffZ * DiffZ);
                    
                    if(DistToChunk > vd)
                    {
                        // NOTE(Dima): Free chunk data
                        if(Chunk->Mesh.Free)
                        {
                            //free(Chunk->Mesh.Free);
                        }
                        //free(Chunk);
                        
                        // NOTE(Dima): Remove this chunk from map
                        //At->Meta->State.store(MincChunk_ReadyToGenerateChunk);
                        //At->Chunk = 0;
                    }
                }
                
                At = At->NextInHash;
            }
        }
    }
    
    {
        BLOCK_TIMING("Minc: Update Visible");
        
        // NOTE(Dima): Update all chunks within view distance
        for(int j = -vd; j <= vd; j++)
        {
            for(int i = -vd; i <= vd; i++)
            {
                int DistToChunk = Sqrt(i * i + j * j);
                
                if(DistToChunk <= vd)
                {
                    int X = CurChunkX + i;
                    int Z = CurChunkZ + j;
                    
                    UpdateChunkAtIndex(Mine, X, Z);
                }
            }
        }
    }
}