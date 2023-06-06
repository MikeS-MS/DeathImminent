// Copyright MikeSMediaStudios™ 2023

#include "Environment/BuildingGrid/GridMeshUtilities.h"

#include "KismetProceduralMeshLibrary.h"
#include "Environment/BuildingGrid/Chunk.h"
#include "Environment/Blocks/BlockStructs.h"

void UGridMeshUtilities::SetDataForMeshingAlgorithms(AChunk* Chunk)
{
	if (!Chunk)
		return;

	sm_Chunk = Chunk;
	sm_Width = Chunk->m__WidthInBlocks;
	sm_Height = Chunk->m__HeightInBlocks;
	sm_BlockSize = Chunk->m__BlockSize;
	sm_OwnerTransform = Chunk->GetActorTransform();
	sm_OwnerLocation = sm_OwnerTransform.GetLocation();
	sm_OwnerExtent = Chunk->m__OverlapCollision->GetUnscaledBoxExtent();
	sm_BlocksArray = Chunk->m__Blocks;
}

void UGridMeshUtilities::GreedyMeshing(FVoxelMeshSectionData& MeshData)
{
	TRACE_CPUPROFILER_EVENT_SCOPE_STR(TEXT("UGridMeshUtilities::GreedyMeshing()"))

	TArray<bool> VisitedBlocks;
	VisitedBlocks.SetNum(UGridUtilities::GetArrayDesiredSize(sm_Width, sm_Height));

	for (int x = 0; x < sm_Width; x++)
	{
		for (int z = 0; z < sm_Height; z++)
		{
			for (int y = 0; y < sm_Width; y++)
			{
				FBlockDataForGreedyMeshGeneration CurrentBlockData;
				if (!__IsBlockValidForShape(VisitedBlocks, FIntVector(x, y, z), CurrentBlockData))
					continue;

				FGreedyMeshShape CurrentShape;
				CurrentShape.Blocks.Add(CurrentBlockData);
				VisitedBlocks[CurrentBlockData.Index] = true;
				FIntVector CurrentBlockLocation = FIntVector(x, y, z);

				int32 MaxY = y;
				TArray<FBlockDataForGreedyMeshGeneration> BlocksOnY = __CheckBlocksForShapeOnY(VisitedBlocks, CurrentBlockLocation, MaxY);
				CurrentShape.LengthY = (MaxY - y) + 1;

				if (!BlocksOnY.IsEmpty())
					CurrentShape.Blocks.Append(BlocksOnY);

				int32 MaxZ = z;
				TArray<FBlockDataForGreedyMeshGeneration> BlocksOnZ = __CheckBlocksForShapeOnZ(VisitedBlocks, CurrentBlockLocation, MaxY, MaxZ);
				CurrentShape.LengthZ = (MaxZ - z) + 1;

				if (!BlocksOnZ.IsEmpty())
					CurrentShape.Blocks.Append(BlocksOnZ);

				int32 MaxX = x;
				TArray<FBlockDataForGreedyMeshGeneration> BlocksOnX = __CheckBlocksForShapeOnX(VisitedBlocks, CurrentBlockLocation, MaxY, MaxZ, MaxX);
				CurrentShape.LengthX = (MaxX - x) + 1;

				if (!BlocksOnX.IsEmpty())
					CurrentShape.Blocks.Append(BlocksOnX);

				__GenerateBox(CurrentShape, MeshData);
			}
		}
	}
}

void UGridMeshUtilities::MarchingCubes(FVoxelMeshSectionData& MeshData)
{
	TRACE_CPUPROFILER_EVENT_SCOPE_STR(TEXT("UGridMeshUtilities::MarchingCubes()"))

	const auto& MeshSmoothing = [](TArray<FVector>& Positions, int iterations, float lambda)
	{
		// Iterate for the specified number of smoothing iterations
		for (int iter = 0; iter < iterations; ++iter)
		{
			TArray<FVector> smoothedPositions = Positions; // Create a copy of the positions for smoothing

			// Iterate over each vertex
			for (size_t i = 0; i < Positions.Num(); ++i)
			{
				FVector sum(0.0f, 0.0f, 0.0f);
				int count = 0;

				// Iterate over each neighboring vertex (connected by an edge)
				for (size_t j = 0; j < Positions.Num(); ++j)
				{
					if (i == j) // Skip the current vertex
						continue;

					const float distance = FVector::Distance(Positions[j], Positions[i]);

					if (distance < lambda) // Consider only nearby vertices within the specified threshold
					{
						sum = sum + Positions[j];
						count++;
					}
				}

				if (count > 0)
				{
					// Calculate the average position of the neighboring vertices
					FVector average = sum / static_cast<float>(count);

					// Update the position of the current vertex by moving towards the average position
					smoothedPositions[i] = (Positions[i] + average) * 0.5f;
				}
			}

			// Update the original positions with the smoothed positions
			Positions = smoothedPositions;
		}
	};

	const int32& Width = sm_Width;
	const int32& Height = sm_Height;
	const float BlockSize = static_cast<float>(sm_BlockSize);

	for (int x = 0; x < Width; x++)
	{
		for (int y = 0; y < Width; y++)
		{
			for (int z = 0; z < Height; z++)
			{
				FBlockDataForMarchingCubes Voxel;

				const FBlockLocations BlockLocations = sm_Chunk->ConvertBlockGridPosToWorldPos(x, y, z);
				__GetBlockDataForMarchingCubes(x, y, z, BlockLocations, Voxel);

				__AddMeshDataFromBlock(Voxel, x, y, MeshData.Positions, MeshData.Triangles, MeshData.UVs, MeshData.Normals, MeshData.Tangents);
			}
		}
	}

	//MeshSmoothing(MeshData.Positions, 5, 25.f);
}

//void UGridMeshUtilities::SurfaceNets(FVoxelMeshSectionData& MeshData)
//{
//	// Start at x, y and z = -1 to account for the fact
//	// that we only check neighbors and create mesh in positive direction
//
//	const auto& ConvertToWorldSpace = [](const int& x, const int& y, const int& z)
//	{
//		const FVector position = (FVector(x, y, z) * static_cast<float>(sm_BlockSize)) - sm_OwnerExtent;
//		return position;
//		return UGridUtilities::ConvertToWorldSpaceFVector(position, sm_OwnerTransform);
//	};
//
//	TArray<FBlockDataForSurfaceNets> SurfaceBlocks;
//	for (int x = 0; x < sm_Width; x++)
//	{
//		for (int y = 0; y < sm_Width; y++)
//		{
//			for (int z = 0; z < sm_Height; z++)
//			{
//				// check 8 neighboring voxels
//				int sum = 0;
//				FBlockDataForSurfaceNets BlockData;
//
//				for (int dx = x; dx <= x + 1; dx++)
//				{
//					for (int dy = y; dy <= y + 1; dy++)
//					{
//						for (int dz = z; dz <= z + 1; dz++)
//						{
//							// If a voxel exists, add one to our (check-)sum
//							if (UGridUtilities::IsValidBlock(sm_Chunk->_GetBlockAtGridLocationOptimizedForLocal(dx, dy, dz)))
//							{
//								if (!(dx == x && dy == y && dz == z))
//								{
//									BlockData.SurroundingBlocks.Add(ConvertToWorldSpace(dx, dy, dz));
//								}
//								sum++;
//							}
//						}
//					}
//				}
//
//				// The sum is only 0 or 8 if all checked blocks are the same
//				if (sum % 8 != 0)
//				{
//					BlockData.WorldLocation = ConvertToWorldSpace(x, y, z);
//					BlockData.GridLocation = FIntVector(x, y, z);
//					// Surface Cube found, add Vertex
//					SurfaceBlocks.Add(BlockData);
//				}
//			}
//		}
//	}
//
//	for (FBlockDataForSurfaceNets& BlockData : SurfaceBlocks)
//	{
//		//FVector Sum = FVector::Zero();
//		//for (const FVector& Location : BlockData.SurroundingBlocks)
//		//{
//		//	Sum += Location;
//		//}
//		//FVector Position = FMath::VInterpTo(BlockData.WorldLocation, Sum / BlockData.SurroundingBlocks.Num(), 1, .1f);
//		FVector Position = BlockData.WorldLocation;
//		//DrawDebugPoint(sm_Chunk->GetWorld(), Position, 5.f, FColor::Red, false, 10);
//		MeshData.Positions.Add(Position);
//		MeshData.UVs.Add(FVector2D(BlockData.GridLocation.X, BlockData.GridLocation.Y));
//	}
//
//	//MeshData.Positions.Sort([](const FVector& V1, const FVector& V2)
//	//	{
//	//		const double angle1 = FMath::Atan2(V1.Y, V1.X);
//	//		const double angle2 = FMath::Atan2(V2.Y, V2.X);
//	//		return angle1 < angle2;
//	//	});
//
//	[&MeshData]()
//	{
//		if (MeshData.Positions.Num() < 3 || MeshData.Positions.Num() % 3 != 0)
//			return;
//
//		for(int i = 0; i <= MeshData.Positions.Num() - 3; i += 3)
//		{
//			MeshData.Triangles.Add(i);
//			MeshData.Triangles.Add(i + 1);
//			MeshData.Triangles.Add(i + 2);
//		}
//	}();
//
//	//MeshData.Tangents.SetNum(MeshData.Positions.Num());
//	//MeshData.Normals.SetNum(MeshData.Positions.Num());
//}


void UGridMeshUtilities::SurfaceNetsNew(FVoxelMeshSectionData& MeshData)
{
	TArray<FBlockDataForSurfaceNets> SurfaceBlocks;

	const auto& Triangulate = [](const TArray<FVector>& Positions, TArray<int>& OutTriangles)
	{
		for (int i = 0; i < Positions.Num() - 4; i += 4)
		{
			OutTriangles.Add(i);
			OutTriangles.Add(i + 2);
			OutTriangles.Add(i + 1);

			OutTriangles.Add(i + 2);
			OutTriangles.Add(i + 3);
			OutTriangles.Add(i + 1);
		}
	};

	for(int z = 0; z < sm_Height; z++)
	{
		for(int y = 0; y < sm_Width; y++)
		{
			for(int x = 0; x < sm_Width; x++)
			{

				if (!UGridUtilities::IsValidBlock(sm_Chunk->_GetBlockAtGridLocationOptimizedForLocal(x, y, z)))
					continue;

				const FBlockLocations BlockLocations = sm_Chunk->ConvertBlockGridPosToWorldPos(x, y, z);
				FBlockDataForSurfaceNets BlockData;
				__GetBlockDataForSurfaceNets(x, y, z, BlockLocations, BlockData);

				__AddMeshDataFromBlock(BlockData, BlockLocations, x, y, MeshData.Positions, MeshData.Triangles, MeshData.UVs);
			}
		}
	}

	//Triangulate(MeshData.Positions, MeshData.Triangles);
	//UKismetProceduralMeshLibrary::CalculateTangentsForMesh(MeshData.Positions, MeshData.Triangles, MeshData.UVs, MeshData.Normals, MeshData.Tangents);
}

bool UGridMeshUtilities::__IsBlockValidForShape(TArray<bool>& VisitedBlocks, const FIntVector& Location,
                                                FBlockDataForGreedyMeshGeneration& OutBlockData)
{
	const int32 BlockIndex = UGridUtilities::ConvertToArrayIndex(Location.X, Location.Y, Location.Z, sm_Width);

	if (VisitedBlocks[BlockIndex])
		return false;

	const int32& BlockID = sm_BlocksArray[BlockIndex];

	if (!UGridUtilities::IsValidBlock(BlockID))
		return false;

	OutBlockData = __GetBlockDataForGreedyMeshing(Location);

	return true;
}

TArray<FBlockDataForGreedyMeshGeneration> UGridMeshUtilities::__CheckBlocksForShapeOnY(TArray<bool>& VisitedBlocks,
                                                                                      const FIntVector& Location, int32& OutMaxY)
{
	TArray<FBlockDataForGreedyMeshGeneration> Blocks;

	// Invalid index
	if (Location.Y > sm_Width - 1)
		return Blocks;

	for (int y = Location.Y; y < sm_Width; y++)
	{
		if (FIntVector(Location.X, y, Location.Z) == Location)
			continue;

		FBlockDataForGreedyMeshGeneration BlockData;
		if (!__IsBlockValidForShape(VisitedBlocks, FIntVector(Location.X, y, Location.Z), BlockData))
			return Blocks;

		Blocks.Add(BlockData);
		VisitedBlocks[BlockData.Index] = true;
		OutMaxY = y;
	}

	return Blocks;
}

TArray<FBlockDataForGreedyMeshGeneration> UGridMeshUtilities::__CheckBlocksForShapeOnZ(TArray<bool>& VisitedBlocks,
	const FIntVector& Location, const int32& MaxY, int32& OutMaxZ)
{
	TArray<FBlockDataForGreedyMeshGeneration> Blocks;

	int32 MaxZ = sm_Height - 1;

	// Invalid index
	if (Location.Z > MaxZ)
		return Blocks;

	// Get the lowest average index for Z to see if we can combine
	for (int y = Location.Y; y <= MaxY; y++)
	{
		for (int z = Location.Z; z < sm_Width; z++)
		{
			if ((y >= Location.Y && y <= MaxY) && z == Location.Z)
				continue;

			FBlockDataForGreedyMeshGeneration BlockData;
			if (!__IsBlockValidForShape(VisitedBlocks, FIntVector(Location.X, y, z), BlockData))
			{
				if (z == Location.Z + 1)
					return Blocks;

				if (z - 1 < MaxZ)
					MaxZ = z - 1;

				break;
			}
		}
	}

	OutMaxZ = MaxZ;

	// Add all blocks from the given span
	for (int y = Location.Y; y <= MaxY; y++)
	{
		for (int z = Location.Z; z <= MaxZ; z++)
		{
			FBlockDataForGreedyMeshGeneration BlockData = __GetBlockDataForGreedyMeshing(FIntVector(Location.X, y, z));
			Blocks.Add(BlockData);
			VisitedBlocks[BlockData.Index] = true;
		}
	}

	return Blocks;
}

TArray<FBlockDataForGreedyMeshGeneration> UGridMeshUtilities::__CheckBlocksForShapeOnX(TArray<bool>& VisitedBlocks,
	const FIntVector& Location, const int32& MaxY, const int32& MaxZ, int32& OutMaxX)
{
	TArray<FBlockDataForGreedyMeshGeneration> Blocks;

	int32 MaxX = sm_Width - 1;

	// Invalid index
	if (Location.X > MaxX)
		return Blocks;

	// fix checks for X axis
	for (int y = Location.Y; y <= MaxY; y++)
	{
		for (int z = Location.Z; z <= MaxZ; z++)
		{
			for (int x = Location.X; x < sm_Width; x++)
			{
				// Skip initial block as we have already added that one.
				if (x == Location.X && ((y >= Location.Y && y <= MaxY) && (z >= Location.Z && z <= MaxZ)))
					continue;

				FBlockDataForGreedyMeshGeneration BlockData;
				if (!__IsBlockValidForShape(VisitedBlocks, FIntVector(x, y, z), BlockData))
				{
					if (x == Location.X + 1)
						return Blocks;

					if (x - 1 < MaxX)
						MaxX = x - 1;
					break;
				}
			}
		}
	}

	OutMaxX = MaxX;

	for (int y = Location.Y; y <= MaxY; y++)
	{
		for (int z = Location.Z; z <= MaxZ; z++)
		{
			for (int x = Location.X; x <= MaxX; x++)
			{
				FBlockDataForGreedyMeshGeneration BlockData = __GetBlockDataForGreedyMeshing(FIntVector(x, y, z));
				Blocks.Add(BlockData);
				VisitedBlocks[BlockData.Index] = true;
			}
		}
	}

	return Blocks;
}

void UGridMeshUtilities::__GenerateBox(const FGreedyMeshShape& Shape, FVoxelMeshSectionData& MeshData)
{
	const FBlockDataForGreedyMeshGeneration FirstBlock = Shape.Blocks[0];
	const FBlockDataForGreedyMeshGeneration LastBlock = Shape.Blocks[Shape.Blocks.Num() - 1];

	const float HalfBlockSize = static_cast<float>(sm_BlockSize) / 2.f;

	const float XMin = FirstBlock.Center.X - HalfBlockSize;
	const float XMax = LastBlock.Center.X + HalfBlockSize;
	const float LengthX = FMath::Abs(XMax - XMin);
	const float HalfLengthX = LengthX / 2;

	const float YMin = FirstBlock.Center.Y - HalfBlockSize;
	const float YMax = LastBlock.Center.Y + HalfBlockSize;
	const float LengthY = FMath::Abs(YMax - YMin);
	const float HalfLengthY = LengthY / 2;

	const float ZMin = FirstBlock.Center.Z - HalfBlockSize;
	const float ZMax = LastBlock.Center.Z + HalfBlockSize;
	const float LengthZ = FMath::Abs(ZMax - ZMin);
	const float HalfLengthZ = LengthZ / 2;

	FVector Center(XMax - HalfLengthX, YMax - HalfLengthY, ZMax - HalfLengthZ);
	Center -= sm_OwnerLocation;

	AppendBoxMesh(FVector(HalfLengthX, HalfLengthY, HalfLengthZ), FTransform(Center), MeshData);
}

FBlockDataForGreedyMeshGeneration UGridMeshUtilities::__GetBlockDataForGreedyMeshing(const FIntVector& Location)
{
	const int32 Index = UGridUtilities::ConvertToArrayIndex(Location.X, Location.Y, Location.Z, sm_Width);
	FBlockDataForGreedyMeshGeneration BlockData;
	const FVector BlockLocationLocal = ((FVector(Location) * (sm_BlockSize)) - sm_OwnerExtent + sm_BlockSize / 2);

	UGridUtilities::UnpackInt32ToInt16(sm_BlocksArray[Index], BlockData.Block.Source, BlockData.Block.ID);

	BlockData.Index = Index;
	BlockData.RelativeLocation = Location;
	BlockData.Center = UGridUtilities::ConvertToWorldSpaceFVector(BlockLocationLocal, sm_OwnerTransform);

	return BlockData;
}

void UGridMeshUtilities::AppendBoxMesh(const FVector& BoxRadius, const FTransform& BoxTransform, FVoxelMeshSectionData& MeshData)
{
	// Generate verts
	FVector BoxVerts[8];
	BoxVerts[0] = BoxTransform.TransformPosition(FVector(-BoxRadius.X, BoxRadius.Y, BoxRadius.Z));
	BoxVerts[1] = BoxTransform.TransformPosition(FVector(BoxRadius.X, BoxRadius.Y, BoxRadius.Z));
	BoxVerts[2] = BoxTransform.TransformPosition(FVector(BoxRadius.X, -BoxRadius.Y, BoxRadius.Z));
	BoxVerts[3] = BoxTransform.TransformPosition(FVector(-BoxRadius.X, -BoxRadius.Y, BoxRadius.Z));

	BoxVerts[4] = BoxTransform.TransformPosition(FVector(-BoxRadius.X, BoxRadius.Y, -BoxRadius.Z));
	BoxVerts[5] = BoxTransform.TransformPosition(FVector(BoxRadius.X, BoxRadius.Y, -BoxRadius.Z));
	BoxVerts[6] = BoxTransform.TransformPosition(FVector(BoxRadius.X, -BoxRadius.Y, -BoxRadius.Z));
	BoxVerts[7] = BoxTransform.TransformPosition(FVector(-BoxRadius.X, -BoxRadius.Y, -BoxRadius.Z));

	// Generate triangles (from quads)
	const int32 StartVertex = MeshData.Positions.Num();
	const int32 NumVerts = 24; // 6 faces x 4 verts per face
	const int32 NumIndices = 36;

	// Make sure the secondary arrays are the same length, zeroing them if necessary
	MeshData.Normals.SetNumZeroed(StartVertex);
	MeshData.Tangents.SetNumZeroed(StartVertex);
	MeshData.UVs.SetNumZeroed(StartVertex);

	MeshData.Positions.Reserve(StartVertex + NumVerts);
	MeshData.Normals.Reserve(StartVertex + NumVerts);
	MeshData.Tangents.Reserve(StartVertex + NumVerts);
	MeshData.UVs.Reserve(StartVertex + NumVerts);
	MeshData.Triangles.Reserve(MeshData.Triangles.Num() + NumIndices);

	const auto ConvertQuadToTriangles = [](TArray<int32>& Triangles, int32 Vert0, int32 Vert1, int32 Vert2, int32 Vert3)
	{
		Triangles.Add(Vert0);
		Triangles.Add(Vert1);
		Triangles.Add(Vert3);

		Triangles.Add(Vert1);
		Triangles.Add(Vert2);
		Triangles.Add(Vert3);
	};

	const auto WriteToNextFour = [](TArray<FVector>& Array, const FVector& Value)
	{
		Array.Add(Value);
		Array.Add(Value);
		Array.Add(Value);
		Array.Add(Value);
	};

	const auto WriteToNextFourTangents = [](TArray<FProcMeshTangent>& Array, const FProcMeshTangent& Value)
	{
		Array.Add(Value);
		Array.Add(Value);
		Array.Add(Value);
		Array.Add(Value);
	};

	const auto WriteQuadPositions = [&MeshData](const FVector& VertA, const FVector& VertB, const FVector& VertC, const FVector& VertD)
	{
		MeshData.Positions.Add(VertA);
		MeshData.Positions.Add(VertB);
		MeshData.Positions.Add(VertC);
		MeshData.Positions.Add(VertD);
	};

	FVector Tangent = BoxTransform.TransformVectorNoScale(FVector(0.0f, -1.0f, 0.0f));
	// Top Face
	WriteQuadPositions(BoxVerts[0], BoxVerts[1], BoxVerts[2], BoxVerts[3]);
	WriteToNextFour(MeshData.Normals, BoxTransform.TransformVectorNoScale(FVector(0.0f, 0.0f, 1.0f)));
	WriteToNextFourTangents(MeshData.Tangents, FProcMeshTangent(Tangent.X, Tangent.Y, Tangent.Z));
	ConvertQuadToTriangles(MeshData.Triangles, StartVertex + 0, StartVertex + 1, StartVertex + 2, StartVertex + 3);

	Tangent = BoxTransform.TransformVectorNoScale(FVector(0.0f, -1.0f, 0.0f));
	// Left Face
	WriteQuadPositions(BoxVerts[4], BoxVerts[0], BoxVerts[3], BoxVerts[7]);
	WriteToNextFour(MeshData.Normals, BoxTransform.TransformVectorNoScale(FVector(-1.0, 0.0, 0.0)));
	WriteToNextFourTangents(MeshData.Tangents, FProcMeshTangent(Tangent.X, Tangent.Y, Tangent.Z));
	ConvertQuadToTriangles(MeshData.Triangles, StartVertex + 4, StartVertex + 5, StartVertex + 6, StartVertex + 7);

	Tangent = BoxTransform.TransformVectorNoScale(FVector(-1.0f, 0.0f, 0.0f));
	// Front Face
	WriteQuadPositions(BoxVerts[5], BoxVerts[1], BoxVerts[0], BoxVerts[4]);
	WriteToNextFour(MeshData.Normals, BoxTransform.TransformVectorNoScale(FVector(0.0, 1.0, 0.0)));
	WriteToNextFourTangents(MeshData.Tangents, FProcMeshTangent(Tangent.X, Tangent.Y, Tangent.Z));
	ConvertQuadToTriangles(MeshData.Triangles, StartVertex + 8, StartVertex + 9, StartVertex + 10, StartVertex + 11);

	Tangent = BoxTransform.TransformVectorNoScale(FVector(0.0f, 1.0f, 0.0f));
	// Right Face
	WriteQuadPositions(BoxVerts[6], BoxVerts[2], BoxVerts[1], BoxVerts[5]);
	WriteToNextFour(MeshData.Normals, BoxTransform.TransformVectorNoScale(FVector(1.0, 0.0, 0.0)));
	WriteToNextFourTangents(MeshData.Tangents, FProcMeshTangent(Tangent.X, Tangent.Y, Tangent.Z));
	ConvertQuadToTriangles(MeshData.Triangles, StartVertex + 12, StartVertex + 13, StartVertex + 14, StartVertex + 15);

	Tangent = BoxTransform.TransformVectorNoScale(FVector(1.0f, 0.0f, 0.0f));
	// Back Face
	WriteQuadPositions(BoxVerts[7], BoxVerts[3], BoxVerts[2], BoxVerts[6]);
	WriteToNextFour(MeshData.Normals, BoxTransform.TransformVectorNoScale(FVector(0.0, -1.0, 0.0)));
	WriteToNextFourTangents(MeshData.Tangents, FProcMeshTangent(Tangent.X, Tangent.Y, Tangent.Z));
	ConvertQuadToTriangles(MeshData.Triangles, StartVertex + 16, StartVertex + 17, StartVertex + 18, StartVertex + 19);

	Tangent = BoxTransform.TransformVectorNoScale(FVector(0.0f, 1.0f, 0.0f));
	// Bottom Face
	WriteQuadPositions(BoxVerts[7], BoxVerts[6], BoxVerts[5], BoxVerts[4]);
	WriteToNextFour(MeshData.Normals, BoxTransform.TransformVectorNoScale(FVector(0.0, 0.0, -1.0)));
	WriteToNextFourTangents(MeshData.Tangents, FProcMeshTangent(Tangent.X, Tangent.Y, Tangent.Z));
	ConvertQuadToTriangles(MeshData.Triangles, StartVertex + 20, StartVertex + 21, StartVertex + 22, StartVertex + 23);

	// UVs
	for (int32 Index = 0; Index < 6; Index++)
	{
		MeshData.UVs.Add(FVector2D(0.0f, 0.0f));
		MeshData.UVs.Add(FVector2D(0.0f, 1.0f));
		MeshData.UVs.Add(FVector2D(1.0f, 1.0f));
		MeshData.UVs.Add(FVector2D(1.0f, 0.0f));
	}
}

void UGridMeshUtilities::__GetBlockDataForMarchingCubes(const int& x, const int& y, const int& z,
	const FBlockLocations& CurrentBlockPositions, FBlockDataForMarchingCubes& CurrentVoxel)
{
	TRACE_CPUPROFILER_EVENT_SCOPE_STR(TEXT("UGridMeshUtilities::_GetBlockDataForMarchingCubes()"))

	CurrentVoxel.CornerLocations[0] = CurrentBlockPositions.BottomBackLeft;
	CurrentVoxel.CornerLocations[1] = CurrentBlockPositions.BottomBackRight();
	CurrentVoxel.CornerLocations[2] = CurrentBlockPositions.BottomFrontRight();
	CurrentVoxel.CornerLocations[3] = CurrentBlockPositions.BottomFrontLeft();
	CurrentVoxel.CornerLocations[4] = CurrentBlockPositions.TopBackLeft();
	CurrentVoxel.CornerLocations[5] = CurrentBlockPositions.TopBackRight();
	CurrentVoxel.CornerLocations[6] = CurrentBlockPositions.TopFrontRight();
	CurrentVoxel.CornerLocations[7] = CurrentBlockPositions.TopFrontLeft();

	const int32 RightIndex = x + 1;
	const int32 FrontIndex = y + 1;
	const int32 TopIndex = z + 1;

	CurrentVoxel.CornersStatus[0] = UGridUtilities::IsValidBlock(sm_Chunk->_GetBlockAtGridLocationOptimizedForLocal(x, y, z));
	CurrentVoxel.CornersStatus[1] = UGridUtilities::IsValidBlock(sm_Chunk->_GetBlockAtGridLocationOptimizedForLocal(RightIndex, y, z));
	CurrentVoxel.CornersStatus[2] = UGridUtilities::IsValidBlock(sm_Chunk->_GetBlockAtGridLocationOptimizedForLocal(RightIndex, FrontIndex, z));
	CurrentVoxel.CornersStatus[3] = UGridUtilities::IsValidBlock(sm_Chunk->_GetBlockAtGridLocationOptimizedForLocal(x, FrontIndex, z));
	CurrentVoxel.CornersStatus[4] = UGridUtilities::IsValidBlock(sm_Chunk->_GetBlockAtGridLocationOptimizedForLocal(x, y, TopIndex));
	CurrentVoxel.CornersStatus[5] = UGridUtilities::IsValidBlock(sm_Chunk->_GetBlockAtGridLocationOptimizedForLocal(RightIndex, y, TopIndex));
	CurrentVoxel.CornersStatus[6] = UGridUtilities::IsValidBlock(sm_Chunk->_GetBlockAtGridLocationOptimizedForLocal(RightIndex, FrontIndex, TopIndex));
	CurrentVoxel.CornersStatus[7] = UGridUtilities::IsValidBlock(sm_Chunk->_GetBlockAtGridLocationOptimizedForLocal(x, FrontIndex, TopIndex));
}

int UGridMeshUtilities::__GetConfigurationIndex(const TArray<int>& CornersStatus)
{
	if (CornersStatus.Num() != 8)
		return 0;

	int ConfigIndex = 0;
	int CurrentConfigNumber = 1;

	for (int i = 0; i < 8; i++)
	{
		const int Status = CornersStatus[i];
		if (Status != 0)
		{
			ConfigIndex |= CurrentConfigNumber;
		}
		CurrentConfigNumber *= 2;
	}

	return ConfigIndex;
}

void UGridMeshUtilities::__AddMeshDataFromBlock(const FBlockDataForMarchingCubes& BlockData, const int& U, const int& V,
                                                TArray<FVector>& Positions, TArray<int>& Triangles, TArray<FVector2D>& UVs, TArray<FVector>& Normals,
                                                TArray<FVector>& Tangents)
{
	__AddMeshDataFromBlock(BlockData, U, V, Positions, Triangles, UVs, Normals, &Tangents, nullptr);
}

void UGridMeshUtilities::__AddMeshDataFromBlock(const FBlockDataForMarchingCubes& BlockData, const int& U, const int& V,
	TArray<FVector>& Positions, TArray<int>& Triangles, TArray<FVector2D>& UVs, TArray<FVector>& Normals,
	TArray<FProcMeshTangent>& Tangents)
{
	__AddMeshDataFromBlock(BlockData, U, V, Positions, Triangles, UVs, Normals, nullptr, &Tangents);
}

//void UGridUtilities::AppendBoxMesh(const FVector& BoxRadius, const FTransform& BoxTransform,
//	UE::Geometry::FDynamicMesh3& MeshData)
//{
//	// Generate verts
//	FVector BoxVerts[8];
//	BoxVerts[0] = BoxTransform.TransformPosition(FVector(-BoxRadius.X, BoxRadius.Y, BoxRadius.Z));
//	BoxVerts[1] = BoxTransform.TransformPosition(FVector(BoxRadius.X, BoxRadius.Y, BoxRadius.Z));
//	BoxVerts[2] = BoxTransform.TransformPosition(FVector(BoxRadius.X, -BoxRadius.Y, BoxRadius.Z));
//	BoxVerts[3] = BoxTransform.TransformPosition(FVector(-BoxRadius.X, -BoxRadius.Y, BoxRadius.Z));
//
//	BoxVerts[4] = BoxTransform.TransformPosition(FVector(-BoxRadius.X, BoxRadius.Y, -BoxRadius.Z));
//	BoxVerts[5] = BoxTransform.TransformPosition(FVector(BoxRadius.X, BoxRadius.Y, -BoxRadius.Z));
//	BoxVerts[6] = BoxTransform.TransformPosition(FVector(BoxRadius.X, -BoxRadius.Y, -BoxRadius.Z));
//	BoxVerts[7] = BoxTransform.TransformPosition(FVector(-BoxRadius.X, -BoxRadius.Y, -BoxRadius.Z));
//
//	//// Generate triangles (from quads)
//	//const int32 StartVertex = MeshData.VertexCount();
//	//const int32 NumVerts = 24; // 6 faces x 4 verts per face
//	//const int32 NumIndices = 36;
//
//	//// Make sure the secondary arrays are the same length, zeroing them if necessary
//	//MeshData.Normals.SetNumZeroed(StartVertex);
//	////MeshData.Tangents.SetNumZeroed(StartVertex);
//	//MeshData.UVs.SetNumZeroed(StartVertex);
//
//	//MeshData.Positions.Reserve(StartVertex + NumVerts);
//	//MeshData.Normals.Reserve(StartVertex + NumVerts);
//	////MeshData.Tangents.Reserve(StartVertex + NumVerts);
//	//MeshData.UVs.Reserve(StartVertex + NumVerts);
//	//MeshData.Triangles.Reserve(MeshData.Triangles.Num() + NumIndices);
//
//	const auto ConvertQuadToTriangles = [](FDynamicMesh3& MeshData, int32 Vert0, int32 Vert1, int32 Vert2, int32 Vert3)
//	{
//		MeshData.AppendTriangle(Vert0, Vert1, Vert3);
//		MeshData.AppendTriangle(Vert1, Vert2, Vert3);
//	};
//
//	const auto WriteToNextFour = [&MeshData](const int& FirstVertex, const int& SecondVertex, const int& ThirdVertex, const int& FourthVertex, const FVector3f& Value)
//	{
//		MeshData.SetVertexNormal(FirstVertex, Value);
//		MeshData.SetVertexNormal(SecondVertex, Value);
//		MeshData.SetVertexNormal(ThirdVertex, Value);
//		MeshData.SetVertexNormal(FourthVertex, Value);
//	};
//
//	//const auto WriteToNextFourTangents = [](TArray<FProcMeshTangent>& Array, const FProcMeshTangent& Value)
//	//{
//	//	Array.Add(Value);
//	//	Array.Add(Value);
//	//	Array.Add(Value);
//	//	Array.Add(Value);
//	//};
//
//	const auto WriteQuadPositions = [&MeshData](const FVector& VertA, const FVector& VertB, const FVector& VertC, const FVector& VertD, int& VertAID, int& VertBID, int& VertCID, int& VertDID)
//	{
//		VertAID = MeshData.AppendVertex(VertA);
//		VertBID = MeshData.AppendVertex(VertB);
//		VertCID = MeshData.AppendVertex(VertC);
//		VertDID = MeshData.AppendVertex(VertD);
//	};
//
//	const auto AppendUVs = [&MeshData](const int& AID, const int& BID, const int& CID, const int& DID)
//	{
//		MeshData.SetVertexUV(AID, FVector2f(0.0f, 0.0f));
//		MeshData.SetVertexUV(BID, FVector2f(0.0f, 1.0f));
//		MeshData.SetVertexUV(CID, FVector2f(1.0f, 1.0f));
//		MeshData.SetVertexUV(DID, FVector2f(1.0f, 0.0f));
//	};
//
//	int AID, BID, CID, DID;
//	//FVector Tangent = BoxTransform.TransformVectorNoScale(FVector(0.0f, -1.0f, 0.0f));
//	// Top Face
//	WriteQuadPositions(BoxVerts[0], BoxVerts[1], BoxVerts[2], BoxVerts[3], AID, BID, CID, DID);
//	WriteToNextFour(AID, BID, CID, DID, FVector3f(BoxTransform.TransformVectorNoScale(FVector(0.0f, 0.0f, 1.0f))));
//	//WriteToNextFourTangents(MeshData.Tangents, FProcMeshTangent(Tangent.X, Tangent.Y, Tangent.Z));
//	ConvertQuadToTriangles(MeshData, AID, BID, CID, DID);
//	AppendUVs(AID, BID, CID, DID);
//
//	//Tangent = BoxTransform.TransformVectorNoScale(FVector(0.0f, -1.0f, 0.0f));
//	// Left Face
//	WriteQuadPositions(BoxVerts[4], BoxVerts[0], BoxVerts[3], BoxVerts[7], AID, BID, CID, DID);
//	WriteToNextFour(AID, BID, CID, DID, FVector3f(BoxTransform.TransformVectorNoScale(FVector(-1.0, 0.0, 0.0))));
//	ConvertQuadToTriangles(MeshData, AID, BID, CID, DID);
//	AppendUVs(AID, BID, CID, DID);
//
//	//Tangent = BoxTransform.TransformVectorNoScale(FVector(-1.0f, 0.0f, 0.0f));
//	// Front Face
//	WriteQuadPositions(BoxVerts[5], BoxVerts[1], BoxVerts[0], BoxVerts[4], AID, BID, CID, DID);
//	WriteToNextFour(AID, BID, CID, DID, FVector3f(BoxTransform.TransformVectorNoScale(FVector(0.0, 1.0, 0.0))));
//	ConvertQuadToTriangles(MeshData, AID, BID, CID, DID);
//	AppendUVs(AID, BID, CID, DID);
//
//	//Tangent = BoxTransform.TransformVectorNoScale(FVector(0.0f, 1.0f, 0.0f));
//	// Right Face
//	WriteQuadPositions(BoxVerts[6], BoxVerts[2], BoxVerts[1], BoxVerts[5], AID, BID, CID, DID);
//	WriteToNextFour(AID, BID, CID, DID, FVector3f(BoxTransform.TransformVectorNoScale(FVector(1.0, 0.0, 0.0))));
//	ConvertQuadToTriangles(MeshData, AID, BID, CID, DID);
//	AppendUVs(AID, BID, CID, DID);
//
//	//Tangent = BoxTransform.TransformVectorNoScale(FVector(1.0f, 0.0f, 0.0f));
//	// Back Face
//	WriteQuadPositions(BoxVerts[7], BoxVerts[3], BoxVerts[2], BoxVerts[6], AID, BID, CID, DID);
//	WriteToNextFour(AID, BID, CID, DID, FVector3f(BoxTransform.TransformVectorNoScale(FVector(0.0, -1.0, 0.0))));
//	ConvertQuadToTriangles(MeshData, AID, BID, CID, DID);
//	AppendUVs(AID, BID, CID, DID);
//
//	//Tangent = BoxTransform.TransformVectorNoScale(FVector(0.0f, 1.0f, 0.0f));
//	// Bottom Face
//	WriteQuadPositions(BoxVerts[7], BoxVerts[6], BoxVerts[5], BoxVerts[4], AID, BID, CID, DID);
//	WriteToNextFour(AID, BID, CID, DID, FVector3f(BoxTransform.TransformVectorNoScale(FVector(0.0, 0.0, -1.0))));
//	ConvertQuadToTriangles(MeshData, AID, BID, CID, DID);
//	AppendUVs(AID, BID, CID, DID);
//
//	//// UVs
//	//for (int32 Index = 0; Index < 6; Index++)
//	//{
//	//	MeshData.UVs.Add(FVector2D(0.0f, 0.0f));
//	//	MeshData.UVs.Add(FVector2D(0.0f, 1.0f));
//	//	MeshData.UVs.Add(FVector2D(1.0f, 1.0f));
//	//	MeshData.UVs.Add(FVector2D(1.0f, 0.0f));
//	//}
//}

//void UGridUtilities::AddMeshDataFromBlock(const FBlockDataForMarchingCubes& BlockData, const int& U, const int& V, FDynamicMesh3& MeshData, FKConvexElem& Collision)
//{
//	const int ConfigIndex = GetConfigurationIndex(BlockData.CornersStatus);
//
//	/* Cube is entirely in/out of the surface */
//	if (edgeTable[ConfigIndex] != 0)
//	{
//		for (int32 i = 0; TriangleCombinations[ConfigIndex][i] != -1; i += 3)
//		{
//			/* Gets the vertices for the configuration */
//			const int A0 = cornerIndexAFromEdge[TriangleCombinations[ConfigIndex][i]];
//			const int B0 = cornerIndexBFromEdge[TriangleCombinations[ConfigIndex][i]];
//
//			const int A1 = cornerIndexAFromEdge[TriangleCombinations[ConfigIndex][i + 1]];
//			const int B1 = cornerIndexBFromEdge[TriangleCombinations[ConfigIndex][i + 1]];
//
//			const int A2 = cornerIndexAFromEdge[TriangleCombinations[ConfigIndex][i + 2]];
//			const int B2 = cornerIndexBFromEdge[TriangleCombinations[ConfigIndex][i + 2]];
//
//			const FVector FirstVertex = (BlockData.CornerLocations[A2] + BlockData.CornerLocations[B2]) / 2.f;
//			const FVector SecondVertex = (BlockData.CornerLocations[A1] + BlockData.CornerLocations[B1]) / 2.f;
//			const FVector ThirdVertex = (BlockData.CornerLocations[A0] + BlockData.CornerLocations[B0]) / 2.f;
//
//			/* Creates indexes for triangles array */
//			const int FirstVertexIndex = MeshData.AppendVertex(FirstVertex);
//			const int SecondVertexIndex = MeshData.AppendVertex(SecondVertex);
//			const int ThirdVertexIndex = MeshData.AppendVertex(ThirdVertex);
//
//			/* Triangles array */
//			MeshData.AppendTriangle(FirstVertexIndex, ThirdVertexIndex, SecondVertexIndex);
//
//			/* Add Collision Data */
//			const int FirstVertexCollisionIndex = Collision.VertexData.Add(FirstVertex);
//			const int SecondVertexCollisionIndex = Collision.VertexData.Add(SecondVertex);
//			const int ThirdVertexCollisionIndex = Collision.VertexData.Add(FirstVertex);
//			Collision.IndexData.Add(FirstVertexCollisionIndex);
//			Collision.IndexData.Add(ThirdVertexCollisionIndex);
//			Collision.IndexData.Add(SecondVertexCollisionIndex);
//
//			/* Creates basic UVs */
//			/* We do it 3 times cuz it should be correlate wth indexes of Vertices array */
//			MeshData.SetVertexUV(FirstVertexIndex, FVector2f(U, V));
//			MeshData.SetVertexUV(SecondVertexIndex, FVector2f(U, V));
//			MeshData.SetVertexUV(ThirdVertexIndex, FVector2f(U, V));
//			/* Creates basic normals */
//			/* If you want to smooth the mesh out you would probably need to find an average of adjacent normals or something like that I dunno, google it */
//			/* We do it 3 times cuz it should be correlate wth indexes of Vertices array */
//			const FVector3f Normal = FVector3f(FVector::CrossProduct(FirstVertex - ThirdVertex,
//				SecondVertex - ThirdVertex).GetSafeNormal());
//
//			MeshData.SetVertexNormal(FirstVertexIndex, Normal);
//			MeshData.SetVertexNormal(SecondVertexIndex, Normal);
//			MeshData.SetVertexNormal(ThirdVertexIndex, Normal);
//
//
//			///* Basic tangets */
//			///* We do it 3 times cuz it should be correlate wth indexes of Vertices array */
//			//const FVector Tangent = (FirstVertex - SecondVertex).GetSafeNormal2D();
//			//if(GenericTangents)
//			//{
//			//	GenericTangents->Add(Tangent);
//			//	GenericTangents->Add(Tangent);
//			//	GenericTangents->Add(Tangent);
//			//}
//			//else if(ProcMeshTangents)
//			//{
//			//	ProcMeshTangents->Add(FProcMeshTangent(Tangent.X, Tangent.Y, Tangent.Z));
//			//	ProcMeshTangents->Add(FProcMeshTangent(Tangent.X, Tangent.Y, Tangent.Z));
//			//	ProcMeshTangents->Add(FProcMeshTangent(Tangent.X, Tangent.Y, Tangent.Z));
//			//}
//		}
//	}
//}

void UGridMeshUtilities::__AddMeshDataFromBlock(const FBlockDataForMarchingCubes& BlockData, const int& U, const int& V,
	TArray<FVector>& Positions, TArray<int>& Triangles, TArray<FVector2D>& UVs, TArray<FVector>& Normals,
	TArray<FVector>* GenericTangents, TArray<FProcMeshTangent>* ProcMeshTangents)
{
	TRACE_CPUPROFILER_EVENT_SCOPE_STR(TEXT("UGridUtilities::AddMeshDataFromBlock()"))

	const auto& InterpolateVerts = [](const FVector& FirstCorner, const FVector& SecondCorner)
	{
		return (FMath::VInterpTo(FirstCorner, SecondCorner, 1.f, .5f));
		//const float t = (Threshold - FirstCornerValue) / (SecondCornerValue - FirstCornerValue);
		//return FirstCorner + t * (SecondCorner - FirstCorner);
	};

	const int ConfigIndex = __GetConfigurationIndex(BlockData.CornersStatus);
	/* Cube is entirely in/out of the surface */
	if (sc_EdgeConfigurations[ConfigIndex] == 0)
		return;

	for (int32 i = 0; sc_TrianglePoints[ConfigIndex][i] != -1; i += 3)
	{
		/* Gets the vertices for the configuration */
		const int A0 = sc_CornerIndexAFromEdge[sc_TrianglePoints[ConfigIndex][i]];
		const int B0 = sc_CornerIndexBFromEdge[sc_TrianglePoints[ConfigIndex][i]];

		const int A1 = sc_CornerIndexAFromEdge[sc_TrianglePoints[ConfigIndex][i + 1]];
		const int B1 = sc_CornerIndexBFromEdge[sc_TrianglePoints[ConfigIndex][i + 1]];

		const int A2 = sc_CornerIndexAFromEdge[sc_TrianglePoints[ConfigIndex][i + 2]];
		const int B2 = sc_CornerIndexBFromEdge[sc_TrianglePoints[ConfigIndex][i + 2]];

		const FVector FirstVertex = InterpolateVerts(BlockData.CornerLocations[A2], BlockData.CornerLocations[B2]);
		const FVector SecondVertex = InterpolateVerts(BlockData.CornerLocations[A1], BlockData.CornerLocations[B1]);
		const FVector ThirdVertex = InterpolateVerts(BlockData.CornerLocations[A0], BlockData.CornerLocations[B0]);

		/* Creates indexes for triangles array */
		int FirstVertexIndex = Positions.Add(FirstVertex);
		int SecondVertexIndex = Positions.Add(SecondVertex);
		int ThirdVertexIndex = Positions.Add(ThirdVertex);

		/* Triangles array */
		Triangles.Add(FirstVertexIndex);
		Triangles.Add(ThirdVertexIndex);
		Triangles.Add(SecondVertexIndex);

		/* Creates basic UVs */
		/* We do it 3 times cuz it should be correlate wth indexes of Vertices array */
		UVs.Add(FVector2D(U, V));
		UVs.Add(FVector2D(U, V));
		UVs.Add(FVector2D(U, V));

		/* Creates basic normals */
		/* If you want to smooth the mesh out you would probably need to find an average of adjacent normals or something like that I dunno, google it */
		/* We do it 3 times cuz it should be correlate wth indexes of Vertices array */
		const FVector Normal = FVector::CrossProduct(FirstVertex - ThirdVertex,
			SecondVertex - ThirdVertex).GetSafeNormal();
		Normals.Add(Normal);
		Normals.Add(Normal);
		Normals.Add(Normal);


		/* Basic tangents */
		/* We do it 3 times cuz it should be correlate wth indexes of Vertices array */
		const FVector Tangent = (FirstVertex - SecondVertex).GetSafeNormal2D();
		if (GenericTangents)
		{
			GenericTangents->Add(Tangent);
			GenericTangents->Add(Tangent);
			GenericTangents->Add(Tangent);
		}
		else if (ProcMeshTangents)
		{
			ProcMeshTangents->Add(FProcMeshTangent(Tangent.X, Tangent.Y, Tangent.Z));
			ProcMeshTangents->Add(FProcMeshTangent(Tangent.X, Tangent.Y, Tangent.Z));
			ProcMeshTangents->Add(FProcMeshTangent(Tangent.X, Tangent.Y, Tangent.Z));
		}
	}
}

void UGridMeshUtilities::__GetBlockDataForSurfaceNets(const int& x, const int& y, const int& z, const FBlockLocations& BlockLocations, FBlockDataForSurfaceNets& BlockData)
{
#define ReturnCheck() if (BlockData.IsSurface) return;

	const int Left = x - 1;
	const int Right = x + 1;

	const int Back = y - 1;
	const int Front = y + 1;

	const int Bottom = z - 1;
	const int Top = z + 1;

	BlockData.WorldLocation = BlockLocations.Center();

	BlockData.IsSurface = !UGridUtilities::IsValidBlock(sm_Chunk->_GetBlockAtGridLocationOptimizedForLocal(Left, y, z));
	ReturnCheck()

	BlockData.IsSurface = !UGridUtilities::IsValidBlock(sm_Chunk->_GetBlockAtGridLocationOptimizedForLocal(Right, y, z));
	ReturnCheck()

	BlockData.IsSurface = !UGridUtilities::IsValidBlock(sm_Chunk->_GetBlockAtGridLocationOptimizedForLocal(x, Back, z));
	ReturnCheck()

	BlockData.IsSurface = !UGridUtilities::IsValidBlock(sm_Chunk->_GetBlockAtGridLocationOptimizedForLocal(x, Front, z));
	ReturnCheck()

	BlockData.IsSurface = !UGridUtilities::IsValidBlock(sm_Chunk->_GetBlockAtGridLocationOptimizedForLocal(x, y, Bottom));
	ReturnCheck()

	BlockData.IsSurface = !UGridUtilities::IsValidBlock(sm_Chunk->_GetBlockAtGridLocationOptimizedForLocal(x, y, Top));
	ReturnCheck()


	//for (int StartX = Left; StartX <= Right; StartX++)
	//{
	//	for (int StartY = Back; StartY <= Front; StartY++)
	//	{
	//		for (int StartZ = Bottom; StartZ <= Top; StartZ++)
	//		{
	//			if ((StartX == x && StartY == y && StartZ == z))
	//				continue;

	//			if (!UGridUtilities::IsValidBlock(sm_Chunk->_GetBlockAtGridLocationOptimizedForLocal(StartX, StartY, StartZ)))
	//			{
	//				BlockData.IsSurface = true;
	//				BlockData.WorldLocation = BlockLocations.Center();

	//				return;
	//			}
	//		}
	//	}
	//}
}

void UGridMeshUtilities::__AddMeshDataFromBlock(const FBlockDataForSurfaceNets& BlockData,
	const FBlockLocations& CurrentBlockLocations, const int& U, const int& V, TArray<FVector>& OutPositions,
	TArray<int>& OutTriangles, TArray<FVector2D>& OutUVs)
{
	if (!BlockData.IsSurface)
		return;

	OutPositions.Add(BlockData.WorldLocation);
	
	OutUVs.Add(FVector2D(U, V));
}

int32& UGridMeshUtilities::sm_Width = *new int(0);
int32& UGridMeshUtilities::sm_Height = *new int(0);
int32& UGridMeshUtilities::sm_BlockSize = *new int(0);
FTransform& UGridMeshUtilities::sm_OwnerTransform = *new FTransform();
FVector& UGridMeshUtilities::sm_OwnerLocation = *new FVector();
FVector& UGridMeshUtilities::sm_OwnerExtent = *new FVector();
TArray<int32>& UGridMeshUtilities::sm_BlocksArray = *new TArray<int32>();
AChunk* UGridMeshUtilities::sm_Chunk = nullptr;

const int UGridMeshUtilities::sc_EdgeConfigurations[256] =
{
	0x0, 0x109, 0x203, 0x30a, 0x406, 0x50f, 0x605, 0x70c,
	0x80c, 0x905, 0xa0f, 0xb06, 0xc0a, 0xd03, 0xe09, 0xf00,
	0x190, 0x99, 0x393, 0x29a, 0x596, 0x49f, 0x795, 0x69c,
	0x99c, 0x895, 0xb9f, 0xa96, 0xd9a, 0xc93, 0xf99, 0xe90,
	0x230, 0x339, 0x33, 0x13a, 0x636, 0x73f, 0x435, 0x53c,
	0xa3c, 0xb35, 0x83f, 0x936, 0xe3a, 0xf33, 0xc39, 0xd30,
	0x3a0, 0x2a9, 0x1a3, 0xaa, 0x7a6, 0x6af, 0x5a5, 0x4ac,
	0xbac, 0xaa5, 0x9af, 0x8a6, 0xfaa, 0xea3, 0xda9, 0xca0,
	0x460, 0x569, 0x663, 0x76a, 0x66, 0x16f, 0x265, 0x36c,
	0xc6c, 0xd65, 0xe6f, 0xf66, 0x86a, 0x963, 0xa69, 0xb60,
	0x5f0, 0x4f9, 0x7f3, 0x6fa, 0x1f6, 0xff, 0x3f5, 0x2fc,
	0xdfc, 0xcf5, 0xfff, 0xef6, 0x9fa, 0x8f3, 0xbf9, 0xaf0,
	0x650, 0x759, 0x453, 0x55a, 0x256, 0x35f, 0x55, 0x15c,
	0xe5c, 0xf55, 0xc5f, 0xd56, 0xa5a, 0xb53, 0x859, 0x950,
	0x7c0, 0x6c9, 0x5c3, 0x4ca, 0x3c6, 0x2cf, 0x1c5, 0xcc,
	0xfcc, 0xec5, 0xdcf, 0xcc6, 0xbca, 0xac3, 0x9c9, 0x8c0,
	0x8c0, 0x9c9, 0xac3, 0xbca, 0xcc6, 0xdcf, 0xec5, 0xfcc,
	0xcc, 0x1c5, 0x2cf, 0x3c6, 0x4ca, 0x5c3, 0x6c9, 0x7c0,
	0x950, 0x859, 0xb53, 0xa5a, 0xd56, 0xc5f, 0xf55, 0xe5c,
	0x15c, 0x55, 0x35f, 0x256, 0x55a, 0x453, 0x759, 0x650,
	0xaf0, 0xbf9, 0x8f3, 0x9fa, 0xef6, 0xfff, 0xcf5, 0xdfc,
	0x2fc, 0x3f5, 0xff, 0x1f6, 0x6fa, 0x7f3, 0x4f9, 0x5f0,
	0xb60, 0xa69, 0x963, 0x86a, 0xf66, 0xe6f, 0xd65, 0xc6c,
	0x36c, 0x265, 0x16f, 0x66, 0x76a, 0x663, 0x569, 0x460,
	0xca0, 0xda9, 0xea3, 0xfaa, 0x8a6, 0x9af, 0xaa5, 0xbac,
	0x4ac, 0x5a5, 0x6af, 0x7a6, 0xaa, 0x1a3, 0x2a9, 0x3a0,
	0xd30, 0xc39, 0xf33, 0xe3a, 0x936, 0x83f, 0xb35, 0xa3c,
	0x53c, 0x435, 0x73f, 0x636, 0x13a, 0x33, 0x339, 0x230,
	0xe90, 0xf99, 0xc93, 0xd9a, 0xa96, 0xb9f, 0x895, 0x99c,
	0x69c, 0x795, 0x49f, 0x596, 0x29a, 0x393, 0x99, 0x190,
	0xf00, 0xe09, 0xd03, 0xc0a, 0xb06, 0xa0f, 0x905, 0x80c,
	0x70c, 0x605, 0x50f, 0x406, 0x30a, 0x203, 0x109, 0x0 };

const int UGridMeshUtilities::sc_CornerIndexAFromEdge[12] = { 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3 };

const int UGridMeshUtilities::sc_CornerIndexBFromEdge[12] = { 1, 2, 3, 0, 5, 6, 7, 4, 4, 5, 6, 7 };

const int UGridMeshUtilities::sc_TrianglePoints[256][16] =
{
	{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{0, 8, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{0, 1, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{1, 8, 3, 9, 8, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{1, 2, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{0, 8, 3, 1, 2, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{9, 2, 10, 0, 2, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{2, 8, 3, 2, 10, 8, 10, 9, 8, -1, -1, -1, -1, -1, -1, -1},
	{3, 11, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{0, 11, 2, 8, 11, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{1, 9, 0, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{1, 11, 2, 1, 9, 11, 9, 8, 11, -1, -1, -1, -1, -1, -1, -1},
	{3, 10, 1, 11, 10, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{0, 10, 1, 0, 8, 10, 8, 11, 10, -1, -1, -1, -1, -1, -1, -1},
	{3, 9, 0, 3, 11, 9, 11, 10, 9, -1, -1, -1, -1, -1, -1, -1},
	{9, 8, 10, 10, 8, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{4, 7, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{4, 3, 0, 7, 3, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{0, 1, 9, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{4, 1, 9, 4, 7, 1, 7, 3, 1, -1, -1, -1, -1, -1, -1, -1},
	{1, 2, 10, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{3, 4, 7, 3, 0, 4, 1, 2, 10, -1, -1, -1, -1, -1, -1, -1},
	{9, 2, 10, 9, 0, 2, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1},
	{2, 10, 9, 2, 9, 7, 2, 7, 3, 7, 9, 4, -1, -1, -1, -1},
	{8, 4, 7, 3, 11, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{11, 4, 7, 11, 2, 4, 2, 0, 4, -1, -1, -1, -1, -1, -1, -1},
	{9, 0, 1, 8, 4, 7, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1},
	{4, 7, 11, 9, 4, 11, 9, 11, 2, 9, 2, 1, -1, -1, -1, -1},
	{3, 10, 1, 3, 11, 10, 7, 8, 4, -1, -1, -1, -1, -1, -1, -1},
	{1, 11, 10, 1, 4, 11, 1, 0, 4, 7, 11, 4, -1, -1, -1, -1},
	{4, 7, 8, 9, 0, 11, 9, 11, 10, 11, 0, 3, -1, -1, -1, -1},
	{4, 7, 11, 4, 11, 9, 9, 11, 10, -1, -1, -1, -1, -1, -1, -1},
	{9, 5, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{9, 5, 4, 0, 8, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{0, 5, 4, 1, 5, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{8, 5, 4, 8, 3, 5, 3, 1, 5, -1, -1, -1, -1, -1, -1, -1},
	{1, 2, 10, 9, 5, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{3, 0, 8, 1, 2, 10, 4, 9, 5, -1, -1, -1, -1, -1, -1, -1},
	{5, 2, 10, 5, 4, 2, 4, 0, 2, -1, -1, -1, -1, -1, -1, -1},
	{2, 10, 5, 3, 2, 5, 3, 5, 4, 3, 4, 8, -1, -1, -1, -1},
	{9, 5, 4, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{0, 11, 2, 0, 8, 11, 4, 9, 5, -1, -1, -1, -1, -1, -1, -1},
	{0, 5, 4, 0, 1, 5, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1},
	{2, 1, 5, 2, 5, 8, 2, 8, 11, 4, 8, 5, -1, -1, -1, -1},
	{10, 3, 11, 10, 1, 3, 9, 5, 4, -1, -1, -1, -1, -1, -1, -1},
	{4, 9, 5, 0, 8, 1, 8, 10, 1, 8, 11, 10, -1, -1, -1, -1},
	{5, 4, 0, 5, 0, 11, 5, 11, 10, 11, 0, 3, -1, -1, -1, -1},
	{5, 4, 8, 5, 8, 10, 10, 8, 11, -1, -1, -1, -1, -1, -1, -1},
	{9, 7, 8, 5, 7, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{9, 3, 0, 9, 5, 3, 5, 7, 3, -1, -1, -1, -1, -1, -1, -1},
	{0, 7, 8, 0, 1, 7, 1, 5, 7, -1, -1, -1, -1, -1, -1, -1},
	{1, 5, 3, 3, 5, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{9, 7, 8, 9, 5, 7, 10, 1, 2, -1, -1, -1, -1, -1, -1, -1},
	{10, 1, 2, 9, 5, 0, 5, 3, 0, 5, 7, 3, -1, -1, -1, -1},
	{8, 0, 2, 8, 2, 5, 8, 5, 7, 10, 5, 2, -1, -1, -1, -1},
	{2, 10, 5, 2, 5, 3, 3, 5, 7, -1, -1, -1, -1, -1, -1, -1},
	{7, 9, 5, 7, 8, 9, 3, 11, 2, -1, -1, -1, -1, -1, -1, -1},
	{9, 5, 7, 9, 7, 2, 9, 2, 0, 2, 7, 11, -1, -1, -1, -1},
	{2, 3, 11, 0, 1, 8, 1, 7, 8, 1, 5, 7, -1, -1, -1, -1},
	{11, 2, 1, 11, 1, 7, 7, 1, 5, -1, -1, -1, -1, -1, -1, -1},
	{9, 5, 8, 8, 5, 7, 10, 1, 3, 10, 3, 11, -1, -1, -1, -1},
	{5, 7, 0, 5, 0, 9, 7, 11, 0, 1, 0, 10, 11, 10, 0, -1},
	{11, 10, 0, 11, 0, 3, 10, 5, 0, 8, 0, 7, 5, 7, 0, -1},
	{11, 10, 5, 7, 11, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{10, 6, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{0, 8, 3, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{9, 0, 1, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{1, 8, 3, 1, 9, 8, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1},
	{1, 6, 5, 2, 6, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{1, 6, 5, 1, 2, 6, 3, 0, 8, -1, -1, -1, -1, -1, -1, -1},
	{9, 6, 5, 9, 0, 6, 0, 2, 6, -1, -1, -1, -1, -1, -1, -1},
	{5, 9, 8, 5, 8, 2, 5, 2, 6, 3, 2, 8, -1, -1, -1, -1},
	{2, 3, 11, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{11, 0, 8, 11, 2, 0, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1},
	{0, 1, 9, 2, 3, 11, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1},
	{5, 10, 6, 1, 9, 2, 9, 11, 2, 9, 8, 11, -1, -1, -1, -1},
	{6, 3, 11, 6, 5, 3, 5, 1, 3, -1, -1, -1, -1, -1, -1, -1},
	{0, 8, 11, 0, 11, 5, 0, 5, 1, 5, 11, 6, -1, -1, -1, -1},
	{3, 11, 6, 0, 3, 6, 0, 6, 5, 0, 5, 9, -1, -1, -1, -1},
	{6, 5, 9, 6, 9, 11, 11, 9, 8, -1, -1, -1, -1, -1, -1, -1},
	{5, 10, 6, 4, 7, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{4, 3, 0, 4, 7, 3, 6, 5, 10, -1, -1, -1, -1, -1, -1, -1},
	{1, 9, 0, 5, 10, 6, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1},
	{10, 6, 5, 1, 9, 7, 1, 7, 3, 7, 9, 4, -1, -1, -1, -1},
	{6, 1, 2, 6, 5, 1, 4, 7, 8, -1, -1, -1, -1, -1, -1, -1},
	{1, 2, 5, 5, 2, 6, 3, 0, 4, 3, 4, 7, -1, -1, -1, -1},
	{8, 4, 7, 9, 0, 5, 0, 6, 5, 0, 2, 6, -1, -1, -1, -1},
	{7, 3, 9, 7, 9, 4, 3, 2, 9, 5, 9, 6, 2, 6, 9, -1},
	{3, 11, 2, 7, 8, 4, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1},
	{5, 10, 6, 4, 7, 2, 4, 2, 0, 2, 7, 11, -1, -1, -1, -1},
	{0, 1, 9, 4, 7, 8, 2, 3, 11, 5, 10, 6, -1, -1, -1, -1},
	{9, 2, 1, 9, 11, 2, 9, 4, 11, 7, 11, 4, 5, 10, 6, -1},
	{8, 4, 7, 3, 11, 5, 3, 5, 1, 5, 11, 6, -1, -1, -1, -1},
	{5, 1, 11, 5, 11, 6, 1, 0, 11, 7, 11, 4, 0, 4, 11, -1},
	{0, 5, 9, 0, 6, 5, 0, 3, 6, 11, 6, 3, 8, 4, 7, -1},
	{6, 5, 9, 6, 9, 11, 4, 7, 9, 7, 11, 9, -1, -1, -1, -1},
	{10, 4, 9, 6, 4, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{4, 10, 6, 4, 9, 10, 0, 8, 3, -1, -1, -1, -1, -1, -1, -1},
	{10, 0, 1, 10, 6, 0, 6, 4, 0, -1, -1, -1, -1, -1, -1, -1},
	{8, 3, 1, 8, 1, 6, 8, 6, 4, 6, 1, 10, -1, -1, -1, -1},
	{1, 4, 9, 1, 2, 4, 2, 6, 4, -1, -1, -1, -1, -1, -1, -1},
	{3, 0, 8, 1, 2, 9, 2, 4, 9, 2, 6, 4, -1, -1, -1, -1},
	{0, 2, 4, 4, 2, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{8, 3, 2, 8, 2, 4, 4, 2, 6, -1, -1, -1, -1, -1, -1, -1},
	{10, 4, 9, 10, 6, 4, 11, 2, 3, -1, -1, -1, -1, -1, -1, -1},
	{0, 8, 2, 2, 8, 11, 4, 9, 10, 4, 10, 6, -1, -1, -1, -1},
	{3, 11, 2, 0, 1, 6, 0, 6, 4, 6, 1, 10, -1, -1, -1, -1},
	{6, 4, 1, 6, 1, 10, 4, 8, 1, 2, 1, 11, 8, 11, 1, -1},
	{9, 6, 4, 9, 3, 6, 9, 1, 3, 11, 6, 3, -1, -1, -1, -1},
	{8, 11, 1, 8, 1, 0, 11, 6, 1, 9, 1, 4, 6, 4, 1, -1},
	{3, 11, 6, 3, 6, 0, 0, 6, 4, -1, -1, -1, -1, -1, -1, -1},
	{6, 4, 8, 11, 6, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{7, 10, 6, 7, 8, 10, 8, 9, 10, -1, -1, -1, -1, -1, -1, -1},
	{0, 7, 3, 0, 10, 7, 0, 9, 10, 6, 7, 10, -1, -1, -1, -1},
	{10, 6, 7, 1, 10, 7, 1, 7, 8, 1, 8, 0, -1, -1, -1, -1},
	{10, 6, 7, 10, 7, 1, 1, 7, 3, -1, -1, -1, -1, -1, -1, -1},
	{1, 2, 6, 1, 6, 8, 1, 8, 9, 8, 6, 7, -1, -1, -1, -1},
	{2, 6, 9, 2, 9, 1, 6, 7, 9, 0, 9, 3, 7, 3, 9, -1},
	{7, 8, 0, 7, 0, 6, 6, 0, 2, -1, -1, -1, -1, -1, -1, -1},
	{7, 3, 2, 6, 7, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{2, 3, 11, 10, 6, 8, 10, 8, 9, 8, 6, 7, -1, -1, -1, -1},
	{2, 0, 7, 2, 7, 11, 0, 9, 7, 6, 7, 10, 9, 10, 7, -1},
	{1, 8, 0, 1, 7, 8, 1, 10, 7, 6, 7, 10, 2, 3, 11, -1},
	{11, 2, 1, 11, 1, 7, 10, 6, 1, 6, 7, 1, -1, -1, -1, -1},
	{8, 9, 6, 8, 6, 7, 9, 1, 6, 11, 6, 3, 1, 3, 6, -1},
	{0, 9, 1, 11, 6, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{7, 8, 0, 7, 0, 6, 3, 11, 0, 11, 6, 0, -1, -1, -1, -1},
	{7, 11, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{7, 6, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{3, 0, 8, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{0, 1, 9, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{8, 1, 9, 8, 3, 1, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1},
	{10, 1, 2, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{1, 2, 10, 3, 0, 8, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1},
	{2, 9, 0, 2, 10, 9, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1},
	{6, 11, 7, 2, 10, 3, 10, 8, 3, 10, 9, 8, -1, -1, -1, -1},
	{7, 2, 3, 6, 2, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{7, 0, 8, 7, 6, 0, 6, 2, 0, -1, -1, -1, -1, -1, -1, -1},
	{2, 7, 6, 2, 3, 7, 0, 1, 9, -1, -1, -1, -1, -1, -1, -1},
	{1, 6, 2, 1, 8, 6, 1, 9, 8, 8, 7, 6, -1, -1, -1, -1},
	{10, 7, 6, 10, 1, 7, 1, 3, 7, -1, -1, -1, -1, -1, -1, -1},
	{10, 7, 6, 1, 7, 10, 1, 8, 7, 1, 0, 8, -1, -1, -1, -1},
	{0, 3, 7, 0, 7, 10, 0, 10, 9, 6, 10, 7, -1, -1, -1, -1},
	{7, 6, 10, 7, 10, 8, 8, 10, 9, -1, -1, -1, -1, -1, -1, -1},
	{6, 8, 4, 11, 8, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{3, 6, 11, 3, 0, 6, 0, 4, 6, -1, -1, -1, -1, -1, -1, -1},
	{8, 6, 11, 8, 4, 6, 9, 0, 1, -1, -1, -1, -1, -1, -1, -1},
	{9, 4, 6, 9, 6, 3, 9, 3, 1, 11, 3, 6, -1, -1, -1, -1},
	{6, 8, 4, 6, 11, 8, 2, 10, 1, -1, -1, -1, -1, -1, -1, -1},
	{1, 2, 10, 3, 0, 11, 0, 6, 11, 0, 4, 6, -1, -1, -1, -1},
	{4, 11, 8, 4, 6, 11, 0, 2, 9, 2, 10, 9, -1, -1, -1, -1},
	{10, 9, 3, 10, 3, 2, 9, 4, 3, 11, 3, 6, 4, 6, 3, -1},
	{8, 2, 3, 8, 4, 2, 4, 6, 2, -1, -1, -1, -1, -1, -1, -1},
	{0, 4, 2, 4, 6, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{1, 9, 0, 2, 3, 4, 2, 4, 6, 4, 3, 8, -1, -1, -1, -1},
	{1, 9, 4, 1, 4, 2, 2, 4, 6, -1, -1, -1, -1, -1, -1, -1},
	{8, 1, 3, 8, 6, 1, 8, 4, 6, 6, 10, 1, -1, -1, -1, -1},
	{10, 1, 0, 10, 0, 6, 6, 0, 4, -1, -1, -1, -1, -1, -1, -1},
	{4, 6, 3, 4, 3, 8, 6, 10, 3, 0, 3, 9, 10, 9, 3, -1},
	{10, 9, 4, 6, 10, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{4, 9, 5, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{0, 8, 3, 4, 9, 5, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1},
	{5, 0, 1, 5, 4, 0, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1},
	{11, 7, 6, 8, 3, 4, 3, 5, 4, 3, 1, 5, -1, -1, -1, -1},
	{9, 5, 4, 10, 1, 2, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1},
	{6, 11, 7, 1, 2, 10, 0, 8, 3, 4, 9, 5, -1, -1, -1, -1},
	{7, 6, 11, 5, 4, 10, 4, 2, 10, 4, 0, 2, -1, -1, -1, -1},
	{3, 4, 8, 3, 5, 4, 3, 2, 5, 10, 5, 2, 11, 7, 6, -1},
	{7, 2, 3, 7, 6, 2, 5, 4, 9, -1, -1, -1, -1, -1, -1, -1},
	{9, 5, 4, 0, 8, 6, 0, 6, 2, 6, 8, 7, -1, -1, -1, -1},
	{3, 6, 2, 3, 7, 6, 1, 5, 0, 5, 4, 0, -1, -1, -1, -1},
	{6, 2, 8, 6, 8, 7, 2, 1, 8, 4, 8, 5, 1, 5, 8, -1},
	{9, 5, 4, 10, 1, 6, 1, 7, 6, 1, 3, 7, -1, -1, -1, -1},
	{1, 6, 10, 1, 7, 6, 1, 0, 7, 8, 7, 0, 9, 5, 4, -1},
	{4, 0, 10, 4, 10, 5, 0, 3, 10, 6, 10, 7, 3, 7, 10, -1},
	{7, 6, 10, 7, 10, 8, 5, 4, 10, 4, 8, 10, -1, -1, -1, -1},
	{6, 9, 5, 6, 11, 9, 11, 8, 9, -1, -1, -1, -1, -1, -1, -1},
	{3, 6, 11, 0, 6, 3, 0, 5, 6, 0, 9, 5, -1, -1, -1, -1},
	{0, 11, 8, 0, 5, 11, 0, 1, 5, 5, 6, 11, -1, -1, -1, -1},
	{6, 11, 3, 6, 3, 5, 5, 3, 1, -1, -1, -1, -1, -1, -1, -1},
	{1, 2, 10, 9, 5, 11, 9, 11, 8, 11, 5, 6, -1, -1, -1, -1},
	{0, 11, 3, 0, 6, 11, 0, 9, 6, 5, 6, 9, 1, 2, 10, -1},
	{11, 8, 5, 11, 5, 6, 8, 0, 5, 10, 5, 2, 0, 2, 5, -1},
	{6, 11, 3, 6, 3, 5, 2, 10, 3, 10, 5, 3, -1, -1, -1, -1},
	{5, 8, 9, 5, 2, 8, 5, 6, 2, 3, 8, 2, -1, -1, -1, -1},
	{9, 5, 6, 9, 6, 0, 0, 6, 2, -1, -1, -1, -1, -1, -1, -1},
	{1, 5, 8, 1, 8, 0, 5, 6, 8, 3, 8, 2, 6, 2, 8, -1},
	{1, 5, 6, 2, 1, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{1, 3, 6, 1, 6, 10, 3, 8, 6, 5, 6, 9, 8, 9, 6, -1},
	{10, 1, 0, 10, 0, 6, 9, 5, 0, 5, 6, 0, -1, -1, -1, -1},
	{0, 3, 8, 5, 6, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{10, 5, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{11, 5, 10, 7, 5, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{11, 5, 10, 11, 7, 5, 8, 3, 0, -1, -1, -1, -1, -1, -1, -1},
	{5, 11, 7, 5, 10, 11, 1, 9, 0, -1, -1, -1, -1, -1, -1, -1},
	{10, 7, 5, 10, 11, 7, 9, 8, 1, 8, 3, 1, -1, -1, -1, -1},
	{11, 1, 2, 11, 7, 1, 7, 5, 1, -1, -1, -1, -1, -1, -1, -1},
	{0, 8, 3, 1, 2, 7, 1, 7, 5, 7, 2, 11, -1, -1, -1, -1},
	{9, 7, 5, 9, 2, 7, 9, 0, 2, 2, 11, 7, -1, -1, -1, -1},
	{7, 5, 2, 7, 2, 11, 5, 9, 2, 3, 2, 8, 9, 8, 2, -1},
	{2, 5, 10, 2, 3, 5, 3, 7, 5, -1, -1, -1, -1, -1, -1, -1},
	{8, 2, 0, 8, 5, 2, 8, 7, 5, 10, 2, 5, -1, -1, -1, -1},
	{9, 0, 1, 5, 10, 3, 5, 3, 7, 3, 10, 2, -1, -1, -1, -1},
	{9, 8, 2, 9, 2, 1, 8, 7, 2, 10, 2, 5, 7, 5, 2, -1},
	{1, 3, 5, 3, 7, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{0, 8, 7, 0, 7, 1, 1, 7, 5, -1, -1, -1, -1, -1, -1, -1},
	{9, 0, 3, 9, 3, 5, 5, 3, 7, -1, -1, -1, -1, -1, -1, -1},
	{9, 8, 7, 5, 9, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{5, 8, 4, 5, 10, 8, 10, 11, 8, -1, -1, -1, -1, -1, -1, -1},
	{5, 0, 4, 5, 11, 0, 5, 10, 11, 11, 3, 0, -1, -1, -1, -1},
	{0, 1, 9, 8, 4, 10, 8, 10, 11, 10, 4, 5, -1, -1, -1, -1},
	{10, 11, 4, 10, 4, 5, 11, 3, 4, 9, 4, 1, 3, 1, 4, -1},
	{2, 5, 1, 2, 8, 5, 2, 11, 8, 4, 5, 8, -1, -1, -1, -1},
	{0, 4, 11, 0, 11, 3, 4, 5, 11, 2, 11, 1, 5, 1, 11, -1},
	{0, 2, 5, 0, 5, 9, 2, 11, 5, 4, 5, 8, 11, 8, 5, -1},
	{9, 4, 5, 2, 11, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{2, 5, 10, 3, 5, 2, 3, 4, 5, 3, 8, 4, -1, -1, -1, -1},
	{5, 10, 2, 5, 2, 4, 4, 2, 0, -1, -1, -1, -1, -1, -1, -1},
	{3, 10, 2, 3, 5, 10, 3, 8, 5, 4, 5, 8, 0, 1, 9, -1},
	{5, 10, 2, 5, 2, 4, 1, 9, 2, 9, 4, 2, -1, -1, -1, -1},
	{8, 4, 5, 8, 5, 3, 3, 5, 1, -1, -1, -1, -1, -1, -1, -1},
	{0, 4, 5, 1, 0, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{8, 4, 5, 8, 5, 3, 9, 0, 5, 0, 3, 5, -1, -1, -1, -1},
	{9, 4, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{4, 11, 7, 4, 9, 11, 9, 10, 11, -1, -1, -1, -1, -1, -1, -1},
	{0, 8, 3, 4, 9, 7, 9, 11, 7, 9, 10, 11, -1, -1, -1, -1},
	{1, 10, 11, 1, 11, 4, 1, 4, 0, 7, 4, 11, -1, -1, -1, -1},
	{3, 1, 4, 3, 4, 8, 1, 10, 4, 7, 4, 11, 10, 11, 4, -1},
	{4, 11, 7, 9, 11, 4, 9, 2, 11, 9, 1, 2, -1, -1, -1, -1},
	{9, 7, 4, 9, 11, 7, 9, 1, 11, 2, 11, 1, 0, 8, 3, -1},
	{11, 7, 4, 11, 4, 2, 2, 4, 0, -1, -1, -1, -1, -1, -1, -1},
	{11, 7, 4, 11, 4, 2, 8, 3, 4, 3, 2, 4, -1, -1, -1, -1},
	{2, 9, 10, 2, 7, 9, 2, 3, 7, 7, 4, 9, -1, -1, -1, -1},
	{9, 10, 7, 9, 7, 4, 10, 2, 7, 8, 7, 0, 2, 0, 7, -1},
	{3, 7, 10, 3, 10, 2, 7, 4, 10, 1, 10, 0, 4, 0, 10, -1},
	{1, 10, 2, 8, 7, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{4, 9, 1, 4, 1, 7, 7, 1, 3, -1, -1, -1, -1, -1, -1, -1},
	{4, 9, 1, 4, 1, 7, 0, 8, 1, 8, 7, 1, -1, -1, -1, -1},
	{4, 0, 3, 7, 4, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{4, 8, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{9, 10, 8, 10, 11, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{3, 0, 9, 3, 9, 11, 11, 9, 10, -1, -1, -1, -1, -1, -1, -1},
	{0, 1, 10, 0, 10, 8, 8, 10, 11, -1, -1, -1, -1, -1, -1, -1},
	{3, 1, 10, 11, 3, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{1, 2, 11, 1, 11, 9, 9, 11, 8, -1, -1, -1, -1, -1, -1, -1},
	{3, 0, 9, 3, 9, 11, 1, 2, 9, 2, 11, 9, -1, -1, -1, -1},
	{0, 2, 11, 8, 0, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{3, 2, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{2, 3, 8, 2, 8, 10, 10, 8, 9, -1, -1, -1, -1, -1, -1, -1},
	{9, 10, 2, 0, 9, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{2, 3, 8, 2, 8, 10, 0, 1, 8, 1, 10, 8, -1, -1, -1, -1},
	{1, 10, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{1, 3, 8, 9, 1, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{0, 9, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{0, 3, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}
};