#include "MoreHardDriveRerolls.h"
#include "FGResearchManager.h"
#include "FGResearchSettings.h"
#include "SessionSettingsManager.h"

void AMoreHardDriveRerolls::BeginPlay()
{
	Super::BeginPlay();

	UpdateMaxRerolls();
}

AMoreHardDriveRerolls::AMoreHardDriveRerolls()
{
	ReplicationPolicy = ESubsystemReplicationPolicy::SpawnOnServer_Replicate;
}

void AMoreHardDriveRerolls::UpdateMaxRerolls()
{
	if (auto* ResearchManager = GetMutableDefault<UFGResearchSettings>())
	{
		const auto sessionManager = GetWorld()->GetSubsystem<USessionSettingsManager>();
		auto MaxHardDriveRescans = static_cast<int>(sessionManager->GetFloatOptionValue("MaxHardDriveRescans"));

		ResearchManager->mNumRerollsPerHardDrive = MaxHardDriveRescans > 0 ? MaxHardDriveRescans : 1;
	}
}

FRerollHardDriveData AMoreHardDriveRerolls::GetHardDriveData(const UFGHardDrive* HardDrive)
{
	auto UnclaimedHardDriveData = AFGResearchManager::Get(GetWorld())->mUnclaimedHardDriveData;
	FRerollHardDriveData HardDriveData;

	for (auto UnclaimedData : UnclaimedHardDriveData)
	{
		if (UnclaimedData.HardDrive != HardDrive) continue;

		const auto NumRerollPerHardDrive = UFGResearchSettings::Get()->mNumRerollsPerHardDrive;
		const int32 RerollsExecuted = UnclaimedData.PendingRewardsRerollsExecuted;
		const int32 RemainingRerolls = NumRerollPerHardDrive - RerollsExecuted;

		HardDriveData.PendingRewardsRerollsExecuted = RerollsExecuted;
		HardDriveData.RemainingRerolls = RemainingRerolls > 0 ? RemainingRerolls : 0;

		break;
	}

	return HardDriveData;
}