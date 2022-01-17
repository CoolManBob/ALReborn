#ifndef __CALLBACK_CHARACTER_PROPERTY_H__
#define __CALLBACK_CHARACTER_PROPERTY_H__



#include "ApDefine.h"


// 문자열 조합용 유틸함수
BOOL MakeFormattedString( char* pBuffer, char* pFormat, int nValueOriginal, int nValueCurrent, DWORD dwBaseColor, DWORD dwUpperColor, DWORD dwLowerColor );
BOOL MakeFormattedStringUINT( char* pBuffer, char* pFormat, int nValueCurrent, DWORD dwBaseColor, DWORD dwUpperColor );


// 공격력 탭
BOOL fn_CharacterProperty_PhysicalDamage( void* pData, void* pCharacter );
BOOL fn_CharacterProperty_HeroicDamage( void* pData, void* pCharacter );
BOOL fn_CharacterProperty_RateAttack( void* pData, void* pCharacter );
BOOL fn_CharacterProperty_RateHit( void* pData, void* pCharacter );
BOOL fn_CharacterProperty_DurationCastSkill( void* pData, void* pCharacter );
BOOL fn_CharacterProperty_DurationSkillCoolTime( void* pData, void* pCharacter );
BOOL fn_CharacterProperty_SpeedMove( void* pData, void* pCharacter );
BOOL fn_CharacterProperty_SpeedAttack( void* pData, void* pCharacter );
BOOL fn_CharacterProperty_RateCritical( void* pData, void* pCharacter );
BOOL fn_CharacterProperty_CriticalDamage( void* pData, void* pCharacter );
BOOL fn_CharacterProperty_IgnoreTargetRegistance( void* pData, void* pCharacter );
BOOL fn_CharacterProperty_IgnoreTargetDefense( void* pData, void* pCharacter );
BOOL fn_CharacterProperty_AttackRange( void* pData, void* pCharacter );

// 방어력 탭
BOOL fn_CharacterProperty_PhysicalDefense( void* pData, void* pCharacter );
BOOL fn_CharacterProperty_HeroicDefense( void* pData, void* pCharacter );
BOOL fn_CharacterProperty_DefenseRate( void* pData, void* pCharacter );
BOOL fn_CharacterProperty_PhysicalRegistance( void* pData, void* pCharacter );
BOOL fn_CharacterProperty_BlockRate( void* pData, void* pCharacter );
BOOL fn_CharacterProperty_SkillBlockRate( void* pData, void* pCharacter );
BOOL fn_CharacterProperty_EvadeRate( void* pData, void* pCharacter );
BOOL fn_CharacterProperty_DodgeRate( void* pData, void* pCharacter );
BOOL fn_CharacterProperty_CriticalRegistance( void* pData, void* pCharacter );
BOOL fn_CharacterProperty_StunRegistance( void* pData, void* pCharacter );
BOOL fn_CharacterProperty_ReflectDamageRate( void* pData, void* pCharacter );
BOOL fn_CharacterProperty_RefectDamage( void* pData, void* pCharacter );
BOOL fn_CharacterProperty_HeroicMeleeRegistance( void* pData, void* pCharacter );
BOOL fn_CharacterProperty_HeroicMissileRegistance( void* pData, void* pCharacter );
BOOL fn_CharacterProperty_HeroicMagicRegistance( void* pData, void* pCharacter );

// 속성 탭
BOOL fn_CharacterProperty_MagicDamage( void* pData, void* pCharacter );
BOOL fn_CharacterProperty_MagicRegistance( void* pData, void* pCharacter );
BOOL fn_CharacterProperty_FireDamage( void* pData, void* pCharacter );
BOOL fn_CharacterProperty_FireRegistance( void* pData, void* pCharacter );
BOOL fn_CharacterProperty_AirDamage( void* pData, void* pCharacter );
BOOL fn_CharacterProperty_AirRegistance( void* pData, void* pCharacter );
BOOL fn_CharacterProperty_WaterDamage( void* pData, void* pCharacter );
BOOL fn_CharacterProperty_WaterRegistance( void* pData, void* pCharacter );
BOOL fn_CharacterProperty_EarthDamage( void* pData, void* pCharacter );
BOOL fn_CharacterProperty_EarthRegistance( void* pData, void* pCharacter );
BOOL fn_CharacterProperty_IceDamage( void* pData, void* pCharacter );
BOOL fn_CharacterProperty_IceRegistance( void* pData, void* pCharacter );
BOOL fn_CharacterProperty_ThunderDamage( void* pData, void* pCharacter );
BOOL fn_CharacterProperty_ThunderRegistance( void* pData, void* pCharacter );
BOOL fn_CharacterProperty_PoisonDamage( void* pData, void* pCharacter );
BOOL fn_CharacterProperty_PoisonRegistance( void* pData, void* pCharacter );

// PK정보 탭
BOOL fn_CharacterProperty_RemainTimeAttacker( void* pData, void* pCharacter );
BOOL fn_CharacterProperty_MurdererPoint( void* pData, void* pCharacter );
BOOL fn_CharacterProperty_RemainTimeRemission( void* pData, void* pCharacter );
BOOL fn_CharacterProperty_MurdererLevel( void* pData, void* pCharacter );




#endif