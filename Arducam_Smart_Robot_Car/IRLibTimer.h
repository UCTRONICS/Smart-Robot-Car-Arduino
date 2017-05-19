#ifndef IRLibTimer_h
#define IRLibTimer_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#define IR_RECV_TIMER_OVERRIDE
//#define IR_RECV_TIMER1
#define IR_RECV_TIMER2









#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
	//#define IR_SEND_TIMER1	11
	//#define IR_SEND_TIMER2		9
	//#define IR_SEND_TIMER3	5
	//#define IR_SEND_TIMER4	6
	//#define IR_SEND_TIMER5	46
/* Teensy 1.0 */
#elif defined(__AVR_AT90USB162__)
	#define IR_SEND_TIMER1	17
/* Teensy 2.0 versus Leonardo These boards use the same chip but the 
 * pinouts are different.*/
#elif defined(__AVR_ATmega32U4__)
	#ifdef CORE_TEENSY
		// it's Teensy 2.0
		//#define IR_SEND_TIMER1	14
		//#define IR_SEND_TIMER3	9
		#define IR_SEND_TIMER4_HS	10
	#else
	/* it's probably Leonardo */
		#define IR_SEND_TIMER1		9
		//#define IR_SEND_TIMER3	5
		//#define IR_SEND_TIMER4_HS	13
	#endif
/* Teensy++ 1.0 & 2.0 */
#elif defined(__AVR_AT90USB646__) || defined(__AVR_AT90USB1286__)
	//#define IR_SEND_TIMER1	25
	#define IR_SEND_TIMER2		1
	//#define IR_SEND_TIMER3	16
/*  Sanguino */
#elif defined(__AVR_ATmega644P__) || defined(__AVR_ATmega644__)
	//#define IR_SEND_TIMER1	13
	#define IR_SEND_TIMER2		14
/* Pinoccio Scout */
#elif defined(__AVR_ATmega256RFR2__)
	#define IR_SEND_TIMER3		3
/* Arduino Duemilanove, Diecimila, LilyPad, Mini, Fio, etc */
#else
	#define IR_SEND_TIMER1	9
//	#define IR_SEND_TIMER2		3
#endif //end of setting IR_SEND_TIMER based on hardware detection

/* If you want to use bit-bang PWM output, you should un-comment the line below.
 * The definition must include the pin number for bit-bang output.  This could be any
 * available digital output pin. It need not be a designated PWM pin.
 * NOTE: By un-commenting this line, you are forcing the library to ignore
 * hardware detection and timer specifications above. The bit-bang frequency
 * code is not as accurate as using a hardware timer but it is more flexible and 
 * less hardware platform dependent.
 */
//#define IR_SEND_BIT_BANG  3  //Be sure to set this pin number if you un-comment

/* This is a fudge factor that adjusts bit-bang timing. Feel free to experiment
 * for best results.*/
#define IR_BIT_BANG_OVERHEAD 10




/* Everything below this point is a computation based on user settings above.
 * In all likelihood you would never need to modify anything below this point
 * unless you are adding some completely new feature or seriously modifying
 * the behavior of existing features. In other words don't try this at home.
 */
#if !defined(IR_RECV_TIMER_OVERRIDE)
	#if defined(IR_SEND_TIMER1)
		#define IR_RECV_TIMER1
	#elif defined(IR_SEND_TIMER2)
		#define IR_RECV_TIMER2
	#elif defined(IR_SEND_TIMER3)
		#define IR_RECV_TIMER3
	#elif defined(IR_SEND_TIMER4)
		#define IR_RECV_TIMER4
	#elif defined(IR_SEND_TIMER4_HS)
		#define IR_RECV_TIMER4_HS
	#elif defined(IR_SEND_TIMER5)
		#define IR_RECV_TIMER5
	#else
		#error "Unable to set IR_RECV_TIMER"
	#endif
#endif

#if defined(IR_SEND_BIT_BANG)  //defines for bit-bang output
	#define IR_SEND_PWM_PIN	IR_SEND_BIT_BANG
	#define IR_SEND_PWM_START   unsigned int jmax=time/iLength;\
		for(unsigned int j=0;j<jmax;j++) {\
		  digitalWrite(IR_SEND_BIT_BANG, HIGH);  delayMicroseconds(OnTime);\
		  digitalWrite(IR_SEND_BIT_BANG, LOW);   delayMicroseconds(OffTime);}
	#define IR_SEND_MARK_TIME(time) 
	#define IR_SEND_PWM_STOP
	#define IR_SEND_CONFIG_KHZ(val)  float Length=1000.0/(float)khz;\
		iLength=int(Length+0.5); OnTime=int(Length/3.0); \
		OffTime=iLength-OnTime-IR_BIT_BANG_OVERHEAD-(val<40);

#elif defined(IR_SEND_TIMER1) // defines for timer1 (16 bits)
	#define IR_SEND_PWM_START     (TCCR1A |= _BV(COM1A1))
	#define IR_SEND_MARK_TIME(time)  My_delay_uSecs(time)
	#define IR_SEND_PWM_STOP    (TCCR1A &= ~(_BV(COM1A1)))
	#define IR_SEND_CONFIG_KHZ(val) ({ \
		const uint16_t pwmval = SYSCLOCK / 2000 / (val); \
		TCCR1A = _BV(WGM11); TCCR1B = _BV(WGM13) | _BV(CS10); \
		ICR1 = pwmval; OCR1A = pwmval / 3; })
	#if defined(CORE_OC1A_PIN)
		#define IR_SEND_PWM_PIN        CORE_OC1A_PIN  /* Teensy */
	#else
		#define IR_SEND_PWM_PIN	IR_SEND_TIMER1
	#endif

#elif defined(IR_SEND_TIMER2)  // defines for timer2 (8 bits)
	#define IR_SEND_PWM_START     (TCCR2A |= _BV(COM2B1))
	#define IR_SEND_MARK_TIME(time)  My_delay_uSecs(time)
	#define IR_SEND_PWM_STOP    (TCCR2A &= ~(_BV(COM2B1)))
	#define IR_SEND_CONFIG_KHZ(val) ({ \
		const uint8_t pwmval = SYSCLOCK / 2000 / (val); \
		TCCR2A = _BV(WGM20);  TCCR2B = _BV(WGM22) | _BV(CS20); \
		OCR2A = pwmval; OCR2B = pwmval / 3; })
	#if defined(CORE_OC2B_PIN)
		#define IR_SEND_PWM_PIN        CORE_OC2B_PIN  /* Teensy */
	#else
		#define IR_SEND_PWM_PIN	IR_SEND_TIMER2
	#endif

#elif defined(IR_SEND_TIMER3) // defines for timer3 (16 bits)
	#define IR_SEND_PWM_START     (TCCR3A |= _BV(COM3A1))
	#define IR_SEND_MARK_TIME(time)  My_delay_uSecs(time)
	#define IR_SEND_PWM_STOP    (TCCR3A &= ~(_BV(COM3A1)))
	#define IR_SEND_CONFIG_KHZ(val) ({ \
		const uint16_t pwmval = SYSCLOCK / 2000 / (val); \
		TCCR3A = _BV(WGM31);   TCCR3B = _BV(WGM33) | _BV(CS30); \
		ICR3 = pwmval;   OCR3A = pwmval / 3; })
	#if defined(CORE_OC3A_PIN)
		#define IR_SEND_PWM_PIN        CORE_OC3A_PIN  /* Teensy */
	#else
		#define IR_SEND_PWM_PIN	IR_SEND_TIMER3
	#endif

#elif defined(IR_SEND_TIMER4_HS) // defines for timer4 (10 bits, high speed option)
	#define IR_SEND_PWM_START     (TCCR4A |= _BV(COM4A1))
	#define IR_SEND_MARK_TIME(time)  My_delay_uSecs(time)
	#define IR_SEND_PWM_STOP    (TCCR4A &= ~(_BV(COM4A1)))
	#define IR_SEND_CONFIG_KHZ(val) ({ \
		const uint16_t pwmval = SYSCLOCK / 2000 / (val); \
		TCCR4A = (1<<PWM4A);   TCCR4B = _BV(CS40); \
		TCCR4C = 0;   TCCR4D = (1<<WGM40); \
		TCCR4E = 0;   TC4H = pwmval >> 8; \
		OCR4C = pwmval;   TC4H = (pwmval / 3) >> 8;   OCR4A = (pwmval / 3) & 255; })
	#if defined(CORE_OC4A_PIN)
		#define IR_SEND_PWM_PIN        CORE_OC4A_PIN  /* Teensy */
	#else
		#define IR_SEND_PWM_PIN	IR_SEND_TIMER4_HS
	#endif

#elif defined(IR_SEND_TIMER4) // defines for timer4 (16 bits)
	#define IR_SEND_PWM_START     (TCCR4A |= _BV(COM4A1))
	#define IR_SEND_MARK_TIME(time)  My_delay_uSecs(time)
	#define IR_SEND_PWM_STOP    (TCCR4A &= ~(_BV(COM4A1)))
	#define IR_SEND_CONFIG_KHZ(val) ({ \
		const uint16_t pwmval = SYSCLOCK / 2000 / (val); \
		TCCR4A = _BV(WGM41); TCCR4B = _BV(WGM43) | _BV(CS40); \
		ICR4 = pwmval;   OCR4A = pwmval / 3; })
	#if defined(CORE_OC4A_PIN)
		#define IR_SEND_PWM_PIN        CORE_OC4A_PIN
	#else
		#define IR_SEND_PWM_PIN	IR_SEND_TIMER4
	#endif

#elif defined(IR_SEND_TIMER5) // defines for timer5 (16 bits)
	#define IR_SEND_PWM_START     (TCCR5A |= _BV(COM5A1))
	#define IR_SEND_MARK_TIME(time)  My_delay_uSecs(time)
	#define IR_SEND_PWM_STOP    (TCCR5A &= ~(_BV(COM5A1)))
	#define IR_SEND_CONFIG_KHZ(val) ({ \
		const uint16_t pwmval = SYSCLOCK / 2000 / (val); \
		TCCR5A = _BV(WGM51);   TCCR5B = _BV(WGM53) | _BV(CS50); \
		ICR5 = pwmval;   OCR5A = pwmval / 3; })
	#if defined(CORE_OC5A_PIN)
		#define IR_SEND_PWM_PIN        CORE_OC5A_PIN
	#else
		#define IR_SEND_PWM_PIN	IR_SEND_TIMER5
	#endif
#else // unknown timer
	#error "Internal code configuration error, no known IR_SEND_TIMER# defined\n"
#endif

/* This section sets up the 50 microsecond interval timer used by the
 * IRrecv receiver class. The various timers hhave already been selected 
 * earlier in this file. 
 */

#if defined(IR_RECV_TIMER1)  // defines for timer1 (16 bits)
	#define IR_RECV_ENABLE_INTR    (TIMSK1 = _BV(OCIE1A))
	#define IR_RECV_DISABLE_INTR   (TIMSK1 = 0)
	#define IR_RECV_INTR_NAME      TIMER1_COMPA_vect
	#define IR_RECV_CONFIG_TICKS() ({ \
		TCCR1A = 0;   TCCR1B = _BV(WGM12) | _BV(CS10); \
		OCR1A = SYSCLOCK * USECPERTICK / 1000000;   TCNT1 = 0; })

#elif defined(IR_RECV_TIMER2)  // defines for timer2 (8 bits)
	#define IR_RECV_ENABLE_INTR    (TIMSK2 = _BV(OCIE2A))
	#define IR_RECV_DISABLE_INTR   (TIMSK2 = 0)
	#define IR_RECV_INTR_NAME      TIMER2_COMPA_vect
	#define IR_RECV_COUNT_TOP      (SYSCLOCK * USECPERTICK / 1000000)
	#if (IR_RECV_COUNT_TOP < 256)
		#define IR_RECV_CONFIG_TICKS() ({ \
			TCCR2A = _BV(WGM21);  TCCR2B = _BV(CS20); \
			OCR2A = IR_RECV_COUNT_TOP;   TCNT2 = 0; })
	#else
		#define IR_RECV_CONFIG_TICKS() ({ \
			TCCR2A = _BV(WGM21);   TCCR2B = _BV(CS21); \
			OCR2A = IR_RECV_COUNT_TOP / 8;   TCNT2 = 0; })
	#endif

#elif defined(IR_RECV_TIMER3)  // defines for timer3 (16 bits)
	#define IR_RECV_ENABLE_INTR    (TIMSK3 = _BV(OCIE3A))
	#define IR_RECV_DISABLE_INTR   (TIMSK3 = 0)
	#define IR_RECV_INTR_NAME      TIMER3_COMPA_vect
	#define IR_RECV_CONFIG_TICKS() ({ \
		TCCR3A = 0;   TCCR3B = _BV(WGM32) | _BV(CS30); \
		OCR3A = SYSCLOCK * USECPERTICK / 1000000;   TCNT3 = 0; })

#elif defined(IR_RECV_TIMER4_HS)  // defines for timer4 (10 bits, high speed option)
	#define IR_RECV_ENABLE_INTR    (TIMSK4 = _BV(TOIE4))
	#define IR_RECV_DISABLE_INTR   (TIMSK4 = 0)
	#define IR_RECV_INTR_NAME      TIMER4_OVF_vect
	#define IR_RECV_CONFIG_TICKS() ({ \
		TCCR4A = 0;   TCCR4B = _BV(CS40); \
		TCCR4C = 0;   TCCR4D = 0; TCCR4E = 0; \
		TC4H = (SYSCLOCK * USECPERTICK / 1000000) >> 8; \
		OCR4C = (SYSCLOCK * USECPERTICK / 1000000) & 255; \
		TC4H = 0;   TCNT4 = 0; })

#elif defined(IR_RECV_TIMER4) // defines for timer4 (16 bits)
	#define IR_RECV_ENABLE_INTR    (TIMSK4 = _BV(OCIE4A))
	#define IR_RECV_DISABLE_INTR   (TIMSK4 = 0)
	#define IR_RECV_INTR_NAME      TIMER4_COMPA_vect
	#define IR_RECV_CONFIG_TICKS() ({ \
		TCCR4A = 0;   TCCR4B = _BV(WGM42) | _BV(CS40); \
		OCR4A = SYSCLOCK * USECPERTICK / 1000000;   TCNT4 = 0; })

#elif defined(IR_RECV_TIMER5)  // defines for timer5 (16 bits)
	#define IR_RECV_ENABLE_INTR    (TIMSK5 = _BV(OCIE5A))
	#define IR_RECV_DISABLE_INTR   (TIMSK5 = 0)
	#define IR_RECV_INTR_NAME      TIMER5_COMPA_vect
	#define IR_RECV_CONFIG_TICKS() ({ \
		TCCR5A = 0;   TCCR5B = _BV(WGM52) | _BV(CS50); \
		OCR5A = SYSCLOCK * USECPERTICK / 1000000;   TCNT5 = 0; })
#else // unknown timer
	#error "Internal code configuration error, no known IR_RECV_TIMER# defined\n"
#endif

// defines for blinking the LED
#if defined(CORE_LED0_PIN)
#define BLINKLED       CORE_LED0_PIN
#define BLINKLED_ON()  (digitalWrite(CORE_LED0_PIN, HIGH))
#define BLINKLED_OFF() (digitalWrite(CORE_LED0_PIN, LOW))
#elif defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
#define BLINKLED       13
#define BLINKLED_ON()  (PORTB |= B10000000)
#define BLINKLED_OFF() (PORTB &= B01111111)
#elif defined(__AVR_ATmega644P__) || defined(__AVR_ATmega644__)
#define BLINKLED       0
#define BLINKLED_ON()  (PORTD |= B00000001)
#define BLINKLED_OFF() (PORTD &= B11111110)
#elif defined(__AVR_ATmega32U4__) && defined(IR_SEND_TIMER4_HS)
//Leonardo not teensy. When using Timer4 output is on 13. Therefore disabling blink LED
//You can add an LED elsewhere if you want
#define BLINKLED       1
#define BLINKLED_ON()  (digitalWrite(BLINKLED, HIGH))
#define BLINKLED_OFF() (digitalWrite(BLINKLED, LOW))
#else
#define BLINKLED       13
#define BLINKLED_ON()  (PORTB |= B00100000)
#define BLINKLED_OFF() (PORTB &= B11011111)
#endif

#endif //IRLibTimer_h
