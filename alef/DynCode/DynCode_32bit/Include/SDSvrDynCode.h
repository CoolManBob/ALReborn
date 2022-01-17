#ifndef SDSVRDYNCODE_H
#define SDSVRDYNCODE_H

#pragma once

#ifndef SDAPI
#define SDAPI   __stdcall
#endif

/* ====================================================================
 * ISDSvrDynCode: server side dynamic code component. ISDSvrDynCode provides
 * dynamic encrypt and decrypt methods.
 *
 * Dynamic Code Binary File: compiled machine code of Encrypt and Decrypt
 * function are saved in binary files. File name format of these binary 
 * files are *.server.bin and *.client.bin, * stand for number, for example 
 * 1.server.bin, 1.client.bin. These numbers are called CodeIndex. Each 
 * server binary files has a corresponding client binary files with same 
 * CodeIndex. *.server.bin contains binary code for server, *.client.bin 
 * contains binary code for client. Both server and client binary files 
 * are used by server. Generally, there are multiple pairs of server and 
 * client binary files in one server. Each pair contains different encrypt
 * -decrypt binary code.
 *
 * Usage:
 * 1. Create ISDSvrDynCode component, call SDCreateSvrDynCode
 * 2. Load Dynamic Code Binary Files, call ISDSvrDynCode::LoadBinary
 * 3. Get random CodeIndex, call ISDSvrDynCode::GetRandIdx
 * 4. Get Client binary code, call ISDSvrDynCode::GetCltDynCode
 * 5. Transfer the client code to client
 * 6. To encrypt: call ISDSvrDynCode::Encode
 * 7. To decrypt: call ISDSvrDynCode::Decode
 * 8. when finish, call ISDSvrDynCode::Release to release component
 */


namespace SGDP{

class ISDSvrDynCode
{
public:
    virtual SDAPI ~ISDSvrDynCode() {}

    //Function: import dynamic code
    //Parameter: pszSvrBinDir, path for server dynamic code files
    //           pszsCltBinDir, path for client dynbamic code files
    //Return: the number of successfully imported pairs of files.
    virtual int SDAPI LoadBinary(const char * pszSvrBinDir, const char * pszsCltBinDir) = 0;

    //Function: get a random index for dynamic code
    //Return: >= 0 if successful£¬-1 if error
    virtual int SDAPI GetRandIdx() = 0;

    //Function: Get client dynamic code
    //Return: return a null string if nCodeIdx is invalid
    virtual int SDAPI GetCltDynCode(int nCodeIdx, const unsigned char **ppCodeRet) = 0;

    //Function: encrypt the data, it will return false if nCodeIdx is invalid
    //Parameter: lpData, the original data, it is used as a [in, out] parameter
    //		       nLen, the data length, it will not change after encryption
    //Return: true if successful
    virtual bool SDAPI Encode(unsigned char * lpData, unsigned long nLen, int nCodeIdx) = 0;

    //Function: decrypt the data, it will return false if nCodeIdx is invalid
    //Parameter: lpData, the original data to be decrypted, it is used as a [in, out] parameter
    //		       nLen, the data length, it will not change after decryption
    //Return: true if successful
    virtual bool SDAPI Decode(unsigned char * lpData, unsigned long nLen, int nCodeIdx) = 0;

	//Function: Release the compoment
    virtual void SDAPI Release() = 0;
};

//Function: Create ISDSvrDynCode component
ISDSvrDynCode * SDAPI SDCreateSvrDynCode();

};

#endif
