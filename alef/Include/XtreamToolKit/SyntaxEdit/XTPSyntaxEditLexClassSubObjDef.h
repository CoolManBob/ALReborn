// XTPSyntaxEditLexClassSubObjDef.h
//
// This file is a part of the XTREME TOOLKIT PRO MFC class library.
// (c)1998-2007 Codejock Software, All Rights Reserved.
//
// THIS SOURCE FILE IS THE PROPERTY OF CODEJOCK SOFTWARE AND IS NOT TO BE
// RE-DISTRIBUTED BY ANY MEANS WHATSOEVER WITHOUT THE EXPRESSED WRITTEN
// CONSENT OF CODEJOCK SOFTWARE.
//
// THIS SOURCE CODE CAN ONLY BE USED UNDER THE TERMS AND CONDITIONS OUTLINED
// IN THE XTREME SYNTAX EDIT LICENSE AGREEMENT. CODEJOCK SOFTWARE GRANTS TO
// YOU (ONE SOFTWARE DEVELOPER) THE LIMITED RIGHT TO USE THIS SOFTWARE ON A
// SINGLE COMPUTER.
//
// CONTACT INFORMATION:
// support@codejock.com
// http://www.codejock.com
//
//////////////////////////////////////////////////////////////////////

//{{AFX_CODEJOCK_PRIVATE
#if !defined(__XTPSYNTAXEDITLEXCLASSSUBOBJDEF_H__)
#define __XTPSYNTAXEDITLEXCLASSSUBOBJDEF_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#pragma warning(disable: 4097 4786)

namespace XTPSyntaxEditLexAnalyser
{
//{{AFX_CODEJOCK_PRIVATE
	//===========================================================================
	#define XTP_EDIT_LEX_CLASS_OBJ_MEMBER_NAME(_Name, _str) inline LPCTSTR _Name() { return _str;}

	//===========================================================================
	#define XTP_EDIT_LEX_CLASS_OBJ_MEMBER(id, _member) public: \
		TBase::_Type##id _member; \
		protected: \
		virtual void SetSubMember##id() { \
			m_p##id = &_member; \
		}

	//===========================================================================
	#define XTP_EDIT_DEFINE_LEX_CLASS_OBJ_BASE_N1(subN1) \
		CXTPSyntaxEditLexObj_SpecCollT<CXTPSyntaxEditLexVariantPtrArrayExAutomatT<subN1> >

	#define XTP_EDIT_DEFINE_LEX_CLASS_OBJ_BASE_N2(subN1, subN2) \
		CXTPSyntaxEditLexObj_SpecCollT<CXTPSyntaxEditLexVariantPtrArrayExAutomatT<subN1>,\
							 CXTPSyntaxEditLexVariantPtrArrayExAutomatT<subN2> >

	#define XTP_EDIT_DEFINE_LEX_CLASS_OBJ_BASE_N3(subN1, subN2, subN3) \
		CXTPSyntaxEditLexObj_SpecCollT<CXTPSyntaxEditLexVariantPtrArrayExAutomatT<subN1>, \
							 CXTPSyntaxEditLexVariantPtrArrayExAutomatT<subN2>, \
							 CXTPSyntaxEditLexVariantPtrArrayExAutomatT<subN3> >

	//===========================================================================
	#define XTP_EDIT_DEFINE_LEX_CLASS_OBJ(objClass, DEF_BASE_Nx) \
		class objClass: public DEF_BASE_Nx \
		{   \
		public: \
			typedef DEF_BASE_Nx TBase; \
			objClass() { \
				SetSubMembers(); \
			}; \
			virtual ~objClass() {};

	#define XTP_EDIT_DEFINE_LEX_CLASS_OBJ_N1(objClass, subN1) \
				XTP_EDIT_DEFINE_LEX_CLASS_OBJ(objClass, \
					XTP_EDIT_DEFINE_LEX_CLASS_OBJ_BASE_N1(subN1) )

	#define XTP_EDIT_DEFINE_LEX_CLASS_OBJ_N2(objClass, subN1, subN2) \
				XTP_EDIT_DEFINE_LEX_CLASS_OBJ(objClass, \
					XTP_EDIT_DEFINE_LEX_CLASS_OBJ_BASE_N2(subN1, subN2) )

	#define XTP_EDIT_DEFINE_LEX_CLASS_OBJ_N3(objClass, subN1, subN2, subN3) \
				XTP_EDIT_DEFINE_LEX_CLASS_OBJ(objClass, \
					XTP_EDIT_DEFINE_LEX_CLASS_OBJ_BASE_N3(subN1, subN2, subN3) )

	//===========================================================================
	#define XTP_EDIT_DEFINE_LEX_CLASS_OBJ_END };
	////////////////////////////////////////////////////////////////////////////

	//= Prevoius =============================================================
	XTP_EDIT_LEX_CLASS_OBJ_MEMBER_NAME(FnName_previous_class,  _T("previous:class"))
	XTP_EDIT_LEX_CLASS_OBJ_MEMBER_NAME(FnName_previous_tag,    _T("previous:tag"))
	XTP_EDIT_LEX_CLASS_OBJ_MEMBER_NAME(FnName_previous_tag_separators, _T("previous:tag:separators"))

	XTP_EDIT_DEFINE_LEX_CLASS_OBJ_N3(CXTPSyntaxEditLexObj_Previous, FnName_previous_class,
							FnName_previous_tag, FnName_previous_tag_separators)
		XTP_EDIT_LEX_CLASS_OBJ_MEMBER(0, m_class)
		XTP_EDIT_LEX_CLASS_OBJ_MEMBER(1, m_tag)
		XTP_EDIT_LEX_CLASS_OBJ_MEMBER(2, m_tag_separators)
	XTP_EDIT_DEFINE_LEX_CLASS_OBJ_END

	//= Start ================================================================
	XTP_EDIT_LEX_CLASS_OBJ_MEMBER_NAME(FnName_start_class, _T("start:class"))
	XTP_EDIT_LEX_CLASS_OBJ_MEMBER_NAME(FnName_start_tag,    _T("start:tag"))

	XTP_EDIT_DEFINE_LEX_CLASS_OBJ_N2(CXTPSyntaxEditLexObj_Start, FnName_start_class, FnName_start_tag)
		XTP_EDIT_LEX_CLASS_OBJ_MEMBER(0, m_class)
		XTP_EDIT_LEX_CLASS_OBJ_MEMBER(1, m_tag)
	XTP_EDIT_DEFINE_LEX_CLASS_OBJ_END

	//= End ================================================================
	XTP_EDIT_LEX_CLASS_OBJ_MEMBER_NAME(FnName_end_class,       _T("end:class"))
	XTP_EDIT_LEX_CLASS_OBJ_MEMBER_NAME(FnName_end_tag,     _T("end:tag"))
	XTP_EDIT_LEX_CLASS_OBJ_MEMBER_NAME(FnName_end_separators,  _T("end:separators"))

	XTP_EDIT_DEFINE_LEX_CLASS_OBJ_N3(CXTPSyntaxEditLexObj_End, FnName_end_class, FnName_end_tag, FnName_end_separators)
		XTP_EDIT_LEX_CLASS_OBJ_MEMBER(0, m_class)
		XTP_EDIT_LEX_CLASS_OBJ_MEMBER(1, m_tag)
		XTP_EDIT_LEX_CLASS_OBJ_MEMBER(2, m_separators)
	XTP_EDIT_DEFINE_LEX_CLASS_OBJ_END

	//= Token ================================================================
	XTP_EDIT_LEX_CLASS_OBJ_MEMBER_NAME(FnName_tokeFnName_tag,              _T("token:tag"))
	XTP_EDIT_LEX_CLASS_OBJ_MEMBER_NAME(FnName_tokeFnName_start_separators,_T("token:start:separators"))
	XTP_EDIT_LEX_CLASS_OBJ_MEMBER_NAME(FnName_tokeFnName_end_separators,   _T("token:end:separators"))

	XTP_EDIT_DEFINE_LEX_CLASS_OBJ_N3(CXTPSyntaxEditLexObj_Token, FnName_tokeFnName_tag,
							FnName_tokeFnName_start_separators, FnName_tokeFnName_end_separators)
		XTP_EDIT_LEX_CLASS_OBJ_MEMBER(0, m_tag)
		XTP_EDIT_LEX_CLASS_OBJ_MEMBER(1, m_start_separators)
		XTP_EDIT_LEX_CLASS_OBJ_MEMBER(2, m_end_separators)
	XTP_EDIT_DEFINE_LEX_CLASS_OBJ_END

	//= Skip ================================================================
	XTP_EDIT_LEX_CLASS_OBJ_MEMBER_NAME(FnName_skip_tag, _T("skip:tag"))

	XTP_EDIT_DEFINE_LEX_CLASS_OBJ_N1(CXTPSyntaxEditLexObj_Skip, FnName_skip_tag)
		XTP_EDIT_LEX_CLASS_OBJ_MEMBER(0, m_tag)
	XTP_EDIT_DEFINE_LEX_CLASS_OBJ_END

	//= active tags (INTERNAL OBJECT) ================================================================
	XTP_EDIT_LEX_CLASS_OBJ_MEMBER_NAME(FnName_active_tags, _T("_active_tags"))

	class CXTPSyntaxEditLexObj_ActiveTags : public CXTPSyntaxEditLexVariantPtrArrayExAutomatT<FnName_active_tags>
	{
		typedef CXTPSyntaxEditLexVariantPtrArrayExT<FnName_active_tags> TBase;
	public:
		virtual ~CXTPSyntaxEditLexObj_ActiveTags(){};
	};
//}}AFX_CODEJOCK_PRIVATE
////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////
#endif // !defined(__XTPSYNTAXEDITLEXCLASSSUBOBJDEF_H__)
