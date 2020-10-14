// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
#ifdef SOFTDESIGNTRAINING_SDTProjectile_generated_h
#error "SDTProjectile.generated.h already included, missing '#pragma once' in SDTProjectile.h"
#endif
#define SOFTDESIGNTRAINING_SDTProjectile_generated_h

#define TP2_Source_SoftDesignTraining_SDTProjectile_h_15_SPARSE_DATA
#define TP2_Source_SoftDesignTraining_SDTProjectile_h_15_RPC_WRAPPERS
#define TP2_Source_SoftDesignTraining_SDTProjectile_h_15_RPC_WRAPPERS_NO_PURE_DECLS
#define TP2_Source_SoftDesignTraining_SDTProjectile_h_15_INCLASS_NO_PURE_DECLS \
private: \
	static void StaticRegisterNativesASDTProjectile(); \
	friend struct Z_Construct_UClass_ASDTProjectile_Statics; \
public: \
	DECLARE_CLASS(ASDTProjectile, AStaticMeshActor, COMPILED_IN_FLAGS(0 | CLASS_Config), CASTCLASS_None, TEXT("/Script/SoftDesignTraining"), NO_API) \
	DECLARE_SERIALIZER(ASDTProjectile)


#define TP2_Source_SoftDesignTraining_SDTProjectile_h_15_INCLASS \
private: \
	static void StaticRegisterNativesASDTProjectile(); \
	friend struct Z_Construct_UClass_ASDTProjectile_Statics; \
public: \
	DECLARE_CLASS(ASDTProjectile, AStaticMeshActor, COMPILED_IN_FLAGS(0 | CLASS_Config), CASTCLASS_None, TEXT("/Script/SoftDesignTraining"), NO_API) \
	DECLARE_SERIALIZER(ASDTProjectile)


#define TP2_Source_SoftDesignTraining_SDTProjectile_h_15_STANDARD_CONSTRUCTORS \
	/** Standard constructor, called after all reflected properties have been initialized */ \
	NO_API ASDTProjectile(const FObjectInitializer& ObjectInitializer); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(ASDTProjectile) \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, ASDTProjectile); \
DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(ASDTProjectile); \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	NO_API ASDTProjectile(ASDTProjectile&&); \
	NO_API ASDTProjectile(const ASDTProjectile&); \
public:


#define TP2_Source_SoftDesignTraining_SDTProjectile_h_15_ENHANCED_CONSTRUCTORS \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	NO_API ASDTProjectile(ASDTProjectile&&); \
	NO_API ASDTProjectile(const ASDTProjectile&); \
public: \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, ASDTProjectile); \
DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(ASDTProjectile); \
	DEFINE_DEFAULT_CONSTRUCTOR_CALL(ASDTProjectile)


#define TP2_Source_SoftDesignTraining_SDTProjectile_h_15_PRIVATE_PROPERTY_OFFSET \
	FORCEINLINE static uint32 __PPO__m_TimeToLive() { return STRUCT_OFFSET(ASDTProjectile, m_TimeToLive); }


#define TP2_Source_SoftDesignTraining_SDTProjectile_h_12_PROLOG
#define TP2_Source_SoftDesignTraining_SDTProjectile_h_15_GENERATED_BODY_LEGACY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	TP2_Source_SoftDesignTraining_SDTProjectile_h_15_PRIVATE_PROPERTY_OFFSET \
	TP2_Source_SoftDesignTraining_SDTProjectile_h_15_SPARSE_DATA \
	TP2_Source_SoftDesignTraining_SDTProjectile_h_15_RPC_WRAPPERS \
	TP2_Source_SoftDesignTraining_SDTProjectile_h_15_INCLASS \
	TP2_Source_SoftDesignTraining_SDTProjectile_h_15_STANDARD_CONSTRUCTORS \
public: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


#define TP2_Source_SoftDesignTraining_SDTProjectile_h_15_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	TP2_Source_SoftDesignTraining_SDTProjectile_h_15_PRIVATE_PROPERTY_OFFSET \
	TP2_Source_SoftDesignTraining_SDTProjectile_h_15_SPARSE_DATA \
	TP2_Source_SoftDesignTraining_SDTProjectile_h_15_RPC_WRAPPERS_NO_PURE_DECLS \
	TP2_Source_SoftDesignTraining_SDTProjectile_h_15_INCLASS_NO_PURE_DECLS \
	TP2_Source_SoftDesignTraining_SDTProjectile_h_15_ENHANCED_CONSTRUCTORS \
private: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


template<> SOFTDESIGNTRAINING_API UClass* StaticClass<class ASDTProjectile>();

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID TP2_Source_SoftDesignTraining_SDTProjectile_h


PRAGMA_ENABLE_DEPRECATION_WARNINGS
