#ifndef __AUSTRINGTABLE_H__
#define __AUSTRINGTABLE_H__

// 2006.02.16. steeple
// ignore deprecated warning message in VS2005
#if _MSC_VER >= 1400
#pragma warning (disable : 4996)
#endif

// -========================================================================-
// string index
// -========================================================================-

// STI is "String Table Index"

#define STI_OPTION										0
#define STI_MATERIAL									1
#define STI_BASIC										2
#define STI_PRICE										3
#define STI_MASTERY										4
#define STI_EXP											5
#define STI_MINUTE										6
#define STI_HOUR										7
#define STI_SECOND										8
#define STI_METER										9
#define STI_GELD										10
#define STI_NUMBER_UNIT									11
#define STI_GRADE										12
#define STI_SKILLBOOK									13
#define STI_SKILL										14
#define STI_COMBAT										15
#define STI_REINFORCE									16
#define STI_SPECIFIC									17
#define STI_PASSIVE										18
#define STI_ACTIVE_SKILL								19
#define STI_PASSIVE_SKILL								20
#define STI_NEED_WEAPON									21
#define STI_NEED_SHIELD									22
#define STI_NEED_STUFF									23
#define STI_CURRENT_GRADE								24
#define STI_NEXT_GRADE									25
#define STI_USE_ENABLE									26
#define STI_HUNT										27
#define STI_COOK										28
#define STI_ALCHEMY										29
#define STI_TITLE										30
#define STI_COOK_DESC									31
#define STI_SELECT_COOK									32
#define STI_ALCHEMY_DESC								33
#define STI_SELECT_ALCHEMY								34
#define STI_MAKE_IT										35
#define STI_MAKE										36
#define STI_CLOSE										37
#define STI_NEED_MATERIAL								38
#define STI_UP_RATE										39
#define STI_AUTO_USE									40
#define STI_SKILL_SUCCESS								41
#define STI_GET_ITEM									42
#define STI_GET_ITEM2									43
#define STI_SKILLMASTERY								44
#define STI_SKILL_FAIL									45
#define STI_NO_ITEM										46
#define STI_WRONG_TARGET								47
#define STI_TOO_FAR										48
#define STI_NO_SPACE									49
#define STI_NO_MATERIAL									50
#define STI_NO_GET										51

#define STI_BASIC_ATTR_LEVEL							52
#define STI_BASIC_ATTR_TYPE								53
#define STI_BASIC_ATTR_DETAIL_TYPE						54

#define STI_BASIC_ATTR_REQUIRE_CLASS_KNIGHT				55
#define STI_BASIC_ATTR_REQUIRE_CLASS_ARCHER				56
#define STI_BASIC_ATTR_REQUIRE_CLASS_WIZARD				57
#define STI_BASIC_ATTR_REQUIRE_CLASS_BERSERKER			58
#define STI_BASIC_ATTR_REQUIRE_CLASS_HUNTER				59
#define STI_BASIC_ATTR_REQUIRE_CLASS_SORCERER			60
#define STI_BASIC_ATTR_REQUIRE_CLASS_ASSASSIN			61
#define STI_BASIC_ATTR_REQUIRE_CLASS_RANGER				62
#define STI_BASIC_ATTR_REQUIRE_CLASS_ELEMENTALLIST		63
#define STI_BASIC_ATTR_REQUIRE_CLASS_SLAYER				64
#define STI_BASIC_ATTR_REQUIRE_CLASS_SCOUTER			65
#define STI_BASIC_ATTR_REQUIRE_CLASS_SUMMONER			66

#define STI_BASIC_ATTR_REQUIRE_LEVEL					67
#define STI_BASIC_ATTR_REQUIRE_POINT					68

#define STI_CAST_TIME									69
#define STI_RECAST_TIME									70
#define STI_DURATION									71
#define STI_RANGE_DISTANCE								72
#define STI_RANGE_RADIUS								73
#define STI_RANGE_BOX_X									74
#define STI_RANGE_BOX_Y									75
#define STI_INTERVAL									76
														
#define STI_COST_HP										77
#define STI_COST_MP										78
#define STI_COST_ARROW									79
#define STI_ENDSKILL_COST_HP							80
#define STI_DAMAGE_PHYSICAL								81
#define STI_DAMAGE_MAGIC								82
#define STI_DAMAGE_WATER								83
#define STI_DAMAGE_AIR									84
#define STI_DAMAGE_EARTH								85
#define STI_DAMAGE_FIRE									86
#define STI_DAMAGE_POISON								87
#define STI_DAMAGE_ICE									88
#define STI_DAMAGE_LIGHTENING							89

#define STI_DAMAGE_PERCENT_PHYSICAL						90
#define STI_DAMAGE_PERCENT_MAGIC						91
#define STI_DAMAGE_PERCENT_WATER						92
#define STI_DAMAGE_PERCENT_AIR							93
#define STI_DAMAGE_PERCENT_EARTH						94
#define STI_DAMAGE_PERCENT_FIRE							95
#define STI_DAMAGE_PERCENT_POISON						96
#define STI_DAMAGE_PERCENT_ICE							97
#define STI_DAMAGE_PERCENT_LIGHTENING					98

#define STI_DOT_DAMAGE_PHYSICAL							99
#define STI_DOT_DAMAGE_MAGIC							100
#define STI_DOT_DAMAGE_WATER							101
#define STI_DOT_DAMAGE_AIR								102
#define STI_DOT_DAMAGE_EARTH							103
#define STI_DOT_DAMAGE_FIRE								104
#define STI_DOT_DAMAGE_POISON							105
#define STI_DOT_DAMAGE_ICE								106
#define STI_DOT_DAMAGE_LIGHTENING						107

#define STI_DOT_DAMAGE_PERCENT_PHYSICAL					108
#define STI_DOT_DAMAGE_PERCENT_MAGIC					109
#define STI_DOT_DAMAGE_PERCENT_WATER					110
#define STI_DOT_DAMAGE_PERCENT_AIR						111
#define STI_DOT_DAMAGE_PERCENT_EARTH					112
#define STI_DOT_DAMAGE_PERCENT_FIRE						113
#define STI_DOT_DAMAGE_PERCENT_POISON					114
#define STI_DOT_DAMAGE_PERCENT_ICE						115
#define STI_DOT_DAMAGE_PERCENT_LIGHTENING				116

#define STI_MOVE_DISTANCE								117
#define STI_REFLECT_DAMAGE								118
#define STI_BLOCK_RATE									119
#define STI_STUN_TIME									120
#define STI_CRITICAL									121
#define STI_ADJUST_SKILL_RANGE							122
#define STI_SKILL_RATE									123
#define STI_DAMAGE_CONVERT_HP							124
#define STI_DAMAGE_CONVERT_MP							125
#define STI_MP_CONVERT_HP								126
#define STI_REGEN_HP									127
#define STI_REGEN_MP									128
#define STI_CHARGE_DAMAGE_1								129
#define STI_CHARGE_DAMAGE_2								130
#define STI_CHARGE_DAMAGE_3								131
#define STI_AGRODOLL_HP									132
#define STI_DOT_INTERVAL								133
#define STI_DECREASE_COST_MP							134
#define STI_DECREASE_CAST_TIME							135
#define STI_ADJUST_SKILL_LEVEL							136
#define STI_LEARN_COST									137
#define STI_DEATH										138
#define STI_SUMMONS										139

#define STI_ADDITIONAL_DURATION							140
#define STI_MAX_TARGET_NUM								141
#define STI_LIMIT_QUANTITY								142
#define STI_EA_DMG_POINT_PHYSICAL						143
#define STI_EA_DMG_POINT_MAGIC							144
#define STI_EA_DMG_POINT_WATER							145
#define STI_EA_DMG_POINT_AIR							146
#define STI_EA_DMG_POINT_EARTH							147
#define STI_EA_DMG_POINT_FIRE							148
#define STI_EA_DMG_POINT_POISON							149
#define STI_EA_DMG_POINT_ICE							150
#define STI_EA_DMG_POINT_LIGHTENING						151
#define STI_EA_DMG_PERCENT_PHYSICAL						152
#define STI_EA_DMG_PERCENT_MAGIC						153
#define STI_EA_DMG_PERCENT_WATER						154
#define STI_EA_DMG_PERCENT_AIR							155
#define STI_EA_DMG_PERCENT_EARTH						156
#define STI_EA_DMG_PERCENT_FIRE							157
#define STI_EA_DMG_PERCENT_POISON						158
#define STI_EA_DMG_PERCENT_ICE							159
#define STI_EA_DMG_PERCENT_LIGHTENING					160
#define STI_FACTOR_POINT_CON							161
#define STI_FACTOR_POINT_STR							162
#define STI_FACTOR_POINT_INT							163
#define STI_FACTOR_POINT_DEX							164
#define STI_FACTOR_POINT_CHA							165
#define STI_FACTOR_POINT_WIS							166
#define STI_FACTOR_POINT_MOVEMENT						167
#define STI_FACTOR_POINT_HP								168
#define STI_FACTOR_POINT_MP								169
#define STI_FACTOR_POINT_AGRO							170
#define STI_FACTOR_POINT_MAX_HP							171
#define STI_FACTOR_POINT_MAX_MP							172
#define STI_FACTOR_POINT_ATK_SPEED						173
#define STI_FACTOR_POINT_ATK_RANGE						174
#define STI_FACTOR_POINT_SKILL_CAST						175
#define STI_FACTOR_POINT_SKILL_DELAY					176
#define STI_FACTOR_POINT_HIT_RATE						177
#define STI_FACTOR_POINT_EVADE_RATE						178
#define STI_FACTOR_POINT_DODGE_RATE						179
#define STI_FACTOR_POINT_DMG_MIN_PHYSICAL				180
#define STI_FACTOR_POINT_DMG_MIN_MAGIC					181
#define STI_FACTOR_POINT_DMG_MIN_WATER					182
#define STI_FACTOR_POINT_DMG_MIN_AIR					183
#define STI_FACTOR_POINT_DMG_MIN_EARTH					184
#define STI_FACTOR_POINT_DMG_MIN_FIRE					185
#define STI_FACTOR_POINT_DMG_MIN_POISON					186
#define STI_FACTOR_POINT_DMG_MIN_ICE					187
#define STI_FACTOR_POINT_DMG_MIN_LIGHTENING				188
#define STI_FACTOR_POINT_DMG_MAX_PHYSICAL				189
#define STI_FACTOR_POINT_DMG_MAX_MAGIC					190
#define STI_FACTOR_POINT_DMG_MAX_WATER					191
#define STI_FACTOR_POINT_DMG_MAX_AIR					192
#define STI_FACTOR_POINT_DMG_MAX_EARTH					193
#define STI_FACTOR_POINT_DMG_MAX_FIRE					194
#define STI_FACTOR_POINT_DMG_MAX_POISON					195
#define STI_FACTOR_POINT_DMG_MAX_ICE					196
#define STI_FACTOR_POINT_DMG_MAX_LIGHTENING				197
#define STI_FACTOR_POINT_DEF_POINT_PHYSICAL				198
#define STI_FACTOR_POINT_DEF_POINT_MAGIC				199
#define STI_FACTOR_POINT_DEF_POINT_WATER				200
#define STI_FACTOR_POINT_DEF_POINT_AIR					201
#define STI_FACTOR_POINT_DEF_POINT_EARTH				202
#define STI_FACTOR_POINT_DEF_POINT_FIRE					203
#define STI_FACTOR_POINT_DEF_POINT_POISON				204
#define STI_FACTOR_POINT_DEF_POINT_ICE					205
#define STI_FACTOR_POINT_DEF_POINT_LIGHTENING			206
#define STI_FACTOR_POINT_DEF_RATE_PHYSICAL				207
#define STI_FACTOR_POINT_DEF_RATE_SKILL_BLOCK			208

#define STI_FACTOR_PERCENT_CON							209
#define STI_FACTOR_PERCENT_STR							210
#define STI_FACTOR_PERCENT_INT							211
#define STI_FACTOR_PERCENT_DEX							212
#define STI_FACTOR_PERCENT_CHA							213
#define STI_FACTOR_PERCENT_WIS							214
#define STI_FACTOR_PERCENT_MOVEMENT						215
#define STI_FACTOR_PERCENT_HP							216
#define STI_FACTOR_PERCENT_MP							217
#define STI_FACTOR_PERCENT_AGRO							218
#define STI_FACTOR_PERCENT_MAX_HP						219
#define STI_FACTOR_PERCENT_MAX_MP						220
#define STI_FACTOR_PERCENT_ATK_SPEED					221
#define STI_FACTOR_PERCENT_ATK_RANGE					222
#define STI_FACTOR_PERCENT_SKILL_CAST					223
#define STI_FACTOR_PERCENT_SKILL_DELAY					224
#define STI_FACTOR_PERCENT_HIT_RATE						225
#define STI_FACTOR_PERCENT_EVADE_RATE					226
#define STI_FACTOR_PERCENT_DODGE_RATE					227
#define STI_FACTOR_PERCENT_DMG_MIN_PHYSICAL				228
#define STI_FACTOR_PERCENT_DMG_MIN_MAGIC				229
#define STI_FACTOR_PERCENT_DMG_MIN_WATER				230
#define STI_FACTOR_PERCENT_DMG_MIN_AIR					231
#define STI_FACTOR_PERCENT_DMG_MIN_EARTH				232
#define STI_FACTOR_PERCENT_DMG_MIN_FIRE					233
#define STI_FACTOR_PERCENT_DMG_MIN_POISON				234
#define STI_FACTOR_PERCENT_DMG_MIN_ICE					235
#define STI_FACTOR_PERCENT_DMG_MIN_LIGHTENING			236
#define STI_FACTOR_PERCENT_DMG_MAX_PHYSICAL				237
#define STI_FACTOR_PERCENT_DMG_MAX_MAGIC				238
#define STI_FACTOR_PERCENT_DMG_MAX_WATER				239
#define STI_FACTOR_PERCENT_DMG_MAX_AIR					240
#define STI_FACTOR_PERCENT_DMG_MAX_EARTH				241
#define STI_FACTOR_PERCENT_DMG_MAX_FIRE					242
#define STI_FACTOR_PERCENT_DMG_MAX_POISON				243
#define STI_FACTOR_PERCENT_DMG_MAX_ICE					244
#define STI_FACTOR_PERCENT_DMG_MAX_LIGHTENING			245
#define STI_FACTOR_PERCENT_DEF_POINT_PHYSICAL			246
#define STI_FACTOR_PERCENT_DEF_POINT_MAGIC				247
#define STI_FACTOR_PERCENT_DEF_POINT_WATER				248
#define STI_FACTOR_PERCENT_DEF_POINT_AIR				249
#define STI_FACTOR_PERCENT_DEF_POINT_EARTH				250
#define STI_FACTOR_PERCENT_DEF_POINT_FIRE				251
#define STI_FACTOR_PERCENT_DEF_POINT_POISON				252
#define STI_FACTOR_PERCENT_DEF_POINT_ICE				253
#define STI_FACTOR_PERCENT_DEF_POINT_LIGHTENING			254
#define STI_FACTOR_PERCENT_DEF_RATE_PHYSICAL			255
#define STI_FACTOR_PERCENT_DEF_RATE_SKILL_BLOCK			256
#define STI_UPGRADE										257
#define STI_UPGRADE_REQUIRE_LEVEL						258
#define STI_UPGRADE_REQUIRE_POINT						259
#define STI_UPGRADE_COST								260
#define STI_NAME										261
#define STI_LEVEL										262
#define STI_CLASS										263
#define STI_LEADER_NAME									264
#define STI_NEED_NUMBER									265
#define STI_NEED_LEVEL									266
#define STI_GUARD										267
#define STI_CANT_BUY_MINE								268
#define STI_HOW_MANY									269
#define STI_CANCEL_MANY									270
#define STI_CANCEL_ONE									271
#define STI_SELL_MANY									272
#define STI_SELL_ONE									273
#define STI_COUNT_RANGE									274
#define STI_NO_INVENTORY								275
#define STI_ALREADY_SELL								276
#define STI_BUY_MANY									277
#define STI_BUY_ONE										278
#define STI_CANT_REGISTER								279
#define STI_CANT_CANCEL									280
#define STI_WRONG_WORD									281
#define STI_WRONG_CATEGORY								282
#define STI_PUBLIC_CHANNEL								283
#define STI_PRIVATE_CHANNEL								284
#define STI_GUILD_CHANNEL								285
#define STI_CHANNEL_INVITE								286
#define STI_TARGET_ATTR									287
#define STI_VILLAIN										288
#define STI_AMUNESIA									289
#define STI_THOULAN										290
#define STI_DELPARASS									291
#define STI_ELAN										292
#define STI_JUNGLE										293
#define STI_ELORR										294
#define STI_MEMBREATHEN									295
#define STI_SHINEWOOD									296
#define STI_HARIEL										297
#define STI_LIMELITE									298
#define STI_THRILGARD									299
#define STI_KUN											300
#define STI_WHISTLE										301
#define STI_CIESELVA									302
#define STI_XAILOK										303
#define STI_EDGEOROCK									304
#define STI_SANTUS										305
#define STI_HELINK										306
#define STI_GRIMROAR									307
#define STI_SERENDO										308
#define STI_BATTLE										309
#define STI_WINDRILL									310
#define STI_GORGOB										311
#define STI_KUSKUT										312
#define STI_ELKA										313
#define STI_SHAMLUK										314
#define STI_NUK											315
#define STI_RAZMARQ										316

#define STI_REPAIR_COMPLETE 							317
#define STI_REPAIR_FAIL									318
#define STI_REPAIR_CANT									319
#define STI_REPAIR_WITH_MOUNT							320
#define STI_REPAIR_LIST_FULL							321
#define STI_REPAIR_NO_ITEM								322
#define STI_CANT_USE									323
#define STI_BUY_SELL									324
#define STI_REPAIR										325
#define STI_WAREHOUSE									326
#define STI_ENHANCE										327
#define STI_GUILD										328
#define STI_PRODUCE										329
#define STI_TELEPORT									330
#define STI_QUEST										331
#define STI_AUCTION										332
#define STI_REFINE										333
#define STI_CUSTOMIZE									334
#define STI_FORGIVE										335
#define STI_WANTED										336
#define STI_BATTLE_WITH									337
#define STI_BATTLE_START								338
#define STI_BATTLE_BEGIN								339
#define STI_PRE											340
#define STI_OUR_TEAM									341
#define STI_ENEMY										342
#define STI_ENEMY_DECLARE_DEFEAT						343
#define STI_DECLARE_DEFEAT								344
#define STI_GUILD_INFO									345
#define STI_CANT_USE_MORPH								346
#define STI_CANT_USE_MOVEP								347
#define STI_CANT_USE_ATTACKP							348
#define STI_GELD_POST									349
#define STI_UNIT_POST									350
#define STI_ITEM_POST									351
#define STI_NOLIMIT										352
#define STI_REMAIN_TIME									353
#define STI_LIFE										354
#define STI_RECOVER										355
#define STI_MANA										356
#define STI_HOW_MANY_SELL								357
#define STI_INPUT_SELL_PRICE							358
#define STI_WRONG_ITEM									359
#define STI_SELL_COUNT_RANGE							360
#define STI_SELL_PRICE_ONE								361
#define STI_SELL_PRICE_RANGE							362
#define STI_SELLING										363
#define STI_SELL_COMPLETE								364
#define STI_OK											365
#define STI_CANCEL										366
#define STI_SELL_PRICE									367
#define STI_TYPE										368
#define STI_ITEM_RANK									369
#define STI_DAMAGE										370
#define STI_ATTACK_SPEED								371
#define STI_DEFENSE										372
#define STI_BLOCK										373
#define STI_DURABILITY									374
#define STI_PHYSICAL_AR									375
#define STI_MAGICAL_AR									376
#define STI_PHYSICAL_DR									377
#define STI_RESTRICT_LEVEL								378
#define STI_RESTRICT_RACE								379
#define STI_RESTRICT_CLASS								380
#define STI_USE_LEVEL									381
#define STI_USE_RACE									382
#define STI_SS_DAMAGE_FIRE								383
#define STI_SS_DAMAGE_WATER								384
#define STI_SS_DAMAGE_AIR								385
#define STI_SS_DAMAGE_EARTH								386
#define STI_SS_DAMAGE_MAGIC								387
#define STI_SS_DAMAGE_POISON							388
#define STI_SS_DAMAGE_LIGHTENING						389
#define STI_SS_DAMAGE_ICE								390
#define STI_SS_DEFENSE_FIRE								391
#define STI_SS_DEFENSE_WATER							392
#define STI_SS_DEFENSE_AIR								393
#define STI_SS_DEFENSE_EARTH							394
#define STI_SS_DEFENSE_MAGIC							395
#define STI_SS_DEFENSE_STR								396
#define STI_SS_DEFENSE_CON								397
#define STI_SS_DEFENSE_INT								398
#define STI_SS_DEFENSE_WIS								399
#define STI_SS_DEFENSE_DEX								400
#define STI_DEFENSE_FIRE								401
#define STI_DEFENSE_WATER								402
#define STI_DEFENSE_AIR									403
#define STI_DEFENSE_EARTH								404
#define STI_DEFENSE_MAGIC								405
#define STI_STR											406
#define STI_CON											407
#define STI_INT											408
#define STI_WIS											409
#define STI_DEX											410
#define STI_USE_RUNE_LEVEL								411
#define STI_MAX_HP										412
#define STI_MAX_MP										413
#define STI_ADD_DAMAGE									414
#define STI_ADD_DEFENSE									415
#define STI_REPAIR_COST									416
#define STI_MOVE_SPEED									417
#define STI_RUNE_USE_WEAPON								418
#define STI_RUNE_USE_ARMOUR								419
#define STI_RUNE_USE_SHIELD								420
#define STI_RUNE_USE_RING								421
#define STI_RUNE_USE_AMULET								422
#define STI_SAFE_AREA									423
#define STI_FREE_BATTLEFIELD							424
#define STI_DANGEROUS_BATTLEFIELD						425
#define STI_ALREADY_LEARN								426
#define STI_SKILL_LEARN									427
#define STI_SKILL_LEARN2								428
#define STI_CANT_LEARN_NO_GELD							429
#define STI_CANT_LEARN_NEED_SKILL						430
#define STI_COMPLETE									431
#define STI_ACCEPT										432
#define STI_POSSIBLE									433
#define STI_ING											434
#define STI_QUEST_CANCEL								435
#define STI_TAKE_QUEST									436
#define STI_QUEST_COMPLETE								437
#define STI_QUEST_COMPLETE2								438
#define STI_NO_MORE_QUEST								439
#define STI_NO_MORE_QUEST2								440
#define STI_CONTENTS									441
#define STI_COMPLETE_CONDITION							442
#define STI_REWARD										443
#define STI_NO_MORE_ITEM								444
#define STI_GET_GELD									445
#define STI_REMOVE_QUEST								446
#define STI_NO_SPACE_CANT_COMPLETE						447
#define STI_QUEST_LIST									448
#define STI_REFINERY_INTRO								449
#define STI_REFINERYING									450
#define STI_ITEM_REGISTERED								451
#define STI_REGISTER_CANCEL								452
#define STI_SELL_CONFIRM								453
#define STI_BUY_COMPLETE								454
#define STI_SELL_COMPLETED								455
#define STI_NO_SPLIT									456
#define STI_CONNECTING_SERVER							457
#define STI_SERVER										458
#define STI_SA_GREETING									459
#define STI_SA_CELEBRATION								460
#define STI_SA_THANKS									461
#define STI_SA_GREATE									462
#define STI_SA_INSULT									463
#define STI_SA_DANCE									464
#define STI_SA_ZZZ										465
#define STI_SA_RISING									466
#define STI_SA_LAUGH									467
#define STI_SA_GLOOM									468
#define STI_SA_ANGER									469
#define STI_SA_SULKY									470
#define STI_SA_SORRY									471
#define STI_SA_DRINK									472
#define STI_SA_CHEERS									473
#define STI_SA_CHARGE									474
#define STI_SA_SITDOWN									475
#define STI_SA_GREETING_E								476
#define STI_SA_CELEBRATION_E							477
#define STI_SA_THANKS_E									478
#define STI_SA_GREATE_E									479
#define STI_SA_INSULT_E									480
#define STI_SA_DANCE_E									481
#define STI_SA_ZZZ_E									482
#define STI_SA_RISING_E									483
#define STI_SA_LAUGH_E									484
#define STI_SA_GLOOM_E									485
#define STI_SA_ANGER_E									486
#define STI_SA_SULKY_E									487
#define STI_SA_SORRY_E									488
#define STI_SA_DRINK_E									489
#define STI_SA_CHEERS_E									490
#define STI_SA_CHARGE_E									491
#define STI_SA_SITDOWN_E								492
#define STI_GM_GREETING									493
#define STI_GM_GREETING_E								494
#define STI_GM_CELEBRATION								495
#define STI_GM_CELEBRATION_E							496
#define STI_GM_DANCE									497
#define STI_GM_DANCE_E									498
#define STI_GM_GLOOM									499
#define STI_GM_GLOOM_E									500
#define STI_GM_BOW										501
#define STI_GM_BOW_E									502
#define STI_GM_HI										503
#define STI_GM_HI_E										504
#define STI_GM_WAIT										505
#define STI_GM_WAIT_E									506
#define STI_GM_NICEDAY									507
#define STI_GM_NICEDAY_E								508
#define STI_KNIGHT										509
#define STI_ARCHER										510
#define STI_MAGE										511
#define STI_BERSERKER									512
#define STI_HUNTER										513
#define STI_SORCERER									514
#define STI_ELEMETALIST									515
#define STI_RANGER										516
#define STI_ITEM_GET									517
#define STI_MASTERY_UP									518
#define STI_WANT_TRADE									519
#define STI_WAIT_TRADE									520
#define STI_CUR_LOC										521
#define STI_USE_SKILL									522
#define STI_BACKGROUND									523
#define STI_DECO										524
#define STI_SYMBOL										525
#define STI_TOTALCOST									526
#define STI_CANT_WITH_HORSE								527
#define STI_INPUT_EKEY									528
#define	STI_SIEGEWAR_NPC								529

#define	STI_CHAT_JOIN_CHANNEL_LOWER						530
#define	STI_CHAT_JOIN_CHANNEL_UPPER						531
#define	STI_CHAT_JOIN_CHANNEL_CUSTOM					532
#define	STI_CHAT_JOIN_CHANNEL_LOCALIZE					533
														
#define	STI_CHAT_LEAVE_CHANNEL_LOWER					534
#define	STI_CHAT_LEAVE_CHANNEL_UPPER					535
#define	STI_CHAT_LEAVE_CHANNEL_CUSTOM					536
#define	STI_CHAT_LEAVE_CHANNEL_LOCALIZE					537
														
#define	STI_CHAT_WORD_BALLOON_LOWER						538
#define	STI_CHAT_WORD_BALLOON_UPPER						539
#define	STI_CHAT_WORD_BALLOON_LOCALIZE					540
														
#define	STI_CHAT_NORMAL_AREA_CHAT_SPECIAL				541
														
#define	STI_CHAT_WHISPER_LOWER							542
#define	STI_CHAT_WHISPER_UPPER							543
#define	STI_CHAT_WHISPER_LOCALIZE						544
#define	STI_CHAT_WHISPER_SPECIAL						545
														
#define	STI_CHAT_REPLY_LOWER							546
#define	STI_CHAT_REPLY_UPPER							547
														
#define	STI_CHAT_SEND_GUILD_LOWER						548
#define	STI_CHAT_SEND_GUILD_UPPER						549
#define	STI_CHAT_SEND_GUILD_LOCALIZE					550
#define	STI_CHAT_SEND_GUILD_SPECIAL						551
														
#define	STI_CHAT_SEND_PARTY_LOWER						552
#define	STI_CHAT_SEND_PARTY_UPPER						553
#define	STI_CHAT_SEND_PARTY_LOCALIZE					554
#define	STI_CHAT_SEND_PARTY_SPECIAL						555
														
#define	STI_CHAT_INVITE_GUILD_MEMBER_ENG				556
#define	STI_CHAT_INVITE_GUILD_MEMBER_LOCALIZE			557
#define	STI_CHAT_INVITE_PARTY_MEMBER_ENG				558
#define	STI_CHAT_INVITE_PARTY_MEMBER_LOCALIZE			559
														
#define	STI_CHAT_GUILD_LEAVE_ENG						560
#define	STI_CHAT_GUILD_LEAVE_LOCALIZE					561
														
#define	STI_CHAT_QUEST_CLEAR							562
														
#define	STI_CHAT_HELP_ENG								563
#define	STI_CHAT_HELP_LOCALIZE							564
														
#define	STI_CHAT_NOTICE_ENG								565
#define	STI_CHAT_NOTICE_LOCALIZE						566
														
#define	STI_CHAT_CREATE_ENG								567
#define	STI_CHAT_QCREATE_ENG							568
														
#define	STI_CHAT_MOVE_SPEED_UP							569
#define	STI_CHAT_MOVE_SPEED_DOWN						570
														
#define	STI_CHAT_OPEN_ENG								571
#define	STI_CHAT_OPEN_LOCALIZE							572
														
#define	STI_CHAT_RESURRECT								573
#define	STI_CHAT_SUICIDE								574
#define	STI_CHAT_CHARPOINT_FULL							575
														
#define	STI_CHAT_TRADEOFF_ENG							576
#define	STI_CHAT_TRADEOFF_LOCALIZE						577
														
#define	STI_CHAT_TRADEON_ENG							578
#define	STI_CHAT_TRADEON_LOCALIZE						579
														
#define	STI_CHAT_PARTYOFF_ENG							580
#define	STI_CHAT_PARTYOFF_LOCALIZE						581
														
#define	STI_CHAT_PARTYON_ENG							582
#define	STI_CHAT_PARTYON_LOCALIZE						583
														
#define	STI_CHAT_GUILDBATTLEOFF_ENG						584
#define	STI_CHAT_GUILDBATTLEOFF_LOCALIZE				585
														
#define	STI_CHAT_GUILDBATTLEON_ENG						586
#define	STI_CHAT_GUILDBATTLEON_LOCALIZE					587
														
#define	STI_CHAT_BATTLEOFF_ENG							588
#define	STI_CHAT_BATTLEOFF_LOCALIZE						589
														
#define	STI_CHAT_BATTLEON_ENG							590
#define	STI_CHAT_BATTLEON_LOCALIZE						591
														
#define	STI_CHAT_GUILDINOFF_ENG							592
#define	STI_CHAT_GUILDINOFF_LOCALIZE					593
														
#define	STI_CHAT_GUILDINON_ENG							594
#define	STI_CHAT_GUILDINON_LOCALIZE						595
														
#define	STI_CHAT_RIDE_ENG								596
#define	STI_CHAT_SUMMONS								597
														
#define	STI_CHAT_AREA_CHAT_TYPE_RACE					598
#define	STI_CHAT_AREA_CHAT_TYPE_ALL						599
#define	STI_CHAT_AREA_CHAT_TYPE_GLOBAL					600
														
#define	STI_CHAT_NOTICE_REPEAT_ENG						601
#define	STI_CHAT_NOTICE_REPEAT_LOCALIZE					602
														
#define	STI_CHAT_NOTICE_LIST_ENG						603
#define	STI_CHAT_NOTICE_LIST_LOCALIZE					604
														
#define	STI_CHAT_NOTICE_DELETE_ENG						605
#define	STI_CHAT_NOTICE_DELETE_LOCALIZE					606
														
#define	STI_CHAT_NOTICE_CLEAR_ENG						607
#define	STI_CHAT_NOTICE_CLEAR_LOCALIZE					608
														
#define	STI_TAX											609
#define STI_WRONG_CLIENT								610
#define STI_GUILD_WAREHOUSE								611
														
#define STI_INVINCIBLE									612
#define STI_NEAR_TOWN									613
#define STI_TOPDOG_GUILD								614
														
#define STI_RUNE_USE_PART_ARMOUR						615
#define STI_RUNE_USE_PART_GREAVE						616
#define STI_RUNE_USE_PART_GAUNTLET						617
#define STI_RUNE_USE_PART_FOOT							618
#define STI_RUNE_USE_PART_HELMET						619
														
#define STI_ALREADYCHANNEL_JOIN							620
														
#define STI_CHAT_EMPHASIS								621
														
#define STI_COMMAND_SET_GUARD							622
#define STI_COMMAND_CANCEL_GUARD						623
#define STI_COMMAND_GUARD_INFO							624
#define STI_COMMAND_SET_ARCHLORD						625
#define STI_COMMAND_CANCEL_ARCHLORD						626
#define STI_COMMAND_START_ARCHLORD						627
#define STI_COMMAND_END_ARCHLORD						628
#define STI_COMMAND_NEXT_STEP_ARCHLORD					629
														
#define STI_ARCHLORD_SET_GUARD							630
#define STI_ARCHLORD_CANCEL_GUARD						631
#define STI_ARCHLORD_GUARD_INFO							632
#define STI_ARCHLORD_SET_ARCHLORD						633
#define STI_ARCHLORD_CANCEL_ARCHLORD					634
														
#define STI_ARCHLORD									635
#define STI_COMMAND_ARCHLORD_NOTICE						636
#define STI_ARCHLORD_ENG_UPPER							637
														
#define STI_GUARD_SET_SUCCESS							638
#define STI_GUARD_SET_FAIL								639
#define STI_GUARD_CANCEL_SUCCESS						640
#define STI_GUARD_CANCEL_FAIL							641
#define STI_START_DUNGEON								642
#define STI_START_LANSPHERE								643
#define STI_ARCHLORD_BATTLE								644
#define STI_ARCHLORD_END								645
#define STI_NOT_ENOUGH_INVENTORY						646
#define STI_DONT_SET_GUARD								647
#define STI_DONT_CANCEL_GUARD							648
#define STI_NEED_ARCHON_ITEM							649
#define STI_GUARD_INFO									650
#define STI_RACE_BATTLE_TIME							651
#define STI_RACE_BATTLE_HUMAN							652
#define STI_RACE_BATTLE_ORC								653
#define STI_RACE_BATTLE_MOONELF							654
#define STI_RACE_BATTLE_RESULT							655
														
#define STI_ONLY_GUILD_MASTER_ITEM						656
#define STI_SET_CASTLE_OWNER							657
#define STI_CANCEL_CASTLE_OWNER							658
														
#define	STI_CHAT_SKILLEFFECTON_ENG						659
#define	STI_CHAT_SKILLEFFECTON_LOCALIZE					660
														
#define STI_COMMAND_NICKNAME							661
#define STI_COMMAND_SUNDRY								662
#define STI_COMMAND_BANK								663
														
#define STI_ALARM_AUTOQUEST								664
#define STI_ALARM_TRANSFORM								665
#define STI_ALARM_RETURNSCROLL							666
#define STI_ALARM_ARROW									667
#define STI_ALARM_BOLT									668
#define STI_ALARM_FREEAREA								669
#define STI_ALARM_DANGERAREA							670
#define STI_EVENT_GAMBLE								671
#define STI_EVENT_ARCHLORD								672
														
#define STI_BONUS_EXP									673
#define STI_BONUS_MONEY									674
#define STI_BONUS_DROP_RATE								675
#define STI_DIVIDE_RATE									676
#define STI_PHYSICAL_BLOCK_RATE							677
#define STI_PREVIOUS_STEP								678
														
#define STI_GREEN_STATUS								679
#define STI_YELLOW_STATUS								680
#define STI_RED_STATUS									681
#define STI_GREEN_GREETING								682
#define STI_GREEN_TEXT									683
#define STI_YELLOW_TEXT									684
#define STI_RED_TEXT									685
														
#define STI_NP_INIT_ERROR								686
#define STI_NP_SPEEDHACK								687
#define STI_NP_HACK_KILLED								688
#define STI_NP_HACK_DETECT								689
#define STI_NP_HACK_DOUBT								690
#define STI_NP_EXIST									691
#define STI_NP_GAME_EXIST								692
#define STI_NP_ERROR_INIT								693
#define STI_NP_NOT_FOUND								694
#define STI_NP_CRYPTOAPI								695
#define STI_NP_EXECUTE_FAIL								696
#define STI_NP_ILLEGAL_PRO								697
#define STI_NP_ABORT_UPDATE								698
#define STI_NP_CONNECT_FAIL								699
#define STI_NP_TIMEOUT									700
#define STI_NP_GAMEGUARD								701
#define STI_NP_NO_INI									702
#define STI_NP_NPGMUP									703
#define STI_NP_DOWNCFG									704
#define STI_NP_AUTH_FAIL								705
#define STI_NP_NPSCAN_FAIL								706
#define STI_NP_DEFAULT									707
														
#define STI_REFUSE_GUILD_RELATION_ON					708
#define STI_REFUSE_GUILD_RELATION_OFF					709
#define STI_REFUSE_BUDDY_ON								710
#define STI_REFUSE_BUDDY_OFF							711
#define STI_REFUSE_GUILD_RELATION						712
#define STI_REFUSE_BUDDY								713
														
#define STI_MAX_SUMMONS_COUNT							714
#define STI_MAX_CANNOT_REGISTER_BUDDY					715
														
#define STI_MAX_HIGHLEVEL_SKILL							716
														
#define STI_BANK_SLOT_IS_MAX							717
#define STI_BANK_ADD_SLOT_SUCCESS						718
														
#define STI_HANCOIN_AGREEMENT							719		// 일본에서만 쓰임
#define STI_CASHITEM_MAX_COUNT							720		// 소지 개수 한계로 로니 아이템을 더 이상 구매할 수 없습니다.
#define STI_HAVE_NOT_MORE_GELD							721		 // 소지 금액 초과로 가질 수 없습니다.
														
#define STI_SWASHBUKLER									722
														
#define STI_BANK_MONEY_FIRST							723
														
#define STI_SKILL_SPECIFIC_LEVELUP						724
#define STI_ITEM_LIMITED_LEVEL							725
#define STI_LACK_OF_MONEY								726
#define STI_SALES_OUT_OF_DATE							727
#define STI_SELL_WARNING								728

// 가챠용 메시지
#define STI_GACHA_EC_NOT_ENOUGH_ITEM					729
#define STI_GACHA_EC_NOT_ENOUGH_MONEY					730
#define STI_GACHA_EC_NOT_ENOUGH_INVENTORY				731
#define STI_GACHA_EC_NOT_ETC_ERROR						732
#define STI_GACHA_EC_LEVEL_LIMIT						733
#define STI_GACHA_DISPLAY_MESSAGE						734
#define STI_GACHA_DISPLAY_MESSAGE2						735
#define STI_GACHA_DISPLAY_RESULT						736

#define STI_NO_SALES_CONTENTS							737	// 등록일 초과로 표시할 수 없거나, 이미 판매되거나 취소된...

#define STI_SUMMONS_FAILURE_LEVEL_LIMIT_REGION			738

#define STI_GACHA_EC_NOT_ENOUGH_CHARISMA				739

#define STI_CASH_INVEN_HUNGRY							740
#define STI_DAY											741
#define STI_PET_HUNGRY_SYSTEM_MESSAGE					742
#define STI_PET_HUNGRY_TOOLTIP							743
#define STI_DISABLE_AUCTION								744

// Skill_Const2에 있는 SkillToolTip추가
#define STI_FACTOR_PERCENT_DEF_IGNORE_PHYSICAL			745
#define STI_FACTOR_PERCENT_DEF_IGNORE_ATTRIBUTE			746
#define STI_FACTOR_PERCENT_DEF_CRITICAL					747

// Skill 초기화시 에러코드에 따른 에러메세지
#define STL_SKILL_INITIALIZE_FAIL_MONEY_FULL			748

#define STI_RATE_CRITICAL								749
#define STI_RATE_STUN									750
#define STI_INCREASE_SUMMON_LEVEL						751
#define STI_EFFECT_NUMBER								752
#define STI_HEAD_CLASS									753

#define STI_EVENT_WORLD_CHAMPIONSHIP					754
#define STI_EVENT_WORLD_CHAMPIONSHIP_NOTICE				755
#define STI_EVENT_WORLD_CHAMPIONSHIP_SUCCESS			756
#define STI_EVENT_WORLD_CHAMPIONSHIP_FAIL				757

#define STI_UI_STRING_POINT								758

#define STI_STATICTEXT_RACE								760
#define STI_STATICTEXT_CLASS							761

#define STI_STATICTEXT_NO_GUILD							762

#define STI_STATICTEXT_NO_TEXT							763
#define STI_STATICTEXT_CASTLENAME_HUMAN					764
#define STI_STATICTEXT_CASTLENAME_ORC					765
#define STI_STATICTEXT_CASTLENAME_MOONELF				766
#define STI_STATICTEXT_CASTLENAME_DRAGONSCION			767
#define STI_STATICTEXT_CASTLENAME_ARCHLORD				768

#define STI_STATICTEXT_RANK								769
#define STI_STATICTEXT_UNKNOWN							770	

#define STI_STATICTEXT_SUBMASTER_APPOINT				771
#define STI_STATICTEXT_SUBMASTER_CANCEL					772

#define STI_DRAGONSCION									773
#define STI_SCION										774
#define STI_SLAYER										775
#define STI_OBITER										776
#define STI_SUMMERNER									777

#define STI_DECREASE_DURABILITY							778
#define STI_NUMBER_ITEM									779

#define STI_START_BATTLEGROUND							780
#define STI_END_BATTLEGROUND							781

#define STI_START_EVENT_BATTLEGROUND					782
#define STI_END_EVENT_BATTLEGROUND						783

#define STI_DISABLEEQUIPITEM_THISREGION					784
#define STI_DISABLEUSE_THISREGION						785

#define STI_UI_STRING_UP								786

#define STI_BOW_SERIES									787
#define STI_CROSSBOW_SERIES								788
#define STI_DRAG_BUILDUP_ITEM							789
#define STI_SMITH_BUILDUP_ITEM							790

#define STI_BASIC_ATTR_REQUIRE_HEROIC					791
#define STI_BASIC_ATTR_REQUIRE_CHARISMA					792

#define STI_HEROIC_ATTACK_POINT							793
#define STI_HEROIC_ATTACK_PERCENT						794
#define STI_HEROIC_DURATION_ATTACK_POINT				795
#define STI_HEROIC_STUN_GENERATE_DURATION				796
#define STI_HEROIC_TIME_CHANGE							797
#define STI_HEROIC_ITEM_PART							798
#define STI_HEROIC_MIN_HEROIC_DAMAGE					799
#define STI_HEROIC_MAX_HEROIC_DAMAGE					800
#define STI_HEROIC_HEROIC_DEFENCE						801
#define STI_HEROIC_NEAR_RESISTANCE						802
#define STI_HEROIC_FAR_RESISTANCE						803
#define STI_HEROIC_MAGIC_RESISTANCE						804
#define STI_HEROIC										805

#define STI_MENTOR_INVITE								806
#define STI_MENTOR_RELEASE								807
class AuStrTable
{
public:
	friend AuStrTable& ClientStr();
	friend AuStrTable& ServerStr();

private:
	AuStrTable(bool encrypt);

public:
	~AuStrTable();

	bool		Load( char* filename = "ini\\sysstr.txt", unsigned tableSize = 2048 );
	char*		GetStr(unsigned index);

private:
	int			ReadIndex( char* line );
	char*		ReadStr( char* line );
	char*		GetDecryptStr( char* str, unsigned size );

private:
	char**		m_szStringTable;
	unsigned	m_tableSize;
	unsigned	m_maxIndex;

	bool		m_isLoad;
	bool		m_encrypt;
};

// Forward Declaration
extern bool		g_ClientStrEncrypt;
extern char *	g_INIFileName;

AuStrTable& ClientStr();
AuStrTable& ServerStr();

#endif
