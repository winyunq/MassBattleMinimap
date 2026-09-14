#include "MassBattleMinimapRegion.h"
#include "Components/BoxComponent.h"
#include "DrawDebugHelpers.h"

AMinimapRegion::AMinimapRegion()
{
	PrimaryActorTick.bCanEverTick = false;

	BoundsComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("MapBounds"));
	RootComponent = BoundsComponent;
	// 设置半长度为 0.5，这样总长度为 1.0。
	// 此时 Actor 的 Scale 属性直接等于地图的 UU 尺寸 (例如 Scale 65536 = 65536 UU)。
	BoundsComponent->SetBoxExtent(FVector(0.5f, 0.5f, 0.5f));
	// The region is metadata/editor visualization only. If it participates in
	// collision, its map-sized bounds make every build-placement overlap test
	// fail and can also interfere with other world queries.
	BoundsComponent->SetCollisionProfileName(FName(TEXT("NoCollision")));
	BoundsComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BoundsComponent->SetGenerateOverlapEvents(false);
	BoundsComponent->ShapeColor = FColor::Green;
	BoundsComponent->bDrawOnlyIfSelected = false;
	OverflowComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("OverflowBounds"));
	OverflowComponent->SetupAttachment(RootComponent);
	OverflowComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	// 视觉上使用绝对缩放，避免父级缩放干扰溢出区 UU 计算
	OverflowComponent->SetUsingAbsoluteScale(true);
	OverflowComponent->ShapeColor = FColor::Cyan;
	OverflowComponent->bDrawOnlyIfSelected = false;
}

void AMinimapRegion::BeginPlay()
{
	Super::BeginPlay();
	UpdateVisuals();
}

void AMinimapRegion::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	
	if (BoundsComponent)
	{
		// 强制物理底座为 0.5，确保 Scale 1.0 = 1.0 UU 全宽。
		// 这样即便编辑器之前缓存了 32 的默认值，也会被这里的代码在构造时强行修正。
		BoundsComponent->SetBoxExtent(FVector(0.5f, 0.5f, 0.5f));
		// Re-apply for existing serialized level instances created before the
		// component default was corrected.
		BoundsComponent->SetCollisionProfileName(FName(TEXT("NoCollision")));
		BoundsComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		BoundsComponent->SetGenerateOverlapEvents(false);
	}

	UpdateVisuals();
}

#if WITH_EDITOR
void AMinimapRegion::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	UpdateVisuals();
}
#endif

void AMinimapRegion::UpdateVisuals()
{
	if (BoundsComponent && OverflowComponent)
	{
		// 修正单位问题：UE 的组件附加会继承缩放。
		// 如果用户对 BoundsComponent (Root) 进行了缩放(如 100倍)，
		// 直接在 UnscaledBoxExtent 上加 Overflow 会导致误差被放大 100 倍。
		
		// 解决方法：我们将 OverflowComponent 设置为绝对缩放，并手动计算其需要的世界空间 BoxExtent
		OverflowComponent->SetUsingAbsoluteScale(true);
		OverflowComponent->SetWorldScale3D(FVector(1.0f, 1.0f, 1.0f));

		const FVector ScaledMapExtent = BoundsComponent->GetScaledBoxExtent();
		OverflowComponent->SetBoxExtent(FVector(ScaledMapExtent.X + MapOverflowUU, ScaledMapExtent.Y + MapOverflowUU, ScaledMapExtent.Z + 10.0f));
	}
}
