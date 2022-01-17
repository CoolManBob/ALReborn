#include "AgpmDropItem.h"
#include "ApMemoryTracker.h"
#include "ApModuleStream.h"

AgpdDropItemInfo::AgpdDropItemInfo()
{
	Reset();
}

AgpdDropItemInfo::~AgpdDropItemInfo()
{
	;
}

void AgpdDropItemInfo::Reset()
{
	m_lItemTID = 0;
	m_lDropRate = 0;
	m_lERate = 0;
	m_lMinQuantity = 0;
	m_lMaxQuantity = 0;
	m_lJackpotRate = 0;
	m_lJackpotPiece = 0;
	m_lRuneOptDamage = 0;
	m_lRuneOptMinDamage = 0;
	m_lRuneOptMaxDamage = 0;
	m_lRuneOptAttackPoint = 0;
	m_lRuneOptMinAttackPoint = 0;
	m_lRuneOptMaxAttackPoint = 0;
	m_lRuneOptDefenseRate = 0;
	m_lRuneOptMinDefenseRate = 0;
	m_lRuneOptMaxDefenseRate = 0;
	m_lRuneOptDefensePoint = 0;
	m_lRuneOptMinDefensePoint = 0;
	m_lRuneOptMaxDefensePoint = 0;
	m_lRuneOptHP = 0;
	m_lRuneOptMinHP = 0;
	m_lRuneOptMaxHP = 0;
	m_lRuneOptMP = 0;
	m_lRuneOptMinMP = 0;
	m_lRuneOptMaxMP = 0;
	m_lRuneOptSP = 0;
	m_lRuneOptMinSP = 0;
	m_lRuneOptMaxSP = 0;
	m_lRuneOptHPRegen = 0;
	m_lRuneOptMinHPRegen = 0;
	m_lRuneOptMaxHPRegen = 0;
	m_lRuneOptMPRegen = 0;
	m_lRuneOptMinMPRegen = 0;
	m_lRuneOptMaxMPRegen = 0;
	m_lRuneOptSPRegen = 0;
	m_lRuneOptMinSPRegen = 0;
	m_lRuneOptMaxSPRegen = 0;
	m_lRuneOptAttackSpeed = 0;
	m_lRuneOptMinAttackSpeed = 0;
	m_lRuneOptMaxAttackSpeed = 0;

	m_alDropRuneCountRate.MemSetAll();
}

AgpmDropItem::AgpmDropItem()
{
	SetModuleName("AgpmDropItem");

	SetModuleData(sizeof(AgpdDropItemTemplate), AGPMDROPITEM_DATA_TEMPLATE );
	SetModuleData(sizeof(AgpdDropItemEquipTemplate), AGPMDROPITEM_DATA_EQUIP_TEMPLATE );

	m_aDropItemTemplate.InitializeObject( sizeof(AgpdDropItemTemplate *), AGPADROPITEMTEMPLATE );
	m_aEquipItemTemplate.InitializeObject( sizeof(AgpdDropItemEquipTemplate *), AGPADROPITEMTEMPLATE );
	
	m_pcsAgpmItem	= NULL;
}

AgpmDropItem::~AgpmDropItem()
{
	// 마고자 , 메모리 제거과정이 없어서 대량의 릭이 발생..
	// 여기서 제거해줌..
	// 2004/03/26

	AgpdDropItemTemplate *	pcsTemplate;
	AgpdDropItemEquipTemplate *	pcsEquipTemplate;
	ApModuleStream			csStream;

	INT32				lIndex = 0;

	for (
		pcsTemplate = m_aDropItemTemplate.GetTemplateSequence(&lIndex);
		pcsTemplate;
		pcsTemplate = m_aDropItemTemplate.GetTemplateSequence(&lIndex))
	{
		delete pcsTemplate;
	}

	m_aDropItemTemplate.RemoveObjectAll();

	lIndex	= 0;

	for (
		pcsEquipTemplate = m_aEquipItemTemplate.GetEquipTemplateSequence(&lIndex);
		pcsEquipTemplate;
		pcsEquipTemplate = m_aEquipItemTemplate.GetEquipTemplateSequence(&lIndex))
	{
		delete pcsEquipTemplate;
	}

	m_aEquipItemTemplate.RemoveObjectAll();
	
}

BOOL AgpmDropItem::OnAddModule()
{
	m_pcsAgpmItem = (AgpmItem *) GetModule("AgpmItem");

	if( !m_pcsAgpmItem )
	{
		return FALSE;
	}

//	if(!AddStreamCallback(AGPMDROPTEIM_DATA_TEMPLATE, CBDropItemTemplateRead, CBDropItemTemplateWrite, this))
//		return FALSE;

	return TRUE;
}

BOOL AgpmDropItem::ReadEquipItemDropTemplate( char *pstrFileName )
{
	BOOL			bResult;

	bResult = FALSE;

	if( pstrFileName )
	{
		if( m_csExcelLib.OpenExcelFile( pstrFileName, true ) )
		{
			INT32				lMaxRow, lMaxColumn;
			INT32				lStartRow, lEndRow;
			INT32				lTemplateCount;
			INT32				lReadTemplateCount;
			char				*pstrTempBuffer;

			lTemplateCount = 0;
			lReadTemplateCount = 0;

			//0번째줄은 컬럼네임이 있고 1번 row부터 데이터가 들어간다.
			lStartRow = 0;
			lEndRow = 0;

			lMaxRow = m_csExcelLib.GetRow();
			lMaxColumn = m_csExcelLib.GetColumn();

			//실제 갯수를 세어둔다.
			for( INT32 lRow=lStartRow; lRow<lMaxRow; lRow++ )
			{
				pstrTempBuffer = m_csExcelLib.GetData( 0, lRow );

				if( pstrTempBuffer )
				{
					if( !strcmp( pstrTempBuffer, "-" ) )
					{
						lTemplateCount++;
					}
				}
			}

			while( 1 )
			{
				if( lReadTemplateCount >= lTemplateCount )
					break;

				for( INT32 lRow=lStartRow; lRow<lMaxRow; lRow++ )
				{
					pstrTempBuffer = m_csExcelLib.GetData( 0, lRow );

					if( pstrTempBuffer )
					{
						if( !strcmp( pstrTempBuffer, "-" ) )
						{
							lEndRow = lRow;
							break;
						}
					}
				}

				//lStartRow부터 lEndRow까지 읽어서 드랍템플릿을 완성시킨다.
				AgpdDropItemEquipTemplate		*pcsDropItemEquipTemplate;
				AgpdDropEquipData				*pcsDropEquipData;

				char			*pstrData;
				char			*pstrData2;
				char			*pstrData3;

				INT32			lIndex;
				INT32			lRowCounter;
				INT32			lColCounter;

				pcsDropItemEquipTemplate = new AgpdDropItemEquipTemplate;
				pcsDropEquipData = NULL;

				lStartRow+=1;

				pstrData = m_csExcelLib.GetData( 0, lStartRow );

				if( pstrData )
				{
					pcsDropItemEquipTemplate->m_lTID = atoi( pstrData );

					for( lRowCounter=lStartRow; lRowCounter<lEndRow; lRowCounter+=2 )
					{
						//현재 종족,클래스에대한 정보는 여기까지라면...
						pstrData = m_csExcelLib.GetData( 1, lRowCounter );
						pstrData2 = m_csExcelLib.GetData( 2, lRowCounter );

						if( (pstrData!=NULL) && (pstrData2!=NULL) )
						{
							//해당 Race,Class가 있는지본다. 없어야하며 새로 세팅해준다.
							pcsDropEquipData = GetEmptyEquipRaceSlot( pcsDropItemEquipTemplate );
							pcsDropEquipData->m_lRace=atoi(pstrData);
							pcsDropEquipData->m_lClass=atoi(pstrData2);
						}

						for( lColCounter=0; lColCounter<5; lColCounter++ )
						{
							lIndex = GetEmptyEquipItemSlot( pcsDropEquipData );

							if( lIndex >= 0 && lIndex < AGPDDROP_EQUIPITEM_COUNT )
							{
								pstrData = m_csExcelLib.GetData( lColCounter*3+3, lRowCounter );
								pstrData2 = m_csExcelLib.GetData( lColCounter*3+4, lRowCounter );
								pstrData3 = m_csExcelLib.GetData( lColCounter*3+5, lRowCounter );

								if( !pstrData || !pstrData2 || !pstrData3 )
									continue;

								if( !strcmp(pstrData,"?") || !strcmp(pstrData2,"?") || !strcmp(pstrData3,"?") )
									continue;

								memset( pcsDropEquipData->m_strItemName[lIndex], 0, sizeof( pcsDropEquipData->m_strItemName[lIndex] ) );
								strcat( pcsDropEquipData->m_strItemName[lIndex], pstrData );
								ASSERT(strlen(pcsDropEquipData->m_strItemName[lIndex]) < sizeof(pcsDropEquipData->m_strItemName[lIndex]));

								pcsDropEquipData->m_alTID[lIndex] = atoi(pstrData2);
								pcsDropEquipData->m_alScalar[lIndex] = atoi(pstrData3);

								pcsDropEquipData->m_lItemCount++;
							}
						}
					}

					//Drop템플릿을 채워넣는다.
					if (!m_aEquipItemTemplate.AddDropItemEquipTemplate( pcsDropItemEquipTemplate ))
					{
						ASSERT(!"AgpmDropItem::EquipDropTemplate 추가 실패");
					}
				}

				lReadTemplateCount++;

				lStartRow = lEndRow+1;

				if( lStartRow >=lMaxRow )
				{
					break;
				}
			}

			bResult= TRUE;
			m_csExcelLib.CloseFile();
		}
	}

	return bResult;
}

BOOL AgpmDropItem::ReadTemplate( char *pstrFileName )
{
	BOOL			bResult;

	bResult = FALSE;

	if( pstrFileName )
	{
		//Excel파일을 읽어낸다.
		if( m_csExcelLib.OpenExcelFile( pstrFileName, true ) )
		{
			INT32				lMaxRow, lMaxColumn;
			INT32				lTemplateCount;
			INT32				lReadTemplateCount;

			lMaxRow = m_csExcelLib.GetRow();
			lMaxColumn = m_csExcelLib.GetColumn();

			INT32				lStartRow, lEndRow;
			char				*pstrTempBuffer;

			lTemplateCount = 0;
			lReadTemplateCount = 0;

			lStartRow = 0;
			lEndRow = 0;

			//실제 갯수를 세어둔다.
			for( INT32 lRow=lStartRow; lRow<lMaxRow; lRow++ )
			{
				pstrTempBuffer = m_csExcelLib.GetData( 0, lRow );

				if( pstrTempBuffer )
				{
					if( !strcmp( pstrTempBuffer, "-" ) )
					{
						lTemplateCount++;
					}
				}
			}

			while( 1 )
			{
				if( lReadTemplateCount >= lTemplateCount )
					break;

				for( INT32 lRow=lStartRow; lRow<lMaxRow; lRow++ )
				{
					pstrTempBuffer = m_csExcelLib.GetData( 0, lRow );

					if( pstrTempBuffer )
					{
						if( !strcmp( pstrTempBuffer, "-" ) )
						{
							lEndRow = lRow;
							break;
						}
					}
				}

				//lStartRow부터 lEndRow까지 읽어서 드랍템플릿을 완성시킨다.
				AgpdDropItemTemplate		*pcsDropItemTemplate;
				char			*pstrData;
				char			*pstrData2;
				char			*pstrData3;

				INT32			lIndex;
				INT32			lCounter;

				pcsDropItemTemplate = new AgpdDropItemTemplate;

				//템플릿의 TID와 이름을 읽는다.
				pstrData = m_csExcelLib.GetData( 0, lStartRow );
				pstrData2 = m_csExcelLib.GetData( 1, lStartRow );

				if( pstrData )
				{
					pcsDropItemTemplate->m_lTID = atoi(pstrData );

					memset( pcsDropItemTemplate->m_strTemplateName, 0, sizeof(pcsDropItemTemplate->m_strTemplateName) );
					strcat( pcsDropItemTemplate->m_strTemplateName, pstrData2 );
					ASSERT(strlen(pcsDropItemTemplate->m_strTemplateName) < sizeof(pcsDropItemTemplate->m_strTemplateName));

					//장비형 데이터를 읽어들인다.
					pstrData = m_csExcelLib.GetData( 1, lStartRow+1 );
					pstrData2 = m_csExcelLib.GetData( 0, lStartRow+3 );

					if( pstrData && pstrData2 )
					{
						pcsDropItemTemplate->m_cDropCategory[AGPMDROPITEM_EQUIP].m_fDropProbable = (float)atof(pstrData);
						pcsDropItemTemplate->m_lEquip = atoi( pstrData2 );
					}

					//소비형 아이템 정보를 읽어들인다.
					pstrData = m_csExcelLib.GetData( 3, lStartRow+1 );
					
					if( pstrData )
					{
						pcsDropItemTemplate->m_cDropCategory[AGPMDROPITEM_CONSUME].m_fDropProbable = (float)atof( pstrData );

						for( lCounter=lStartRow+3; lCounter<lEndRow; lCounter++ )
						{
							pstrData = m_csExcelLib.GetData( 2, lCounter );
							pstrData2 = m_csExcelLib.GetData( 3, lCounter );
							pstrData3 = m_csExcelLib.GetData( 4, lCounter );

							if( pstrData && pstrData2 && pstrData3 )
							{
								lIndex = GetEmptySlot( &pcsDropItemTemplate->m_cDropCategory[AGPMDROPITEM_CONSUME] );

								strcat( pcsDropItemTemplate->m_cDropCategory[AGPMDROPITEM_CONSUME].m_strItemName[lIndex], pstrData );
								ASSERT(strlen(pcsDropItemTemplate->m_cDropCategory[AGPMDROPITEM_CONSUME].m_strItemName[lIndex]) < sizeof(pcsDropItemTemplate->m_cDropCategory[AGPMDROPITEM_CONSUME].m_strItemName[lIndex]));

								pcsDropItemTemplate->m_cDropCategory[AGPMDROPITEM_CONSUME].m_alTID[lIndex] = atoi(pstrData2);
								pcsDropItemTemplate->m_cDropCategory[AGPMDROPITEM_CONSUME].m_alScalar[lIndex] = atoi(pstrData3);

								pcsDropItemTemplate->m_cDropCategory[AGPMDROPITEM_CONSUME].m_lItemCount++;
							}
							else
							{
								break;
							}
						}
					}

					//귀중품 아이템의 정보를 읽어들인다.
					pstrData = m_csExcelLib.GetData( 6, lStartRow+1 );

					if( pstrData )
					{
						pcsDropItemTemplate->m_cDropCategory[AGPMDROPITEM_VALUABLE].m_fDropProbable = (float)atof( pstrData );

						for( lCounter=lStartRow+3; lCounter<lEndRow; lCounter++ )
						{
							pstrData = m_csExcelLib.GetData( 5, lCounter );
							pstrData2 = m_csExcelLib.GetData( 6, lCounter );
							pstrData3 = m_csExcelLib.GetData( 7, lCounter );

							if( pstrData && pstrData2 && pstrData3 )
							{
								lIndex = GetEmptySlot( &pcsDropItemTemplate->m_cDropCategory[AGPMDROPITEM_VALUABLE] );

								strcat( pcsDropItemTemplate->m_cDropCategory[AGPMDROPITEM_VALUABLE].m_strItemName[lIndex], pstrData );
								ASSERT(strlen(pcsDropItemTemplate->m_cDropCategory[AGPMDROPITEM_VALUABLE].m_strItemName[lIndex]) < sizeof(pcsDropItemTemplate->m_cDropCategory[AGPMDROPITEM_VALUABLE].m_strItemName[lIndex]));

								pcsDropItemTemplate->m_cDropCategory[AGPMDROPITEM_VALUABLE].m_alTID[lIndex] = atoi(pstrData2);
								pcsDropItemTemplate->m_cDropCategory[AGPMDROPITEM_VALUABLE].m_alScalar[lIndex] = atoi(pstrData3);
								pcsDropItemTemplate->m_cDropCategory[AGPMDROPITEM_VALUABLE].m_lItemCount++;
							}
							else
							{
								break;
							}
						}
					}

					//기원석의 정보를 읽어들인다.
					pstrData = m_csExcelLib.GetData( 9, lStartRow+1 );

					if( pstrData )
					{
						pcsDropItemTemplate->m_cDropCategory[AGPMDROPITEM_OPTIONSTONE].m_fDropProbable = (float)atof( pstrData );

						for( lCounter=lStartRow+3; lCounter<lEndRow; lCounter++ )
						{
							pstrData = m_csExcelLib.GetData( 8, lCounter );
							pstrData2 = m_csExcelLib.GetData( 9, lCounter );
							pstrData3 = m_csExcelLib.GetData( 10, lCounter );

							if( pstrData && pstrData2 && pstrData3 )
							{
								lIndex = GetEmptySlot( &pcsDropItemTemplate->m_cDropCategory[AGPMDROPITEM_OPTIONSTONE] );

								strcat( pcsDropItemTemplate->m_cDropCategory[AGPMDROPITEM_OPTIONSTONE].m_strItemName[lIndex], pstrData );
								ASSERT(strlen(pcsDropItemTemplate->m_cDropCategory[AGPMDROPITEM_OPTIONSTONE].m_strItemName[lIndex]) < sizeof(pcsDropItemTemplate->m_cDropCategory[AGPMDROPITEM_OPTIONSTONE].m_strItemName[lIndex]));

								pcsDropItemTemplate->m_cDropCategory[AGPMDROPITEM_OPTIONSTONE].m_alTID[lIndex] = atoi(pstrData2);
								pcsDropItemTemplate->m_cDropCategory[AGPMDROPITEM_OPTIONSTONE].m_alScalar[lIndex] = atoi(pstrData3);
								pcsDropItemTemplate->m_cDropCategory[AGPMDROPITEM_OPTIONSTONE].m_lItemCount++;
							}
							else
							{
								break;
							}
						}
					}

					//겔드의 정보를 읽어들인다.
					pstrData = m_csExcelLib.GetData( 12, lStartRow+1 );

					if( pstrData )
					{
						pcsDropItemTemplate->m_cDropCategory[AGPMDROPITEM_GHELLD].m_fDropProbable = (float)atof( pstrData );

						for( lCounter=lStartRow+3; lCounter<lEndRow; lCounter++ )
						{
							pstrData = m_csExcelLib.GetData( 11, lCounter );
							pstrData2 = m_csExcelLib.GetData( 12, lCounter );
							pstrData3 = m_csExcelLib.GetData( 13, lCounter );

							if( pstrData && pstrData2 && pstrData3 )
							{
								lIndex = GetEmptySlot( &pcsDropItemTemplate->m_cDropCategory[AGPMDROPITEM_GHELLD] );

								strcat( pcsDropItemTemplate->m_cDropCategory[AGPMDROPITEM_GHELLD].m_strItemName[lIndex], pstrData );
								ASSERT(strlen(pcsDropItemTemplate->m_cDropCategory[AGPMDROPITEM_GHELLD].m_strItemName[lIndex]) < sizeof(pcsDropItemTemplate->m_cDropCategory[AGPMDROPITEM_GHELLD].m_strItemName[lIndex]));

								pcsDropItemTemplate->m_cDropCategory[AGPMDROPITEM_GHELLD].m_alTID[lIndex] = atoi(pstrData2);
								pcsDropItemTemplate->m_cDropCategory[AGPMDROPITEM_GHELLD].m_alScalar[lIndex] = atoi(pstrData3);
								pcsDropItemTemplate->m_cDropCategory[AGPMDROPITEM_GHELLD].m_lItemCount++;
							}
							else
							{
								break;
							}
						}
					}

					//생산재 아이템의 정보를 읽어들인다.
					pstrData = m_csExcelLib.GetData( 15, lStartRow+1 );

					if( pstrData )
					{
						pcsDropItemTemplate->m_cDropCategory[AGPMDROPITEM_PRODUCT].m_fDropProbable = (float)atof( pstrData );

						for( lCounter=lStartRow+3; lCounter<lEndRow; lCounter++ )
						{
							pstrData = m_csExcelLib.GetData( 14, lCounter );
							pstrData2 = m_csExcelLib.GetData( 15, lCounter );
							pstrData3 = m_csExcelLib.GetData( 16, lCounter );

							if( pstrData && pstrData2 && pstrData3 )
							{
								lIndex = GetEmptySlot( &pcsDropItemTemplate->m_cDropCategory[AGPMDROPITEM_PRODUCT] );

								strcat( pcsDropItemTemplate->m_cDropCategory[AGPMDROPITEM_PRODUCT].m_strItemName[lIndex], pstrData );
								ASSERT(strlen(pcsDropItemTemplate->m_cDropCategory[AGPMDROPITEM_PRODUCT].m_strItemName[lIndex]) < sizeof(pcsDropItemTemplate->m_cDropCategory[AGPMDROPITEM_PRODUCT].m_strItemName[lIndex]));
								pcsDropItemTemplate->m_cDropCategory[AGPMDROPITEM_PRODUCT].m_alTID[lIndex] = atoi(pstrData2);
								pcsDropItemTemplate->m_cDropCategory[AGPMDROPITEM_PRODUCT].m_alScalar[lIndex] = atoi(pstrData3);
								pcsDropItemTemplate->m_cDropCategory[AGPMDROPITEM_PRODUCT].m_lItemCount++;
							}
							else
							{
								break;
							}
						}
					}

					//레어 아이템의 정보를 읽어들인다.
					pstrData = m_csExcelLib.GetData( 18, lStartRow+1 );

					if( pstrData )
					{
						pcsDropItemTemplate->m_cDropCategory[AGPMDROPITEM_RARE].m_fDropProbable = (float)atof( pstrData );

						for( lCounter=lStartRow+3; lCounter<lEndRow; lCounter++ )
						{
							pstrData = m_csExcelLib.GetData( 17, lCounter );
							pstrData2 = m_csExcelLib.GetData( 18, lCounter );
							pstrData3 = m_csExcelLib.GetData( 19, lCounter );

							if( pstrData && pstrData2 && pstrData3 )
							{
								lIndex = GetEmptySlot( &pcsDropItemTemplate->m_cDropCategory[AGPMDROPITEM_RARE] );

								strcat( pcsDropItemTemplate->m_cDropCategory[AGPMDROPITEM_RARE].m_strItemName[lIndex], pstrData );
								ASSERT(strlen(pcsDropItemTemplate->m_cDropCategory[AGPMDROPITEM_RARE].m_strItemName[lIndex]) < sizeof(pcsDropItemTemplate->m_cDropCategory[AGPMDROPITEM_RARE].m_strItemName[lIndex]));
								pcsDropItemTemplate->m_cDropCategory[AGPMDROPITEM_RARE].m_alTID[lIndex] = atoi(pstrData2);
								pcsDropItemTemplate->m_cDropCategory[AGPMDROPITEM_RARE].m_alScalar[lIndex] = atoi(pstrData3);
								pcsDropItemTemplate->m_cDropCategory[AGPMDROPITEM_RARE].m_lItemCount++;
							}
							else
							{
								break;
							}
						}
					}

					//이벤트 아이템의 정보를 읽어들인다.
					pstrData = m_csExcelLib.GetData( 21, lStartRow+1 );

					if( pstrData )
					{
						pcsDropItemTemplate->m_cDropCategory[AGPMDROPITEM_EVENT].m_fDropProbable = (float)atof( pstrData );

						for( lCounter=lStartRow+3; lCounter<lEndRow; lCounter++ )
						{
							pstrData = m_csExcelLib.GetData( 20, lCounter );
							pstrData2 = m_csExcelLib.GetData( 21, lCounter );
							pstrData3 = m_csExcelLib.GetData( 22, lCounter );

							if( pstrData && pstrData2 && pstrData3 )
							{
								lIndex = GetEmptySlot( &pcsDropItemTemplate->m_cDropCategory[AGPMDROPITEM_EVENT] );

								strcat( pcsDropItemTemplate->m_cDropCategory[AGPMDROPITEM_EVENT].m_strItemName[lIndex], pstrData );
								ASSERT(strlen(pcsDropItemTemplate->m_cDropCategory[AGPMDROPITEM_EVENT].m_strItemName[lIndex]) < sizeof(pcsDropItemTemplate->m_cDropCategory[AGPMDROPITEM_EVENT].m_strItemName[lIndex]));
								pcsDropItemTemplate->m_cDropCategory[AGPMDROPITEM_EVENT].m_alTID[lIndex] = atoi(pstrData2);
								pcsDropItemTemplate->m_cDropCategory[AGPMDROPITEM_EVENT].m_alScalar[lIndex] = atoi(pstrData3);
								pcsDropItemTemplate->m_cDropCategory[AGPMDROPITEM_EVENT].m_lItemCount++;
							}
							else
							{
								break;
							}
						}
					}

					//Drop템플릿을 채워넣는다.
					if (!m_aDropItemTemplate.AddDropItemTemplate( pcsDropItemTemplate ))
					{
						ASSERT(!"AgpmDropItem::ItemDropTemplate 추가 실패");
						delete	pcsDropItemTemplate;
						pcsDropItemTemplate	= NULL;
					}
				}
				else
				{
					//드랍템플릿 이름이 없는경우.
				}

				lStartRow = lEndRow+1;
				lReadTemplateCount++;

				if( lStartRow >=lMaxRow )
				{
					break;
				}
			}

			bResult = TRUE;
			m_csExcelLib.CloseFile();
		}
	}

	return bResult;
}

INT32 AgpmDropItem::GetEmptySlot( AgpdDropItemData *pcsDropItemData )
{
	INT32			lEmptySlot;

	lEmptySlot = -1;

	if( pcsDropItemData )
	{
		for( int lCounter=0; lCounter<AGPDDROPITEM_COUNT; lCounter++ )
		{
			if( pcsDropItemData->m_alTID[lCounter] == 0 )
			{
				lEmptySlot = lCounter;
				break;
			}
		}
	}

	return lEmptySlot;
}

AgpdDropEquipData *AgpmDropItem::GetEmptyEquipRaceSlot( AgpdDropItemEquipTemplate *pcsDropItemEquipTemplate )
{
	AgpdDropEquipData		*pcsAgpdDropEquipData;

	pcsAgpdDropEquipData = NULL;

	if( pcsDropItemEquipTemplate )
	{
		for( int lCounter=0; lCounter<AGPMDROPITEM_EQUIP_MAX_RACE;lCounter++ )
		{
			if( pcsDropItemEquipTemplate->m_csAgpdDropEquipData[lCounter].m_lRace == 0 )
			{
				pcsAgpdDropEquipData = &pcsDropItemEquipTemplate->m_csAgpdDropEquipData[lCounter];
				break;
			}
		}
	}
	
	return pcsAgpdDropEquipData;
}

INT32 AgpmDropItem::GetEmptyEquipItemSlot( AgpdDropEquipData *pcsDropEquipData )
{
	INT32			lEmptySlot;

	lEmptySlot = -1;

	if( pcsDropEquipData )
	{
		for( int lCounter=0; lCounter<AGPDDROP_EQUIPITEM_COUNT; lCounter++ )
		{
			if( pcsDropEquipData->m_alTID[lCounter] == 0 )
			{
				lEmptySlot = lCounter;
				break;
			}
		}
	}

	return lEmptySlot;
}
