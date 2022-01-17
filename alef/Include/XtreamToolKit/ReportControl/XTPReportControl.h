// XTPReportControl.h: interface for the CXTPReportControl class.
//
// This file is a part of the XTREME REPORTCONTROL MFC class library.
// (c)1998-2007 Codejock Software, All Rights Reserved.
//
// THIS SOURCE FILE IS THE PROPERTY OF CODEJOCK SOFTWARE AND IS NOT TO BE
// RE-DISTRIBUTED BY ANY MEANS WHATSOEVER WITHOUT THE EXPRESSED WRITTEN
// CONSENT OF CODEJOCK SOFTWARE.
//
// THIS SOURCE CODE CAN ONLY BE USED UNDER THE TERMS AND CONDITIONS OUTLINED
// IN THE XTREME TOOLKIT PRO LICENSE AGREEMENT. CODEJOCK SOFTWARE GRANTS TO
// YOU (ONE SOFTWARE DEVELOPER) THE LIMITED RIGHT TO USE THIS SOFTWARE ON A
// SINGLE COMPUTER.
//
// CONTACT INFORMATION:
// support@codejock.com
// http://www.codejock.com
//
/////////////////////////////////////////////////////////////////////////////

//{{AFX_CODEJOCK_PRIVATE
#if !defined(__XTPREPORTCONTROL_H__)
#define __XTPREPORTCONTROL_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "XTPReportDefines.h"
#include "XTPReportRow.h"
#include "XTPReportRows.h"
#include "XTPReportPaintManager.h"
#include "XTPReportTip.h"
#include "Common/XTPSmartPtrInternalT.h"

struct XTP_REPORTRECORDITEM_DRAWARGS;
struct XTP_REPORTRECORDITEM_METRICS;
class CXTPReportRecords;
class CXTPReportRecordItem;
class CXTPReportColumns;
class CXTPReportGroups;
class CXTPReportColumn;
class CXTPReportRecord;
class CXTPReportRows;
class CXTPReportPaintManager;
class CXTPReportColumnOrder;
class CXTPImageManager;
class CXTPReportInplaceEdit;
class CXTPReportInplaceButtons;
class CXTPReportInplaceList;
class CXTPPropExchange;
class CXTPToolTipContext;
class CXTPReportNavigator;
class CXTPReportRecordItemConstraint;

struct XTP_REPORTRECORDITEM_ARGS;
struct XTP_NM_REPORTTOOLTIPINFO;

//===========================================================================
// Summary:
//     This structure is sent to Main window in a WM_NOTIFY message from Item
//     and provides all parameters that are needed in processing control specific
//     notifications by the main window
// Example:
// <code>
// BEGIN_MESSAGE_MAP(CReportSampleView, CXTPReportView)
//     ON_NOTIFY(XTP_NM_REPORT_GETITEMMETRICS, XTP_ID_REPORT_CONTROL, OnReportGetItemMetrics)
// END_MESSAGE_MAP()
//
// void CReportSampleView::OnReportGetItemMetrics(NMHDR*  pNotifyStruct, LRESULT* /*result*/)
// {
//     XTP_NM_REPORTITEMMETRICS* pItemNotify = (XTP_NM_REPORTITEMMETRICS*)pNotifyStruct;
//
//     ASSERT(pItemNotify->pDrawArgs);
//     ASSERT(pItemNotify->pDrawArgs->pControl);
//     ASSERT(pItemNotify->pDrawArgs->pRow);
//
//     //     pItemNotify->pDrawArgs->pColumn   - may be NULL (for a group row)
//     //     pItemNotify->pDrawArgs->pItem     - may be NULL (for a group row)
//
//     ASSERT(pItemNotify->pItemMetrics);
//
//     // TODO: customize members of pItemNotify->pItemMetrics.
// }
// </code>
// See Also:
//     CXTPReportControl, CXTPReportControl::GetItemMetrics(),
//     XTP_NM_REPORT_GETITEMMETRICS
//===========================================================================
struct XTP_NM_REPORTITEMMETRICS
{
	NMHDR hdr;                                      // Standard structure, containing information about a notification message.
	XTP_REPORTRECORDITEM_DRAWARGS*  pDrawArgs;      // Pointer to XTP_REPORTRECORDITEM_DRAWARGS structure.
	XTP_REPORTRECORDITEM_METRICS*   pItemMetrics;   // Pointer to XTP_REPORTRECORDITEM_METRICS structure to be filled.
};

//===========================================================================
// Summary:
//     This structure is sent to main window in a WM_NOTIFY message from from report control
//     to determine Records was dragged or dropped
// Example:
// <code>
// BEGIN_MESSAGE_MAP(CReportSampleView, CXTPReportView)
//     ON_NOTIFY(XTP_NM_REPORT_BEGINDRAG, XTP_ID_REPORT_CONTROL, OnReportBeginDrag)
// END_MESSAGE_MAP()
//
// void CReportSampleView::OnReportBeginDrag(NMHDR*  pNotifyStruct, LRESULT* /*result*/)
// {
//     XTP_NM_REPORTDRAGDROP* pItemNotify = (XTP_NM_REPORTDRAGDROP*)pNotifyStruct;
//
//     ASSERT(pItemNotify->pRecords);
// }
// </code>
// See Also:
//     XTP_NM_REPORT_DROP, XTP_NM_REPORT_BEGINDRAG, CXTPReportControl::EnableDragDrop
//===========================================================================
struct XTP_NM_REPORTDRAGDROP
{
	NMHDR hdr;                              // Standard structure, containing information about a notification message.
	CXTPReportRecords* pRecords;            // Records will be dragged/dropped
	CXTPReportRecord*  pTargetRecord;       // Target record (if any)
	BOOL               bAbove;              // Where to insert (relative to the target record)
};

//===========================================================================
// Summary:
//     This structure is sent to Main window in a WM_NOTIFY message before
//     processing OnKeyDown event.
// See Also:
//     CXTPReportControl::OnPreviewKeyDown, XTP_NM_REPORT_PREVIEWKEYDOWN,
//     CXTPReportControl::OnKeyDown, CWnd::OnKeyDown.
//===========================================================================
struct XTP_NM_REPORTPREVIEWKEYDOWN
{
	NMHDR hdr;      // Standard structure, containing information about a notification message.

	// OnKeyDown parameters
	UINT nChar;     // [in/out] Specifies the virtual key code of the given key.
	UINT nRepCnt;   // [in] Repeat count.
	UINT nFlags;    // [in] Specifies the scan code, key-transition code, previous key state, and context code.

	BOOL bCancel;   // [out] TRUE to cancel processing key, FALSE to continue.
};


//===========================================================================
// Summary:
//     Enumeration of operational mouse modes.
// Remarks:
//     ReportControl has several
//     Mouse states that handled by control. This enumeration helps to
//     clearly identify each of these
//         - Sends Notifications:
//         - Sends Messages:
// See Also: CXTPReportControl::SetMouseMode, CXTPReportControl::GetMouseMode
//
// <KEYWORDS xtpReportMouseNothing, xtpReportMouseOverColumnDivide, xtpReportMousePrepareDragColumn, xtpReportMouseDraggingColumn>
//===========================================================================
enum XTPReportMouseMode
{
	xtpReportMouseNothing,            // User is just watching to the list.
	xtpReportMouseOverColumnDivide,   // User watching, but mouse is under column divider.
	xtpReportMousePrepareDragColumn,  // User holds mouse button pressed on the column header.
	xtpReportMouseDraggingColumn      // User is dragging column header.
};


//-----------------------------------------------------------------------
// Summary:
//     Enumeration of drag and drop flags
// Remarks:
//     Call CXTPReportControl::EnableDragDrop to allow drag/drop operations
// Example:
//     <code>m_wndReport.EnableDragDrop("MyApplication", xtpReportAllowDrop | xtpReportAllowDrag);</code>
// See Also: CXTPReportControl::EnableDragDrop
//-----------------------------------------------------------------------
enum  XTPReportDragDrop
{
	xtpReportAllowDrop     = 1,         // Allow Drop records to report
	xtpReportAllowDragCopy = 2,         // Allow copy records from report
	xtpReportAllowDragMove = 4,         // Allow move records from report
	xtpReportAllowDrag     = 6          // Allow copy and move records from report
};


//===========================================================================
// Summary:
//     The CXTPReportControl class provides an implementation of
//     the Report control.
//
// Remarks:
//     A "report control" is a window that displays a hierarchical list
//     of items, such as emails in the inbox. Each item is called a CXTPReportRow
//     and consists of its properties and corresponding CXTPReportRecord,
//     which contains all the corresponding data (mostly text).
//     Each Row item (as well as Record) can have a list of sub-items
//     associated with it. By clicking a Row item, the user can expand and
//     collapse the associated list of sub-items.
//
//     The CXTPReportRecords collection holds all the CXTPReportRecord objects
//     that are assigned to the Report control. It could be accessible via
//     GetRecords() method. The records in this collection
//     are referred to as the root records. Any record that is subsequently
//     added to a root record is referred to as a child record. Because each
//     CXTPReportRecord can contain a collection of other CXTPReportRecord
//     objects, you might find it difficult to determine your location in the
//     tree structure when you iterate through the collection.
//
//     Record nodes can be expanded to display the next level of child records.
//     The user can expand the CXTPReportRecord by clicking the plus-sign (+)
//     button, if one is displayed, or you can expand the CXTPReportRecord by
//     calling the CXTPReportRecord::SetExpanded method.
//     To expand all the child records, call the ExpandAll method.
//     You can collapse the child CXTPReportRecord level by calling the
//     CXTPReportRecord::SetExpanded(FALSE) method, or the user can press
//     the minus-sign (-) button, if one is displayed. You can also call
//     CollapseAll method to collapse all child records.
//
//     Each record contains an array of record items which are implemented
//     with CXTPReportRecordItem and its descendants. You can create your own
//     types of items simply by inheritance from the base record item class.
//
//     Each record item has an association with corresponding CXTPReportColumn
//     item. The item will be shown below the corresponding column header
//     depending on its position in report control columns array. If a column
//     has not an associated item in the record, there will be an empty item
//     shown in the corresponding cell.
//
//     Columns array is represented by CXTPReportColumns collection and could
//     be accessed via GetColumns() method.
//
//     As a finalization of adding data to the report control, which means
//     adding columns and records, Populate() method should be called. It
//     performs population of control rows with data - creates a rows tree if
//     necessary, rebuilds groups if grouping if enabled, and sorts rows
//     on a specified manner. See Also an example below.
//
//     Handling notification messages sent by the control to the parent
//     window is allowed with ON_NOTIFY handler. The control is using
//     SendMessageToParent function to send notifications. See below for
//     the example of how messages could be handled in a parent window:
//
// <code>
// ON_NOTIFY(NM_CLICK, ID_REPORT_CONTROL, OnReportItemClick)
// ON_NOTIFY(NM_RCLICK, ID_REPORT_CONTROL, OnReportItemRClick)
// ON_NOTIFY(NM_DBLCLK, ID_REPORT_CONTROL, OnReportItemDblClick)
// ON_NOTIFY(XTP_NM_SHOWFIELDCHOOSER, ID_REPORT_CONTROL, OnShowFieldChooser)
// ON_NOTIFY(XTP_NM_HEADER_RCLICK, ID_REPORT_CONTROL, OnReportColumnRClick)
// ON_NOTIFY(NM_KEYDOWN, ID_REPORT_CONTROL, OnReportKeyDown)
// </code>
//
//     You can also change the appearance of the CXTPReportControl control
//     by setting some of its display and style properties.
//
//     Also Report control has an ability to store and restore its
//     settings, which includes all columns with their settings, and some
//     required control's settings. It is implemented via standard MFC and XTP
//     serialization and available with the member functions
//     SerializeState(CArchive& ar), DoPropExchange(CXTPPropExchange* pPX);
//
//     Report control supports Copy/Paste clipboard operations.
//     See methods: CanCut(), CanCopy(), CanPaste(), Cut(), Copy(), Paste().
//     There are 2 clipboard formats are supported:
//          Binary  - contains all record(s) data;
//          Text    - contains visible columns texts.
//
//     To support binary format the XTP serialization is used -
//     DoPropExchange() methods are implemented for CXTPReportRecord,
//     CXTPReportRecordItem and derived CXTPReportRecordItemXXX classes.
//     Also some part of standard MFC serialization is used
//     (see DECLARE_SERIAL macro) to automatically create classes when
//     loading from the data source.
//
//     If you are creating custom records and records items classes you have
//     to use DECLARE_SERIAL macro and may need to override DoPropExchange()
//     methods to serialize custom data as well as standard records data.
//
//     The storing records way is simple: CXTPReportRecord (or derived class)
//     is stored first, then record items (CXTPReportRecordItemXXX classes)
//     are stored one by one.
//     The class information, which allow to create object instances when
//     loading, is stored for all classes. See CArchive::WriteClass(),
//     CArchive::ReadClass() and other CArchive members for more details
//     about this idea.
//
//     When report control loads records from the data source:
//     The record class is created automatically (using stored
//     class information).
//     Then items collection cleared and record items are created
//     automatically and added to items collection.
//     For example see ReportSample project: CMessageRecord class.
//
//     We support text format with '\t' dividers for record items and
//     "\r\n" dividers for records (simple tab-separated text).
//     Such format is also supported by Excel and some other applications.
//
//     There are few methods and corresponding notifications which allow to
//     customize copy/paste operations:
//      OnBeforeCopyToText(); OnBeforePasteFromText(); OnBeforePaste();
//
// See Also:
//     CXTPReportView, CXTPReportHeader, CXTPReportRow, CXTPReportRecord,
//     CXTPReportColumn,
//     CXTPReportRecords, CXTPReportRows, CXTPReportColumns,
//     CXTPReportSubListControl, CXTPReportFilterEditControl
//===========================================================================
class _XTP_EXT_CLASS CXTPReportControl : public CWnd
{
	DECLARE_DYNCREATE(CXTPReportControl)

	friend class CXTPReportSubListControl;
	friend class CXTPReportRow;
	friend class CXTPReportGroupRow;
	friend class CXTPReportHeader;
	friend class CXTPReportNavigator;
	friend class CXTPReportInplaceEdit;
	friend class CXTPReportHeaderDragWnd;
	friend class CXTPReportInplaceList;
	class CReportDropTarget;

public:

	//===========================================================================
	// Summary:
	//     Internal report update helper.
	//===========================================================================
	class _XTP_EXT_CLASS CUpdateContext
	{
	public:

		//-----------------------------------------------------------------------
		// Summary:
		//     Constructs a CXTPReportControlUpdateContext object.
		// Parameters:
		//     pControl - Pointer to a Report Control object.
		//-----------------------------------------------------------------------
		CUpdateContext(CXTPReportControl* pControl)
		{
			m_pControl = pControl;
			pControl->BeginUpdate();
		}

		//-------------------------------------------------------------------------
		// Summary:
		//     Destroys a CXTPReportControlUpdateContext object, handles cleanup and deallocation.
		//-------------------------------------------------------------------------
		~CUpdateContext()
		{
			m_pControl->EndUpdate();
		}
	protected:
		CXTPReportControl* m_pControl;          // Updated report control pointer
	};

public:
	//-------------------------------------------------------------------------
	// Summary:
	//     Call this function to use custom heap feature.
	//
	// Remarks:
	//     If it is enabled, report data will be allocated in custom (separate)
	//     heap instead of standard application heap. This optimize memory
	//     using (fragmentation) for big amount of data.
	//     For custom heap used for classes derived from CXTPHeapObjectT template,
	//     like CXTPReportRecord, CXTPReportRecordItem, CXTPReportRow and so others.
	//     This template just overrides operators new and delete.
	//     <P>It must be called on initialization before any allocations of classes
	//     which use custom heap. OnInitInstance is a fine place for this.
	// Returns:
	//     TRUE if custom heap feature is enabled for all report allocators,
	//     FALSE otherwise.
	//-------------------------------------------------------------------------
	static BOOL AFX_CDECL UseReportCustomHeap();

	//-------------------------------------------------------------------------
	// Summary:
	//     Call this function to enable batch allocation feature for report rows.
	//
	// Remarks:
	//      Batch allocation means that memory allocated not for one object only,
	//      but for many objects at one time (for 1024 objects by default).
	//      Next allocations take memory from this big block. New blocks allocated
	//      when necessary. This increase performance and reduce heap fragmentation.
	//      Batch allocation mechanism responsible for allocation/deallocation
	//      blocks of memory from heap and internally organize free/busy lists of
	//      memory pieces. When object deleted, its memory stored in free list and
	//      used for new objects.
	//      When all memory pieces from block free, it may be deallocated from
	//      heap automatically (this depends on options in _TBatchAllocData)
	//      or by FreeExtraData call,
	//      <P>It must be called on initialization before any allocations of classes
	//      which use batch allocation. OnInitInstance is a fine place for this.
	// Returns:
	//     TRUE if batch allocation feature is enabled for report rows,
	//     FALSE otherwise.
	//-------------------------------------------------------------------------
	static BOOL AFX_CDECL UseRowBatchAllocation();

	//-----------------------------------------------------------------------
	// Summary:
	//      This member function when rows batch allocation enabled to check
	//      all allocated blocks and deallocate which are completely free.
	// See Also:
	//      UseRowBatchAllocation, CXTPBatchAllocObjT
	//-----------------------------------------------------------------------
	static void AFX_CDECL FreeRowBatchExtraData();

public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPReportControl object.
	// Remarks:
	//     You construct a CXTPReportControl object in two steps.
	//     First, call the constructor CXTPReportControl and then call
	//     Create method, which initializes the window.
	//
	// Example:
	// <code>
	// // Declare a local CXTPReportControl object.
	// CXTPReportControl myReport;
	//
	// // Declare a dynamic CXTPReportControl object.
	// CXTPReportControl* pMyReport = new CXTPReportControl();
	//
	// if (!myReport.Create(WS_CHILD | WS_TABSTOP | WS_VISIBLE | WM_VSCROLL, CRect(0, 0, 0, 0), this, ID_REPORT_CONTROL))
	// {
	//     TRACE(_T("Failed to create view window\n"));
	// }
	// </code>
	// See Also: Create
	//-----------------------------------------------------------------------
	CXTPReportControl();

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPReportControl object, handles cleanup and deallocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPReportControl();

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to create a report control.
	// Parameters:
	//     dwStyle     - Specifies the window style attributes.
	//                   It could be a combination of standard window styles.
	//     rect        - The size and position of the window, in client
	//                   coordinates of pParentWnd.
	//     pParentWnd  - Specifies the report control parent window.
	//     nID         - Specifies the report control identifier.
	//     pContext    - The create context of the window.
	// Remarks:
	//     You construct a CXTPReportControl object in two steps.
	//     First, call the constructor CXTPReportControl and then call
	//     Create method, which initializes the window.
	// Example:
	//     See the example for CXTPReportControl::CXTPReportControl
	// Returns:
	//     Nonzero if successful; otherwise 0.
	// See Also: CXTPReportControl::CXTPReportControl
	//-----------------------------------------------------------------------
	BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);

	//-----------------------------------------------------------------------
	// Summary:
	//     (Re)Stores control configuration to/from the provided archive stream.
	// Parameters:
	//     ar - Archive stream for serializing.
	//-----------------------------------------------------------------------
	virtual void SerializeState(CArchive& ar);

	//-----------------------------------------------------------------------
	// Summary:
	//     Reads or writes configuration to/from the provided data source.
	// Parameters:
	//     pPX - A CXTPPropExchange object to serialize to or from.
	//----------------------------------------------------------------------
	virtual void DoPropExchange(CXTPPropExchange* pPX);

public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Adds new column to the end of the columns array.
	// Parameters:
	//     pColumn - Pointer to the specified column class object.
	// Remarks:
	//     Call this member function if you want to add a new column
	//     to a report control. It will be added to the end of the columns array.
	// Example:
	// <code>
	// // this function adds a column with "Subject" caption and 250 pixels initial width
	// void AddSubjectColumn(CXTPReportControl* pReportCtrl)
	// {
	//     pReportCtrl->AddColumn(new CXTPReportColumn(1, _T("Subject"), 250));
	// }
	// </code>
	// See Also: CXTPReportColumn overview
	//-----------------------------------------------------------------------
	CXTPReportColumn* AddColumn(CXTPReportColumn* pColumn);

	//-----------------------------------------------------------------------
	// Summary:
	//     Adds record to records collection.
	// Parameters:
	//     pRecord - Record data with items.
	// Remarks:
	//     Call this member function if you want to add a data record to the
	//     report control's internal storage.
	// Example:
	// <code>
	// // this function adds 2 empty records to a report control
	// void Add2Empties(CXTPReportControl* pReportCtrl)
	// {
	//     pReportCtrl->AddRecord(new CXTPReportRecord());
	//     pReportCtrl->AddRecord(new CXTPReportRecord());
	// }
	// </code>
	// Returns:
	//     Pointer to the recently added record object.
	// See Also: CXTPReportRecord overview, GetRecords
	//-----------------------------------------------------------------------
	virtual CXTPReportRecord* AddRecord(CXTPReportRecord* pRecord);

	//-----------------------------------------------------------------------
	// Summary:
	//     Removes record with data and associated row. It also remove all
	//     children records and their rows.
	// Parameters:
	//     pRecord       - Pointer to a record object.
	//     bAdjustLayout - If TRUE AdjustLayout will be called.
	// Remarks:
	//     Call this member function if you want to remove record on the fly,
	//     without Populate call.
	// Returns:
	//     TRUE if operation succeeded, FALSE otherwise.
	// See Also: RemoveRowEx, CXTPReportRecords::RemoveRecord
	//-----------------------------------------------------------------------
	virtual BOOL RemoveRecordEx(CXTPReportRecord* pRecord, BOOL bAdjustLayout = TRUE);

	//-----------------------------------------------------------------------
	// Summary:
	//     Removes row and associated record. It also remove all
	//     children records and rows.
	// Parameters:
	//     pRow          - Pointer to a row object.
	//     bAdjustLayout - If TRUE AdjustLayout will be called.
	// Remarks:
	//     Call this member function if you want to remove row and record on the fly,
	//     without Populate call.
	// Returns:
	//     TRUE if operation succeeded, FALSE otherwise.
	// See Also: RemoveRowEx, CXTPReportRecords::RemoveRecord
	//-----------------------------------------------------------------------
	virtual BOOL RemoveRowEx(CXTPReportRow* pRow, BOOL bAdjustLayout = TRUE);

	//-----------------------------------------------------------------------
	// Summary:
	//     Prevents the control from redrawing until the EndUpdate
	//     method is called.
	// Remarks:
	//     If you want to add items one at a time using the AddRecord method,
	//     or to make some another operations in a single sequence,
	//     you can use the BeginUpdate method to prevent the control
	//     from repainting the CXTPReportControl each time an item is added.
	//     Once you have completed the task of adding items to the control,
	//     call the EndUpdate method to enable the CXTPReportControl to repaint.
	//     This way of adding items can prevent flickered drawing of
	//     the CXTPReportControl when a large number of items are being
	//     added to the control.
	// Example:
	// <code>
	// // This function collapses all rows for the specified report control
	// void CollapseAll(CXTPReportControl* pReportCtrl)
	// {
	//     pReportCtrl->BeginUpdate();
	//     for (int i = pReportCtrl->GetRows()->GetCount() - 1; i >= 0; i --)
	//         pReportCtrl->GetRows()->GetAt(i)->SetExpanded(FALSE);
	//
	//     pReportCtrl->EndUpdate();
	// }
	// </code>
	// See Also: EndUpdate, RedrawControl, AddRecord
	//-----------------------------------------------------------------------
	void BeginUpdate();

	//-----------------------------------------------------------------------
	// Summary:
	//     This method deletes all rows and records from Report Control.
	// Parameters:
	//     bUpdateControl - Set TRUE to redraw control, otherwise FALSE.
	//-----------------------------------------------------------------------
	void ResetContent(BOOL bUpdateControl = TRUE);

	//-----------------------------------------------------------------------
	// Summary:
	//     Resumes drawing of the report control after drawing is
	//     suspended by the BeginUpdate method.
	// Remarks:
	//     If you want to add items one at a time using the AddRecord method,
	//     or to make some other operations in a single sequence,
	//     you can use the BeginUpdate method to prevent the control
	//     from repainting the CXTPReportControl each time an item is added.
	//     Once you have completed the task of adding items to the control,
	//     call the EndUpdate method to enable the CXTPReportControl to repaint.
	//     This way of adding items can prevent flickered drawing of
	//     the CXTPReportControl when a large number of items are being
	//     added to the control.
	// Example:  See example for CXTPReportControl::BeginUpdate method.
	// See Also: BeginUpdate, RedrawControl
	//-----------------------------------------------------------------------
	void EndUpdate();

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns TRUE if control in Update state.
	//-----------------------------------------------------------------------
	int GetLockUpdateCount() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Initiates report control redrawing.
	// Remarks:
	//     Call this member function if you want to initialize the
	//     report control redrawing. The control will be redrawn taking
	//     into account its latest state.
	// See Also: BeginUpdate, EndUpdate
	//-----------------------------------------------------------------------
	void RedrawControl();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to get tooltip context pointer.
	//-----------------------------------------------------------------------
	CXTPToolTipContext* GetToolTipContext() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to enable built in drag and drop operations
	// Parameters:
	//     lpszClipboardFormat - Name of clipboard format to be used for Report Control
	//     dwFlags - Combination of XTPReportDragDrop flags, can be one or more of following:
	//        <b>xtpReportAllowDrop</b> - Allow Drop records to report
	//        <b>xtpReportAllowDragCopy</b> - Allow copy records from report
	//        <b>xtpReportAllowDragMove</b> - Allow move records from report
	//        <b>xtpReportAllowDrag</b> - Allow copy and move records from report
	// Returns:
	//     Clipboard format that will be used with Report Control
	// See Also: XTPReportDragDrop
	//-----------------------------------------------------------------------
	CLIPFORMAT EnableDragDrop(LPCTSTR lpszClipboardFormat, DWORD dwFlags);

	//-----------------------------------------------------------------------
	// Summary:
	//     Set or clear grid drawing style.
	// Parameters:
	//     bVertical - TRUE for changing vertical grid style,
	//                 FALSE for changing horizontal grid style.
	//     gridStyle - New grid style. Can be any of the values listed in the Remarks section.
	// Remarks:
	//     Call this member function if you want to change a style
	//     of report grid lines.
	//
	//     Possible grid line styles are the following:
	//     * <b>xtpReportGridNoLines</b>  Empty line
	//     * <b>xtpReportGridSmallDots</b> Line is drawn with small dots
	//     * <b>xtpReportGridLargeDots</b> Line is drawn with large dots
	//     * <b>xtpReportGridDashes</b> Line is drawn with dashes
	//     * <b>xtpReportGridSolid</b> Draws solid line
	//
	// See Also: XTPReportGridStyle overview, GetGridStyle, SetGridColor
	//-----------------------------------------------------------------------
	void SetGridStyle(BOOL bVertical, XTPReportGridStyle gridStyle);

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns current grid drawing mode.
	// Parameters:
	//     bVertical - TRUE for vertical grid style,
	//                 FALSE for horizontal grid style.
	// Remarks:
	//     Call this member function if you want to retrieve current
	//     grid lines drawing style for the report control.
	// Returns:
	//     Current grid drawing style.
	// See Also: XTPReportGridStyle overview, SetGridStyle, SetGridColor
	//-----------------------------------------------------------------------
	XTPReportGridStyle GetGridStyle(BOOL bVertical) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Change color of GridLines.
	// Parameters:
	//     clrGridLine - New Grid Lines color.
	// Remarks:
	//     Call this member function if you want to change a color of
	//     report control grid lines.
	// Returns:
	//     Old Grid Lines color.
	// See Also: SetGridStyle, GetGridStyle
	//-----------------------------------------------------------------------
	COLORREF SetGridColor(COLORREF clrGridLine);

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns the collection of the data records.
	// Returns:
	//     The data records collection.
	// Remarks:
	//     Call this member function if you want to retrieve an access
	//     to the collection of report records. You may then perform
	//     standard operations on the collection like adding, removing, etc.
	// See Also: CXTPReportRecords overview, AddRecord
	//-----------------------------------------------------------------------
	CXTPReportRecords* GetRecords() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns the collection of the report rows
	// Remarks:
	//     Use this member function to retrieve an access to the collection
	//     of report rows, representing current control view.
	//
	//     Note that rows collection could be rebuilt automatically
	//     on executing Populate method.
	// Returns:
	//     The report rows collection.
	// Example:
	//     See example for CXTPReportControl::BeginUpdate method.
	// See Also: CXTPReportRows overview, Populate
	//-----------------------------------------------------------------------
	CXTPReportRows* GetRows() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Interface for accessing list columns.
	// Remarks:
	//     Use this member function to retrieve an access to the collection
	//     of report columns.
	// Returns:
	//     The report Columns collection.
	// Example:
	// <code>
	// // this function adds a column with "Subject" caption and 250 pixels initial width
	// void AddSubjectColumn(CXTPReportControl* pReportCtrl)
	// {
	//     pReportCtrl->GetColumns()->Add(new CXTPReportColumn(1, _T("Subject"), 250));
	// }
	// </code>
	// See Also: CXTPReportColumns overview
	//-----------------------------------------------------------------------
	CXTPReportColumns* GetColumns() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns the currently used control's Paint Manager.
	// Remarks:
	//     Call this member function to get the paint manager object used
	//     for drawing a report control window.
	// Returns:
	//     Pointer to the paint manager object.
	// See Also: CXTPReportPaintManager overview
	//-----------------------------------------------------------------------
	CXTPReportPaintManager* GetPaintManager() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to set custom paint manager.
	// Parameters:
	//     pPaintManager - A pointer to the custom paint manager.
	//-----------------------------------------------------------------------
	void SetPaintManager(CXTPReportPaintManager* pPaintManager);

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns the control's Navigator.
	// Remarks:
	//     Call this member function to get the navigator object used
	//     for cell navigation.
	// Returns:
	//     Pointer to the navigator object.
	// See Also: CXTPReportNavigator overview
	//-----------------------------------------------------------------------
	CXTPReportNavigator* GetNavigator() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Performs control population, creating view from the data.
	// Remarks:
	//     Call this member function to populate control's Rows collection
	//     with the data containing in Records collection.
	//     It automatically creates Tree View references if necessary
	//     (for example, in Grouping mode).
	//     , it is the main function which should be called for
	//     (re)populating all data changes that you have made into the
	//     Records collection.
	// See Also: CXTPReportPaintManager overview
	//-----------------------------------------------------------------------
	virtual void Populate();

	//-----------------------------------------------------------------------
	// Summary:
	//     Ensures that a report control row is at least partially visible.
	// Parameters:
	//     pCheckRow - A pointer to the row that is to be visible.
	// Remarks:
	//     Ensures that a report row item is at least partially visible.
	//     The list view control is scrolled if necessary.
	// See Also: MoveDown, MoveUp, MovePageDown, MovePageUp, MoveFirst, MoveLast
	//-----------------------------------------------------------------------
	void EnsureVisible(CXTPReportRow* pCheckRow);

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines which report row item, if any, is at a specified position.
	// Parameters:
	//     pt - A point to test.
	// Returns:
	//     The row item at the position specified by pt, if any,
	//     or NULL otherwise.
	//-----------------------------------------------------------------------
	CXTPReportRow* HitTest(CPoint pt) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns the collection of the selected rows.
	// Remarks:
	//     Use this member function to retrieve an access to the collection
	//     of currently selected report rows.
	// Returns:
	//     The selected rows collection.
	// See Also: CXTPReportSelectedRows overview.
	//-----------------------------------------------------------------------
	CXTPReportSelectedRows* GetSelectedRows() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves the index of the row that currently has focus
	//     in the report control's view.
	// Returns:
	//     Returns pointer to the focused row, or NULL otherwise.
	// See Also: SetFocusedRow
	//-----------------------------------------------------------------------
	CXTPReportRow* GetFocusedRow() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Makes the provided row as currently focused.
	// Parameters:
	//     pRow             - The row to set as focused to.
	//     bIgnoreSelection - TRUE if select new focused row.
	//     bSelectBlock     - TRUE when selecting rows up to new focused row,
	//                        FALSE otherwise.
	// See Also: GetFocusedRow
	//-----------------------------------------------------------------------
	void SetFocusedRow(CXTPReportRow* pRow, BOOL bIgnoreSelection = FALSE);
	void SetFocusedRow(CXTPReportRow* pRow, BOOL bSelectBlock, BOOL bIgnoreSelection);// <COMBINE CXTPReportControl::SetFocusedRow@CXTPReportRow*@BOOL>

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns rows from the visible report area.
	// Parameters:
	//     nStartRow - Row index to start calculating from.
	//     bMoveDown - Rows moving direction.
	// Remarks:
	//     Call this member function if you want to calculate the amount
	//     of currently visible rows between nStartRow and the top/bottom
	//     of the current view.
	// Returns:
	//     Visible row index starting from.
	// See Also: MovePageDown, MovePageUp
	//-----------------------------------------------------------------------
	int GetReportAreaRows(int nStartRow, BOOL bMoveDown);

	//-----------------------------------------------------------------------
	// Summary:
	//     Force provided row to the top.
	// Parameters:
	//     nIndex - An index of the row to force.
	// Remarks:
	//     The system scrolls the report control view until either the
	//     item specified by nIndex appears at the top of the view
	//     the maximum scroll range has been reached.
	// See Also: GetReportAreaRows, EnsureVisible
	//-----------------------------------------------------------------------
	void SetTopRow(int nIndex);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to retrieve top row index.
	// Returns:
	//     Top row index.
	// See Also: SetTopRow
	//-----------------------------------------------------------------------
	long GetTopRowIndex() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to set horizontal scroll position.
	// Parameters:
	//     nOffset - Horizontal scroll position.
	// Remarks:
	//     This method takes effect only if auto column sizing is disable
	// See Also: CXTPReportHeader::SetAutoColumnSizing
	//-----------------------------------------------------------------------
	void SetLeftOffset(int nOffset);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to set right-to-left mode.
	// Parameters:
	//     bRightToLeft - TRUE to set right-to-left reading-order properties.
	//-----------------------------------------------------------------------
	void SetLayoutRTL(BOOL bRightToLeft);

	//-----------------------------------------------------------------------
	// Summary:
	//     Enable/disable preview mode for the control.
	// Parameters:
	//     bIsPreviewMode - TRUE for enabling preview mode,
	//                      FALSE for disabling.
	// Remarks:
	//     Call this member function if you want to hide or show the
	//     row preview item.
	// See Also: IsPreviewMode
	//-----------------------------------------------------------------------
	void EnablePreviewMode(BOOL bIsPreviewMode);

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns current preview mode.
	// Remarks:
	//     Call this member function if you want to determine whether the
	//     row preview item is shown or not.
	// Returns:
	//     TRUE when preview mode is enabled,
	//     FALSE when preview mode is disabled.
	// See Also: EnablePreviewMode
	//-----------------------------------------------------------------------
	BOOL IsPreviewMode() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Shows/hides Group By control area.
	// Parameters:
	//     bEnable - TRUE for showing GroupBy area,
	//               FALSE for hiding GroupBy area.
	// Remarks:
	//     Call this member function if you want to hide or show
	//     Group By area in the report header.
	// See Also: IsGroupByVisible
	//-----------------------------------------------------------------------
	void ShowGroupBy(BOOL bEnable = TRUE);

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns current Group By area mode.
	// Remarks:
	//     Call this member function if you want to determine whether
	//     Group By area in the report header is visible or not.
	// Returns:
	//     TRUE when Group By area is shown on the control,
	//     when Group By area is hidden.
	// See Also: ShowGroupBy
	//-----------------------------------------------------------------------
	BOOL IsGroupByVisible() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to hide/show column headers.
	// Parameters:
	//     bShow - TRUE is column headers will be displayed, FALSE to hide column headers.
	// See Also: IsHeaderVisible, ShowFooter
	//-----------------------------------------------------------------------
	void ShowHeader(BOOL bShow = TRUE);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to determine is column headers are currently visible.
	// Returns:
	//     TRUE is column headers are visible, FALSE is column headers are hidden.
	// See Also: ShowHeader
	//-----------------------------------------------------------------------
	BOOL IsHeaderVisible() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to hide/show report footers.
	// Parameters:
	//     bShow - TRUE is column footers will be displayed, FALSE to hide column footers.
	// See Also: IsFooterVisible, ShowHeader
	//-----------------------------------------------------------------------
	void ShowFooter(BOOL bShow = TRUE);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to determine is column footers are currently visible.
	// Returns:
	//     TRUE is column footers are visible, FALSE is column footers are hidden.
	// See Also: ShowHeader
	//-----------------------------------------------------------------------
	BOOL IsFooterVisible() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Enable/disable group shade.
	// Parameters:
	//     bEnable - TRUE for enabling group items shade,
	//               FALSE for disabling.
	// Remarks:
	//     Call this member function if you want to hide or show
	//     group rows' headings.
	// See Also:
	//     IsShadeGroupHeadingsEnabled, IsGroupRowsBold, SetGroupRowsBold,
	//     GetItemMetrics
	//-----------------------------------------------------------------------
	void ShadeGroupHeadings(BOOL bEnable = TRUE);

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns current group shade mode.
	// Remarks:
	//     Call this member function if you want to determine whether
	//     group rows' headings are enabled or not.
	// Returns:
	//     TRUE when group items shading is enabled,
	//     FALSE when it is disabled.
	// See Also:
	//     ShadeGroupHeadings, IsGroupRowsBold, SetGroupRowsBold,
	//     GetItemMetrics
	//-----------------------------------------------------------------------
	BOOL IsShadeGroupHeadingsEnabled() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Set group rows text style: bold or normal.
	// Parameters:
	//     bBold - TRUE for bold text style, FALSE for normal.
	// Remarks:
	//     Call this member function if you want to change group rows
	//     bold text style.
	// See Also:
	//     IsGroupRowsBold, ShadeGroupHeadings, IsShadeGroupHeadingsEnabled,
	//     GetItemMetrics
	//-----------------------------------------------------------------------
	void SetGroupRowsBold(BOOL bBold = TRUE);

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns current group rows text bold style.
	// Remarks:
	//     Call this member function if you want to determine whether
	//     group rows' text are bold or normal.
	// Returns:
	//     TRUE when group text are bold,
	//     FALSE when it is normal.
	// See Also:
	//     SetGroupRowsBold, ShadeGroupHeadings, IsShadeGroupHeadingsEnabled,
	//     GetItemMetrics
	//-----------------------------------------------------------------------
	BOOL IsGroupRowsBold() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to retrieve count of none-scrolled columns
	//     at the left side.
	// Returns:
	//     Count of none-scrolled columns
	//     at the left side.
	//-----------------------------------------------------------------------
	int GetFreezeColumnsCount() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to set count of none-scrolled columns
	//     at the left side.
	// Parameters:
	//     nCount - Count of none-scrolled columns at the left side.
	//-----------------------------------------------------------------------
	void SetFreezeColumnsCount(int nCount);

	//-----------------------------------------------------------------------
	// Summary:
	//      Get horizontal scrolling mode.
	// Returns:
	//      Returns TRUE if horizontal scrolling is by full columns,
	//      otherwise horizontal scrolling is by pixels.
	// See Also: SetFullColumnScrolling
	//-----------------------------------------------------------------------
	BOOL IsFullColumnScrolling() const;

	//-----------------------------------------------------------------------
	// Summary:
	//      Set horizontal scrolling mode.
	// Parameters:
	//      bSet - If TRUE full columns scrolling mode is set,
	//             if FALSE horizontal scrolling by pixels mode is set.
	// See Also: IsFullColumnScrolling
	//-----------------------------------------------------------------------
	void SetFullColumnScrolling(BOOL bSet);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to remove all items and column in specified position.
	// Parameters:
	//     nIndex - Index of item to remove.
	//-----------------------------------------------------------------------
	void ReleaseItem(int nIndex);

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to get a pointer to the image manager of Report Control.
	// Returns:
	//     Pointer to the image manager of Report Control.
	// See Also: SetImageManager
	//-----------------------------------------------------------------------
	CXTPImageManager* GetImageManager() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to set new image manager.
	// Parameters:
	//     pImageManager - Image manager to be set
	// Example:
	// <code>
	// CXTPImageManager* pImageManager = new CXTPImageManager();
	// pImageManager->SetIcons(IDR_MAINFRAME);
	// m_wndReport.SetImageManager(pImageManager);
	// </code>
	// See Also: GetImageManager
	//-----------------------------------------------------------------------
	void SetImageManager(CXTPImageManager* pImageManager);

	//-----------------------------------------------------------------------
	// Summary:
	//     Initiates ImageList of Paint Manager.
	// Parameters:
	//     pImageList - Image list.
	// Remarks:
	//     You use this function to set up your own ImageList
	//     with set bitmaps that represent various states of rows
	//     and depict any other information.
	// Note:
	//     Recommended to use SetImageManager/GetImageManager methods instead.
	// Example:
	// <code>
	// CImageList listIcons;
	// listIcons.Create(16, 16, ILC_COLOR24 | ILC_MASK, 0, 1));
	// CBitmap bmp;
	// // load bitmap by id
	// bmp.LoadBitmap(IDB_BMREPORT);
	// ilIcons.Add(&bmp, RGB(255, 0, 255));
	// m_wndReport.SetImageList(&lIcons);
	// </code>
	// See Also: GetImageManager
	//-----------------------------------------------------------------------
	void SetImageList(CImageList* pImageList);

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Returns a pointer to the associated report header object.
	// Remarks:
	//     Call this member function if you want to retrieve access
	//     to the report header object properties and methods.
	// Returns:
	//     A pointer to the associated report header.
	// See Also: CXTPReportHeader overview
	//-----------------------------------------------------------------------
	CXTPReportHeader* GetReportHeader() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to change report header of Report Control
	// Parameters:
	//     pReportHeader - Report header to be set
	// Example:
	//     <code>m_wndReport.SetReportHeader(new CMyReportHeader());</code>
	// See Also: CXTPReportHeader overview
	//-----------------------------------------------------------------------
	void SetReportHeader(CXTPReportHeader* pReportHeader);

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns indentation for the tree view row of the specified depth level.
	// Parameters:
	//     nLevel - Tree depth level.
	// Remarks:
	//     Calculates row indentation in pixels based on the provided
	//     indentation level.
	// Returns:
	//     Row indentation in pixels.
	// See Also: Populate
	//-----------------------------------------------------------------------
	int GetIndent(int nLevel) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Notifies parent control of some event that has happened.
	// Parameters:
	//     pRow       - Specified row of event if used.
	//     pItem      - Specified item of event if used.
	//     pColumn    - Specified column of event if used.
	//     nMessage   - A message to sent to parent window.
	//     pPoint     - A point where the message was sent from in
	//                        client coordinates.
	//     nHyperlink - Hyperlink order number, where the message was sent
	//                  from (-1 if message was not send from the hyperlink).
	// Remarks:
	//     Sends a message to the parent in the form of a WM_NOTIFY message
	//     with a specific structure attached.
	// Returns:
	//     The result of the message processing;
	//     its value depends on the message sent. (see CWnd::SendMessage)
	// See Also: CXTPReportControl overview, SendNotifyMessage
	//-----------------------------------------------------------------------
	LRESULT SendMessageToParent(CXTPReportRow* pRow, CXTPReportRecordItem* pItem, CXTPReportColumn* pColumn, UINT nMessage, CPoint* pPoint, int nHyperlink = -1) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Sends the specified message to the window.
	// Parameters:
	//     nMessage - The message to be sent.
	//     pNMHDR - Notify header
	// Returns:
	//     Nonzero if successful; otherwise returns zero.
	//-----------------------------------------------------------------------
	LRESULT SendNotifyMessage(UINT nMessage, NMHDR* pNMHDR = NULL) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Collapses all rows.
	// Remarks:
	//     The CollapseAll method collapses all the CXTPReportRow objects,
	//     including all the child rows, that are in the report control.
	// See Also: ExpandAll, CXTPReportRow::SetExpanded
	//-----------------------------------------------------------------------
	void CollapseAll();

	//-----------------------------------------------------------------------
	// Summary:
	//     Expands all rows.
	// Remarks:
	//     The ExpandAll method expands all the CXTPReportRow objects,
	//     including all the child rows, that are in the report control.
	// See Also: CollapseAll, CXTPReportRow::SetExpanded
	//-----------------------------------------------------------------------
	void ExpandAll();

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines if the report allows multiple selections.
	// Returns:
	//     TRUE if the report allows multiple selections.
	// See Also: SetMultipleSelection, GetSelectedRows
	//-----------------------------------------------------------------------
	BOOL IsMultipleSelection() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Changes allowing multiple selections for the control.
	// Parameters:
	//     bMultipleSelection - TRUE for enabling, FALSE for disabling.
	// Remarks:
	//     Sets the flag that determines whether the report allows multiple selections.
	// See Also: IsMultipleSelection, GetSelectedRows
	//-----------------------------------------------------------------------
	void SetMultipleSelection(BOOL bMultipleSelection);

	//-----------------------------------------------------------------------
	// Summary:
	//     Enables/disables showing tooltips for the control.
	// Parameters:
	//     bEnable - TRUE for enabling, FALSE for disabling.
	// Remarks:
	//     Call this member function to enable or disable tooltips
	//     show for the report control window.
	//-----------------------------------------------------------------------
	void EnableToolTips(BOOL bEnable);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to specify whether groups are skipped when navigating
	//     the ReportControl with the Up and Down keys.
	// Parameters:
	//     bSkipFocus - If TRUE, when navigating the rows with the Up and Down
	//                  keys in the ReportControl the group headings will be skipped
	//                  and the next non-group heading row will be selected.
	//                  If FALSE, when navigating the rows with the Up and Down
	//                  keys in the ReportControlall rows will be selected,
	//                  even group headings.
	//-----------------------------------------------------------------------
	void SkipGroupsFocus(BOOL bSkipFocus);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to determine is groups are skipped when navigating the
	//     with the Up and Down arrow keys.
	// Returns:
	//     TRUE if groups are skipped, FALSE is groups receive focus when navigating
	//     the report control with the Up and Down arrow keys.
	//-----------------------------------------------------------------------
	BOOL IsSkipGroupsFocusEnabled() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to retrieve indentation of the header
	// Returns:
	//     Header indent.
	//-----------------------------------------------------------------------
	int GetHeaderIndent() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to retrieve Report bounding rectangle.
	// Returns:
	//     Report bounding rectangle.
	//-----------------------------------------------------------------------
	CRect GetReportRectangle() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Adjusts scroll bars depending on currently visible rows.
	//-----------------------------------------------------------------------
	virtual void AdjustScrollBars();

	//-----------------------------------------------------------------------
	// Summary:
	//     Adjusts main control areas depending on current control size.
	//-----------------------------------------------------------------------
	void AdjustLayout();

public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to retrieve a pointer to the currently selected column.
	// Returns:
	//     A pointer to the currently selected item.
	//-----------------------------------------------------------------------
	CXTPReportColumn* GetFocusedColumn() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to set selected column.
	// Parameters:
	//     pColumn - column to be selected.
	//-----------------------------------------------------------------------
	void SetFocusedColumn(CXTPReportColumn* pColumn);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to specify whether each individual CXTPReportRecordItem
	//     will show focus when the item in a row is clicked.
	// Parameters:
	//     bFocusSubItems - If TRUE, when a ReportRecordItem is clicked, the
	//                      entire row will become highlighted except the individual
	//                      item that was clicked.
	//                      If FALSE, the entire row will become highlighted when
	//                      an item is clicked, including the item that was clicked.
	// See Also: IsFocusSubItems
	//-----------------------------------------------------------------------
	void FocusSubItems(BOOL bFocusSubItems);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to determine if individual CXTPReportRecordItem(s) will
	//     receive focus when that item in the CXTPReportRow that they belong to is clicked.
	// Returns:
	//     TRUE if individual items can receive focus, FALSE if only the entire row can receive focus.
	// See Also: FocusSubItems
	//-----------------------------------------------------------------------
	BOOL IsFocusSubItems() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to determine if the CXTPReportRecordItem(s)are editable.
	// Returns:
	//     TRUE is the CXTPReportRecordItem(s) are editable, FALSE otherwise.
	// See Also: AllowEdit, EditOnClick, IsEditOnClick
	//-----------------------------------------------------------------------
	BOOL IsAllowEdit() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to allow the text in all CXTPReportRecordItem(s) to be edited.  This will
	//     add an edit box to the item where the text can be edited.
	// Parameters:
	//     bAllowEdit - TRUE to add an edit box to the CXTPReportRecordItem(s) so they are editable.
	//                  FALSE to remove the edit box and not allow them to be edited.
	// See Also: IsAllowEdit, EditOnClick, IsEditOnClick
	//-----------------------------------------------------------------------
	void AllowEdit(BOOL bAllowEdit);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to specify whether a CXTPReportRecordItem can be edited by
	//     single-clicking on the item.
	// Parameters:
	//     bEditOnClick - If TRUE, when the CXTPReportRecordItem is single-clicked,
	//                    the item will become editable.  The entire ReportControl
	//                    or the specific CXTPReportRecordItem must have the bAllowEdit
	//                    property set to TRUE for this to work.
	//                    If FALSE, the item must be double-clicked to become editable.
	// See Also: AllowEdit, IsAllowEdit, IsEditOnClick
	//-----------------------------------------------------------------------
	void EditOnClick(BOOL bEditOnClick);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to determine whether items are edited with a single-click.
	// Returns:
	//     TRUE it items can be edited with a single-click, FALSE is a
	//     double-click is needed to edit items.
	// See Also: AllowEdit, IsAllowEdit, EditOnClick
	//-----------------------------------------------------------------------
	BOOL IsEditOnClick() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to enable auto check mode for check box items.
	// Parameters:
	//     bAutoCheck - TRUE to enable auto check mode, FALSE to disable auto check mode.
	// Remarks:
	//     When TRUE, the check box will become checked or unchecked automatically when the
	//     user clicks on the check box.
	// See Also: IsAutoCheckItems, CXTPReportRecordItem::OnClick, CXTPReportRecordItem::OnChar
	//-----------------------------------------------------------------------
	void SetAutoCheckItems(BOOL bAutoCheck);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to determine id auto check mode for check box items is enabled.
	// Remarks:
	//     If TRUE is returned, the check box will become checked or unchecked automatically when the
	//     user clicks on the check box.
	// Returns:
	//      TRUE if auto check mode is enabled, FALSE if auto check mode is disable.
	// See Also: SetAutoCheckItems, CXTPReportRecordItem::OnClick, CXTPReportRecordItem::OnChar
	//-----------------------------------------------------------------------
	BOOL IsAutoCheckItems() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to start editing a report cell.
	//     It makes a specified cell focused and starts in-place editing
	//     control which was specified there.
	// Remarks:
	//     Note that all editing options should be enabled for this method
	//     to be executed successfully.
	// Parameters:
	//     pItemArgs - Arguments of item to be edit.
	// Remarks:
	//     Call this method with NULL as parameter to stop item edit.
	// See Also: AllowEdit, CXTPReportColumn::SetEditable
	//-----------------------------------------------------------------------
	void EditItem(XTP_REPORTRECORDITEM_ARGS* pItemArgs);

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves in-place edit pointer
	// Returns:in-place
	//     Pointer to in-place edit control
	//-----------------------------------------------------------------------
	CXTPReportInplaceEdit* GetInplaceEdit() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves in-place buttons of report control
	// Returns:
	//     Collection of in-place buttons.
	//-----------------------------------------------------------------------
	CXTPReportInplaceButtons* GetInplaceButtons() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves in-place list of report controls
	// Returns:
	//     Pointer to in-place list.
	//-----------------------------------------------------------------------
	CXTPReportInplaceList* GetInplaceList() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to determine if the ReportControl has focus.
	// Returns:
	//     TRUE if the ReportControl has focus, FALSE otherwise.
	//-----------------------------------------------------------------------
	BOOL HasFocus() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to retrieve the currently active item, which
	//     is the item that has focus.
	// Returns:
	//     A pointer to the currently active(focused) CXTPReportRecordItem.
	//-----------------------------------------------------------------------
	CXTPReportRecordItem* GetActiveItem() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to specifies the indentation placed before the
	//     text of each child node in a hierarchical tree structure.
	// Parameters:
	//     nIndent - Indentation used when displaying child nodes.
	//-----------------------------------------------------------------------
	void SetTreeIndent(int nIndent);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to enable virtual mode of the control
	// Parameters:
	//     pVirtualRecord - record to be used as virtual for all rows.
	//     nCount - Count of virtual records.
	// Example:
	// <code>
	// class CVirtualRecord : public CXTPReportRecord
	// {
	// public:
	//     CVirtualRecord()
	//     {
	//         AddItem(new CXTPReportRecordItem());
	//         AddItem(new CXTPReportRecordItem());
	//         AddItem(new CXTPReportRecordItem());
	//     }
	//
	//     void GetItemMetrics (XTP_REPORTRECORDITEM_DRAWARGS* pDrawArgs, XTP_REPORTRECORDITEM_METRICS* pItemMetrics)
	//     {
	//         // Draw virtual record
	//     }
	// }
	// ...
	// m_wndReport.SetVirtualMode(new CVirtualRecord(), 540);
	// </code>
	// See Also: IsVirtualMode
	//-----------------------------------------------------------------------
	void SetVirtualMode(CXTPReportRecord* pVirtualRecord, int nCount);

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines if control in virtual mode
	// Returns;
	//     TRUE if virtual mode is enabled; FALSE otherwise.
	// See Also: IsVirtualMode
	//-----------------------------------------------------------------------
	BOOL IsVirtualMode() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns the text from the associated filter edit control.
	// Returns:
	//     Text string entered by user inside filter edit control.
	// See Also: SetFilterText
	//-----------------------------------------------------------------------
	virtual CString GetFilterText();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to set new filter for control.
	// Parameters:
	//     strFilterText - Filter text to be applied for control.
	// Remarks:
	//     You must call Populate method to update rows.
	// See Also: GetFilterText, Populate
	//-----------------------------------------------------------------------
	void SetFilterText(LPCTSTR strFilterText);

	//-----------------------------------------------------------------------
	// Summary:
	//     Register the window class if it has not already been registered.
	// Parameters:
	//     hInstance - Instance of resource where control is located
	// Returns:
	//     TRUE if the window class was successfully registered.
	//-----------------------------------------------------------------------
	BOOL RegisterWindowClass(HINSTANCE hInstance = NULL);

	//-----------------------------------------------------------------------
	// Summary:
	//      Returns TRUE if one or more records are selected indicating the
	//  Cut command can be used.
	// See Also:
	//      CanPaste, Cut, Copy, Paste.
	//-----------------------------------------------------------------------
	virtual BOOL CanCut();

	//-----------------------------------------------------------------------
	// Summary:
	//      Returns TRUE if one or more records are selected indicating the
	//  Copy command can be used.
	// See Also:
	//      CanPaste, Cut, Copy, Paste.
	//-----------------------------------------------------------------------
	virtual BOOL CanCopy();

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns TRUE if BinaryRecords or Text data exists in the clipboard.
	// See Also:
	//      CanCut, CanCopy, Cut, Copy, Paste.
	//-----------------------------------------------------------------------
	virtual BOOL CanPaste();

	//-----------------------------------------------------------------------
	// Summary:
	//      Copy selected records data (in the Binary and Text data formats) to
	//      the clipboard and delete records.
	// See Also:
	//      CanCut, CanCopy, CanPaste, Copy, Paste,
	//      OnBeforeCopyToText, OnBeforePasteFromText, OnBeforePaste.
	//-----------------------------------------------------------------------
	virtual void Cut();

	//-----------------------------------------------------------------------
	// Summary:
	//      Copy selected records data (in the Binary and Text data formats) to
	//      the clipboard.
	// See Also:
	//      CanCut, CanCopy, CanPaste, Cut, Paste,
	//      OnBeforeCopyToText, OnBeforePasteFromText, OnBeforePaste.
	//-----------------------------------------------------------------------
	virtual void Copy();

	//-----------------------------------------------------------------------
	// Summary:
	//      Reads records from the clipboard and add them to records collection.
	//      The Binary data format is used rather than Text format.
	// See Also:
	//      CanCut, CanCopy, CanPaste, Cut, Copy,
	//      OnBeforeCopyToText, OnBeforePasteFromText, OnBeforePaste.
	//-----------------------------------------------------------------------
	virtual void Paste();

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns TRUE if inplace edit control is active (visible and focused),
	//     FALSE otherwise.
	// See Also:
	//      GetInplaceEdit.
	//-----------------------------------------------------------------------
	virtual BOOL IsEditMode();

	//-----------------------------------------------------------------------
	// Summary:
	//     Set compare function to sort.
	// Parameters:
	//     pCompareFunc - A T_CompareFunc function pointer that is used
	//                    to compare rows or NULL to use the default one.
	// Remarks:
	//     This method uses Visual C++ run-time library (MSVCRT)
	//     implementation of the quick-sort function, qsort, for sorting
	//     stored CXTPReportRow objects.
	//     If pCompareFunc = NULL the default compare function is used.
	//     Call Populate() method to resort items.
	//
	// See Also:
	//     SortRows, CXTPReportRows::SortEx, CXTPReportRows::Sort,
	//     CXTPReportRows::T_CompareFunc
	//-----------------------------------------------------------------------
	virtual void SetRowsCompareFunc(CXTPReportRows::T_CompareFunc pCompareFunc);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to apply sort order for record children
	// Parameters:
	//     bSortRecordChilds - TRUE to sort record children.
	//-----------------------------------------------------------------------
	void SetSortRecordChilds(BOOL bSortRecordChilds);

	//-----------------------------------------------------------------------
	// Summary:
	//      Use this function to get horizontal scrolling step.
	// Returns:
	//      Horizontal scrolling step in pixels.
	//-----------------------------------------------------------------------
	virtual int GetHScrollStep();

	//-----------------------------------------------------------------------
	// Summary:
	//      Use this function to set horizontal scrolling step.
	// Parameters:
	//      nStep - Horizontal scrolling step in pixels.
	//-----------------------------------------------------------------------
	virtual void SetHScrollStep(int nStep);

protected:
	//-----------------------------------------------------------------------
	// Summary:
	//     Sets new operational control mouse mode.
	// Parameters:
	//     nMode - New mouse mode. For available values, see XTPReportMouseMode enum.
	// See Also: XTPReportMouseMode overview, GetMouseMode
	//-----------------------------------------------------------------------
	void SetMouseMode(XTPReportMouseMode nMode);

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns the current control mouse mode.
	// Returns:
	//     Current control mouse mode.
	//     For available values, see XTPReportMouseMode enum.
	// See Also: XTPReportMouseMode overview, SetMouseMode
	//-----------------------------------------------------------------------
	XTPReportMouseMode GetMouseMode() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Performs all drawing logic.
	// Parameters:
	//     pDC - Provided DC to draw control image with.
	//-----------------------------------------------------------------------
	virtual void OnDraw(CDC* pDC);

	//-----------------------------------------------------------------------
	// Summary:
	//     Draws all rows on the provided DC.
	// Parameters:
	//     pDC - Provided DC to draw rows image with.
	//     rcClient - A rectangle to draw rows image into.
	//-----------------------------------------------------------------------
	void DrawRows(CDC* pDC, CRect& rcClient);

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns changed status flag of the control. Used for caching control image drawing.
	// Returns:
	//     TRUE if the internal control state was changed since last drawing, FALSE otherwise.
	//-----------------------------------------------------------------------
	BOOL IsChanged() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets new changed status flag for the control. Used for caching control image drawing.
	// Parameters:
	//     bChanged - TRUE when something was changed in the control contents and control needs to be republished.
	//-----------------------------------------------------------------------
	void SetChanged(BOOL bChanged = TRUE);

	//-----------------------------------------------------------------------
	// Summary:
	//     Sorts rows corresponding to the sort order taken from columns.
	// Parameters:
	//     pRows - A rows collection to sort.
	//-----------------------------------------------------------------------
	virtual void SortRows(CXTPReportRows* pRows);

	//-----------------------------------------------------------------------
	// Summary:
	//     Update field chooser control with its content.
	//-----------------------------------------------------------------------
	void UpdateSubList();

	//-----------------------------------------------------------------------
	// Summary:
	//     Builds rows tree based on provided data record.
	// Parameters:
	//     pTree      - Rows tree to add items into.
	//     pParentRow - Parent tree row.
	//     pRecords   - Records collection for transferring to rows.
	// Remarks:
	//     Builds rows tree based on provided data record.
	//     Recursively calls itself when build nested branches of rows
	//-----------------------------------------------------------------------
	void BuildTree(CXTPReportRows* pTree, CXTPReportRow* pParentRow, CXTPReportRecords* pRecords);

	//-----------------------------------------------------------------------
	// Summary:
	//     Collapses all children of the specified row.
	// Parameters:
	//     pRow - A row to collapse.
	//-----------------------------------------------------------------------
	void _DoCollapse(CXTPReportRow* pRow);

	//-----------------------------------------------------------------------
	// Summary:
	//     Expands all children of the specified row.
	// Parameters:
	//     nIndex - An index to insert rows from.
	//     pRow   - A row to expand.
	// Returns:
	//     A count of the newly added rows or void.
	//-----------------------------------------------------------------------
	int _DoExpand(int nIndex, CXTPReportRow* pRow);
	void _DoExpand(CXTPReportRow* pRow); // <COMBINE CXTPReportControl::_DoExpand@int@CXTPReportRow*>

	//-----------------------------------------------------------------------
	// Summary:
	//     Inserts the specified row at the specified position.
	// Parameters:
	//     nIndex - A position to insert row at.
	//     pRow   - A row to insert.
	// Remarks:
	//     Inserts the specified row to the rows array at the specified position
	//     with all its children expanded. Use _DoExpand() to expand
	//     all child items
	// Returns:
	//     A number of the inserted rows.
	//-----------------------------------------------------------------------
	int InsertRow(int nIndex, CXTPReportRow* pRow);

	//-----------------------------------------------------------------------
	// Summary:
	//     Recalculates indexes of all rows in the specified collection.
	//-----------------------------------------------------------------------
	void _RefreshIndexes(BOOL bAdjustLayout = TRUE);

	//-----------------------------------------------------------------------
	// Summary:
	//     Checks a record for filter text.
	// Parameters:
	//     pRecord         - A record to apply filter to.
	//     strFilterText   - Filter string text.
	//     bIncludePreview - Include preview item in filtering or not.
	// Remarks:
	//     This member function represents filter functionality. First, it parses
	//     the input text of a filter string by tokens; second it enumerates all visible
	//     columns to find text matching to the filter string. Returns TRUE if
	//     matching found, FALSE otherwise
	// Returns:
	//     TRUE if record is filtered with the specified filter,
	//     FALSE otherwise.
	//-----------------------------------------------------------------------
	virtual BOOL ApplyFilter(CXTPReportRecord* pRecord, CString strFilterText, BOOL bIncludePreview);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is called when a user selects a row
	// Remarks:
	//     When user selects row ReportControl, it sends notification to the
	//     parent window by calling OnSelectionChanged()
	// See Also: XTP_NM_REPORT_SELCHANGED, SendNotifyMessage()
	//-----------------------------------------------------------------------
	virtual void OnSelectionChanged();

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns drawing metrics for the row.
	// Parameters:
	//     pDrawArgs    - Pointer to the provided draw arguments structure for calculating metrics.
	//     pItemMetrics - Pointer to the metrics structure to fill.
	//-----------------------------------------------------------------------
	virtual void GetItemMetrics(XTP_REPORTRECORDITEM_DRAWARGS* pDrawArgs, XTP_REPORTRECORDITEM_METRICS* pItemMetrics);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to create new CXTPReportRow object. Overwrite it
	//     to use derived CXTPReportRow class.
	// Returns:
	//     New CXTPReportRow object.
	//-----------------------------------------------------------------------
	virtual CXTPReportRow* CreateRow();

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to create new CXTPReportGroupRow object. Overwrite it
	//     to use derived CXTPReportGroupRow class.
	// Returns:
	//     New CXTPReportGroupRow object.
	//-----------------------------------------------------------------------
	virtual CXTPReportGroupRow* CreateGroupRow();

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called when user start drag row
	// Parameters:
	//     point - Drag point
	// Remarks:
	//     Use can override this method or catch LVN_BEGINDRAG message to proceed drag
	//     operations.
	//-----------------------------------------------------------------------
	virtual void OnBeginDrag(CPoint point);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called before add record text data to the clipboard.
	// Parameters:
	//     pRecord      - [in] A pointer to source record;
	//     rarStrings   - [in/out] Reference to strings array with record items
	//                    values. By default this array contains visible items
	//                    captions.
	// Remarks:
	//     If you would like to customize text data for the clipboard you can
	//     change rarStrings as you need.
	//
	//     Default implementation sends XTP_NM_REPORT_BEFORE_COPY_TOTEXT
	//     notification.
	// Returns:
	//     TRUE to cancel copying this record to the clipboard in text format,
	//     FALSE to continue.
	// See Also:
	//     OnBeforePasteFromText, OnBeforePaste, Cut, Copy, Paste.
	//-----------------------------------------------------------------------
	virtual BOOL OnBeforeCopyToText(CXTPReportRecord* pRecord, CStringArray& rarStrings);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called before creating new record using text data
	//     from the clipboard.
	// Parameters:
	//     arStrings    - [in] Strings array with record items values.
	//     ppRecord     - [out] A pointer to new record pointer;
	// Remarks:
	//     If you would like to customize creating new record from text data
	//     you have to create new record object with record items and fill
	//     them using default values and strings provided in arStrings parameter.
	//
	//     If new record will not be set to ppRecord parameter, control will
	//     create CXTPReportRecord object with
	//     CXTPReportRecordItemVariant items and fill visible items using
	//     strings provided in arStrings parameter.
	//
	//     Default implementation sends XTP_NM_REPORT_BEFORE_PASTE_FROMTEXT
	//     notification.
	// Returns:
	//     TRUE to cancel adding this record, FALSE to continue.
	// See Also:
	//     OnBeforeCopyToText, OnBeforePaste, Cut, Copy, Paste.
	//-----------------------------------------------------------------------
	virtual BOOL OnBeforePasteFromText(CStringArray& arStrings, CXTPReportRecord** ppRecord);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called after creating new record using binary data
	//     from the clipboard, but before add it to records collection.
	// Parameters:
	//     ppRecord     - [in/out] A pointer to new record pointer;
	// Remarks:
	//     If you would like to customize new record created from binary data
	//     you may to create new or change provided record using
	//     ppRecord parameter.
	//
	//     Default implementation sends XTP_NM_REPORT_BEFORE_PASTE
	//     notification.
	// Returns:
	//     TRUE to cancel adding this record, FALSE to continue.
	// See Also:
	//     OnBeforeCopyToText, OnBeforePasteFromText, Cut, Copy, Paste.
	//-----------------------------------------------------------------------
	virtual BOOL OnBeforePaste(CXTPReportRecord** ppRecord);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called before processing OnKeyDown event.
	// Parameters:
	//     rnChar     - [in/out] A reference to variable which specifies
	//                  the virtual key code of the given key. For a list of
	//                  of standard virtual key codes, see Winuser.h.
	//     nRepCnt    - [in] Repeat count (the number of times the keystroke
	//                  is repeated as a result of the user holding down the key).
	//     nFlags     - [in] Specifies the scan code, key-transition code,
	//                  previous key state, and context code.
	// Remarks:
	//     If you would like to customize keyboard behavior you can change
	//     rnChar parameter value or perform your logic and return FALSE to
	//     disable default processing.
	//
	//     Default implementation sends XTP_NM_REPORT_PREVIEWKEYDOWN
	//     notification.
	// Returns:
	//     TRUE to continue processing key, FALSE to cancel.
	// See Also:
	//     OnKeyDown, CWnd::OnKeyDown, XTP_NM_REPORT_PREVIEWKEYDOWN.
	//-----------------------------------------------------------------------
	virtual BOOL OnPreviewKeyDown(UINT& rnChar, UINT nRepCnt, UINT nFlags);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is used to enable monitoring mouse position for automatic
	//     vertical scrolling.
	// Remarks:
	//     Used when dragging records.
	// See Also:
	//     EnsureStopAutoVertScroll, DoAutoVertScrollIfNeed
	//-----------------------------------------------------------------------
	virtual void EnsureStartAutoVertScroll();

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is used to stop monitoring mouse position for automatic
	//     vertical scrolling.
	// Remarks:
	//     Used when dragging records.
	// See Also:
	//     EnsureStartAutoVertScroll, DoAutoVertScrollIfNeed
	//-----------------------------------------------------------------------
	virtual void EnsureStopAutoVertScroll();

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is used to monitoring mouse position. If mouse
	//     will be moved to 20 pixels before top or before bottom report borders
	//     the control will be scrolled vertically.
	// Parameters:
	//     ptClick - Coordinates of initial click (or mouse down).
	//     pt      - Current mouse position.
	// Remarks:
	//     Used when dragging records.
	// See Also:
	//     EnsureStartAutoVertScroll, EnsureStopAutoVertScroll
	//-----------------------------------------------------------------------
	virtual void DoAutoVertScrollIfNeed(CPoint ptClick, CPoint pt);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is used to notify about changing constraint selection.
	// Parameters:
	//     pRow         - A pointer to current row object;
	//     pItem        - A pointer to current record item object;
	//     pColumn      - A pointer to current column object;
	//     pConstraint  - A pointer to hot selected constraint;
	// Remarks:
	//     Send for every selection change before new selection applied or canceled.
	// Returns:
	//     TRUE if notification successfully sent, FALSE otherwise.
	//-----------------------------------------------------------------------
	virtual BOOL OnConstraintSelecting(CXTPReportRow* pRow, CXTPReportRecordItem* pItem, CXTPReportColumn* pColumn,
									   CXTPReportRecordItemConstraint* pConstraint);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is used to notify about changing constraint selection.
	// Parameters:
	//     pRow             - A pointer to current row object;
	//     pItem            - A pointer to current record item object;
	//     rstrToolTipText  - A reference to CString object to customize tool tip
	//                        text. It contains default tool tip text;
	// Remarks:
	//     If rstrToolTipText set to empty - the default tool tip will be shown
	//     when not enough space to draw item text.
	//     If rstrToolTipText set to single space (" ") no tool tip will be shown.
	// Returns:
	//     A reference to XTP_NM_REPORTTOOLTIPINFO structure.
	//-----------------------------------------------------------------------
	virtual const XTP_NM_REPORTTOOLTIPINFO& OnGetToolTipInfo(CXTPReportRow* pRow, CXTPReportRecordItem* pItem, CString& rstrToolTipText);

protected:
//{{AFX_CODEJOCK_PRIVATE
	virtual CString _GetSelectedRowsVisibleColsText();
	BOOL _GetSelectedRows(CXTPReportRecords* pRecords, CXTPInternalCollectionT<CXTPReportRow> * pRows = NULL);
	void _SelectRows(CXTPReportRecords* pRecords);
	virtual BOOL _ReadRecodsFromText(LPCTSTR pcszText, CXTPReportRecords& rarRecords);
	virtual CXTPReportRecord* _CreateRecodFromText(LPCTSTR pcszRecord);

	virtual BOOL _WriteSelectedRowsData(CXTPPropExchange* pPX);
	virtual BOOL _ReadRecodsFromData(CXTPPropExchange* pPX, CXTPReportRecords& rarRecords);
	BOOL _WriteRecordsData(CXTPPropExchange* pPX, CXTPReportRecords* pRecords);
	virtual void DrawDropMarker(CDC* pDC);
//}}AFX_CODEJOCK_PRIVATE

protected:
	//-----------------------------------------------------------------------
	// Summary:
	//     Adjust control indentation properties depending on current tree depth.
	// See Also: GetIndent
	//-----------------------------------------------------------------------
	void AdjustIndentation();

protected:
	//-----------------------------------------------------------------------
	// Summary:
	//     This member is called when an item is dragged over the report control.
	// Parameters:
	//     pDataObject - Points to the COleDataObject being dragged over the
	//                   drop target.
	//     dwKeyState  - State of keys on keyboard.  Contains the state of the
	//                   modifier keys. This is a combination of any number of
	//                   the following: MK_CONTROL, MK_SHIFT, MK_ALT, MK_LBUTTON,
	//                   MK_MBUTTON, and MK_RBUTTON.
	//     point - The current mouse position relative to the report control.
	// Returns:
	//     A value from the DROPEFFECT enumerated type, which indicates the type
	//     of drop that would occur if the user dropped the object at this
	//     position. The type of drop often depends on the current key state as
	//     indicated by dwKeyState. A standard mapping of keystates to DROPEFFECT
	//     values is:
	//       * <b>DROPEFFECT_NONE</b> The data object cannot be dropped in this
	//                                window.
	//       * <b>DROPEFFECT_COPY</b> for <b>MK_CONTROL</b>  Creates a copy of
	//                                                       the dropped object.
	//       * <b>DROPEFFECT_MOVE</b> for <b>MK_ALT</b> Creates a copy of the dropped
	//                                                  object and delete the original object.
	//                                                  This is typically the default drop effect,
	//                                                  when the view can accept the data object.
	//-----------------------------------------------------------------------
	virtual DROPEFFECT OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member is called when an item has been dropped into the report control.
	// Parameters:
	//     pDataObject - Points to the COleDataObject that is dropped into the drop target.
	//     dropEffect  - The drop effect that the user has requested. Can be any of the values listed in the Remarks section.
	//     point - The current mouse position relative to the report control.
	// Remarks:
	//     The <i>dropEffect</i> parameter can be one of the following values:
	//     * <b>DROPEFFECT_COPY</b> Creates a copy of the data object being dropped.
	//     * <b>DROPEFFECT_MOVE</b> Moves the data object to the current mouse location.
	// Returns:
	//     TRUE if the drop was successful, otherwise FALSE.
	//-----------------------------------------------------------------------
	virtual BOOL OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point);


protected:
//{{AFX_CODEJOCK_PRIVATE
	DECLARE_MESSAGE_MAP()

	//{{AFX_VIRTUAL(CXTPReportControl)
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	virtual INT_PTR OnToolHitTest(CPoint point, TOOLINFO* pTI) const;
	virtual CScrollBar* GetScrollBarCtrl(int nBar) const;
	//{{AFX_VIRTUAL(CXTPReportControl)

	//{{AFX_MSG(CXTPReportControl)
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint pos);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) ;
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint ptDblClick);
	afx_msg void OnCaptureChanged(CWnd* pWnd);
	afx_msg void OnSysColorChange();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus (CWnd* pNewWnd);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSysKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg UINT OnGetDlgCode();
	afx_msg void OnChar(UINT nChar, UINT nRepCntr, UINT nFlags);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseLeave();
	afx_msg void OnStyleChanged(int nStyleType, LPSTYLESTRUCT lpStyleStruct);
	afx_msg void OnEnable(BOOL bEnable);
	afx_msg void OnTimer(UINT_PTR uTimerID);
	//}}AFX_MSG
//}}AFX_CODEJOCK_PRIVATE

	//{{AFX_CODEJOCK_PRIVATE
	static UINT AFX_CDECL GetMouseScrollLines();

	virtual void RelayToolTipEvent(UINT message);
	virtual CXTPReportColumn* GetNextFocusableColumn(CXTPReportRow* pRow, int nColumnIndex, int nDirection);
	//}}AFX_CODEJOCK_PRIVATE
private:
	BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);

protected:
	BOOL m_bChanged;                // Internal member for storing changed flag.
	BOOL m_bRefreshIndexes;         // TRUE when it is required to refresh items indexes.

	int m_nLockUpdateCount;         // A counter of the update locks. An image will be redrawn only when the lock counter is equal to zero.

	CRect m_rcGroupByArea;          // The area occupied by Group By item.
	CRect m_rcHeaderArea;           // The area occupied by the header.
	CRect m_rcFooterArea;           // The area occupied by the footer.
	CRect m_rcReportArea;           // The area occupied by rows.

	CXTPReportRows* m_pRows;        // Virtual list rows container. Used for changing order, etc.
	CXTPReportRows* m_pPlainTree;   // Tree rows container.
	CXTPReportRecords* m_pRecords;  // List records container.
	CXTPReportColumns* m_pColumns;  // List columns container.

	CXTPReportPaintManager* m_pPaintManager;    // Paint manager.
	CXTPReportNavigator* m_pNavigator;          // Navigator
	int m_nTopRow;                              // Current top row in the visible area.
	int m_nLeftOffset;                          // Horizontal scroll position.
	int m_nFreezeColumnsCount;                  // Count of none-scrolled columns at the left side.

	int m_nHScrollStep;                         // Horizontal scroll step (in pixels).

	BOOL m_bFullColumnScrolling;                // Store Full Column Scrolling mode for horizontal scrolling.

	int m_nFocusedRow;                          // Pointer to the currently focused row.
	CXTPReportSelectedRows* m_pSelectedRows;    // Container for the currently selected rows.

	CXTPReportRow* m_pHotRow;                   // Hot row

	int m_nRowsPerWheel;                        // Amount of rows to scroll by mouse wheel.

	CXTPReportTip m_wndTip;                     // Tip window.
	CBitmap m_bmpCache;                         // Cached window bitmap.

	BOOL m_bGroupByEnabled;                     // TRUE if Group By box is enabled
	BOOL m_bSortRecordChilds;                   // TRUE to apply sort order for Record children.


	XTPReportMouseMode m_mouseMode;             // Current mouse operation mode
	BOOL m_bMultipleSelection;                  // TRUE if multiple selection enabled.
	BOOL m_bShowTooltips;                       // TRUE if showing tool tips enabled.
	BOOL m_bAutoCheckItems;                     // TRUE to enable auto check mode, FALSE to disable auto check mode.

	CXTPImageManager* m_pImageManager;          // Contains image list for report control
	BOOL m_bSkipGroupsFocus;                    // TRUE if group rows are skipped when navigating rows with the Up and Down arrow keys

	BOOL m_bFocusSubItems;                      // TRUE if sub-items can receive focus.
	BOOL m_bEditOnClick;                        // TRUE if sub-items become editable on a single-click
	BOOL m_bAllowEdit;                          // TRUE if sub-items can be edited.
	BOOL m_bHeaderVisible;                      // TRUE if column headers are visible.
	BOOL m_bFooterVisible;                      // TRUE if column footer are visible.
	CXTPReportColumn* m_pFocusedColumn;         // Pointer to the currently focused CXTPReportColumn.

	CXTPToolTipContext* m_pToolTipContext;      // Tool tip Context.
	CXTPReportRecordItem* m_pActiveItem;        // Pointer to the currently focused CXTPReportRecordItem.
	CXTPReportInplaceEdit* m_pInplaceEdit;      // In-place edit pointer
	CXTPReportInplaceButtons* m_pInplaceButtons;// In-place buttons pointer
	CXTPReportInplaceList* m_pInplaceList;      // In-place list pointer
	BOOL m_bVScrollBarVisible;                  // TRUE if vertical scroll bar is visible
	BOOL m_bHScrollBarVisible;                  // TRUE if horizontal scroll bar is visible
	CPoint m_pointDrag;                         // Drag position
	BOOL m_bPrepareDrag;                        // TRUE if user click the report control and doesn't release button.
	CXTPReportRows m_arrScreenRows;             // Rows currently presented on screen.
	CString m_strFilterText;                    // Filter text.
	CXTPReportHeader* m_pReportHeader;          // List header member.

	int m_nPopulatedRecordsCount;               // Current number of records in the report after using m_strFilterText.
	CLIPFORMAT m_cfReport;                      // Report Clipboard format for drag/drop operations

	CReportDropTarget* m_pDropTarget;           // Internal drag/drop helper.
	BOOL m_bDragMode;                           // TRUE if records currently dragging
	DWORD m_dwDragDropFlags;                    // Drag/drop flags.
	int m_nDropPos;                             // Position of records to be dropped
	CXTPReportRow* m_pSelectOnUpRow;            // Row to select on mouse up. Can be null.

	BOOL        m_bOnSizeRunning;               // TRUE if OnSize handler is entered, FALSE otherwise. Used to prevent OnSize reenter and stack overflow in Win95/98/ME.
	UINT_PTR    m_uAutoScrollTimerID;           // Auto scroll timer ID or 0.

	//{{AFX_CODEJOCK_PRIVATE
	long m_nOLEDropMode;                        // Store OLE drop mode.
	//}}AFX_CODEJOCK_PRIVATE

	CXTPReportRow* m_ptrVirtualEditingRow;      // Currently editing row in virtual mode.

	CXTPReportRows::T_CompareFunc m_pRowsCompareFunc;   // Pointer to rows compare function.

private:
	XTP_NM_REPORTTOOLTIPINFO* m_pCachedToolTipInfo;

	friend class CReportControlCtrl;
	friend class CReportDropTarget;
	friend class CXTPReportView;
};

//===========================================================================
// Summary:
//      Helper class with static member functions to control active locale for
//      Report Control and perform some locale dependent tasks.
//===========================================================================
class _XTP_EXT_CLASS CXTPReportControlLocale
{
private:
	CXTPReportControlLocale(){};
public:

	//-----------------------------------------------------------------------
	// Summary:
	//      Determine which locale is used active locale: current user locale
	//      or resource file locale.
	// Returns:
	//      TRUE if resource file locale is used active locale, FALSE otherwise.
	// See Also:
	//      SetUseResourceFileLocale, CXTPResourceManager::GetResourcesLangID,
	//      GetActiveLCID, LOCALE_USER_DEFAULT
	//-----------------------------------------------------------------------
	static BOOL AFX_CDECL IsUseResourceFileLocale();

	//-----------------------------------------------------------------------
	// Summary:
	//      Used to set which locale is used active locale: current user locale
	//      or resource file locale.
	// Parameters:
	//      bUseResourceFileLocale - If TRUE resource file locale will be used,
	//                               if FALSE current user locale will be used.
	// See Also:
	//      IsUseResourceFileLocale, CXTPResourceManager::GetResourcesLangID,
	//      GetActiveLCID, LOCALE_USER_DEFAULT
	//-----------------------------------------------------------------------
	static void AFX_CDECL SetUseResourceFileLocale(BOOL bUseResourceFileLocale);

	//-----------------------------------------------------------------------
	// Summary:
	//      Returns active locale ID (current user locale or resource file locale)
	// Returns:
	//      LOCALE_USER_DEFAULT or resource file locale ID.
	// See Also:
	//      IsUseResourceFileLocale, SetUseResourceFileLocale,
	//      CXTPResourceManager::GetResourcesLangID, LOCALE_USER_DEFAULT
	//-----------------------------------------------------------------------
	static LCID AFX_CDECL GetActiveLCID();

	//-----------------------------------------------------------------------
	// Summary:
	//      Use to change VARIANT type using active locale ID.
	// Parameters:
	//      rVarValue   - [in, out] Reference to VARIANT value to change type.
	//      vartype     - [in] new variant type.
	//      bThrowError - [in] if FALSE function returns TRUE or FALSE,
	//                         if TRUE function throw exception in case of error.
	// Returns:
	//      TRUE if successful, FALSE otherwise.
	// See Also:
	//      GetActiveLCID, ::VariantChangeTypeEx API function.
	//-----------------------------------------------------------------------
	static BOOL AFX_CDECL VariantChangeTypeEx(VARIANT& rVarValue, VARTYPE vartype, BOOL bThrowError = TRUE);

	//-----------------------------------------------------------------------
	// Summary:
	//      Format a string using strftime() function format specifiers.
	//      The active locale ID is used.
	// Parameters:
	//      dt                - [in] A COleDateTime object with date and time value to format.
	//      lpcszFormatString - [in] Format-control string.
	// Returns:
	//      A string which contains date-time formatted using active locale ID.
	// See Also:
	//      GetActiveLCID, strftime
	//-----------------------------------------------------------------------
	static CString AFX_CDECL FormatDateTime(const COleDateTime& dt, LPCTSTR lpcszFormatString);

private:
	static BOOL s_bUseResourceFileLocale;

private:
	static CString AFX_CDECL _FormatDateTime(const COleDateTime& dt, LPCTSTR lpcszFormatString, LCID lcLocaleID);

	static void AFX_CDECL _InitMappedSpecs();

	static void AFX_CDECL _ProcessMappedSpecs(CString& rstrFormat, const SYSTEMTIME* pST, LCID lcLocaleID);
	static void AFX_CDECL _ProcessDateTimeSpecs(CString& rstrFormat, const SYSTEMTIME* pST, LCID lcLocaleID);
	static void AFX_CDECL __ProcessDate_x(CString& rstrFormat, const SYSTEMTIME* pST, LCID lcLocaleID);
	static void AFX_CDECL __ProcessTime_X(CString& rstrFormat, const SYSTEMTIME* pST, LCID lcLocaleID);
	static void AFX_CDECL _ProcessOtherSpecs(CString& rstrFormat, const COleDateTime& dt);

private:
	struct XTP_TIMESPEC
	{
		LPCTSTR pcszSpec;
		LPCTSTR pcszFormat;
		BOOL    bTime;
	};

	static CArray<XTP_TIMESPEC, XTP_TIMESPEC&> s_arMappedSpecs;

	static void AFX_CDECL _AddsMappedSpec(LPCTSTR pcszSpec, LPCTSTR pcszFormat, BOOL bTime);
};

//////////////////////////////////////////////////////////////////////////

AFX_INLINE CXTPReportPaintManager* CXTPReportControl::GetPaintManager() const {
	return m_pPaintManager;
}
AFX_INLINE CXTPReportNavigator* CXTPReportControl::GetNavigator() const {
	return m_pNavigator;
}
AFX_INLINE CXTPReportSelectedRows* CXTPReportControl::GetSelectedRows() const{
	return m_pSelectedRows;
}
AFX_INLINE CXTPReportRows* CXTPReportControl::GetRows() const{
	return m_pRows;
}

AFX_INLINE CXTPReportRecords* CXTPReportControl::GetRecords() const{
	return m_pRecords;
}
AFX_INLINE CXTPReportColumns* CXTPReportControl::GetColumns() const{
	return m_pColumns;
}
AFX_INLINE XTPReportMouseMode CXTPReportControl::GetMouseMode() const {
	return m_mouseMode;
}
AFX_INLINE void CXTPReportControl::SetGridStyle(BOOL bVertical, XTPReportGridStyle gridStyle) {
	m_pPaintManager->SetGridStyle(bVertical, gridStyle);
	AdjustScrollBars();
}
AFX_INLINE XTPReportGridStyle CXTPReportControl::GetGridStyle(BOOL bVertical) const{
	return bVertical ? m_pPaintManager->m_verticalGridStyle : m_pPaintManager->m_horizontalGridStyle;
}
AFX_INLINE COLORREF CXTPReportControl::SetGridColor(COLORREF clrGridLine) {
	return m_pPaintManager->SetGridColor(clrGridLine);
}
AFX_INLINE void CXTPReportControl::EnablePreviewMode(BOOL bIsPreviewMode) {
	m_pPaintManager->EnablePreviewMode(bIsPreviewMode);
}
AFX_INLINE BOOL CXTPReportControl::IsPreviewMode() const{
	return m_pPaintManager->IsPreviewMode();
}
AFX_INLINE BOOL CXTPReportControl::IsChanged() const{
	return m_bChanged;
}
AFX_INLINE void CXTPReportControl::SetChanged(BOOL bChanged) {
	m_bChanged = bChanged;
}
AFX_INLINE void CXTPReportControl::ShowGroupBy(BOOL bEnable) {
	ASSERT(!IsVirtualMode());
	m_bGroupByEnabled = bEnable;
	AdjustLayout();
	AdjustScrollBars();
}
AFX_INLINE BOOL CXTPReportControl::IsGroupByVisible() const {
	return m_bGroupByEnabled;
}

AFX_INLINE void CXTPReportControl::ShowHeader(BOOL bShow /*= TRUE*/) {
	m_bHeaderVisible = bShow;
	AdjustLayout();
	AdjustScrollBars();
}

AFX_INLINE BOOL CXTPReportControl::IsHeaderVisible()const {
	return m_bHeaderVisible;
}

AFX_INLINE void CXTPReportControl::ShowFooter(BOOL bShow /*= TRUE*/) {
	m_bFooterVisible = bShow;
	AdjustLayout();
	AdjustScrollBars();
}

AFX_INLINE BOOL CXTPReportControl::IsFooterVisible()const {
	return m_bFooterVisible;
}

AFX_INLINE void CXTPReportControl::ShadeGroupHeadings(BOOL bEnable) {
	if (m_pPaintManager)
		m_pPaintManager->m_bShadeGroupHeadings = bEnable;
	AdjustScrollBars();
}

AFX_INLINE BOOL CXTPReportControl::IsShadeGroupHeadingsEnabled() const{
	return m_pPaintManager ? m_pPaintManager->m_bShadeGroupHeadings : FALSE;
}

AFX_INLINE void CXTPReportControl::SetGroupRowsBold(BOOL bBold) {
	if (m_pPaintManager)
		m_pPaintManager->m_bGroupRowTextBold = bBold;
}

AFX_INLINE BOOL CXTPReportControl::IsGroupRowsBold() const {
	return m_pPaintManager ? m_pPaintManager->m_bGroupRowTextBold : FALSE;
}

AFX_INLINE CRect CXTPReportControl::GetReportRectangle() const{
	return m_rcReportArea;
}
AFX_INLINE BOOL CXTPReportControl::IsMultipleSelection() const {
	return m_bMultipleSelection;
}
AFX_INLINE void CXTPReportControl::SetMultipleSelection(BOOL bMultipleSelection) {
	m_bMultipleSelection = bMultipleSelection;
	SetFocusedRow(GetFocusedRow());
}
AFX_INLINE void CXTPReportControl::EnableToolTips(BOOL bEnable) {
	m_bShowTooltips = bEnable;
}
AFX_INLINE void CXTPReportControl::SkipGroupsFocus(BOOL bSkipFocus) {
	m_bSkipGroupsFocus = bSkipFocus;
}
AFX_INLINE BOOL CXTPReportControl::IsSkipGroupsFocusEnabled()const {
	return m_bSkipGroupsFocus;
}
AFX_INLINE CXTPImageManager* CXTPReportControl::GetImageManager() const{
	return m_pImageManager;
}
AFX_INLINE CXTPReportColumn* CXTPReportControl::GetFocusedColumn() const{
	return m_pFocusedColumn;
}
AFX_INLINE void CXTPReportControl::FocusSubItems(BOOL bFocusSubItems) {
	m_bFocusSubItems = bFocusSubItems;
	m_pFocusedColumn = NULL;
}
AFX_INLINE BOOL CXTPReportControl::IsAllowEdit() const{
	return m_bAllowEdit;
}
AFX_INLINE CXTPReportInplaceEdit* CXTPReportControl::GetInplaceEdit() const{
	return m_pInplaceEdit;
}
AFX_INLINE CXTPReportInplaceButtons* CXTPReportControl::GetInplaceButtons() const{
	return m_pInplaceButtons;
}
AFX_INLINE CXTPReportInplaceList* CXTPReportControl::GetInplaceList() const{
	return m_pInplaceList;
}
AFX_INLINE CXTPReportRecordItem* CXTPReportControl::GetActiveItem() const{
	return m_pActiveItem;
}
AFX_INLINE void CXTPReportControl::AllowEdit(BOOL bAllowEdit) {
	m_bAllowEdit = bAllowEdit;
}
AFX_INLINE BOOL CXTPReportControl::IsFocusSubItems() const{
	return m_bFocusSubItems;
}
AFX_INLINE void CXTPReportControl::EditOnClick(BOOL bEditOnClick) {
	m_bEditOnClick = bEditOnClick;
}
AFX_INLINE BOOL CXTPReportControl::IsEditOnClick() const{
	return m_bEditOnClick;
}
AFX_INLINE BOOL CXTPReportControl::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) {
	return CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}
AFX_INLINE CString CXTPReportControl::GetFilterText() {
	return m_strFilterText;
}
AFX_INLINE void CXTPReportControl::SetFilterText(LPCTSTR strFilterText) {
	m_strFilterText = strFilterText;
}
AFX_INLINE CXTPReportHeader* CXTPReportControl::GetReportHeader() const {
	return m_pReportHeader;
}
AFX_INLINE long CXTPReportControl::GetTopRowIndex() const{
	return m_nTopRow;
}
AFX_INLINE void CXTPReportControl::SetAutoCheckItems(BOOL bAutoCheck) {
	m_bAutoCheckItems = bAutoCheck;
}
AFX_INLINE BOOL CXTPReportControl::IsAutoCheckItems() const {
	return m_bAutoCheckItems;
}

AFX_INLINE int CXTPReportControl::GetFreezeColumnsCount() const {
	return m_nFreezeColumnsCount;
}

AFX_INLINE void CXTPReportControl::SetFreezeColumnsCount(int nCount) {
	ASSERT(nCount >= 0);
	m_nFreezeColumnsCount = nCount;
}
AFX_INLINE int CXTPReportControl::GetLockUpdateCount() const {
	return m_nLockUpdateCount;
}

AFX_INLINE void CXTPReportControl::SetRowsCompareFunc(CXTPReportRows::T_CompareFunc pCompareFunc) {
	m_pRowsCompareFunc = pCompareFunc;
}

AFX_INLINE BOOL CXTPReportControl::IsFullColumnScrolling() const {
	return m_bFullColumnScrolling;
}

AFX_INLINE int CXTPReportControl::GetHScrollStep() {
	return m_nHScrollStep;
}

AFX_INLINE void CXTPReportControl::SetHScrollStep(int nStep) {
	ASSERT(nStep > 0);
	m_nHScrollStep = max(1, nStep);
}
AFX_INLINE void CXTPReportControl::SetSortRecordChilds(BOOL bSortRecordChilds) {
	m_bSortRecordChilds = bSortRecordChilds;
}


#endif //#if !defined(__XTPREPORTCONTROL_H__)
