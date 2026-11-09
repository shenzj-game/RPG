// Copyright shenzj


#include "AbilitySystem/Data/AbilityInfo.h"

#include "RPG/RPGLogChannels.h"

FRPGAbilityInfo UAbilityInfo::FindAbilityInfoByTag(const FGameplayTag& AbilityTag, bool bLogNotFound) const
{
	for (const FRPGAbilityInfo& Info : AbilityInformation)
	{
		if (Info.AbilityTag == AbilityTag)
		{
			return Info;
		}
	}
	
	if (bLogNotFound)
	{
		UE_LOG(LogRPG, Error, TEXT("Can't find info for AbilityTag [%s] on AbilityInfo [%s]"), *AbilityTag.ToString(), *GetNameSafe(this));
	}
	
	return FRPGAbilityInfo();
}
