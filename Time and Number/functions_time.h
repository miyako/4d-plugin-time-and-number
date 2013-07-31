/*
 *  functions_time.h
 *  Time
 *
 *  Created by miyako on 2012/10/30.
 *
 */

#include "4DPluginAPI.h"
#include "4DPlugin.h"

#include "timezone.h"
#include "smpdtfmt.h"

// --- Time
void TIME_Get_offset(sLONG_PTR *pResult, PackagePtr pParams);
void TIME_Date_to_text(sLONG_PTR *pResult, PackagePtr pParams);
void TIME_Date_from_text(sLONG_PTR *pResult, PackagePtr pParams);
void TIME_Absolute_from_text(sLONG_PTR *pResult, PackagePtr pParams);
void TIME_Absolute_to_text(sLONG_PTR *pResult, PackagePtr pParams);
void TIME_Date_from_absolute(sLONG_PTR *pResult, PackagePtr pParams);
void TIME_Date_to_absolute(sLONG_PTR *pResult, PackagePtr pParams);

// --- Timezone
void TIMEZONE_Get_region(sLONG_PTR *pResult, PackagePtr pParams);
void TIMEZONE_Get_default(sLONG_PTR *pResult, PackagePtr pParams);
void TIMEZONE_Get_offset(sLONG_PTR *pResult, PackagePtr pParams);
void TIMEZONE_Get_display_name(sLONG_PTR *pResult, PackagePtr pParams);
void TIMEZONE_Get_GMT(sLONG_PTR *pResult, PackagePtr pParams);
