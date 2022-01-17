#pragma once

#define STATUS_LOGOUT		0
#define STATUS_LOGIN		1

#define ENTRY_FILENAME	"Entries.rk"
#define USER_FILENAME "users.rk"
#define REPOSITORY_FILENAME	"repository.rk"
#define INFO_FILENAME	"info.rk"

#define REMOTE_BACKUP_PATH	"RK_BAK"
#define REMOTE_ROOT_PATH	"RK"
#define HISTORY_SUBNAME "history_rk"
#define BACKUP_SUBNAME "rk"

#define MAX_TOKEN	50
#define LINE_BUFFER		256
#define MAX_REPOSITORY 256

#define BRANCH_KEY	'/'

#define CONFIRM_REPLACE_FILE	"File already exists. Replace?"

#define USERNAME_LENGTH	8
#define PASSWORD_LENGTH	8

#define SALT_LENGTH	8
#define PASSWD_LENGTH 		34	// MD5 Encryption (with using 8BYTE salt)

typedef BOOL (*CBCopyFile) (void* pData);

#include <afxtempl.h>
#include <io.h>
#include <mmsystem.h>
#include <wincrypt.h>
#include <string.h>

#include "Md5.h"
#include "RKParser.h"
#include "RKWriter.h"

#include "RKRepository.h"
#include "RKEntryManager.h"
#include "RKUserManager.h"

