4d-plugin-time-and-number
=========================

Methods to convert number to text and time across timezones. (an ICU wrapper)

### Platform

| carbon | cocoa | win32 | win64 |
|:------:|:-----:|:---------:|:---------:|
|<img src="https://cloud.githubusercontent.com/assets/1725068/22371562/1b091f0a-e4db-11e6-8458-8653954a7cce.png" width="24" height="24" />|<img src="https://cloud.githubusercontent.com/assets/1725068/22371562/1b091f0a-e4db-11e6-8458-8653954a7cce.png" width="24" height="24" />|<img src="https://cloud.githubusercontent.com/assets/1725068/22371562/1b091f0a-e4db-11e6-8458-8653954a7cce.png" width="24" height="24" />|<img src="https://cloud.githubusercontent.com/assets/1725068/22371562/1b091f0a-e4db-11e6-8458-8653954a7cce.png" width="24" height="24" />|

### Version

<img src="https://cloud.githubusercontent.com/assets/1725068/18940649/21945000-8645-11e6-86ed-4a0f800e5a73.png" width="32" height="32" /> <img src="https://cloud.githubusercontent.com/assets/1725068/18940648/2192ddba-8645-11e6-864d-6d5692d55717.png" width="32" height="32" /> <img src="https://user-images.githubusercontent.com/1725068/41266195-ddf767b2-6e30-11e8-9d6b-2adf6a9f57a5.png" width="32" height="32" />

* New command ``TIMEZONE GET LIST`` returns 620 timezones.

```
  //new constant RFC

$e:=TIME Date to text (Current date;Current time;Milliseconds;"EST";TIME RFC Date;$out)
$e:=TIME Date from text ($out;TIME RFC Date;$date;$time;$ms;$zone)
$out:=String(Current date;Date RFC 1123;Current time)
$e:=TIME Date from text ($out;TIME RFC Date;$date;$time;$ms;$zone)

  //time-date <-> absolute <-> text conversion

  //date-time to absolute absolute = ms since 24:00 GMT, Jan 1, 1970.
  //specify the time zone in which to interpret the date-time
  //default is local
$e:=TIME Date to absolute (Current date;Current time;0;"GMT";$absolute)
$e:=TIME Absolute to text ($absolute;TIME ISO Date Zone Hour GMT;$dateString)
  //shorthand for the above
$e:=TIME Date to text (Current date;Current time;0;"GMT";$dateString)

$e:=TIME Date to absolute (Current date;Current time;0;"";$absolute)
$e:=TIME Absolute to text ($absolute;TIME ISO Date Zone Hour GMT;$dateString)

  //to get the time is a specifc zone, add zone
$e:=TIME Absolute to text ($absolute;TIME ISO Date Zone Hour GMT;$dateString;"JST")
$e:=TIME Absolute to text ($absolute;TIME ISO Date Zone Hour GMT;$dateString;"EST")
$e:=TIME Absolute to text ($absolute;TIME ISO Date Zone Hour GMT;$dateString;"GMT")

  //shorthand for the above
$e:=TIME Date to text (Current date;Current time;0;"GMT";$dateString)
  //absolute to date/time 
  //specify the time zone in which to interpret the date-time
  //default is local
$e:=TIME Date from absolute ($absolute;$date;$time;$ms)
$e:=TIME Date from absolute ($absolute;$date;$time;$ms;"GMT")

$e:=TIME Absolute from text ($dateString;TIME ISO Date;$absolute)

$dateString:=String(Current date;ISO date;Current time)
  //because the ISO Date format has not zone, it is interpreted as local
$e:=TIME Absolute from text ($dateString;TIME ISO Date;$absolute)

  //shorthand for the above
TIME Date from text ($dateString;TIME ISO Date;$date;$time;$ms;$zone)

If (False)
  //format a date as text
  //this example is similar to the 4D String 'ISO Date format'
$e:=TIME Date to text (Current date;Current time;Milliseconds;"EST";TIME ISO Date;$out)
  //PDT
$e:=TIME Date to text (Current date;Current time;Milliseconds;"EST";TIME ISO Date Zone Short;$out)
  //Pacific Daylight Time
$e:=TIME Date to text (Current date;Current time;Milliseconds;"EST";TIME ISO Date Zone Long;$out)
  //-0700
$e:=TIME Date to text (Current date;Current time;Milliseconds;"EST";TIME ISO Date Zone Hours;$out)
  //GMT-0700
$e:=TIME Date to text (Current date;Current time;Milliseconds;"EST";TIME ISO Date Zone Hour GMT;$out)
  //Japan Standard Time
$e:=TIME Date to text (Current date;Current time;Milliseconds;"EST";TIME ISO Date Zone Generic;$out)
  //Japan Time
$e:=TIME Date to text (Current date;Current time;Milliseconds;"EST";TIME ISO Date Zone Location;$out)

  //more generic patterns can be used; see documentation at
  //http://www.icu-project.org/apiref/icu4c/classSimpleDateFormat.html#details

End if 

$e:=TIME Get offset ($absolute;"EST";$offset;$daylightSavingTime)

  //get information about a time zone

$default:=TIMEZONE Get default 

$e:=TIMEZONE Get display name ($default;$displayName)
$e:=TIMEZONE Get region ($default;$region)
$e:=TIMEZONE Get offset ($default;$offset)

$gmt:=TIMEZONE Get GMT 

$e:=TIMEZONE Get display name ($gmt;$displayName)
$e:=TIMEZONE Get region ($gmt;$region)
  //use TIME Get offset to get daylight saving offset as well
$e:=TIMEZONE Get offset ($gmt;$offset)
```

```
C_REAL($number)
C_TEXT($string)

$number:=Random

$error:=NUMBER Format ($number;$string)  //default.
$error:=NUMBER Format ($number;$string;NUMBER Spellout)  //ex. nineteen thousand nine hundred forty-three 
$error:=NUMBER Format ($number;$string;NUMBER Duration)  //ex. 5:32:23
$error:=NUMBER Format ($number;$string;NUMBER Ordinal)  //ex. 19,943rd
  //http://icu-project.org/apiref/icu4c/classicu_1_1RuleBasedNumberFormat.html

$error:=NUMBER Format ($number;$string;NUMBER Spellout;"fr_FR")  //ex. vingt-deux-mille-sept-cent-quatorze

$rule:="-x: minus >>;\n"+\
"x.x: << point >>;\n"+\
"zero; one; two; three; four; five; six;\n"+\
"    seven; eight; nine;\n"+\
"10: << >>;\n"+\
"100: << >>>;\n"+\
"1000: <<, >>>;\n"+\
"1,000,000: <<, >>>;\n"+\
"1,000,000,000: <<, >>>;\n"+\
"1,000,000,000,000: <<, >>>;\n"+\
"1,000,000,000,000,000: =#,##0=;\n";

$error:=NUMBER Format ($number;$string;NUMBER Custom;"en_US";$rule)  //ex. seven, nine three one
  //http://userguide.icu-project.org/formatparse/numbers
```
