#pragma once

class CDefaultEquipmentsDlg : public CDialog
{
// Construction
public:
	CDefaultEquipmentsDlg(AgpdItemADCharTemplate *pcsAgpdItemADCharTemplate, CWnd* pParent = NULL);   // standard constructor

	//{{AFX_DATA(CDefaultEquipmentsDlg)
	enum { IDD = IDD_DEFAULT_EQUIPMENTS };
	CString		m_astrEquipment[AGPMITEM_PART_V_ALL];
	//}}AFX_DATA

protected:
	AgpdItemADCharTemplate	*m_pcsAgpdItemADCharTemplate;
	AgpdItemADCharTemplate	m_csTempData;

private:
	void	UpdateEquip( INT32 lEquipID );
	void	DeleteEquip( INT32 lEquipID );

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();

	//{{AFX_MSG(CDefaultEquipmentsDlg)
	afx_msg void OnButtonDeSetAmulet()	{	UpdateEquip( AGPMITEM_PART_ACCESSORY_NECKLACE );	}
	afx_msg void OnButtonDeSetArms()	{	UpdateEquip( AGPMITEM_PART_ARMS );					}
	//afx_msg void OnButtonDeSetArms()	{	UpdateEquip( AGPMITEM_PART_HEAD );					}
	afx_msg void OnButtonDeSetBody()	{	UpdateEquip( AGPMITEM_PART_BODY );					}
	afx_msg void OnButtonDeSetCloak()	{	UpdateEquip( AGPMITEM_PART_ARMS2 );					}
	afx_msg void OnButtonDeSetFoot()	{	UpdateEquip( AGPMITEM_PART_FOOT );					}
	afx_msg void OnButtonDeSetHands()	{	UpdateEquip( AGPMITEM_PART_HANDS );					}
	afx_msg void OnButtonDeSetHead()	{	UpdateEquip( AGPMITEM_PART_HEAD );					}
	afx_msg void OnButtonDeSetLegs()	{	UpdateEquip( AGPMITEM_PART_LEGS );					}
	afx_msg void OnButtonDeSetRing1()	{	UpdateEquip( AGPMITEM_PART_ACCESSORY_RING1 );		}
	afx_msg void OnButtonDeSetRing2()	{	UpdateEquip( AGPMITEM_PART_ACCESSORY_RING2 );		}
	afx_msg void OnButtonDeSetShield()	{	UpdateEquip( AGPMITEM_PART_HAND_LEFT );				}
	afx_msg void OnButtonDeSetWeapon()	{	UpdateEquip( AGPMITEM_PART_HAND_RIGHT );			}

	afx_msg void OnButtonDeDelAmulet()	{	DeleteEquip( AGPMITEM_PART_ACCESSORY_NECKLACE );	}
	afx_msg void OnButtonDeDelArms()	{	DeleteEquip( AGPMITEM_PART_HEAD );					}
	afx_msg void OnButtonDeDelBody()	{	DeleteEquip( AGPMITEM_PART_BODY );					}
	afx_msg void OnButtonDeDelCloak()	{	DeleteEquip( AGPMITEM_PART_ARMS2 );					}
	afx_msg void OnButtonDeDelFoot()	{	DeleteEquip( AGPMITEM_PART_FOOT );					}
	afx_msg void OnButtonDeDelHands()	{	DeleteEquip( AGPMITEM_PART_HANDS );					}
	afx_msg void OnButtonDeDelHead()	{	DeleteEquip( AGPMITEM_PART_HEAD );					}
	afx_msg void OnButtonDeDelLegs()	{	DeleteEquip( AGPMITEM_PART_LEGS );					}	
	afx_msg void OnButtonDeDelRing1()	{	DeleteEquip( AGPMITEM_PART_ACCESSORY_RING1 );		}
	afx_msg void OnButtonDeDelRing2()	{	DeleteEquip( AGPMITEM_PART_ACCESSORY_RING2 );		}
	afx_msg void OnButtonDeDelShield()	{	DeleteEquip( AGPMITEM_PART_HAND_LEFT );				}
	afx_msg void OnButtonDeDelWeapon()	{	DeleteEquip( AGPMITEM_PART_HAND_RIGHT );			}

	afx_msg void OnButtonDeSetAmulet2()	{	UpdateEquip( AGPMITEM_PART_V_ACCESSORY_NECKLACE );	}
	afx_msg void OnButtonDeSetArms2()	{	UpdateEquip( AGPMITEM_PART_V_ARMS );				}
	afx_msg void OnButtonDeSetBody2()	{	UpdateEquip( AGPMITEM_PART_V_BODY );				}
	afx_msg void OnButtonDeSetCloak2()	{	UpdateEquip( AGPMITEM_PART_V_ARMS2 );				}
	afx_msg void OnButtonDeSetFoot2()	{	UpdateEquip( AGPMITEM_PART_V_FOOT );				}
	afx_msg void OnButtonDeSetHands2()	{	UpdateEquip( AGPMITEM_PART_V_HANDS );				}
	afx_msg void OnButtonDeSetHead2()	{	UpdateEquip( AGPMITEM_PART_V_HEAD );				}
	afx_msg void OnButtonDeSetLegs2()	{	UpdateEquip( AGPMITEM_PART_V_LEGS );				}
	afx_msg void OnButtonDeSetRing3()	{	UpdateEquip( AGPMITEM_PART_V_ACCESSORY_RING1 );		}
	afx_msg void OnButtonDeSetRing4()	{	UpdateEquip( AGPMITEM_PART_V_ACCESSORY_RING2 );		}
	afx_msg void OnButtonDeSetShield2()	{	UpdateEquip( AGPMITEM_PART_V_HAND_LEFT );			}
	afx_msg void OnButtonDeSetWeapon2()	{	UpdateEquip( AGPMITEM_PART_V_HAND_RIGHT );			}

	afx_msg void OnButtonDeDelAmulet2()	{	DeleteEquip( AGPMITEM_PART_V_ACCESSORY_NECKLACE );	}
	afx_msg void OnButtonDeDelArms2()	{	DeleteEquip( AGPMITEM_PART_V_HEAD );				}
	afx_msg void OnButtonDeDelBody2()	{	DeleteEquip( AGPMITEM_PART_V_BODY );				}
	afx_msg void OnButtonDeDelCloak2()	{	DeleteEquip( AGPMITEM_PART_V_ARMS2 );				}
	afx_msg void OnButtonDeDelFoot2()	{	DeleteEquip( AGPMITEM_PART_V_FOOT );				}
	afx_msg void OnButtonDeDelHands2()	{	DeleteEquip( AGPMITEM_PART_V_HANDS );				}
	afx_msg void OnButtonDeDelHead2()	{	DeleteEquip( AGPMITEM_PART_V_HEAD );				}
	afx_msg void OnButtonDeDelLegs2()	{	DeleteEquip( AGPMITEM_PART_V_LEGS );				}	
	afx_msg void OnButtonDeDelRing3()	{	DeleteEquip( AGPMITEM_PART_V_ACCESSORY_RING1 );		}
	afx_msg void OnButtonDeDelRing4()	{	DeleteEquip( AGPMITEM_PART_V_ACCESSORY_RING2 );		}
	afx_msg void OnButtonDeDelShield2()	{	DeleteEquip( AGPMITEM_PART_V_HAND_LEFT );			}
	afx_msg void OnButtonDeDelWeapon2()	{	DeleteEquip( AGPMITEM_PART_V_HAND_RIGHT );			}
	
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
