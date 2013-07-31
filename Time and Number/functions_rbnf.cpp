/*
 *  functions_rbnf.cpp
 *  Time
 *
 *  Created by miyako on 2012/10/30.
 *
 */

#include "functions_rbnf.h"

#define NUMBER_FORMAT_SPELLOUT 0
#define NUMBER_FORMAT_ORDINAL 1
#define NUMBER_FORMAT_DURATION 2
#define NUMBER_FORMAT_NUMBERING_SYSTEM 3
#define NUMBER_FORMAT_COUNT 4
#define NUMBER_FORMAT_CUSTOM 5

// ------------------------------------ Number ------------------------------------


void NUMBER_Format(sLONG_PTR *pResult, PackagePtr pParams)
{
	C_REAL Param1;
	C_TEXT Param2;
	C_LONGINT Param3;
	C_TEXT Param4;
	C_TEXT Param5;
	C_LONGINT returnValue;
	
	Param1.fromParamAtIndex(pParams, 1);
	Param3.fromParamAtIndex(pParams, 3);
	Param4.fromParamAtIndex(pParams, 4);
	Param5.fromParamAtIndex(pParams, 5);
	
	// --- write the code of NUMBER_Format here...
	
	UErrorCode	status = U_ZERO_ERROR;	
	UParseError	perror;
	
	Locale locale;
	
	if(Param4.getUTF16Length()){
	
		CUTF8String u;
		Param4.copyUTF8String(&u);
		
		Locale inLocale = Locale((const char *)u.c_str());
		
		if(!inLocale.isBogus()){
			locale = Locale(inLocale);
		}else{
			status = U_ILLEGAL_ARGUMENT_ERROR;
		}
		
	}	
	
	if(status == U_ZERO_ERROR){
	
		int mode = Param3.getIntValue();
		
		if(mode == NUMBER_FORMAT_CUSTOM){
			
			if(Param5.getUTF16Length()){
				
				UnicodeString rules = UnicodeString((const UChar *)Param5.getUTF16StringPtr());
				RuleBasedNumberFormat fmt = RuleBasedNumberFormat(rules, locale, perror, status);
				
				if(status == U_ZERO_ERROR || status == U_USING_DEFAULT_WARNING || status == U_USING_FALLBACK_WARNING){
					
					UnicodeString numberString;
					fmt.format(Param1.getDoubleValue(), numberString);
					_TextSetUnicodeString(Param2, numberString);
					status = U_ZERO_ERROR;
					
				}
				
			}else{
				status = U_ILLEGAL_ARGUMENT_ERROR;
			}
			
		}else{
			
			switch (mode) {
				case NUMBER_FORMAT_SPELLOUT:
				case NUMBER_FORMAT_ORDINAL:
				case NUMBER_FORMAT_DURATION:
				case NUMBER_FORMAT_NUMBERING_SYSTEM:
				case NUMBER_FORMAT_COUNT:
				{
					RuleBasedNumberFormat fmt = RuleBasedNumberFormat((URBNFRuleSetTag)mode, locale, status);
					
					if(status == U_ZERO_ERROR || status == U_USING_DEFAULT_WARNING || status == U_USING_FALLBACK_WARNING){
						
						UnicodeString numberString;
						fmt.format(Param1.getDoubleValue(), numberString);
						_TextSetUnicodeString(Param2, numberString);
						status = U_ZERO_ERROR;
						
					}
					
				}
					break;				
					
				default:
					status = U_ILLEGAL_ARGUMENT_ERROR;
					break;
			}
			
		}
		
	}
	
	Param2.toParamAtIndex(pParams, 2);
	
	returnValue.setIntValue(status);
	returnValue.setReturn(pResult);
}