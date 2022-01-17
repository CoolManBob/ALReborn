#pragma once

#include <AgcEngine.h>

LuaGlue	LG_MessageBox( lua_State *L );
LuaGlue	LG_AddCharacter( lua_State *L );
LuaGlue	LG_UpdateCustomizeIndex( lua_State *L );
LuaGlue	LG_StartAnimation( lua_State *L );
LuaGlue	LG_Struck( lua_State *L );
LuaGlue	LG_SocialAnimation( lua_State *L );
LuaGlue	LG_SetAttackStatus( lua_State *L );
LuaGlue	LG_SetStruckAnimation( lua_State *L );
LuaGlue	LG_SetDeadAnimation( lua_State *L );
LuaGlue	LG_RemoveCharacter( lua_State *L );
LuaGlue	LG_GetSelfCharacter( lua_State *L );
LuaGlue	LG_GetCharacter( lua_State *L );
LuaGlue	LG_MoveCharacter( lua_State *L );
LuaGlue	LG_UpdatePosition( lua_State *L );
LuaGlue	LG_TurnCharacter( lua_State *L );
LuaGlue	LG_GetSelfCharacterPos( lua_State *L );
LuaGlue	LG_GetCharacterPos( lua_State *L );
LuaGlue	LG_ShowCharacterPos( lua_State *L );
LuaGlue	LG_StartTheaterMode( lua_State *L );
LuaGlue	LG_EndTheaterMode( lua_State *L );
LuaGlue	LG_GotoSiegeZone( lua_State *L );
LuaGlue	LG_StartCharPolyMorphByTID( lua_State *L );
LuaGlue	LG_EndCharPolyMorph( lua_State *L );
LuaGlue	LG_AutoHunt( lua_State *L );
LuaGlue	LG_GetCharacterByTID( lua_State *L );
LuaGlue	LG_SetTargetStruckEffect( lua_State *L );

LuaGlue	LG_UseGG	( lua_State *L );
LuaGlue	LG_SaveGG	( lua_State *L );
LuaGlue	LG_LoadGG	( lua_State *L );
LuaGlue	LG_AddGG	( lua_State *L );
LuaGlue	LG_DeleteGG	( lua_State *L );

static luaDef MVLuaGlue[] = 
{
	{"MessageBox"			,	LG_MessageBox			},
	{"AddCharacter"			,	LG_AddCharacter			},
	{"StartAnimation"		,	LG_StartAnimation		},
	{"SocialAnimation"		,	LG_SocialAnimation		},
	{"RemoveCharacter"		,	LG_RemoveCharacter		},
	{"MoveCharacter"		,	LG_MoveCharacter		},
	{"UpdatePosition"		,	LG_UpdatePosition		},
	{"TurnCharacter"		,	LG_TurnCharacter		},
	{"GetSelfCharacterPos"	,	LG_GetSelfCharacterPos	},
	{"EndTheaterMode"		,	LG_EndTheaterMode		},
	{"StartTheaterMode"		,	LG_StartTheaterMode		},
	{"g1"					,	LG_GotoSiegeZone		},
	{"SetAttackStatus"		,	LG_SetAttackStatus		},
	{"SetStruckAnimation"	,	LG_SetStruckAnimation	},
	{"SetDeadAnimation"		,	LG_SetDeadAnimation		},
	{"UpdateCustomizeIndex"	,	LG_UpdateCustomizeIndex	},
	{"GetSelfCharacter"		,	LG_GetSelfCharacter		},
	{"GetCharacterPos"		,	LG_GetCharacterPos		},
	{"AutoHunt"				,	LG_AutoHunt				},
	{"GetCharacter"			,	LG_GetCharacter			},
	{"GetCharacterByTID"	,	LG_GetCharacterByTID	},
	{"StartCharPolyMorphByTID",	LG_StartCharPolyMorphByTID},
	{"EndCharPolyMorph"		,	LG_EndCharPolyMorph		},
	{"ShowCharacterPos"		,	LG_ShowCharacterPos		},
	{"SetStruckEffect"		,	LG_SetTargetStruckEffect},
	{"Struck"				,	LG_Struck				},
	{"GG"					,	LG_UseGG				},
	{"AddGG"				,	LG_AddGG				},
	{"DeleteGG"				,	LG_DeleteGG				},
	{"SaveGG"				,	LG_SaveGG				},
	{"LoadGG"				,	LG_LoadGG				},
	{NULL					,	NULL					},
};