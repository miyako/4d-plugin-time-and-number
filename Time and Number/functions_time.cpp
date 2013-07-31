/*
 *  functions_time.cpp
 *  Time
 *
 *  Created by miyako on 2012/10/30.
 *
 */

#include "functions_time.h"

#define ISO_DATE_FORMAT_SPRINT "%4d.%2d.%2d.%2d.%2d.%2d.%3d"
#define ISO_DATE_FORMAT_STRING "yyyy.MM.dd.HH.mm.ss.SSS"
#define ISO_DATE_FORMAT_STRING_SIZE 24
#define MAX_REGION_LENGTH 4

UDate _TimeGetDate(C_DATE &DateIn, C_TIME &TimeIn, C_LONGINT &MillisecondIn, C_TEXT &ZoneIn){

	uint16_t y, m, d, hh, mm, ss;
	UErrorCode status = U_ZERO_ERROR;
	UDate date = 0;
	
	DateIn.getYearMonthDay(&y, &m, &d);	
	uint32_t seconds = TimeIn.getSeconds();	

	hh = seconds / 3600;
	mm = (seconds % 3600) / 60;	
	ss = (seconds % 3600) % 60;
	
	uint16_t sss = MillisecondIn.getIntValue() % 1000;
		
	std::vector<char> buf(ISO_DATE_FORMAT_STRING_SIZE);
	sprintf((char *)&buf[0], ISO_DATE_FORMAT_SPRINT, y, m, d, hh, mm, ss, sss);
	
	C_TEXT iso_date_string;
	iso_date_string.setUTF8String((const uint8_t *)&buf[0], ISO_DATE_FORMAT_STRING_SIZE);
	const UnicodeString dateString = UnicodeString((const UChar *)iso_date_string.getUTF16StringPtr());
	
	C_TEXT iso_date_format;
	iso_date_format.setUTF8String((const uint8_t *)ISO_DATE_FORMAT_STRING, strlen(ISO_DATE_FORMAT_STRING));
	const UnicodeString fmtstr = UnicodeString((const UChar *)iso_date_format.getUTF16StringPtr());	
	
	SimpleDateFormat fmt(fmtstr, status);
	
	if(status == U_ZERO_ERROR || status == U_USING_DEFAULT_WARNING || status == U_USING_FALLBACK_WARNING){
	
		UnicodeString zoneId = UnicodeString((const UChar *)ZoneIn.getUTF16StringPtr());
		
		TimeZone *zone;
		
		if(ZoneIn.getUTF16Length()){
			zone = TimeZone::createTimeZone(zoneId);
			
		}else{
			//use local if zone was omitted
			zone = TimeZone::createDefault();
		}
		
		if(zone){
			
			fmt.adoptTimeZone(zone);
			
		}
		
		date = fmt.parse(dateString, status);
	
	}
	
	return date;
	
}

UErrorCode _DateToText(UDate date, UnicodeString &formatString, UnicodeString &zoneId, C_TEXT &StringOut){
	
	UErrorCode status = U_ZERO_ERROR;
	
	SimpleDateFormat fmt(formatString, status);
	
	//the format string is valid
	if(status == U_ZERO_ERROR || status == U_USING_DEFAULT_WARNING  || status == U_USING_FALLBACK_WARNING){
		
		TimeZone *zone;
		
		if(zoneId.length()){
			zone = TimeZone::createTimeZone(zoneId);
			
		}else{
			//use local if zone was omitted
			zone = TimeZone::createDefault();
		}
		
		if(zone){
			
			fmt.adoptTimeZone(zone);
			
		}
		
		UnicodeString str;
		
		//export the date string
		str = fmt.format(date, str, status);
		
		if(status == U_ZERO_ERROR || status == U_USING_DEFAULT_WARNING  || status == U_USING_FALLBACK_WARNING){
			_TextSetUnicodeString(StringOut, str);
			status = U_ZERO_ERROR;
		}
		
	}
	
	return status;
	
}

UErrorCode _DateToDate(UDate date, UnicodeString &zoneId, C_DATE &DateOut, C_TIME &TimeOut, C_LONGINT &MillisecondOut){
	
	UErrorCode status = U_ZERO_ERROR;	
	
	C_TEXT iso_date_format;
	iso_date_format.setUTF8String((const uint8_t *)ISO_DATE_FORMAT_STRING, ISO_DATE_FORMAT_STRING_SIZE);
	const UnicodeString fmtstr = UnicodeString((const UChar *)iso_date_format.getUTF16StringPtr());
	SimpleDateFormat isoFmt(fmtstr, status);
	
	if(status == U_ZERO_ERROR || status == U_USING_DEFAULT_WARNING || status == U_USING_FALLBACK_WARNING){
		
		TimeZone *zone;
		
		if(zoneId.length()){
			zone = TimeZone::createTimeZone(zoneId);
			
		}else{
			//use local if zone was omitted
			zone = TimeZone::createDefault();
		}
		
		if(zone){
			
			isoFmt.adoptTimeZone(zone);
			
		}
		
		UnicodeString str;
		str = isoFmt.format(date, str);
		
		CUTF8String isoDateString;
		_CUTF8SetUnicodeString(isoDateString, str);
		
		int yyyy = atoi((const char *)isoDateString.substr(0, 4).c_str());
		int mm = atoi((const char *)isoDateString.substr(5, 2).c_str());
		int dd = atoi((const char *)isoDateString.substr(8, 2).c_str());
		int h = atoi((const char *)isoDateString.substr(11, 2).c_str());
		int m = atoi((const char *)isoDateString.substr(14, 2).c_str());
		int s = atoi((const char *)isoDateString.substr(17, 2).c_str());
		int ms = atoi((const char *)isoDateString.substr(20, 3).c_str());
		
		DateOut.setYearMonthDay(yyyy, mm, dd);
		TimeOut.setSeconds(s + (m * 60) + (h * 3600));
		MillisecondOut.setIntValue(ms);
		
		status = U_ZERO_ERROR;
		
	}
	
	return status;
	
}

// ------------------------------------- Time -------------------------------------

void TIME_Date_from_absolute(sLONG_PTR *pResult, PackagePtr pParams)
{
	C_REAL AbsoluteIn;
	C_DATE DateOut;
	C_TIME TimeOut;
	C_LONGINT MillisecondOut;
	C_TEXT ZoneIn;
	C_LONGINT returnValue;
	
	AbsoluteIn.fromParamAtIndex(pParams, 1);
	ZoneIn.fromParamAtIndex(pParams, 5);
	
    UErrorCode status = U_ZERO_ERROR;
	
	UDate date = AbsoluteIn.getDoubleValue();
	UnicodeString zoneId = UnicodeString((const UChar *)ZoneIn.getUTF16StringPtr());
	
	status = _DateToDate(date, zoneId, DateOut, TimeOut, MillisecondOut);
		
	returnValue.setIntValue(status);
	
	DateOut.toParamAtIndex(pParams, 2);
	TimeOut.toParamAtIndex(pParams, 3);
	MillisecondOut.toParamAtIndex(pParams, 4);
	returnValue.setReturn(pResult);
}

void TIME_Date_from_text(sLONG_PTR *pResult, PackagePtr pParams)
{
	C_TEXT StringIn;
	C_TEXT FormatIn;
	C_DATE DateOut;
	C_TIME TimeOut;
	C_TEXT ZoneOut;
	C_LONGINT MillisecondOut;
	C_LONGINT returnValue;
	
	StringIn.fromParamAtIndex(pParams, 1);
	FormatIn.fromParamAtIndex(pParams, 2);
	
	UErrorCode status = U_ZERO_ERROR;
	
	UnicodeString dateString = UnicodeString((const UChar *)StringIn.getUTF16StringPtr());
	UnicodeString formatString = UnicodeString((const UChar *)FormatIn.getUTF16StringPtr());	
	
	SimpleDateFormat fmt(formatString, status);	
	
	if(status == U_ZERO_ERROR || status == U_USING_DEFAULT_WARNING || status == U_USING_FALLBACK_WARNING){
	
		UDate date = fmt.parse(dateString, status);
		
		UnicodeString zoneId;		
		zoneId = fmt.getTimeZone().getID(zoneId);
		
		_TextSetUnicodeString(ZoneOut, zoneId);
		
		status = _DateToDate(date, zoneId, DateOut, TimeOut, MillisecondOut);

	}
	
	returnValue.setIntValue(status);
	
	DateOut.toParamAtIndex(pParams, 3);
	TimeOut.toParamAtIndex(pParams, 4);
	MillisecondOut.toParamAtIndex(pParams, 5);	
	ZoneOut.toParamAtIndex(pParams, 6);
	returnValue.setReturn(pResult);
}

void TIME_Absolute_from_text(sLONG_PTR *pResult, PackagePtr pParams)
{
	C_TEXT StringIn;
	C_TEXT FormatIn;
	C_REAL AbsoluteOut;
	C_LONGINT returnValue;
	
	StringIn.fromParamAtIndex(pParams, 1);
	FormatIn.fromParamAtIndex(pParams, 2);
	
	UErrorCode status = U_ZERO_ERROR;
	
	UnicodeString dateString = UnicodeString((const UChar *)StringIn.getUTF16StringPtr());
	UnicodeString formatString = UnicodeString((const UChar *)FormatIn.getUTF16StringPtr());	
	
	SimpleDateFormat fmt(formatString, status);	
	
	//the format string is valid
	if(status == U_ZERO_ERROR || status == U_USING_DEFAULT_WARNING || status == U_USING_FALLBACK_WARNING){
		
		UDate date = fmt.parse(dateString, status);
		
		//the date string is valid
		if(status == U_ZERO_ERROR || status == U_USING_DEFAULT_WARNING || status == U_USING_FALLBACK_WARNING){
			AbsoluteOut.setDoubleValue(date);
			status = U_ZERO_ERROR;
		}
	}
	
	returnValue.setIntValue(status);
	
	AbsoluteOut.toParamAtIndex(pParams, 3);
	returnValue.setReturn(pResult);
}

void TIME_Absolute_to_text(sLONG_PTR *pResult, PackagePtr pParams)
{
	C_REAL AbsoluteIn;
	C_TEXT FormatIn;
	C_TEXT StringOut;
	C_TEXT ZoneIn;
	C_LONGINT returnValue;
	
	AbsoluteIn.fromParamAtIndex(pParams, 1);
	FormatIn.fromParamAtIndex(pParams, 2);
	ZoneIn.fromParamAtIndex(pParams, 4);
	
    UErrorCode status = U_ZERO_ERROR;
	
	UDate date = AbsoluteIn.getDoubleValue();
	UnicodeString formatString = UnicodeString((const UChar *)FormatIn.getUTF16StringPtr());	
	UnicodeString zoneId = UnicodeString((const UChar *)ZoneIn.getUTF16StringPtr());
	
	status = _DateToText(date, formatString, zoneId, StringOut);
	
	returnValue.setIntValue(status);
	
	StringOut.toParamAtIndex(pParams, 3);
	returnValue.setReturn(pResult);
}

void TIME_Date_to_absolute(sLONG_PTR *pResult, PackagePtr pParams)
{
	C_DATE DateIn;
	C_TIME TimeIn;
	C_LONGINT MillisecondIn;
	C_TEXT ZoneIn;
	C_REAL AbsoluteOut;
	C_LONGINT returnValue;
	
	DateIn.fromParamAtIndex(pParams, 1);
	TimeIn.fromParamAtIndex(pParams, 2);
	MillisecondIn.fromParamAtIndex(pParams, 3);
	ZoneIn.fromParamAtIndex(pParams, 4);
		
	AbsoluteOut.setDoubleValue(_TimeGetDate(DateIn, TimeIn, MillisecondIn, ZoneIn));

	returnValue.setIntValue(U_ZERO_ERROR);
	
	AbsoluteOut.toParamAtIndex(pParams, 5);
	returnValue.setReturn(pResult);
}

void TIME_Date_to_text(sLONG_PTR *pResult, PackagePtr pParams)
{
	C_DATE DateIn;
	C_TIME TimeIn;
	C_LONGINT MillisecondIn;
	C_TEXT ZoneIn;
	C_TEXT FormatIn;
	C_TEXT StringOut;
	C_LONGINT returnValue;
	
	DateIn.fromParamAtIndex(pParams, 1);
	TimeIn.fromParamAtIndex(pParams, 2);
	MillisecondIn.fromParamAtIndex(pParams, 3);
	ZoneIn.fromParamAtIndex(pParams, 4);
	FormatIn.fromParamAtIndex(pParams, 5);
	
	UErrorCode status = U_ZERO_ERROR;
	
	UnicodeString formatString = UnicodeString((const UChar *)FormatIn.getUTF16StringPtr());	
	UnicodeString zoneId = UnicodeString((const UChar *)ZoneIn.getUTF16StringPtr());
	
	UDate date = _TimeGetDate(DateIn, TimeIn, MillisecondIn, ZoneIn);
	
	status = _DateToText(date, formatString, zoneId, StringOut);
	
	returnValue.setIntValue(status);
	
	StringOut.toParamAtIndex(pParams, 6);
	returnValue.setReturn(pResult);
}

void TIME_Get_offset(sLONG_PTR *pResult, PackagePtr pParams)
{
	C_REAL AbsoluteIn;
	C_TEXT ZoneIn;
	C_LONGINT OffsetOut;
	C_LONGINT DstOut;	
	C_LONGINT returnValue;
	
	AbsoluteIn.fromParamAtIndex(pParams, 1);
	ZoneIn.fromParamAtIndex(pParams, 2);
	
    UErrorCode status = U_ZERO_ERROR;
	
	UnicodeString zoneId = UnicodeString((const UChar *)ZoneIn.getUTF16StringPtr());
	TimeZone *zone = TimeZone::createTimeZone(zoneId);
	
	if(zone){
		
		int32_t rawOffset, dstOffset;
		UDate date = AbsoluteIn.getDoubleValue();
		
		zone->getOffset(date, true, rawOffset, dstOffset, status);
		
		OffsetOut.setIntValue(rawOffset);
		DstOut.setIntValue(dstOffset);
		
		delete zone;
		
	}else{
		status = U_ILLEGAL_ARGUMENT_ERROR;
	}	
	
	returnValue.setIntValue(status);
	
	OffsetOut.toParamAtIndex(pParams, 3);
	DstOut.toParamAtIndex(pParams, 4);	
	returnValue.setReturn(pResult);
}

// ----------------------------------- Timezone -----------------------------------

void TIMEZONE_Get_region(sLONG_PTR *pResult, PackagePtr pParams)
{
	C_TEXT Param1;
	C_TEXT Param2;
	C_LONGINT returnValue;
	
	Param1.fromParamAtIndex(pParams, 1);
    
	UErrorCode status = U_ZERO_ERROR;
	
	std::vector<char> buf(MAX_REGION_LENGTH);
	
	UnicodeString zoneId = UnicodeString((const UChar *)Param1.getUTF16StringPtr());
	
	int32_t len = TimeZone::getRegion(zoneId, &buf[0], MAX_REGION_LENGTH, status);
    
	if(status == U_ZERO_ERROR || status == U_USING_DEFAULT_WARNING || status == U_USING_FALLBACK_WARNING){
		Param2.setUTF8String((const uint8_t *)&buf[0], len);
		status = U_ZERO_ERROR;
	}
	
	returnValue.setIntValue(status);
	
	Param2.toParamAtIndex(pParams, 2);
	returnValue.setReturn(pResult);
}

void TIMEZONE_Get_default(sLONG_PTR *pResult, PackagePtr pParams)
{
	C_TEXT returnValue;
	
	TimeZone *defaultZone = TimeZone::createDefault();
	
	if(defaultZone){
        
		UnicodeString zoneId;
		defaultZone->getID(zoneId);
		_TextSetUnicodeString(returnValue, zoneId);
		
		delete defaultZone;	
		
	}
	
	returnValue.setReturn(pResult);
}

void TIMEZONE_Get_offset(sLONG_PTR *pResult, PackagePtr pParams)
{
	C_TEXT Param1;
	C_LONGINT Param2;
	C_LONGINT returnValue;
	
	Param1.fromParamAtIndex(pParams, 1);
	
    UErrorCode status = U_ZERO_ERROR;
	
	Param1.fromParamAtIndex(pParams, 1);
	UnicodeString zoneId = UnicodeString((const UChar *)Param1.getUTF16StringPtr());
	TimeZone *zone = TimeZone::createTimeZone(zoneId);
	
	if(zone){
		
		Param2.setIntValue(zone->getRawOffset());
		
		delete zone;
		
	}else{
		status = U_ILLEGAL_ARGUMENT_ERROR;
	}
	
	returnValue.setIntValue(status);
	
	Param2.toParamAtIndex(pParams, 2);
	returnValue.setReturn(pResult);
}

void TIMEZONE_Get_display_name(sLONG_PTR *pResult, PackagePtr pParams)
{
	C_TEXT Param1;
	C_TEXT Param2;
	C_LONGINT returnValue;
	
	Param1.fromParamAtIndex(pParams, 1);
	
    UErrorCode status = U_ZERO_ERROR;
	
	Param1.fromParamAtIndex(pParams, 1);
	UnicodeString zoneId = UnicodeString((const UChar *)Param1.getUTF16StringPtr());
	TimeZone *zone = TimeZone::createTimeZone(zoneId);
	
	if(zone){
		
		UnicodeString displayName;
		zone->getDisplayName (displayName);
		
		_TextSetUnicodeString(Param2, displayName);
		
		delete zone;
		
	}else{
		status = U_ILLEGAL_ARGUMENT_ERROR;
	}
	
	returnValue.setIntValue(status);
	
	Param2.toParamAtIndex(pParams, 2);
	returnValue.setReturn(pResult);
}

void TIMEZONE_Get_GMT(sLONG_PTR *pResult, PackagePtr pParams)
{
	C_TEXT returnValue;
	
	const TimeZone *zone = TimeZone::getGMT();
	
	if(zone){
		UnicodeString zoneId;
		zone->getID(zoneId);
		_TextSetUnicodeString(returnValue, zoneId);
	}
	
	returnValue.setReturn(pResult);
}