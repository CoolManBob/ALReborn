#if (!defined(RPPLUGIN_H))
#define RPPLUGIN_H

/*
 * This is a required file for each plugin. It should be generated by the
 * plugin vendor. The information contained in this file should be unique
 * derived from the vendor id / name to prevent conflict with other plugins
 */

#if (defined(RWDEBUG) && !defined(RPERROR_H))
extern long rpTeamStackDepth;
#define RWDEBUGSTACKDEPTH rpTeamStackDepth
#endif /* (defined(RWDEBUG) !defined(RPERROR_H)) */

#include "rpcriter.h"          /* Note: each vendor can choose their own method for
                                * allocation of unique ID's. This file defines
                                * the ID's used by Criterion.
                                */

/* rwPLUGIN_ID must be defined in order to use the RenderWare debug/error
 * mechanism
 */

#define rwPLUGIN_ID            rwID_TEAMPLUGIN
#define rwPLUGIN_ERRFUNC      _rwdb_CriterionTEAM
#define rwPLUGIN_ERRENUM     e_rwdb_CriterionTEAM
#define rwPLUGIN_ERRENUMLAST e_rwdb_CriterionTEAMLAST

#endif /* (!defined(RPPLUGIN_H)) */
