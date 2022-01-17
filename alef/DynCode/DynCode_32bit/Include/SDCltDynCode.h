#ifndef SDCLTDYNCODE_H
#define SDCLTDYNCODE_H

#pragma once

#ifndef SDAPI
#define SDAPI   __stdcall
#endif

/* ====================================================================
 * ISDCltDynCode: client side dynamic code component. ISDCltDynCode provides
 * dynamic encrypt and decrypt methods.
 *
 * Usage: 
 * 1. Create ISDCltDynCode component, call SDCreateCltDynCode
 * 2. Call ISDCltDynCode.SetDynCode to when getting dynamic code from server
 * 3. To encrypt: call ISDCltDynCode::Encode
 * 4. To decrypt: call ISDCltDynCode::Decode
 * 5. when finish, call ISDCltDynCode::Release to release the component
 */

namespace SGDP{


class ISDCltDynCode
{
public:
    //Function: set dynamic code
    //Return: true if successful
    //Remark: this will overwrite the dynamic doe set before
    virtual bool SDAPI SetDynCode(const unsigned char* lpDynCode, int nLen) = 0;

    //Function: encrypt the data, it will return false if called without setting dynamic code
    //Parameter: lpData, the original data, it is used as a [in, out] parameter
    //		       nLen, the data length, it will not change after encryption
    //Return: true if successful
    virtual bool SDAPI Encode(unsigned char * lpData, unsigned long nLen) = 0;

    //Function: decrypt the data, it will return false if called without setting dynamic code
    //Parameter: lpData, the original data to be decrypted, it is used as a [in, out] parameter
    //		       nLen, the data length, it will not change after decryption
    //Return: true if successful
    virtual bool SDAPI Decode(unsigned char * lpData, unsigned long nLen) = 0;

	//Function: Release the compoment
    virtual void SDAPI Release() = 0;
};

//Function: Create ISDCltDynCode component
ISDCltDynCode * SDAPI SDCreateCltDynCode();

}

#endif
