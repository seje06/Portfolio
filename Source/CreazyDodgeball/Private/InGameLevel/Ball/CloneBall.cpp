#include "InGameLevel/Ball/CloneBall.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Character.h"



// Sets default values
ACloneBall::ACloneBall()
{
 	PrimaryActorTick.bCanEverTick = true;

    // 충돌 영역
    sphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
    RootComponent = sphereComp;
    sphereComp->SetSphereRadius(30.f);
    sphereComp->SetSimulatePhysics(true);
    sphereComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    sphereComp->SetCollisionResponseToAllChannels(ECR_Block);
    sphereComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
    sphereComp->OnComponentHit.AddDynamic(this, &ACloneBall::OnHit);

    // 시각 메쉬
    meshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
    meshComp->SetupAttachment(sphereComp);
    meshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    /*static ConstructorHelpers::FObjectFinder<UStaticMesh> meshAsset(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
    if (meshAsset.Succeeded())
    {
        meshComp->SetStaticMesh(meshAsset.Object);
    }*/
}

// Called when the game starts or when spawned
void ACloneBall::BeginPlay()
{
	Super::BeginPlay();
 
	
}

void ACloneBall::OnHit(UPrimitiveComponent* _hitComp, AActor* _otherActor, UPrimitiveComponent* _otherComp, FVector _normalImpulse, const FHitResult& _hit)
{
    if (!_otherActor || _otherActor == this) return;;

    //땅에 닿았을 때
    if (_otherActor->ActorHasTag("Ground"))
    {
        sphereComp->SetPhysicsLinearVelocity(FVector(sphereComp->GetPhysicsLinearVelocity().X, sphereComp->GetPhysicsLinearVelocity().Y, 0));
        bIsGrounded = true;
         UE_LOG(LogTemp, Warning, TEXT("ground Clone!!"));
    }
}

// Called every frame
void ACloneBall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

