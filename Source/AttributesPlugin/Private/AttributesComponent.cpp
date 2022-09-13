// Copyright© 2022, MFurmanczyk. All rights reserved.


#include "AttributesComponent.h"

// Sets default values for this component's properties
UAttributesComponent::UAttributesComponent()
{
	bAutoActivate = true;
	bWantsInitializeComponent = true;
}


// Called when the game starts
void UAttributesComponent::BeginPlay()
{
	Super::BeginPlay();

}

void UAttributesComponent::InitializeComponent()
{
	Super::InitializeComponent();

	SetHealthValue(Health.GetMaxValue());
	SetXPValue(XP.GetMaxValue());
	SetLevelValue(StartLevel);

	TArray<FName> Keys;
	CustomAttributes.GetKeys(Keys);

	for (FName& Key : Keys)
	{
		SetCustomAttributeValue(Key, CustomAttributes.FindRef(Key).GetMaxValue());
	}
}

FAttribute UAttributesComponent::ApplyHealthValueChange(float Delta)
{
	Health += Delta;
	Health.SetValue(FMath::Clamp(Health.GetCurrentValue(), 0.f, Health.GetMaxValue()));

	Internal_OnHealthValueChanged(nullptr, this, Health, Delta);
	OnHealthValueChanged.Broadcast(nullptr, this, Health, 0.f);

	return Health;
}

FAttribute UAttributesComponent::ApplyXPValueChange(float Delta)
{
	XP += Delta;

	Internal_OnXPValueChanged(nullptr, this, XP, Delta);
	OnXPValueChanged.Broadcast(nullptr, this, XP, 0.f);

	return XP;
}

FAttribute UAttributesComponent::ApplyCustomAttributeValueChange(const FName AttributeName, float Delta)
{
	FAttribute AttributeValue = CustomAttributes.FindRef(AttributeName);
	AttributeValue += Delta;
	AttributeValue.SetValue(FMath::Clamp(AttributeValue.GetCurrentValue(), 0.f, AttributeValue.GetMaxValue()));

	CustomAttributes.Emplace(AttributeName, AttributeValue);

	Internal_OnCustomAttributeValueChanged(nullptr, this, AttributeName, AttributeValue, Delta);
	OnCustomAttributeValueChanged.Broadcast(nullptr, this, AttributeName, AttributeValue, 0.f);

	return AttributeValue;
}

void UAttributesComponent::SetHealthValue(float Value, bool bShouldBeClamped)
{
	if (bShouldBeClamped)
	{
		Health.SetValue(FMath::Clamp(Value, 0.f, Health.GetMaxValue()));
	}
	else
	{
		Health.SetValue(Value);
	}

	Internal_OnHealthValueChanged(nullptr, this, Health, 0.f);
	OnHealthValueChanged.Broadcast(nullptr, this, Health, 0.f);
}

void UAttributesComponent::SetCustomAttributeValue(const FName AttributeName, float Value, bool bShouldBeClamped)
{
	FAttribute Attribute = CustomAttributes.FindRef(AttributeName);
	if (bShouldBeClamped)
	{
		Attribute.SetValue(FMath::Clamp(Value, 0.f, Attribute.GetMaxValue()));
	}
	else
	{
		Attribute.SetValue(Value);
	}
	CustomAttributes.Emplace(AttributeName, Attribute);

	Internal_OnCustomAttributeValueChanged(nullptr, this, AttributeName, Attribute, 0.f);
	OnCustomAttributeValueChanged.Broadcast(nullptr, this, AttributeName, Attribute, 0.f);
}

void UAttributesComponent::SetXPValue(float Value, bool bShouldBeClamped)
{
	if (bShouldBeClamped)
	{
		XP.SetValue(FMath::Clamp(Value, 0.f, XP.GetMaxValue()));
	}
	else
	{
		XP.SetValue(Value);
	}

	Internal_OnXPValueChanged(nullptr, this, XP, 0.f);
	OnXPValueChanged.Broadcast(nullptr, this, XP, 0.f);
}

void UAttributesComponent::SetLevelValue(int32 Value, float XPExcess, bool bShouldBeClamped)
{
	Level.SetValue(Value);

	Internal_OnLevelValueChanged(nullptr, this, Level, XPExcess);
	OnLevelValueChanged.Broadcast(nullptr, this, Level, XPExcess);
}

void UAttributesComponent::KillOwner(AActor* Instigator)
{
	Internal_OnOwnerKilled(nullptr, this);
	OnOwnerKilled.Broadcast(nullptr, this);
}

void UAttributesComponent::AddCustomAttribute(const FName& AttributeName)
{
	FAttribute Attribute(0.f, 0.f);
	CustomAttributes.Add(AttributeName, Attribute);
}

void UAttributesComponent::AddCustomAttribute(const FName& AttributeName, FAttribute& Attribute)
{
	CustomAttributes.Add(AttributeName, Attribute);
}
