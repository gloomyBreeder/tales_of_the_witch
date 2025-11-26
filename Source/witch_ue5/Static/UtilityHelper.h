#pragma once

#include "CoreMinimal.h"

class WITCH_UE5_API UtilityHelper
{
public:
    UtilityHelper() = delete;

	template <typename TEnum>
	static FName EnumToFName(TEnum EnumValue)
	{
		const UEnum* EnumPtr = StaticEnum<TEnum>();
		if (!EnumPtr)
		{
			return FName();
		}
		return EnumPtr->GetNameByValue(static_cast<int64>(EnumValue));
	}
};