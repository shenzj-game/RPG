// Copyright shenzj


#include "UI/WidgetController/RPGWidgetController.h"

#include "AbilitySystem/RPGAttributeSet.h"

void URPGWidgetController::SetWidgetControllerParams(const FWidgetControllerParams& WCParams)
{
	PlayerController = WCParams.PlayerController;
	PlayerState = WCParams.PlayerState;
	AbilitySystemComponent = WCParams.AbilitySystemComponent;
	AttributeSet = WCParams.AttributeSet;
}

void URPGWidgetController::BroadcastInitialValues()
{
	
}

void URPGWidgetController::BindCallbacksToDependencies()
{

}
