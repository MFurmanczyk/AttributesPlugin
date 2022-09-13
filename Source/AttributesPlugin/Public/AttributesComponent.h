// Copyright© 2022, MFurmanczyk. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttributesComponent.generated.h"

USTRUCT(BlueprintType)
struct FAttribute
{
	GENERATED_BODY()

	FAttribute() 
	{
		MaxValue = 0;
		CurrentValue = 0;
	};

	FAttribute(float _Value)
	{
		MaxValue = _Value;
		CurrentValue = _Value;
	};

	FAttribute(float _Value, float _MaxValue)
	{
		MaxValue = _MaxValue;
		CurrentValue = _Value;
	}

	FORCEINLINE void SetMaxValue(float NewValue) { MaxValue = NewValue; };
	FORCEINLINE void SetValue(float NewValue) { CurrentValue = NewValue; };

	FORCEINLINE float GetMaxValue() const { return MaxValue; };
	FORCEINLINE float GetCurrentValue() const { return CurrentValue; };

	FORCEINLINE FAttribute& operator+=(const float& Delta)
	{
		this->CurrentValue += Delta;
		return *this;
	};

	FORCEINLINE FAttribute operator+(const float& Delta)
	{
		FAttribute NewValue = (*this);
		NewValue += Delta;
		return NewValue;
	};

	FORCEINLINE FAttribute& operator-=(const float& Delta)
	{
		this->CurrentValue -= Delta;
		return *this;
	};

	FORCEINLINE FAttribute operator-(const float& Delta)
	{
		FAttribute NewValue = (*this);
		NewValue -= Delta;
		return NewValue;
	};

	friend bool operator==(const FAttribute& LVal, const float& RVal);
	friend bool operator!=(const FAttribute& LVal, const float& RVal);
	friend bool operator>(const FAttribute& LVal, const float& RVal);
	friend bool operator<(const FAttribute& LVal, const float& RVal);
	friend bool operator>=(const FAttribute& LVal, const float& RVal);
	friend bool operator<=(const FAttribute& LVal, const float& RVal);

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
		float MaxValue;

	UPROPERTY(BlueprintReadWrite, Category = "Attributes")
		float CurrentValue;

};

FORCEINLINE bool operator==(const FAttribute& LVal, const float& RVal) { return LVal.CurrentValue == RVal; };
FORCEINLINE bool operator!=(const FAttribute& LVal, const float& RVal) { return LVal.CurrentValue != RVal; };
FORCEINLINE bool operator>(const FAttribute& LVal, const float& RVal) { return LVal.CurrentValue > RVal; };
FORCEINLINE bool operator<(const FAttribute& LVal, const float& RVal) { return LVal.CurrentValue < RVal; };
FORCEINLINE bool operator>=(const FAttribute& LVal, const float& RVal) { return LVal.CurrentValue >= RVal; };
FORCEINLINE bool operator<=(const FAttribute& LVal, const float& RVal) { return LVal.CurrentValue <= RVal; };

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnOwnerKilled, AActor*, Instigator, UAttributesComponent*, OwnerComp);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnHealthValueChanged, AActor*, Instigator, UAttributesComponent*, OwnerComp, FAttribute, AttributeValue, float, Delta);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnXPValueChanged, AActor*, Instigator, UAttributesComponent*, OwnerComp, FAttribute, AttributeValue, float, Delta);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnLevelValueChanged, AActor*, Instigator, UAttributesComponent*, OwnerComp, FAttribute, Level, float, XPExcess);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(FOnCustomAttributeValueChanged, AActor*, Instigator, UAttributesComponent*, OwnerComp, FName, AttributeName, FAttribute, AttributeValue, float, Delta);


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent, DisplayName = "Attributes Component", ShortTooltip = "An AttributesComponent allows to add the various statistics like level, health, rtc. to custom actor.") )
class ATTRIBUTESPLUGIN_API UAttributesComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UAttributesComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:

	/**
	* Initializes every attribute value.
	* If you want custom initialization (eg. loading persisted data from previous level) override this function.
	*/
	virtual void InitializeComponent() override;

	/**
	* Adds Delta to Current value of Health attribute
	* Clamps value between 0 and MaxValue of attribute
	*/
	UFUNCTION(BlueprintCallable, Category = "Attributes")
		virtual FAttribute ApplyHealthValueChange(float Delta);

	/**
	* Adds Delta to Current value of XP attribute
	* If MaxValue is reached increment Level value
	*/
	UFUNCTION(BlueprintCallable, Category = "Attributes")
		virtual FAttribute ApplyXPValueChange(float Delta);

	/**
	* Adds Delta to Current value of Custom attribute with given AttributeName
	* Clamps value between 0 and MaxValue of attribute
	*/
	UFUNCTION(BlueprintCallable, Category = "Attributes")
		virtual FAttribute ApplyCustomAttributeValueChange(const FName AttributeName, float Delta);

	/**
	* Sets Value of Health attribute. If bShouldBeClamped = true Value will be clamped between 0 and MaxValue of th
	*/
	UFUNCTION(BlueprintCallable, Category = "Attributes")
		void SetHealthValue(float Value, bool bShouldBeClamped = false);

	/**
	* Sets Value of Custom attribute with given Name. If bShouldBeClamped = true Value will be clamped between 0 and MaxValue of the attribute.
	*/
	UFUNCTION(BlueprintCallable, Category = "Attributes")
		void SetCustomAttributeValue(const FName AttributeName, float Value, bool bShouldBeClamped = false);
	
	/**
	* Sets Value of XP attribute. If bShouldBeClamped = true Value will be clamped between 0 and MaxValue of the attribute.
	*/
	UFUNCTION(BlueprintCallable, Category = "Attributes")
		void SetXPValue(float Value, bool bShouldBeClamped = false);

	/**
	* Sets Value of Level attribute. If bShouldBeClamped = true Value will be clamped between 0 and MaxValue of the attribute.
	*/
	UFUNCTION(BlueprintCallable, Category = "Attributes")
		void SetLevelValue(int32 Value, float XPExcess = 0.f, bool bShouldBeClamped = false);

	/**
	* Informs that owner has been killed.
	*/
	UFUNCTION(BlueprintCallable, Category = "Attributes")
		void KillOwner(AActor* Instigator);

	/**
	* Returns true if current Health value <= 0.
	*/
	UFUNCTION(BlueprintCallable, Category = "Attributes")
		FORCEINLINE bool ShouldBeDead() const { return Health <= 0.f; };

	//Getters
	FORCEINLINE FAttribute GetHealth() const { return Health; };
	FORCEINLINE FAttribute GetXP() const { return XP; };
	FORCEINLINE FAttribute GetLevel() const { return Level; };
	FORCEINLINE FAttribute GetCustomAttribute(const FName& AttributeName) const { return CustomAttributes.FindRef(AttributeName); };

protected:

	/*
	* Executes once owner dies.
	* Put here everything you want to do inside component after change operation.
	* In order to override this function in C++ use Internal_OnOwnerKilled_Implementation.
	*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Attributes|Events", meta = (BlueprintProtected))
		void Internal_OnOwnerKilled(AActor* Instigator, UAttributesComponent* OwnerComp);

	/**
	* Executes everytime change to Health value is applied.
	* Put here everything you want to do inside component after change operation.
	* In order to override this function in C++ use Internal_OnHealthValueChanged_Implementation.
	*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Attributes|Events", meta = (BlueprintProtected))
		void Internal_OnHealthValueChanged(AActor* Instigator, UAttributesComponent* OwnerComp, FAttribute AttributeValue, float Delta);

	/**
	* Executes everytime change to XP value is applied.
	* Put here everything you want to do inside component after change operation.
	* In order to override this function in C++ use Internal_OnXPValueChanged_Implementation.
	*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Attributes|Events", meta = (BlueprintProtected))
		void Internal_OnXPValueChanged(AActor* Instigator, UAttributesComponent* OwnerComp, FAttribute AttributeValue, float Delta);

	/**
	* Executes everytime change to Level value is applied.All changes related to levelling up can be done here(setting new MaxValue of XP, Health etc.).
	* Put here everything you want to do inside component after change operation.
	* In order to override this function in C++ use Internal_OnLevelValueChanged_Implementation.
	*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Attributes|Events", meta = (BlueprintProtected))
		void Internal_OnLevelValueChanged(AActor* Instigator, UAttributesComponent* OwnerComp, FAttribute AttributeValue, float XPExcess);

	/**
	* Executes everytime change to CustomAttribute(with given AttributeName) value is applied.
	* Put here everything you want to do inside component after change operation.
	* In order to override this function in C++ use Internal_OnCustomAttributeValueChanged_Implementation.
	*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Attributes|Events", meta = (BlueprintProtected))
		void Internal_OnCustomAttributeValueChanged(AActor* Instigator, UAttributesComponent* OwnerComp, FName AttributeName, FAttribute AttributeValue, float Delta);


	virtual void Internal_OnOwnerKilled_Implementation(AActor* Instigator, UAttributesComponent* OwnerComp) {};
	virtual void Internal_OnHealthValueChanged_Implementation(AActor* Instigator, UAttributesComponent* OwnerComp, FAttribute AttributeValue, float Delta) {};
	virtual void Internal_OnXPValueChanged_Implementation(AActor* Instigator, UAttributesComponent* OwnerComp, FAttribute AttributeValue, float Delta) {};
	virtual void Internal_OnLevelValueChanged_Implementation(AActor* Instigator, UAttributesComponent* OwnerComp, FAttribute AttributeValue, float XPExcess) {};
	virtual void Internal_OnCustomAttributeValueChanged_Implementation(AActor* Instigator, UAttributesComponent* OwnerComp, FName AttributeName, FAttribute AttributeValue, float Delta) {};

	virtual void AddCustomAttribute(const FName& AttributeName);
	virtual void AddCustomAttribute(const FName& AttributeName, FAttribute& Attribute);
protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, Category = "Attributes")
		FAttribute Health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, Category = "Attributes|Leveling")
		FAttribute Level;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, Category = "Attributes|Leveling")
		int32 StartLevel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, Category = "Attributes|Leveling")
		FAttribute XP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, Category = "Attributes|Custom Attributes")
		TMap<FName, FAttribute> CustomAttributes;

	//Executes everytime change to Health value is applied. Use it to execute operations outside component eg. updating UI
	UPROPERTY(BlueprintAssignable, Category = "Attributes|Events")
		FOnHealthValueChanged OnHealthValueChanged;

	//Executes everytime change to XP value is applied. Use it to execute operations outside component eg. updating UI
	UPROPERTY(BlueprintAssignable, Category = "Attributes|Events")
		FOnXPValueChanged OnXPValueChanged;

	//Executes everytime change to CustomAttribute (with given AttributeName) value is applied. Use it to execute operations outside component eg. updating UI
	UPROPERTY(BlueprintAssignable, Category = "Attributes|Events")
		FOnCustomAttributeValueChanged OnCustomAttributeValueChanged;
	 
	//Executes once owner dies. Use it to execute operations outside component eg. updating UI
	UPROPERTY(BlueprintAssignable, Category = "Attributes|Events")
		FOnOwnerKilled OnOwnerKilled;

	//Executes everytime change to Level value is applied. Use it to execute operations outside component eg. updating UI
	UPROPERTY(BlueprintAssignable, Category = "Attributes|Events")
		FOnLevelValueChanged OnLevelValueChanged;

};