#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "MassBattleMinimapRegion.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMinimapDataUpdated);

/**
 * AMinimapRegion
 * An actor that defines the spatial bounds and configuration for the Minimap.
 * Exposed for Blueprint inheritance.
 */
UCLASS(Blueprintable, BlueprintType)
class MASSBATTLEMINIMAP_API AMinimapRegion : public AActor
{
	GENERATED_BODY()

public:
	AMinimapRegion();

	/** The logical boundaries of the map (where the action happens). */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Minimap", meta = (DisplayName = "逻辑地图包围盒", ToolTip = "定义地图的可移动和逻辑边界。"))
	UBoxComponent* BoundsComponent;

	/** Visualizer for the overflow boundary (Bounds + Overflow). */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Minimap", meta = (DisplayName = "视野溢出包围盒", ToolTip = "显示包含缓冲区后的实际视野限制范围。"))
	UBoxComponent* OverflowComponent;

	/** Visual buffer outside logical map (UU). Shared with Camera/FogOfWar for edge protection. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Minimap Config", meta = (ClampMin = "0.0", DisplayName = "边界溢出长度 (UU)", ToolTip = "在逻辑地图边缘之外允许相机看到的缓冲区宽度。单位为厘米(UU)。"))
	float MapOverflowUU = 512.0f;

	/** Minimap texture resolution (e.g. 1024, 2048, 4096). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Minimap Config", meta = (DisplayName = "小地图网格分辨率", ToolTip = "定义小地图纹理的精度，例如 2048 代表 2048x2048。"))
	FIntPoint GridResolution = FIntPoint(2048, 2048);

	/** Delegate triggered when minimap data (unit positions etc) changes. */
	UPROPERTY(BlueprintAssignable, Category = "Minimap Events", meta = (DisplayName = "数据更新回调", ToolTip = "当小地图数据（如单位位置）发生变化时触发，供 UI 重画使用。"))
	FOnMinimapDataUpdated OnDataUpdated;

	//~ Actor Interface
	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform& Transform) override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	/** Helper to signal UI that it needs to redraw. */
	UFUNCTION(BlueprintCallable, Category = "Minimap")
	void NotifyDataUpdated() { OnDataUpdated.Broadcast(); }

private:
	void UpdateVisuals();
};
