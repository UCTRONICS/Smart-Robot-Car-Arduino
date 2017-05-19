

#ifndef IRLibMatch_h
#define IRLibMatch_h

/*
 * Originally all timing comparisons for decoding were based on a percent of the
 * target value. However when target values are relatively large, the percent tolerance
 * is too much.  In some instances an absolute tolerance is needed. In order to maintain
 * backward compatibility, the default will be to continue to use percent. If you wish to default
 * to an absolute tolerance, you should comment out the line below.
 */
#define IRLIB_USE_PERCENT

/*
 * These are some miscellaneous definitions that are needed by the decoding routines. 
 * You need not include this file unless you are creating custom decode routines 
 * which will require these macros and definitions. Even if you include it, you probably
 * don't need to be intimately familiar with the internal details.
 */

#define USECPERTICK 50  // microseconds per clock interrupt tick
#define PERCENT_TOLERANCE 25  // percent tolerance in measurements
#define DEFAULT_ABS_TOLERANCE 75 //absolute tolerance in microseconds

/* 
 * These revised MATCH routines allow you to use either percentage or absolute tolerances.
 * Use ABS_MATCH for absolute and PERC_MATCH for percentages. The original MATCH macro
 * is controlled by the IRLIB_USE_PERCENT definition a few lines above.
 */
 
#define PERCENT_LOW(us) (unsigned int) (((us)*(1.0 - PERCENT_TOLERANCE/100.)))
#define PERCENT_HIGH(us) (unsigned int) (((us)*(1.0 + PERCENT_TOLERANCE/100.) + 1))

#define ABS_MATCH(v,e,t) ((v) >= ((e)-(t)) && (v) <= ((e)+(t)))
#define PERC_MATCH(v,e) ((v) >= PERCENT_LOW(e) && (v) <= PERCENT_HIGH(e))

#ifdef IRLIB_USE_PERCENT
#define MATCH(v,e) PERC_MATCH(v,e)
#else
#define MATCH(v,e) ABS_MATCH(v,e,DEFAULT_ABS_TOLERANCE)
#endif

//The following two routines are no longer necessary because mark/space adjustments are done elsewhere
//These definitions maintain backward compatibility.
#define MATCH_MARK(t,u) MATCH(t,u)
#define MATCH_SPACE(t,u) MATCH(t,u)

#ifdef IRLIB_TRACE
void IRLIB_ATTEMPT_MESSAGE(const __FlashStringHelper * s);
void IRLIB_TRACE_MESSAGE(const __FlashStringHelper * s);
byte IRLIB_REJECTION_MESSAGE(const __FlashStringHelper * s);
byte IRLIB_DATA_ERROR_MESSAGE(const __FlashStringHelper * s, unsigned char index, unsigned int value, unsigned int expected);
#define RAW_COUNT_ERROR IRLIB_REJECTION_MESSAGE(F("number of raw samples"));
#define HEADER_MARK_ERROR(expected) IRLIB_DATA_ERROR_MESSAGE(F("header mark"),offset,rawbuf[offset],expected);
#define HEADER_SPACE_ERROR(expected) IRLIB_DATA_ERROR_MESSAGE(F("header space"),offset,rawbuf[offset],expected);
#define DATA_MARK_ERROR(expected) IRLIB_DATA_ERROR_MESSAGE(F("data mark"),offset,rawbuf[offset],expected);
#define DATA_SPACE_ERROR(expected) IRLIB_DATA_ERROR_MESSAGE(F("data space"),offset,rawbuf[offset],expected);
#define TRAILER_BIT_ERROR(expected) IRLIB_DATA_ERROR_MESSAGE(F("RC5/RC6 trailer bit length"),offset,rawbuf[offset],expected);
#else
#define IRLIB_ATTEMPT_MESSAGE(s)
#define IRLIB_TRACE_MESSAGE(s)
#define IRLIB_REJECTION_MESSAGE(s) false
#define IRLIB_DATA_ERROR_MESSAGE(s,i,v,e) false
#define RAW_COUNT_ERROR false
#define HEADER_MARK_ERROR(expected) false
#define HEADER_SPACE_ERROR(expected) false
#define DATA_MARK_ERROR(expected) false
#define DATA_SPACE_ERROR(expected) false
#define TRAILER_BIT_ERROR(expected) false
#endif

#endif //IRLibMatch_h
