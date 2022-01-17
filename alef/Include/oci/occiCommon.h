/* Copyright (c) 2000, 2004, Oracle. All rights reserved.  */
 
/* 
   NAME 
     occiCommon.h - header file for doing forward references

   DESCRIPTION 
     Just declare all the classes

   RELATED DOCUMENTS 
     <note any documents related to this facility>
 
   EXPORT FUNCTION(S) 
     <external functions declared for use outside package - one-line 
      descriptions>

   INTERNAL FUNCTION(S)
     <other external functions declared - one-line descriptions>

   EXAMPLES

   NOTES
     <other useful comments, qualifications, etc.>

   MODIFIED   (MM/DD/YY)
   cparampa    12/05/04 - Added BatchSQLException and its Impl
   cparampa    08/12/04 - Add OCCI_MAJOR_VERSION, OCCI_MINOR_VERSION
   shiyer      05/28/04 - Add LOB array operations 
   shiyer      04/01/04 - Add char_traits<short> impl for gcc 3.2.3 
   cparampa    09/03/03 - Added NotifyResult
   rvallam     02/12/03 - modified BFloat/BDouble interface - BFloat/BDouble
                          type is now a struct  
   shiyer      01/02/03 - add setVector(vector<UString>,string schema/type)
   rvallam     11/19/02 - added objects support for interval classes
   cparampa    12/11/02 - removed declaration for Payload
   srseshad    11/14/02 - Add OCCIIBFLOAT/OCCIIBDOUBLE
   cparampa    10/17/02 - prototypes for getVectorOf BDouble and BFloat taking
                          anydata.
   cparampa    10/09/02 - Declarations for AQ classes
   rvallam     10/12/02 - added new method for array pin - pinVectorOfRefs
   rvallam     10/10/02 - objects performance enhancement - modified
                          getObject and getVector<T *> to take the
                          schName and typeName of the object
   rvallam     10/11/02 - added native float/double API - get(set)vector
                          methods, added the enum values for BFLOAT and
                          BDOUBLE and typecodes for the same
   shiyer      07/31/02 - 10iR1, OCCI Globalization support
   cparampa    08/27/02 - Added forward reference for StatelessConnnectionPool
                          and StatelessConnectionPoolImpl  
   vvinay      06/25/02 - fix #2415021:const of string changed
   aahluwal    06/03/02 - bug 2360115
   gayyappa    12/27/01 - fix bug 2073482 - remove include <map> 
   gayyappa    10/29/01 - removed CharSet - bug 1853748 
   gayyappa    09/21/01 - #include <map>
   rratnam     06/18/01 - fixed bug1777042 -- HP support for "std"
   rvallam     06/11/01 - fixed NT porting problem with template functions-
                          added generic method get(set)VectorOfRefs
   rratnam     04/25/01 - fixed NT porting bug1673780
   rvallam     04/12/01 - added dummy parameter in getVector of AnyData
                          for PObject *
   rratnam     04/10/01 - removed references to wstring
   rvallam     04/02/01 - fixed linux porting bug 1654670
   gayyappa    03/29/01 - remove get/set vector methods for int/float/double/
                          unsigned int for anydata.
   rvallam     03/20/01 - added dummy parameter for Type in getVector for
                          void * in statement and ResultSet
                          added setVector of Ref<T> and Number prototype
                          for statement
   gayyappa    03/15/01 - added parameter to getvector on anydata with void*.
   rratnam     03/13/01 - added virtual destructor to RefCounted
   rkasamse    03/15/01 - add an arg, Type, to getVector of void*
   rratnam     03/06/01 - removed references() from RefCounted
   slari       02/15/01 - suppress lint warnings
   rratnam     02/12/01 - removed #include <iostream.h>
   rvallam     02/07/01 - added enum OCCI_MAX_PREFETCH_DEPTH
   gayyappa    01/15/01 - change ub4 to unsigned int for
                          get/set vector methods..
   gayyappa    01/03/01 - put "using namespace std" in a ifndef
   gayyappa    12/14/00 - add forward declaration for ResultSetImpl 
                          and StatementImpl.
                          Remove commented #defines
   gayyappa    11/17/00 - change Session to Connection
   gayyappa    09/12/00 - remove OCCI_SQLT_INT, OCCI_SQLT_FLT 
                          remove OCCIUNSIGNED_CHAR, OCCISHORT, 
                          OCCIUNSIGNED_SHORT, OCCILONG ,OCCI_LONGDOUBLE,
                          OCCI_UNSIGNEDLONG  from enum Type.
   gayyappa    08/25/00 - add const to get/set vector mthds of anydata.
   rvallam     08/10/00 - updated Type enum
   slari       08/04/00 - add OCCIROWID and OCCICURSOR
   rkasamse    08/04/00 - add setVector methods
   slari       07/24/00 - add BytesImpl
   rratnam     07/25/00 - Added forward declarations for LobStreamImpl,
                          ConnectionPool[Impl], removed those for
                          Connection[Impl]
   rkasamse    07/28/00 - add getVector(ResultSet*...) methods
   rratnam     06/14/00 - Added forward declaration for RefImpl,
                          added DefaultCharSet to the CharSet 
                          enum
   rratnam     06/13/00 - Added DefaultCharSet to the CharSet enum
   kmohan      05/31/00 - RefCounted no more templated
   rratnam     22/05/00 - Added forward declaration of ConnectionImpl, and the
                         LobOpenMode enum
   kmohan      05/05/00 - Added global routine prototypes
   rkasamse    05/23/00 -
   slari       04/28/00 - add forward declaration of SQLExceptionImpl
   kmohan      04/19/00 - Added enums Type,CharSet,CharSetForm
   gayyappa    04/18/00 - removed checkStatus. added checkOCICall and 
                          createSQLEXception functions 
   kmohan      04/11/00 - Creation

*/


#ifndef OCCICOMMON_ORACLE
# define OCCICOMMON_ORACLE

#ifndef _olint

#ifndef OCI_ORACLE
#include <oci.h>
#endif

#ifndef ORASTRINGSTL
#define ORASTRINGSTL
#include <string>
#endif

#ifndef ORAVECTORSTL
#include <vector>
#define ORAVECTORSTL
#endif

#ifndef ORALISTSTL
#include <list>
#define ORALISTSTL
#endif

#define OCCI_STD_NAMESPACE std
#define OCCI_HAVE_STD_NAMESPACE 1

// version definitions
#define OCCI_MAJOR_VERSION OCI_MAJOR_VERSION
#define OCCI_MINOR_VERSION OCI_MINOR_VERSION

namespace oracle {
namespace occi {

//UString is the class for UTF16 characterset

#if (__GNUC__ == 3 && __GNUC_MINOR__ == 2)
 
  //char_traits<T> specialization for utext for gcc 3.2.3
  struct utext_char_traits
  {
  typedef utext          char_type;
  typedef unsigned int   int_type;

  typedef OCCI_STD_NAMESPACE::streampos      pos_type;
  typedef OCCI_STD_NAMESPACE::streamoff      off_type;
  typedef OCCI_STD_NAMESPACE::mbstate_t      state_type;

  //assign one char to another
  static void assign(char_type& c1, const char_type& c2)
    { 
        c1 = c2; 
    }

  //are 2 chars equal ?
  static bool eq(const char_type& c1, const char_type& c2) 
    {
       return c1 == c2; 
    }

  //is char c1 less then c2 ?
  static bool lt(const char_type& c1, const char_type& c2) 
    { 
       return c1 < c2; 
    }

  //compare 2 strings of char
  static int compare(const char_type* s1, const char_type* s2, size_t n) 
    {
      for (size_t i = 0; i < n; ++i)
        if (!eq(s1[i], s2[i]))
          return s1[i] < s2[i] ? -1 : 1;
      return 0;
    }

  //length of a char string
  static size_t length(const char_type* s) 
    {
      const char_type nullchar = char_type(0);
      size_t i = 0;
   
        while (!eq(*s++, nullchar)) i++; 
        return i;
    }

  //find a character in the char string
  static const char_type* find(const char_type* s, size_t n, const char_type& c)
    {
       for ( ; n > 0 ; ++s, --n)
         if (eq(*s, c))
           return s;
       return 0;
    }

  //move n chars from s2 to s1
  static char_type* move(char_type* s1, const char_type* s2, size_t n) 
    {
       memmove(s1, s2, n * sizeof(char_type));
       return s1;
    }
 
  //copy n chars from s2 to s1   
  static char_type* copy(char_type* s1, const char_type* s2, size_t n) 
    {
       memcpy(s1, s2, n * sizeof(char_type));
       return s1;
    } 

  //fill char c into s
  static char_type* assign(char_type* s, size_t n, char_type c) 
    {
      for (size_t i = 0; i < n; ++i)
        assign(s[i], c);
      return s;
    }

  //is the int representation eof ?
  static int_type not_eof(const int_type& c) 
    {
       if (c == eof())
         return 0;
       else
         return c;
    }

  //cast a int type to char
  static char_type to_char_type(const int_type& c)
    { 
       return static_cast<char_type>(c);
    }

  //cast char to int type
  static int_type to_int_type(const char_type& c)
    {  
      return static_cast<int_type>(c);
    }

  //eq operator when the chars are represented as ints
  static bool eq_int_type(const int_type& c1, const int_type& c2) 
    {
      return c1 == c2;
    }

  //eof character
  static int_type eof() 
    {
      return static_cast<int_type>(-1);
    }

   
  }; //end char_traits<unsigned short>

typedef OCCI_STD_NAMESPACE::basic_string<utext, utext_char_traits> UString;
#else
//for non gcc 3.2.3 platforms
typedef OCCI_STD_NAMESPACE::basic_string<utext> UString;
#endif /* if gcc 3.2.3 */

class Environment;
class EnvironmentImpl;
class Connection;
class ConnectionImpl;
class ConnectionPool;
class ConnectionPoolImpl;
class StatelessConnectionPool;
class StatelessConnectionPoolImpl;
class Statement;
class StatementImpl;
class ResultSet;
class ResultSetImpl;
class SQLException;
class SQLExceptionImpl;
class BatchSQLException;
class BatchSQLExceptionImpl;
class Stream;
class PObject;
class Number;
class Bytes;
class BytesImpl;
class Date;
class Timestamp;

class MetaData;
class MetaDataImpl;
template <class T> class Ref;
class RefImpl;
class RefAny;
class Blob;
class Bfile;
class Clob;
class LobStreamImpl;
class AnyData;
class AnyDataImpl;
class Map;
class IntervalDS;
class IntervalYM;

namespace aq {
class Message;
class MessageImpl;
class Agent;
class AgentImpl;
class Producer;
class ProducerImpl;
class Consumer;
class ConsumerImpl;
class Listener;
class Subscription;
class SubscriptionImpl;
class NotifyResult;
}

typedef struct BFloat BFloat;
typedef struct BDouble BDouble;

/*---------------------------------------------------------------------------
                           ENUMERATORS
  ---------------------------------------------------------------------------*/
enum Type
{
 OCCI_SQLT_CHR=SQLT_CHR,
 OCCI_SQLT_NUM=SQLT_NUM,
 OCCIINT = SQLT_INT,
 OCCIFLOAT = SQLT_FLT,
 OCCIBFLOAT = SQLT_BFLOAT,
 OCCIBDOUBLE = SQLT_BDOUBLE,
 OCCIIBFLOAT = SQLT_IBFLOAT,
 OCCIIBDOUBLE = SQLT_IBDOUBLE,
 OCCI_SQLT_STR=SQLT_STR,
 OCCI_SQLT_VNU=SQLT_VNU,
 OCCI_SQLT_PDN=SQLT_PDN,
 OCCI_SQLT_LNG=SQLT_LNG,
 OCCI_SQLT_VCS=SQLT_VCS,
 OCCI_SQLT_NON=SQLT_NON,
 OCCI_SQLT_RID=SQLT_RID,
 OCCI_SQLT_DAT=SQLT_DAT,
 OCCI_SQLT_VBI=SQLT_VBI,
 OCCI_SQLT_BIN=SQLT_BIN,
 OCCI_SQLT_LBI=SQLT_LBI,
 OCCIUNSIGNED_INT = SQLT_UIN,
 OCCI_SQLT_SLS=SQLT_SLS,
 OCCI_SQLT_LVC=SQLT_LVC,
 OCCI_SQLT_LVB=SQLT_LVB,
 OCCI_SQLT_AFC=SQLT_AFC,
 OCCI_SQLT_AVC=SQLT_AVC,
 OCCI_SQLT_CUR=SQLT_CUR,
 OCCI_SQLT_RDD=SQLT_RDD,
 OCCI_SQLT_LAB=SQLT_LAB,
 OCCI_SQLT_OSL=SQLT_OSL,
 OCCI_SQLT_NTY=SQLT_NTY,
 OCCI_SQLT_REF=SQLT_REF,
 OCCI_SQLT_CLOB=SQLT_CLOB,
 OCCI_SQLT_BLOB=SQLT_BLOB,
 OCCI_SQLT_BFILEE=SQLT_BFILEE,
 OCCI_SQLT_CFILEE=SQLT_CFILEE,
 OCCI_SQLT_RSET=SQLT_RSET,
 OCCI_SQLT_NCO=SQLT_NCO,
 OCCI_SQLT_VST=SQLT_VST,
 OCCI_SQLT_ODT=SQLT_ODT,
 OCCI_SQLT_DATE=SQLT_DATE,
 OCCI_SQLT_TIME=SQLT_TIME,
 OCCI_SQLT_TIME_TZ=SQLT_TIME_TZ,
 OCCI_SQLT_TIMESTAMP=SQLT_TIMESTAMP,
 OCCI_SQLT_TIMESTAMP_TZ=SQLT_TIMESTAMP_TZ,
 OCCI_SQLT_INTERVAL_YM=SQLT_INTERVAL_YM,
 OCCI_SQLT_INTERVAL_DS=SQLT_INTERVAL_DS,
 OCCI_SQLT_TIMESTAMP_LTZ=SQLT_TIMESTAMP_LTZ,
 OCCI_SQLT_FILE=SQLT_FILE,
 OCCI_SQLT_CFILE=SQLT_CFILE,
 OCCI_SQLT_BFILE=SQLT_BFILE,
 
 OCCICHAR = 32 *1024,
 OCCIDOUBLE,
 OCCIBOOL,
 OCCIANYDATA ,
 OCCINUMBER,
 OCCIBLOB,
 OCCIBFILE,
 OCCIBYTES,
 OCCICLOB ,
 OCCIVECTOR,
 OCCIMETADATA,
 OCCIPOBJECT,
 OCCIREF ,
 OCCIREFANY,
 OCCISTRING  ,
 OCCISTREAM  ,
 OCCIDATE  ,
 OCCIINTERVALDS  ,
 OCCIINTERVALYM  ,
 OCCITIMESTAMP,
 OCCIROWID,
 OCCICURSOR


};

enum LockOptions {OCCI_LOCK_NONE = OCI_LOCK_NONE,
                      OCCI_LOCK_X = OCI_LOCK_X, 
                      OCCI_LOCK_X_NOWAIT = OCI_LOCK_X_NOWAIT
                     };

enum {OCCI_MAX_PREFETCH_DEPTH = UB4MAXVAL};

enum TypeCode
{

OCCI_TYPECODE_REF = OCI_TYPECODE_REF,
OCCI_TYPECODE_DATE = OCI_TYPECODE_DATE,
OCCI_TYPECODE_REAL = OCI_TYPECODE_REAL,
OCCI_TYPECODE_DOUBLE = OCI_TYPECODE_DOUBLE,
OCCI_TYPECODE_BDOUBLE = OCI_TYPECODE_BDOUBLE,
OCCI_TYPECODE_FLOAT = OCI_TYPECODE_FLOAT,
OCCI_TYPECODE_BFLOAT = OCI_TYPECODE_BFLOAT,
OCCI_TYPECODE_NUMBER = OCI_TYPECODE_NUMBER,
OCCI_TYPECODE_DECIMAL = OCI_TYPECODE_DECIMAL,
OCCI_TYPECODE_OCTET = OCI_TYPECODE_OCTET,
OCCI_TYPECODE_INTEGER = OCI_TYPECODE_INTEGER,
OCCI_TYPECODE_SMALLINT= OCI_TYPECODE_SMALLINT,
OCCI_TYPECODE_RAW = OCI_TYPECODE_RAW,
OCCI_TYPECODE_VARCHAR2 = OCI_TYPECODE_VARCHAR2,
OCCI_TYPECODE_VARCHAR = OCI_TYPECODE_VARCHAR,
OCCI_TYPECODE_CHAR = OCI_TYPECODE_CHAR,
OCCI_TYPECODE_VARRAY= OCI_TYPECODE_VARRAY,
OCCI_TYPECODE_TABLE = OCI_TYPECODE_TABLE,
OCCI_TYPECODE_CLOB = OCI_TYPECODE_CLOB,
OCCI_TYPECODE_BLOB = OCI_TYPECODE_BLOB,
OCCI_TYPECODE_BFILE = OCI_TYPECODE_BFILE,
OCCI_TYPECODE_OBJECT = OCI_TYPECODE_OBJECT,
OCCI_TYPECODE_NAMEDCOLLECTION = OCI_TYPECODE_NAMEDCOLLECTION
};

enum CharSetForm
{
  OCCI_SQLCS_IMPLICIT = SQLCS_IMPLICIT // use local db char set
 ,OCCI_SQLCS_NCHAR = SQLCS_NCHAR // use local db nchar set
 ,OCCI_SQLCS_EXPLICIT = SQLCS_EXPLICIT // char set explicitly specified
 ,OCCI_SQLCS_FLEXIBLE = SQLCS_FLEXIBLE // pl/sql flexible parameter
};

enum LobOpenMode
{ OCCI_LOB_READONLY = OCI_LOB_READONLY
 ,OCCI_LOB_READWRITE = OCI_LOB_READWRITE
};

class RefCounted {
public:
    RefCounted();
    virtual ~RefCounted(){} 
    const RefCounted * newRef() const;
    void deleteRef() const;

private:

    void onZeroReferences();
    unsigned long references_;
  };

template <class T> 
class ConstPtr
{

public:

ConstPtr( const T* ptr = 0 );
ConstPtr( const ConstPtr<T>& mp );
~ConstPtr();
const T * operator->() const;
const T* rawPtr() const;

#ifdef MEMBER_TEMPLATE
template<class OtherType> operator ConstPtr<OtherType>();
#endif

protected:

void operator=( const ConstPtr<T>& mp );
const T* rawPtr_;

};

template <class T>
class Ptr : public ConstPtr<T> {

public:

Ptr( T* ptr = 0 );
Ptr( const Ptr<T>& mp );
void operator=( const Ptr<T>& mp );
const T * operator->() const;
T * operator->();
T* rawPtr() ;
const T* rawPtr() const;



#ifdef MEMBER_TEMPLATE
  template<class OtherType>
  operator Ptr<OtherType>();
#endif

};

/*---------------------------------------------------------------------------
                           EXPORT FUNCTIONS
  ---------------------------------------------------------------------------*/

  void getVector(const AnyData &any, 
  OCCI_STD_NAMESPACE::vector<OCCI_STD_NAMESPACE::string> &vect);
  void getVector(const AnyData &any,
  OCCI_STD_NAMESPACE::vector<UString> &vect);
  void getVector( const AnyData &any,
  OCCI_STD_NAMESPACE::vector<Blob> &vect) ;
  void getVector( const AnyData &any,
  OCCI_STD_NAMESPACE::vector<Clob> &vect) ;
  void getVector( const AnyData &any, 
  OCCI_STD_NAMESPACE::vector<Bfile> &vect) ;
  void getVector( const AnyData &any, 
  OCCI_STD_NAMESPACE::vector<Number> &vect) ;
  void getVector( const AnyData &any,
  OCCI_STD_NAMESPACE::vector<BFloat> &vect);
  void getVector(const AnyData &any,
  OCCI_STD_NAMESPACE::vector<BDouble> &vect);
  void getVector( const AnyData &any, 
  OCCI_STD_NAMESPACE::vector<Bytes> &vect) ;
  void getVector( const AnyData &any, 
  OCCI_STD_NAMESPACE::vector<Date> &vect) ;
  void getVector( const AnyData &any, 
  OCCI_STD_NAMESPACE::vector<Timestamp> &vect) ;
  void getVector( const AnyData &any,
  OCCI_STD_NAMESPACE::vector<IntervalYM> &vect) ;
  void getVector( const AnyData &any,
  OCCI_STD_NAMESPACE::vector<IntervalDS> &vect) ;
  template <class T>
  void getVectorOfRefs( const AnyData &any, 
  OCCI_STD_NAMESPACE::vector< Ref<T> > &vect) ;
  #ifndef WIN32COMMON
  template <class T>
  void getVector(const AnyData &any,
  OCCI_STD_NAMESPACE::vector< Ref<T> > &vect) ;
  #endif
  #ifdef WIN32COMMON
  template <class T>
  void getVector(const AnyData &any,
  OCCI_STD_NAMESPACE::vector<T> &vect,
  void *(*rSQL)(void *));
  #else
  template <class T>
  void getVector(const AnyData &any,
  OCCI_STD_NAMESPACE::vector<T *> &vect,
  void *(*rSQL)(void *));
  #endif

  void setVector( AnyData &any, 
  const OCCI_STD_NAMESPACE::vector<OCCI_STD_NAMESPACE::string> &vect) ;
  void setVector( AnyData &any, 
  const OCCI_STD_NAMESPACE::vector<UString> &vect) ;
  void setVector( AnyData &any,
  const OCCI_STD_NAMESPACE::vector<Blob> &vect) ;
  void setVector( AnyData &any,
  const OCCI_STD_NAMESPACE::vector<Clob> &vect) ;
  void setVector( AnyData &any, 
  const OCCI_STD_NAMESPACE::vector<Bfile> &vect) ;
  void setVector( AnyData &any,
  const OCCI_STD_NAMESPACE::vector<BFloat> &vect) ;
  void setVector( AnyData &any,
  const OCCI_STD_NAMESPACE::vector<BDouble> &vect) ;
  void setVector( AnyData &any, 
  const OCCI_STD_NAMESPACE::vector<Number> &vect) ;
  void setVector( AnyData &any, 
  const OCCI_STD_NAMESPACE::vector<Bytes> &vect) ;
  void setVector( AnyData &any, 
  const OCCI_STD_NAMESPACE::vector<Date> &vect) ;
  void setVector( AnyData &any, 
  const OCCI_STD_NAMESPACE::vector<Timestamp> &vect) ;
  void setVector( AnyData &any,
  const OCCI_STD_NAMESPACE::vector<IntervalYM> &vect) ;
  void setVector( AnyData &any,
  const OCCI_STD_NAMESPACE::vector<IntervalDS> &vect) ;
  template <class T>
  void setVectorOfRefs( AnyData &any, 
  const OCCI_STD_NAMESPACE::vector< Ref<T> > &vect) ;
  #ifndef WIN32COMMON
  template <class T>
  void setVector( AnyData &any, 
  const OCCI_STD_NAMESPACE::vector< Ref<T> > &vect) ;
  #endif
  #ifdef WIN32COMMON
  template <class T>
  void setVector( AnyData &any,
  const OCCI_STD_NAMESPACE::vector< T > &vect) ;
  #else
  template <class T>
  void setVector( AnyData &any,
  const OCCI_STD_NAMESPACE::vector< T* > &vect) ;
  #endif

  void getVector( ResultSet *rs, unsigned int index, 
  OCCI_STD_NAMESPACE::vector<int> &vect) ;
  void getVector( ResultSet *rs, unsigned int index, 
  OCCI_STD_NAMESPACE::vector<OCCI_STD_NAMESPACE::string> &vect) ;
  void getVector( ResultSet *rs, unsigned int index, 
  OCCI_STD_NAMESPACE::vector<UString> &vect) ;//UTF16 support
  void getVector(ResultSet  *rs, unsigned int, 
  OCCI_STD_NAMESPACE::vector<unsigned int> &vect) ;
  void getVector(ResultSet  *rs, unsigned int, 
  OCCI_STD_NAMESPACE::vector<float> &vect); 
  void getVector(ResultSet  *rs, unsigned int,
  OCCI_STD_NAMESPACE::vector<BFloat> &vect);
  void getVector(ResultSet  *rs, unsigned int, 
  OCCI_STD_NAMESPACE::vector<double> &vect);
  void getVector(ResultSet  *rs, unsigned int,
  OCCI_STD_NAMESPACE::vector<BDouble> &vect);
  void getVector(ResultSet  *rs, unsigned int, 
  OCCI_STD_NAMESPACE::vector<Date> &vect) ;
  void getVector(ResultSet  *rs, unsigned int, 
  OCCI_STD_NAMESPACE::vector<Timestamp> &vect) ;
  void getVector(ResultSet  *rs, unsigned int, 
  OCCI_STD_NAMESPACE::vector<RefAny> &vect) ;
  void getVector(ResultSet  *rs, unsigned int, 
  OCCI_STD_NAMESPACE::vector<Blob> &vect) ;
  void getVector(ResultSet  *rs, unsigned int, 
  OCCI_STD_NAMESPACE::vector<Clob> &vect) ;
  void getVector(ResultSet  *rs, unsigned int, 
  OCCI_STD_NAMESPACE::vector<Bfile> &vect) ;
  void getVector(ResultSet  *rs, unsigned int, 
  OCCI_STD_NAMESPACE::vector<Number> &vect) ;
  void getVector(ResultSet  *rs, unsigned int, 
  OCCI_STD_NAMESPACE::vector<IntervalYM> &vect) ;
  void getVector(ResultSet  *rs, unsigned int, 
  OCCI_STD_NAMESPACE::vector<IntervalDS> &vect) ;
  template <class T>
  void getVectorOfRefs(ResultSet  *rs, unsigned int,
  OCCI_STD_NAMESPACE::vector<Ref<T> > &vect) ;
  #ifndef WIN32COMMON
  template <class T>
  void getVector(ResultSet  *rs, unsigned int,
  OCCI_STD_NAMESPACE::vector<Ref<T> > &vect) ;
  #endif
  #ifdef WIN32COMMON
  template <class T>
  void getVector( ResultSet *rs, unsigned int index,
  OCCI_STD_NAMESPACE::vector< T > &vect) ;
  #else
  template <class T>
  void getVector( ResultSet *rs, unsigned int index,
  OCCI_STD_NAMESPACE::vector< T* > &vect) ;
  #endif


  void getVector(Statement  *rs, unsigned int, 
  OCCI_STD_NAMESPACE::vector<RefAny> &vect) ;
  void getVector(Statement  *rs, unsigned int, 
  OCCI_STD_NAMESPACE::vector<Blob> &vect) ;
  void getVector(Statement  *rs, unsigned int, 
  OCCI_STD_NAMESPACE::vector<Clob> &vect) ;
  void getVector(Statement  *rs, unsigned int, 
  OCCI_STD_NAMESPACE::vector<Bfile> &vect) ;
  void getVector(Statement  *rs, unsigned int, 
  OCCI_STD_NAMESPACE::vector<Number> &vect) ;
  void getVector(Statement  *rs, unsigned int, 
  OCCI_STD_NAMESPACE::vector<IntervalYM> &vect) ;
  void getVector(Statement  *rs, unsigned int, 
  OCCI_STD_NAMESPACE::vector<IntervalDS> &vect) ;
  void getVector( Statement *rs, unsigned int index, 
  OCCI_STD_NAMESPACE::vector<int> &vect) ;
  void getVector( Statement *rs, unsigned int index,
  OCCI_STD_NAMESPACE::vector<OCCI_STD_NAMESPACE::string> &vect) ;
  void getVector( Statement *rs, unsigned int index,
  OCCI_STD_NAMESPACE::vector<UString> &vect) ;//UTF16 support
  void getVector(Statement  *rs, unsigned int, 
  OCCI_STD_NAMESPACE::vector<unsigned int> &vect) ;
  void getVector(Statement  *rs, unsigned int, 
  OCCI_STD_NAMESPACE::vector<float> &vect) ;
  void getVector(Statement  *rs, unsigned int,
  OCCI_STD_NAMESPACE::vector<BFloat> &vect) ;
  void getVector(Statement  *rs, unsigned int, 
  OCCI_STD_NAMESPACE::vector<double> &vect) ;
  void getVector(Statement  *rs, unsigned int,
  OCCI_STD_NAMESPACE::vector<BDouble> &vect) ;
  void getVector(Statement  *rs, unsigned int, 
  OCCI_STD_NAMESPACE::vector<Date> &vect) ;
  void getVector(Statement  *rs, unsigned int, 
  OCCI_STD_NAMESPACE::vector<Timestamp> &vect) ;
  template <class T>
  void getVectorOfRefs(Statement  *rs, unsigned int,
  OCCI_STD_NAMESPACE::vector<Ref<T> > &vect) ;
  #ifndef WIN32COMMON
  template <class T>
  void getVector(Statement  *rs, unsigned int,
  OCCI_STD_NAMESPACE::vector<Ref<T> > &vect) ;
  #endif
  #ifdef WIN32COMMON
  template <class T>
  void getVector( Statement *rs, unsigned int index,
  OCCI_STD_NAMESPACE::vector< T > &vect) ;
  #else
  template <class T>
  void getVector( Statement *rs, unsigned int index,
  OCCI_STD_NAMESPACE::vector< T* > &vect) ;
  #endif


  void setVector(Statement *stmt, unsigned int paramIndex, 
                 const OCCI_STD_NAMESPACE::vector<int> &vect, 
                 const OCCI_STD_NAMESPACE::string &sqltype) ;
  void setVector(Statement *stmt, unsigned int paramIndex, 
                 const OCCI_STD_NAMESPACE::vector<unsigned int> &vect, 
                 const OCCI_STD_NAMESPACE::string &sqltype) ;
  void setVector(Statement *stmt, unsigned int paramIndex, 
                 const OCCI_STD_NAMESPACE::vector<double> &vect, 
                 const OCCI_STD_NAMESPACE::string &sqltype) ;
  void setVector(Statement *stmt, unsigned int paramIndex,
                 const OCCI_STD_NAMESPACE::vector<BDouble> &vect,
                 const OCCI_STD_NAMESPACE::string &sqltype) ;
  void setVector(Statement *stmt, unsigned int paramIndex, 
                 const OCCI_STD_NAMESPACE::vector<float> &vect, 
                 const OCCI_STD_NAMESPACE::string &sqltype) ;
  void setVector(Statement *stmt, unsigned int paramIndex,
                 const OCCI_STD_NAMESPACE::vector<BFloat> &vect,
                 const OCCI_STD_NAMESPACE::string &sqltype) ;
  void setVector(Statement *stmt, unsigned int paramIndex,
                 const OCCI_STD_NAMESPACE::vector<Number> &vect,
                 const OCCI_STD_NAMESPACE::string &sqltype);
  void setVector(Statement *stmt, unsigned int paramIndex, 
         const OCCI_STD_NAMESPACE::vector<OCCI_STD_NAMESPACE::string> &vect,
         const OCCI_STD_NAMESPACE::string &sqltype) ;
  void setVector(Statement *stmt, unsigned int paramIndex, 
                 const OCCI_STD_NAMESPACE::vector<RefAny> &vect, 
                 const OCCI_STD_NAMESPACE::string &sqltype) ;
  void setVector(Statement *stmt, unsigned int paramIndex, 
                 const OCCI_STD_NAMESPACE::vector<Blob> &vect, 
                 const OCCI_STD_NAMESPACE::string &sqltype) ;
  void setVector(Statement *stmt, unsigned int paramIndex, 
                 const OCCI_STD_NAMESPACE::vector<Clob> &vect, 
                 const OCCI_STD_NAMESPACE::string &sqltype) ;
  void setVector(Statement *stmt, unsigned int paramIndex, 
                 const OCCI_STD_NAMESPACE::vector<Bfile> &vect, 
                 const OCCI_STD_NAMESPACE::string &sqltype) ;
  void setVector(Statement *stmt, unsigned int paramIndex, 
                 const OCCI_STD_NAMESPACE::vector<Timestamp> &vect, 
                 const OCCI_STD_NAMESPACE::string &sqltype) ;
  void setVector(Statement *stmt, unsigned int paramIndex, 
                 const OCCI_STD_NAMESPACE::vector<IntervalDS> &vect, 
                 const OCCI_STD_NAMESPACE::string &sqltype) ;
  void setVector(Statement *stmt, unsigned int paramIndex, 
                 const OCCI_STD_NAMESPACE::vector<IntervalYM> &vect, 
                 const OCCI_STD_NAMESPACE::string &sqltype) ;
  void setVector(Statement *stmt, unsigned int paramIndex, 
                 const OCCI_STD_NAMESPACE::vector<Date> &vect, 
                 const OCCI_STD_NAMESPACE::string &sqltype) ;
  template  <class T>
  void setVectorOfRefs(Statement *stmt, unsigned int paramIndex,
  const OCCI_STD_NAMESPACE::vector<Ref<T> > &vect,
  const OCCI_STD_NAMESPACE::string &sqltype) ;
  #ifndef WIN32COMMON
  template  <class T>
  void setVector(Statement *stmt, unsigned int paramIndex,
  const OCCI_STD_NAMESPACE::vector<Ref<T> > &vect,
  const OCCI_STD_NAMESPACE::string &sqltype) ;
  #endif
  #ifdef WIN32COMMON
  template <class T>
  void setVector( Statement *stmt, unsigned int paramIndex, 
                  const OCCI_STD_NAMESPACE::vector< T > &vect, 
                  const OCCI_STD_NAMESPACE::string   &sqltype) ;
  #else
  template <class T>
  void setVector( Statement *stmt, unsigned int paramIndex,
                  const OCCI_STD_NAMESPACE::vector<T* > &vect, 
                  const OCCI_STD_NAMESPACE::string &sqltype) ;
  #endif

/* -----------------------------------------------------------------------------------
   Statement setVector functions, schema & type separate arguments - multibyte support
   -----------------------------------------------------------------------------------
*/
  void setVector(Statement *stmt, unsigned int paramIndex, 
                 const OCCI_STD_NAMESPACE::vector<int> &vect, 
                 const OCCI_STD_NAMESPACE::string &schemaName,
                 const OCCI_STD_NAMESPACE::string &typeName) ;
  void setVector(Statement *stmt, unsigned int paramIndex, 
                 const OCCI_STD_NAMESPACE::vector<unsigned int> &vect, 
                 const OCCI_STD_NAMESPACE::string &schemaName,
                 const OCCI_STD_NAMESPACE::string &typeName) ;
  void setVector(Statement *stmt, unsigned int paramIndex, 
                 const OCCI_STD_NAMESPACE::vector<double> &vect, 
                 const OCCI_STD_NAMESPACE::string &schemaName,
                 const OCCI_STD_NAMESPACE::string &typeName) ;
  void setVector(Statement *stmt, unsigned int paramIndex,
                 const OCCI_STD_NAMESPACE::vector<BDouble> &vect,
                 const OCCI_STD_NAMESPACE::string &schemaName,
                 const OCCI_STD_NAMESPACE::string &typeName) ;
  void setVector(Statement *stmt, unsigned int paramIndex, 
                 const OCCI_STD_NAMESPACE::vector<float> &vect, 
                 const OCCI_STD_NAMESPACE::string &schemaName,
                 const OCCI_STD_NAMESPACE::string &typeName) ;
  void setVector(Statement *stmt, unsigned int paramIndex,
                 const OCCI_STD_NAMESPACE::vector<BFloat> &vect,
                 const OCCI_STD_NAMESPACE::string &schemaName,
                 const OCCI_STD_NAMESPACE::string &typeName) ;
  void setVector(Statement *stmt, unsigned int paramIndex,
                 const OCCI_STD_NAMESPACE::vector<Number> &vect,
                 const OCCI_STD_NAMESPACE::string &schemaName,
                 const OCCI_STD_NAMESPACE::string &typeName);
  void setVector(Statement *stmt, unsigned int paramIndex, 
         const OCCI_STD_NAMESPACE::vector<OCCI_STD_NAMESPACE::string> &vect,
         const OCCI_STD_NAMESPACE::string &schemaName,
         const OCCI_STD_NAMESPACE::string &typeName) ;
  void setVector(Statement *stmt, unsigned int paramIndex, 
                 const OCCI_STD_NAMESPACE::vector<RefAny> &vect, 
                 const OCCI_STD_NAMESPACE::string &schemaName,
                 const OCCI_STD_NAMESPACE::string &typeName) ;
  void setVector(Statement *stmt, unsigned int paramIndex, 
                 const OCCI_STD_NAMESPACE::vector<Blob> &vect, 
                 const OCCI_STD_NAMESPACE::string &schemaName,
                 const OCCI_STD_NAMESPACE::string &typeName) ;
  void setVector(Statement *stmt, unsigned int paramIndex, 
                 const OCCI_STD_NAMESPACE::vector<Clob> &vect, 
                 const OCCI_STD_NAMESPACE::string &schemaName,
                 const OCCI_STD_NAMESPACE::string &typeName) ;
  void setVector(Statement *stmt, unsigned int paramIndex, 
                 const OCCI_STD_NAMESPACE::vector<Bfile> &vect, 
                 const OCCI_STD_NAMESPACE::string &schemaName,
                 const OCCI_STD_NAMESPACE::string &typeName) ;
  void setVector(Statement *stmt, unsigned int paramIndex, 
                 const OCCI_STD_NAMESPACE::vector<Timestamp> &vect, 
                 const OCCI_STD_NAMESPACE::string &schemaName,
                 const OCCI_STD_NAMESPACE::string &typeName) ;
  void setVector(Statement *stmt, unsigned int paramIndex, 
                 const OCCI_STD_NAMESPACE::vector<IntervalDS> &vect, 
                 const OCCI_STD_NAMESPACE::string &schemaName,
                 const OCCI_STD_NAMESPACE::string &typeName) ;
  void setVector(Statement *stmt, unsigned int paramIndex, 
                 const OCCI_STD_NAMESPACE::vector<IntervalYM> &vect, 
                 const OCCI_STD_NAMESPACE::string &schemaName,
                 const OCCI_STD_NAMESPACE::string &typeName) ;
  void setVector(Statement *stmt, unsigned int paramIndex, 
                 const OCCI_STD_NAMESPACE::vector<Date> &vect, 
                 const OCCI_STD_NAMESPACE::string &schemaName,
                 const OCCI_STD_NAMESPACE::string &typeName) ;
  template  <class T>
  void setVectorOfRefs(Statement *stmt, unsigned int paramIndex,
  const OCCI_STD_NAMESPACE::vector<Ref<T> > &vect,
  const OCCI_STD_NAMESPACE::string &schemaName,
  const OCCI_STD_NAMESPACE::string &typeName) ;

  #ifndef WIN32COMMON
  template  <class T>
  void setVector(Statement *stmt, unsigned int paramIndex,
  const OCCI_STD_NAMESPACE::vector<Ref<T> > &vect,
  const OCCI_STD_NAMESPACE::string &schemaName,
  const OCCI_STD_NAMESPACE::string &typeName) ;
  #endif

  #ifdef WIN32COMMON
  template <class T>
  void setVector( Statement *stmt, unsigned int paramIndex, 
                  const OCCI_STD_NAMESPACE::vector< T > &vect, 
                  const OCCI_STD_NAMESPACE::string &schemaName,
                  const OCCI_STD_NAMESPACE::string &typeName) ;
  #else
  template <class T>
  void setVector( Statement *stmt, unsigned int paramIndex,
  const OCCI_STD_NAMESPACE::vector<T* > &vect, const OCCI_STD_NAMESPACE::string
  &schemaName, const OCCI_STD_NAMESPACE::string &typeName) ;
  #endif

/*-------------------------------------------------------------------------
  Statement setVector function - UTF16 support
  -------------------------------------------------------------------------
*/
  void setVector(Statement *stmt, unsigned int paramIndex, 
                 const OCCI_STD_NAMESPACE::vector<int> &vect, 
                 const UString &schemaName,
                 const UString &typeName) ;
  void setVector(Statement *stmt, unsigned int paramIndex, 
                 const OCCI_STD_NAMESPACE::vector<unsigned int> &vect, 
                 const UString &schemaName,
                 const UString &typeName) ;
  void setVector(Statement *stmt, unsigned int paramIndex, 
                 const OCCI_STD_NAMESPACE::vector<double> &vect, 
                 const UString &schemaName,
                 const UString &typeName) ;
  void setVector(Statement *stmt, unsigned int paramIndex,
                 const OCCI_STD_NAMESPACE::vector<BDouble> &vect,
                 const UString &schemaName,
                 const UString &typeName) ;
  void setVector(Statement *stmt, unsigned int paramIndex, 
                 const OCCI_STD_NAMESPACE::vector<float> &vect, 
                 const UString &schemaName,
                 const UString &typeName) ;
  void setVector(Statement *stmt, unsigned int paramIndex,
                 const OCCI_STD_NAMESPACE::vector<BFloat> &vect,
                 const UString &schemaName,
                 const UString &typeName) ;
  void setVector(Statement *stmt, unsigned int paramIndex,
                 const OCCI_STD_NAMESPACE::vector<Number> &vect,
                 const UString &schemaName,
                 const UString &typeName);
  void setVector(Statement *stmt, unsigned int paramIndex, 
                 const OCCI_STD_NAMESPACE::vector<UString> &vect,
                 const UString &schemaName,
                 const UString &typeName) ;
  void setVector(Statement *stmt, unsigned int paramIndex, 
                 const OCCI_STD_NAMESPACE::vector<UString> &vect,
                 const OCCI_STD_NAMESPACE::string &schemaName,
                 const OCCI_STD_NAMESPACE::string &typeName) ;
  void setVector(Statement *stmt, unsigned int paramIndex, 
                 const OCCI_STD_NAMESPACE::vector<RefAny> &vect, 
                 const UString &schemaName,
                 const UString &typeName) ;
  void setVector(Statement *stmt, unsigned int paramIndex, 
                 const OCCI_STD_NAMESPACE::vector<Blob> &vect, 
                 const UString &schemaName,
                 const UString &typeName) ;
  void setVector(Statement *stmt, unsigned int paramIndex, 
                 const OCCI_STD_NAMESPACE::vector<Clob> &vect, 
                 const UString &schemaName,
                 const UString &typeName) ;
  void setVector(Statement *stmt, unsigned int paramIndex, 
                 const OCCI_STD_NAMESPACE::vector<Bfile> &vect, 
                 const UString &schemaName,
                 const UString &typeName) ;
  void setVector(Statement *stmt, unsigned int paramIndex, 
                 const OCCI_STD_NAMESPACE::vector<Timestamp> &vect, 
                 const UString &schemaName,
                 const UString &typeName) ;
  void setVector(Statement *stmt, unsigned int paramIndex, 
                 const OCCI_STD_NAMESPACE::vector<IntervalDS> &vect, 
                 const UString &schemaName,
                 const UString &typeName) ;
  void setVector(Statement *stmt, unsigned int paramIndex, 
                 const OCCI_STD_NAMESPACE::vector<IntervalYM> &vect, 
                 const UString &schemaName,
                 const UString &typeName) ;
  void setVector(Statement *stmt, unsigned int paramIndex, 
                 const OCCI_STD_NAMESPACE::vector<Date> &vect, 
                 const UString &schemaName,
                 const UString &typeName) ;
  template  <class T>
  void setVectorOfRefs(Statement *stmt, unsigned int paramIndex,
  const OCCI_STD_NAMESPACE::vector<Ref<T> > &vect,
  const UString &schemaName,
  const UString &typeName) ;

  #ifndef WIN32COMMON
  template  <class T>
  void setVector(Statement *stmt, unsigned int paramIndex,
  const OCCI_STD_NAMESPACE::vector<Ref<T> > &vect,
  const UString &schemaName,
  const UString &typeName) ;
  #endif

  #ifdef WIN32COMMON
  template <class T>
  void setVector( Statement *stmt, unsigned int paramIndex, 
                  const OCCI_STD_NAMESPACE::vector< T > &vect, 
                  const UString &schemaName,
                  const UString &typeName) ;
  #else
  template <class T>
  void setVector( Statement *stmt, unsigned int paramIndex,
  const OCCI_STD_NAMESPACE::vector<T* > &vect, const UString
  &schemaName, const UString &typeName) ;
  #endif


/* Global method for array pins */
template <class T>
void pinVectorOfRefs( const Connection *conn,
OCCI_STD_NAMESPACE::vector<Ref<T> > &vect,
OCCI_STD_NAMESPACE::vector<T* > &vectObj, 
LockOptions lockOpt = OCCI_LOCK_NONE );

template <class T>
void pinVectorOfRefs( const Connection *conn,
OCCI_STD_NAMESPACE::vector<Ref<T> > &vect,
LockOptions lockOpt = OCCI_LOCK_NONE );

#ifdef ORAXB8_DEFINED
      void readVectorOfBfiles(const Connection *conn,
          OCCI_STD_NAMESPACE::vector<Bfile> &vec,
          oraub8 *byte_amts, oraub8 *offsets,
          unsigned char *buffers[], oraub8 *buffer_lens);

      void readVectorOfBlobs(const Connection *conn,
          OCCI_STD_NAMESPACE::vector<Blob> &vec,
          oraub8 *byte_amts, oraub8 *offsets,
          unsigned char *buffers[], oraub8 *buffer_lens);
      void writeVectorOfBlobs(const Connection *conn, 
          OCCI_STD_NAMESPACE::vector<Blob> &vec,
          oraub8 *byte_amts, oraub8 *offsets,
          unsigned char *buffers[], oraub8 *buffer_lens);

      void readVectorOfClobs(const Connection *conn,
          OCCI_STD_NAMESPACE::vector<Clob> &vec,
          oraub8 *byte_amts, oraub8 *char_amts, oraub8 *offsets,
          unsigned char *buffers[], oraub8 *buffer_lens);
      void writeVectorOfClobs(const Connection *conn,
          OCCI_STD_NAMESPACE::vector<Clob> &vec,
          oraub8 *byte_amts, oraub8 *char_amts, oraub8 *offsets,
          unsigned char *buffers[], oraub8 *buffer_lens);
      void readVectorOfClobs(const Connection *conn,
          OCCI_STD_NAMESPACE::vector<Clob> &vec,
          oraub8 *byte_amts, oraub8 *char_amts, oraub8 *offsets,
          utext *buffers[], oraub8 *buffer_lens);
      void writeVectorOfClobs(const Connection *conn,
          OCCI_STD_NAMESPACE::vector<Clob> &vec,
          oraub8 *byte_amts, oraub8 *char_amts, oraub8 *offsets,
          utext *buffers[], oraub8 *buffer_lens);
#endif


/*---------------------------------------------------------------------------
                          INTERNAL FUNCTIONS
  ---------------------------------------------------------------------------*/


} /* end of namespace occi */
} /* end of namespace oracle */


#endif /* _olint */

#endif                                              /* OCCICOMMON_ORACLE */
