![version](https://img.shields.io/badge/version-18%2B-EB8E5F)
![platform](https://img.shields.io/static/v1?label=platform&message=mac-intel%20|%20mac-arm%20|%20win-64&color=blue)
[![license](https://img.shields.io/github/license/miyako/4d-plugin-time-and-number)](LICENSE)
![downloads](https://img.shields.io/github/downloads/miyako/4d-plugin-time-and-number/total)

# 4d-plugin-time-and-number

Time and Number is an ICU wrapper: it converts between 4D date/time values, ICU "absolute" timestamps (milliseconds since 1970-01-01 00:00:00 GMT), and formatted `Text`, with explicit timezone handling on every conversion; it also looks up timezone metadata (region, offset, display name, the full IANA zone list) and formats numbers as spelled-out words, ordinals, durations, or custom rule-based text via ICU's `RuleBasedNumberFormat`. Every command returns its result either as a `Longint` ICU status code (`0` = success; see [Error handling](#error-handling--troubleshooting)) or, for two zone-lookup commands, as the value itself.

## Summary

| Command | Returns | Purpose |
|---|---|---|
| [TIME Get offset](#time-get-offset) | `Longint` | Raw + daylight-saving offset (seconds) for a zone at a given absolute time |
| [TIME Date to text](#time-date-to-text) | `Longint` | Format a 4D date/time/ms/zone as text |
| [TIME Date from text](#time-date-from-text) | `Longint` | Parse text into a 4D date/time/ms/zone |
| [TIME Absolute from text](#time-absolute-from-text) | `Longint` | Parse text into an absolute timestamp |
| [TIME Absolute to text](#time-absolute-to-text) | `Longint` | Format an absolute timestamp as text |
| [TIME Date from absolute](#time-date-from-absolute) | `Longint` | Convert an absolute timestamp to date/time/ms |
| [TIME Date to absolute](#time-date-to-absolute) | `Longint` | Convert date/time/ms/zone to an absolute timestamp |
| [TIMEZONE Get region](#timezone-get-region) | `Longint` | Country/region code for a zone ID |
| [TIMEZONE Get default](#timezone-get-default) | `Text` | The host machine's local zone ID |
| [TIMEZONE Get offset](#timezone-get-offset) | `Longint` | Raw (standard, non-DST) offset for a zone |
| [TIMEZONE Get display name](#timezone-get-display-name) | `Longint` | Human-readable name for a zone |
| [TIMEZONE Get GMT](#timezone-get-gmt) | `Text` | The GMT zone ID |
| [NUMBER Format](#number-format) | `Longint` | Spell out, ordinal-ize, duration-format, or custom-format a number |
| [TIMEZONE GET LIST](#timezone-get-list) | *(none)* | Every timezone ID ICU knows about |

**Platforms:** macOS (Intel + Apple Silicon), Windows 64-bit — 4D v18 or later.

---

## Requirements & platform notes

- No OS-level permissions are required; all functionality is driven by the bundled ICU library, not by OS calendar/timezone APIs, so behavior is consistent across macOS and Windows.
- **Zone IDs are IANA identifiers** (e.g. `"America/New_York"`) or common short aliases the sample code uses (e.g. `"EST"`, `"GMT"`, `"JST"`) — ICU resolves these; see [TIMEZONE GET LIST](#timezone-get-list) to enumerate every ID ICU recognizes on the running system.
- **An unrecognized zone ID is not reported as an error.** Every command that takes a zone `Text` parameter hands it to ICU's zone lookup, which falls back to a generic zone instead of failing when the ID isn't recognized. Passing a typo'd or made-up zone name will not raise an error code — double-check zone spelling against [TIMEZONE GET LIST](#timezone-get-list) rather than relying on the return code to catch a mistake.
- **`TIME Date to absolute` always returns `0` (success)**, regardless of whether the supplied date/time/zone was valid — it has no path that reports failure. Don't branch on its return code.
- Where a zone parameter is documented **optional**, omitting it means "interpret in the local (machine default) timezone" — this is shown throughout the plugin's own test method.
- `TIMEZONE Get offset` and `TIME Get offset` are **not interchangeable**: the former returns only the zone's raw standard-time offset; only the latter also reports the daylight-saving offset for a specific point in time.

---

## TIME Get offset

### Syntax

```
TIME Get offset ( absolute ; zone ; offset ; dst ) → Longint
```

| Parameter | Type | Description |
|---|---|---|
| `absolute` | Real | Absolute timestamp (ms since 1970-01-01 GMT) at which to evaluate the offset |
| `zone` | Text | Zone ID to evaluate (e.g. `"EST"`, `"America/New_York"`) |
| `offset` | Longint | *(output)* Raw (standard) offset from GMT, in seconds |
| `dst` | Longint | *(output)* Daylight-saving offset in effect at `absolute`, in seconds |
| Result | Longint | ICU status code |

### Description

Unlike `TIMEZONE Get offset`, this command evaluates the offset *at a specific instant*, so it correctly reflects whether daylight saving is in effect on that date. Add `offset` and `dst` together to get the total local-vs-GMT difference at that moment.

### Example

From the plugin's own test method (`TEST.4dm`):

```4d
$e:=TIME Get offset($absolute; "EST"; $offset; $daylightSavingTime)
```

```4d
 // total local offset from GMT, in seconds, right now
C_REAL($now)
C_LONGINT($raw;$dst;$total)
$now:=Milliseconds
$e:=TIME Get offset($now; "America/Los_Angeles"; $raw; $dst)
$total:=$raw+$dst
```

---

## TIME Date to text

### Syntax

```
TIME Date to text ( date ; time ; millisecond ; zone ; format ; string ) → Longint
```

| Parameter | Type | Description |
|---|---|---|
| `date` | Date | Date part to format |
| `time` | Time | Time-of-day part to format |
| `millisecond` | Longint | Millisecond component |
| `zone` | Text | Zone the `date`/`time` should be interpreted in; empty string = local |
| `format` | Text | Format pattern or format constant (see below) |
| `string` | Text | *(output)* Formatted result |
| Result | Longint | ICU status code |

### Description

Internally builds an ISO instant from `date`/`time`/`millisecond` interpreted in `zone`, then formats that instant using `format`. `format` accepts either a raw ICU `SimpleDateFormat` pattern string, or one of the plugin's predefined format constants — the plugin's own test/README samples use bare names like `TIME RFC Date` and `TIME ISO Date` (and its `TIME ISO Date Zone Short` / `Long` / `Hours` / `Hour GMT` / `Generic` / `Location` variants) directly as 4D constants. The exact underlying pattern string or numeric value behind each constant isn't in the reviewed source (only the `.cpp`/`.h`/manifest were available, not a constants resource) — check the constant's definition in 4D's Explorer, or pass your own `SimpleDateFormat`-style pattern string directly (see ICU's `SimpleDateFormat` reference for pattern syntax).

### Example

From the plugin's own test method (`TEST.4dm`):

```4d
$e:=TIME Date to text(Current date:C33; Current time:C178; Milliseconds:C459; "EST"; TIME RFC Date; $out)
```

The same test file's commented-out block shows several format-constant variants against the identical input:

```4d
If (False:C215)
  $e:=TIME Date to text(Current date:C33; Current time:C178; Milliseconds:C459; "EST"; TIME ISO Date; $out)
  //PDT
  $e:=TIME Date to text(Current date:C33; Current time:C178; Milliseconds:C459; "EST"; TIME ISO Date Zone Short; $out)
  //Pacific Daylight Time
  $e:=TIME Date to text(Current date:C33; Current time:C178; Milliseconds:C459; "EST"; TIME ISO Date Zone Long; $out)
  //-0700
  $e:=TIME Date to text(Current date:C33; Current time:C178; Milliseconds:C459; "EST"; TIME ISO Date Zone Hours; $out)
  //GMT-0700
  $e:=TIME Date to text(Current date:C33; Current time:C178; Milliseconds:C459; "EST"; TIME ISO Date Zone Hour GMT; $out)
End if
```

A shorthand for `TIME Date to absolute` + `TIME Absolute to text` in one call, interpreting the input as GMT and formatting with a raw ICU pattern:

```4d
C_TEXT($out)
$e:=TIME Date to text(Current date:C33; Current time:C178; 0; "GMT"; "EEE, d MMM yyyy HH:mm:ss 'GMT'"; $out)
```

---

## TIME Date from text

### Syntax

```
TIME Date from text ( string ; format ; date ; time ; millisecond ; zone ) → Longint
```

| Parameter | Type | Description |
|---|---|---|
| `string` | Text | Date/time text to parse |
| `format` | Text | Format pattern or format constant used to interpret `string` (same rules as `TIME Date to text`) |
| `date` | Date | *(output)* Parsed date |
| `time` | Time | *(output)* Parsed time-of-day |
| `millisecond` | Longint | *(output)* Parsed millisecond |
| `zone` | Text | *(output)* Zone ID the parser resolved (from the pattern or a default) |
| Result | Longint | ICU status code |

### Description

The reverse of `TIME Date to text`. `zone` is an **output** here (not an input) — it reports whichever zone ICU's parser actually resolved while reading `string`, which is useful when the format includes a zone/offset token and you don't already know the source zone. If `format` has no zone component, the result is interpreted as local time and `zone` comes back as the local zone.

### Example

From the plugin's own test method (`TEST.4dm`):

```4d
$e:=TIME Date to text(Current date:C33; Current time:C178; Milliseconds:C459; "EST"; TIME RFC Date; $out)
$e:=TIME Date from text($out; TIME RFC Date; $date; $time; $ms; $zone)
$out:=String:C10(Current date:C33; Date RFC 1123:K1:11; Current time:C178)
$e:=TIME Date from text($out; TIME RFC Date; $date; $time; $ms; $zone)
```

Round-tripping a 4D-generated ISO date string (no zone in the pattern, so it's read as local time):

```4d
$dateString:=String:C10(Current date:C33; ISO date:K1:8; Current time:C178)
 //because the ISO Date format has no zone, it is interpreted as local
TIME Date from text($dateString; TIME ISO Date; $date; $time; $ms; $zone)
```

---

## TIME Absolute from text

### Syntax

```
TIME Absolute from text ( string ; format ; absolute ) → Longint
```

| Parameter | Type | Description |
|---|---|---|
| `string` | Text | Date/time text to parse |
| `format` | Text | Format pattern or format constant used to interpret `string` |
| `absolute` | Real | *(output)* Parsed absolute timestamp (ms since 1970-01-01 GMT) |
| Result | Longint | ICU status code |

### Description

Like `TIME Date from text`, but yields a single absolute timestamp instead of separate date/time/ms/zone fields — convenient when you just need a comparable millisecond value rather than the individual components.

### Example

From the plugin's own test method (`TEST.4dm`):

```4d
$e:=TIME Absolute from text($dateString; TIME ISO Date; $absolute)
```

```4d
$dateString:=String:C10(Current date:C33; ISO date:K1:8; Current time:C178)
 //because the ISO Date format has no zone, it is interpreted as local
$e:=TIME Absolute from text($dateString; TIME ISO Date; $absolute)
```

---

## TIME Absolute to text

### Syntax

```
TIME Absolute to text ( absolute ; format ; string {; zone} ) → Longint
```

| Parameter | Type | Description |
|---|---|---|
| `absolute` | Real | Absolute timestamp (ms since 1970-01-01 GMT) to format |
| `format` | Text | Format pattern or format constant |
| `string` | Text | *(output)* Formatted result |
| `zone` | Text | Zone to render the result in. **Optional** — the plugin's own test file omits it in several calls; when omitted, the local zone is used |
| Result | Longint | ICU status code |

### Description

Renders the same absolute instant differently depending on `zone` — the instant itself doesn't change, only its textual representation.

### Example

From the plugin's own test method (`TEST.4dm`), the same `$absolute` value rendered in three different zones:

```4d
$e:=TIME Absolute to text($absolute; TIME ISO Date Zone Hour GMT; $dateString; "JST")
$e:=TIME Absolute to text($absolute; TIME ISO Date Zone Hour GMT; $dateString; "EST")
$e:=TIME Absolute to text($absolute; TIME ISO Date Zone Hour GMT; $dateString; "GMT")
```

Without a zone (local time is used):

```4d
$e:=TIME Absolute to text($absolute; TIME ISO Date Zone Hour GMT; $dateString)
```

---

## TIME Date from absolute

### Syntax

```
TIME Date from absolute ( absolute ; date ; time ; millisecond {; zone} ) → Longint
```

| Parameter | Type | Description |
|---|---|---|
| `absolute` | Real | Absolute timestamp (ms since 1970-01-01 GMT) to convert |
| `date` | Date | *(output)* Date part |
| `time` | Time | *(output)* Time-of-day part |
| `millisecond` | Longint | *(output)* Millisecond component |
| `zone` | Text | Zone to interpret the timestamp in. **Optional** — defaults to local when omitted |
| Result | Longint | ICU status code |

### Description

The reverse of `TIME Date to absolute`. Splits a single absolute instant back into 4D's separate `Date`/`Time`/millisecond representation, in whichever zone you specify (or local, if omitted).

### Example

From the plugin's own test method (`TEST.4dm`):

```4d
$e:=TIME Date from absolute($absolute; $date; $time; $ms)
$e:=TIME Date from absolute($absolute; $date; $time; $ms; "GMT")
```

---

## TIME Date to absolute

### Syntax

```
TIME Date to absolute ( date ; time ; millisecond ; zone ; absolute ) → Longint
```

| Parameter | Type | Description |
|---|---|---|
| `date` | Date | Date part |
| `time` | Time | Time-of-day part |
| `millisecond` | Longint | Millisecond component |
| `zone` | Text | Zone to interpret `date`/`time` in; empty string = local |
| `absolute` | Real | *(output)* Resulting absolute timestamp (ms since 1970-01-01 GMT) |
| Result | Longint | **Always `0`** — see caveat below |

### Description

The reverse of `TIME Date from absolute`. **This command's return code is not meaningful**: it always reports success (`0`), even if `zone` doesn't resolve to a real timezone or the date/time is otherwise unusual — there is no code path in this command that returns anything else. Don't use its return value to validate input.

### Example

From the plugin's own test method (`TEST.4dm`):

```4d
 //date-time to absolute; absolute = ms since 24:00 GMT, Jan 1, 1970.
 //specify the time zone in which to interpret the date-time; default is local
$e:=TIME Date to absolute(Current date:C33; Current time:C178; 0; "GMT"; $absolute)
```

```4d
$e:=TIME Date to absolute(Current date:C33; Current time:C178; 0; ""; $absolute)
 //empty zone -> interpreted as local time
```

---

## TIMEZONE Get region

### Syntax

```
TIMEZONE Get region ( zone ; region ) → Longint
```

| Parameter | Type | Description |
|---|---|---|
| `zone` | Text | Zone ID to look up |
| `region` | Text | *(output)* Country/region code associated with the zone |
| Result | Longint | ICU status code |

### Description

Returns ICU's region/country code for the zone (typically a short code such as a country code). The output buffer is capped at a small fixed size internally, so only short region codes are returned — this matches ICU's own region-code convention and isn't something you need to size for.

### Example

```4d
$default:=TIMEZONE Get default
$e:=TIMEZONE Get region($default; $region)
```

```4d
$gmt:=TIMEZONE Get GMT
$e:=TIMEZONE Get region($gmt; $region)
```

---

## TIMEZONE Get default

### Syntax

```
TIMEZONE Get default → Text
```

| Parameter | Type | Description |
|---|---|---|
| Result | Text | The host machine's default (local) zone ID |

### Description

Takes no parameters. Unlike almost every other command in this plugin, the function result **is** the value itself (a zone ID), not a status code — there's no separate success/failure signal to check.

### Example

From the plugin's own test method (`TEST.4dm`):

```4d
$default:=TIMEZONE Get default
$e:=TIMEZONE Get display name($default; $displayName)
$e:=TIMEZONE Get region($default; $region)
$e:=TIMEZONE Get offset($default; $offset)
```

---

## TIMEZONE Get offset

### Syntax

```
TIMEZONE Get offset ( zone ; offset ) → Longint
```

| Parameter | Type | Description |
|---|---|---|
| `zone` | Text | Zone ID to look up |
| `offset` | Longint | *(output)* Raw (standard-time) offset from GMT, in seconds |
| Result | Longint | ICU status code |

### Description

Returns the zone's **standard** offset only — it does not account for daylight saving. If you need the offset that actually applies at a specific date/time (i.e. with DST folded in), use `TIME Get offset` instead, as the plugin's own test file notes.

### Example

From the plugin's own test method (`TEST.4dm`):

```4d
$gmt:=TIMEZONE Get GMT
 //use TIME Get offset to get daylight saving offset as well
$e:=TIMEZONE Get offset($gmt; $offset)
```

---

## TIMEZONE Get display name

### Syntax

```
TIMEZONE Get display name ( zone ; name ) → Longint
```

| Parameter | Type | Description |
|---|---|---|
| `zone` | Text | Zone ID to look up |
| `name` | Text | *(output)* Human-readable display name for the zone |
| Result | Longint | ICU status code |

### Description

Uses ICU's default display-name style and the process's default locale — there's no parameter to request a different locale or a short/long style variant from this command.

### Example

From the plugin's own test method (`TEST.4dm`):

```4d
$default:=TIMEZONE Get default
$e:=TIMEZONE Get display name($default; $displayName)
```

---

## TIMEZONE Get GMT

### Syntax

```
TIMEZONE Get GMT → Text
```

| Parameter | Type | Description |
|---|---|---|
| Result | Text | The GMT zone ID |

### Description

Takes no parameters. Like `TIMEZONE Get default`, the function result is the value itself, not a status code.

### Example

From the plugin's own test method (`TEST.4dm`):

```4d
$gmt:=TIMEZONE Get GMT
$e:=TIMEZONE Get display name($gmt; $displayName)
```

---

## NUMBER Format

### Syntax

```
NUMBER Format ( number ; string {; mode} {; locale} {; rule} ) → Longint
```

| Parameter | Type | Description |
|---|---|---|
| `number` | Real | Value to format |
| `string` | Text | *(output)* Formatted result |
| `mode` | Longint | Formatting mode constant. **Optional** — omitting it formats as spellout (per the plugin's own test/README comment: `//default.`) |
| `locale` | Text | ICU locale ID (e.g. `"fr_FR"`, `"en_US"`). **Optional** — empty/omitted uses the system default locale |
| `rule` | Text | Custom `RuleBasedNumberFormat` rule set. **Required only** when `mode` is the custom-rules mode; ignored otherwise |
| Result | Longint | ICU status code |

### Description

Backed by ICU's `RuleBasedNumberFormat`. The mode constants demonstrated in the plugin's own sample code are `NUMBER Spellout`, `NUMBER Ordinal`, `NUMBER Duration`, and `NUMBER Custom` — their exact numeric values aren't in a constants file provided alongside the source, so pass the named constant rather than a literal number. When `mode` selects the custom-rules variant, `rule` must be supplied and follow ICU's `RuleBasedNumberFormat` rule-set syntax; the rule text is user-authored, so keep it reasonably sized — a very large rule set costs proportionally more to parse.

### Example

From the plugin's own test method (`TEST.4dm`):

```4d
C_REAL:C285($number)
C_TEXT:C284($string)

$number:=Random:C100

$error:=NUMBER Format($number; $string)  //default.
$error:=NUMBER Format($number; $string; NUMBER Spellout)  //ex. nineteen thousand nine hundred forty-three
$error:=NUMBER Format($number; $string; NUMBER Duration)  //ex. 5:32:23
$error:=NUMBER Format($number; $string; NUMBER Ordinal)  //ex. 19,943rd

$error:=NUMBER Format($number; $string; NUMBER Spellout; "fr_FR")  //ex. vingt-deux-mille-sept-cent-quatorze
```

Custom rule set, verbatim from the same test method:

```4d
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

$error:=NUMBER Format($number; $string; NUMBER Custom; "en_US"; $rule)  //ex. seven, nine three one
```

---

## TIMEZONE GET LIST

### Syntax

```
TIMEZONE GET LIST ( list )
```

| Parameter | Type | Description |
|---|---|---|
| `list` | Array Text | *(output)* Every timezone ID ICU knows about on the running system |
| Result | *(none)* | This command has no function result — it's a procedure, not a function |

### Description

Populates `list` with the full set of IANA zone IDs ICU has data for (typically several hundred). Use this to validate a zone ID before passing it to any other command in this plugin, since — as noted in [Requirements & platform notes](#requirements--platform-notes) — an unrecognized zone ID is otherwise accepted silently rather than reported as an error.

### Example

```4d
ARRAY TEXT:C222($zones; 0)
TIMEZONE GET LIST($zones)
$found:=(Find in array:C230($zones; "America/New_York")#-1)
```

---

## Error handling & troubleshooting

- **A `Longint` result of `0` means success; nonzero is an ICU status code.** Positive values correspond to ICU error codes (e.g. `1` = `U_ILLEGAL_ARGUMENT_ERROR`); consult ICU's `UErrorCode` reference for the full list if you need to branch on a specific failure.
- **Bad zone IDs are not reported as errors on almost every command.** Passing an unrecognized zone string doesn't fail — it silently resolves to a fallback zone. If a conversion looks wrong, check the zone spelling against `TIMEZONE GET LIST` rather than trusting a `0` return code.
- **`TIME Date to absolute` always returns `0`,** even for bad input. Treat this specific command's return value as non-informative.
- **`TIMEZONE Get offset` ignores daylight saving.** If a computed offset looks off by an hour during DST season, you likely want `TIME Get offset` instead, which evaluates DST at a specific instant.
- **`TIMEZONE Get default` and `TIMEZONE Get GMT` return the zone ID directly as their function result**, not a `Longint` status — don't check these two the same way you'd check the rest of the plugin's commands.
- **Format constants (`TIME RFC Date`, `TIME ISO Date`, `NUMBER Spellout`, etc.) are plugin-defined 4D constants**, not literal text you type — their exact underlying values weren't available in the reviewed source. If a name from an example doesn't resolve in your project, check the plugin's constants list in 4D's Explorer.
- **`NUMBER Format`'s `mode` parameter defaults to spellout when omitted** — pass an explicit mode constant if you rely on a specific format rather than the default.
- **Custom `NUMBER Format` rule sets should be kept to a reasonable size.** The `rule` parameter is arbitrary user-authored text handed to ICU's rule parser; a very large or deeply nested rule set costs proportionally more time/memory to parse.

---

## Quick reference

```4d
 // absolute (ms since 1970-01-01 GMT) <-> date/time, and offsets
C_REAL($absolute); C_DATE($date); C_TIME($time); C_LONGINT($ms; $raw; $dst); C_TEXT($zone)

$e:=TIME Date to absolute($date; $time; $ms; "GMT"; $absolute)
$e:=TIME Date from absolute($absolute; $date; $time; $ms; "GMT")
$e:=TIME Get offset($absolute; "America/New_York"; $raw; $dst)

 // text <-> date/time / absolute
C_TEXT($out; $fmt)
$e:=TIME Date to text($date; $time; $ms; "GMT"; $fmt; $out)
$e:=TIME Date from text($out; $fmt; $date; $time; $ms; $zone)
$e:=TIME Absolute to text($absolute; $fmt; $out; "JST")
$e:=TIME Absolute from text($out; $fmt; $absolute)

 // zone metadata
C_TEXT($default; $displayName; $region)
$default:=TIMEZONE Get default
$e:=TIMEZONE Get display name($default; $displayName)
$e:=TIMEZONE Get region($default; $region)
$e:=TIMEZONE Get offset($default; $raw)
ARRAY TEXT:C222($zones; 0)
TIMEZONE GET LIST($zones)

 // number formatting
C_REAL($number); C_TEXT($string)
$e:=NUMBER Format($number; $string; NUMBER Spellout; "fr_FR")
```
