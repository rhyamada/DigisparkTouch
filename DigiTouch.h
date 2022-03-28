/*
 * Based on Obdev's AVRUSB code and under the same license.
 *
 * TODO: Make a proper file header. :-)
 * Modified for Digispark by Digistump
 * And now modified by Sean Murphy (duckythescientist) from a keyboard device to a mouse device
 * Most of the credit for the joystick code should go to Rapha�l Ass�nat
 * And now mouse credit is due to Yiyin Ma and Abby Lin of Cornell
 */
#ifndef __DigiTouch_h__
#define __DigiTouch_h__
 
#include <Arduino.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <avr/delay.h>
#include <string.h>
#include <avr/io.h>
#include <avr/sleep.h>
#include <avr/wdt.h>

#include "usbdrv.h"
//#include "devdesc.h"
#include "oddebug.h"
#include "usbconfig.h"
#include "hidmacros.h"
// report frequency set to default of 50hz
#define DIGITOUCH_DEFAULT_REPORT_INTERVAL 20
static unsigned char idle_rate = DIGITOUCH_DEFAULT_REPORT_INTERVAL / 4; // in units of 4ms

// https://docs.microsoft.com/en-us/windows-hardware/design/component-guidelines/touchscreen-required-hid-top-level-collections
// https://docs.microsoft.com/en-us/windows-hardware/design/component-guidelines/pen-sample-report-descriptors
const PROGMEM unsigned char touch_usbHidReportDescriptor[] = { /* USB report descriptor */
	USAGE_PAGE(USAGE_PAGE_DIGITIZERS),
	USAGE(USAGE_TOUCH_SCREEN),
	COLLECTION(COLLECTION_APPLICATION),
		USAGE(USAGE_STYLUS),
		COLLECTION(COLLECTION_LOGICAL),
			LOGICAL_MINIMUM_8(0),
			LOGICAL_MAXIMUM_8(1),
			USAGE(USAGE_TIP),
			USAGE(USAGE_IN_RANGE),
			REPORT_SIZE(1),
			REPORT_COUNT(2),
			INPUT_DATA_VAR_ABS(),
			
			REPORT_COUNT(1),
			REPORT_SIZE(6),
			PADDING(),
			
			USAGE_PAGE(USAGE_PAGE_DESKTOP),
			UNIT_EXPOENT(),
			UNIT(),
			PHYSICAL_MINIMUM_8(0),
			LOGICAL_MAXIMUM_16(10000),
			REPORT_SIZE(16),

			PHYSICAL_MAXIMUM_16(10000),
			USAGE(USAGE_X),
			INPUT_DATA_VAR_ABS(),
			USAGE(USAGE_Y),
			INPUT_DATA_VAR_ABS(),

		END_COLLECTION(),
	END_COLLECTION()
};
//char (*__kaboom)[sizeof(touch_usbHidReportDescriptor)] = 1;
typedef char assertion_right_size[(sizeof(touch_usbHidReportDescriptor)==USB_CFG_HID_REPORT_DESCRIPTOR_LENGTH)*2-1];

typedef struct report_t{
	union {
		struct {
			uint8_t tip:1;
			uint8_t in_range:1;
			uint8_t confidence:1;
			uint8_t pad:4;
			uint8_t dirty:1;
		} bits;
		uint8_t byte;
	} tip;
	uint16_t x;
	uint16_t y;
};
static report_t reportBuffer;

#define TIP_BIT 0
#define IN_RANGE_BIT 1
#define CONFIDENCE_BIT 2

#define USBDESCR_DEVICE					1
const unsigned char usbDescrDevice[] PROGMEM = {		/* USB device descriptor */
		18,					/* sizeof(usbDescrDevice): length of descriptor in bytes */
		USBDESCR_DEVICE,		/* descriptor type */
		0x01, 0x01, /* USB version supported */
		USB_CFG_DEVICE_CLASS,
		USB_CFG_DEVICE_SUBCLASS,
		0,					/* protocol */
		8,					/* max packet size */
		USB_CFG_VENDOR_ID,	/* 2 bytes */
		USB_CFG_DEVICE_ID,	/* 2 bytes */
		USB_CFG_DEVICE_VERSION, /* 2 bytes */
#if USB_CFG_VENDOR_NAME_LEN
		1,					/* manufacturer string index */
#else
		0,					/* manufacturer string index */
#endif
#if USB_CFG_DEVICE_NAME_LEN
		2,					/* product string index */
#else
		0,					/* product string index */
#endif
#if USB_CFG_SERIAL_NUMBER_LENGTH
		3,					/* serial number string index */
#else
		0,					/* serial number string index */
#endif
		1,					/* number of configurations */
};
 
class DigiTouchDevice {
 unsigned long last_report_time;
 report_t report;
 public:
	DigiTouchDevice () {
		//rt_usbHidReportDescriptor = touch_usbHidReportDescriptor;
		//rt_usbHidReportDescriptorSize = sizeof(touch_usbHidReportDescriptor);
		//rt_usbDeviceDescriptor = usbDescrDevice;
		//rt_usbDeviceDescriptorSize = sizeof(usbDescrDevice);
		report.x = 0;
		report.y = 0;
		report.tip.byte = 0;
	}

	void begin() {
		cli();
		usbDeviceDisconnect();
		_delay_ms(200);
		usbDeviceConnect();			
		usbInit();		
		sei();
		last_report_time = millis();
	}

	void refresh() {
		update();
	}

	void poll() {
		update();
	}
	
	void update() {
		usbPoll();		
		// instead of above code, use millis arduino system to enforce minimum reporting frequency
		unsigned long time_since_last_report = millis() - last_report_time;
		if (time_since_last_report >= (idle_rate * 4 /* in units of 4ms - usb spec stuff */)) {
			last_report_time += idle_rate * 4;
			report.tip.bits.dirty = 1;
		}
		// if we want to send a report, signal the host computer to ask us for it with a usb 'interrupt'
		if (report.tip.bits.dirty) {
			if (usbInterruptIsReady()) {
				report.tip.bits.dirty=0;
				memcpy(&reportBuffer,&report,sizeof(report_t));
				usbSetInterrupt((uchar *)&reportBuffer, sizeof(report_t));
			}
		}
	}	
	void delay(long milli) {
		unsigned long last = millis();
	  while (milli > 0) {
	    unsigned long now = millis();
	    milli -= now - last;
	    last = now;
	    update();
	  }
	}
	
	void moveTo(uint16_t X, uint16_t Y, uint8_t T) {
		report.x = X;
		report.y = Y;
		report.tip.byte = T|0x80; // sets dirty bit
	}
};
// create the global singleton DigiTouch
DigiTouchDevice DigiTouch = DigiTouchDevice();


#ifdef __cplusplus
extern "C"{
#endif 
	// USB_PUBLIC uchar usbFunctionSetup	
	uchar usbFunctionSetup(uchar data[8]) {
		usbRequest_t *rq = (usbRequest_t *)data;	
		if ((rq->bmRequestType & USBRQ_TYPE_MASK) == USBRQ_TYPE_CLASS) {		/* class request type */
			if (rq->bRequest == USBRQ_HID_GET_REPORT) {	 /* wValue: ReportType (highbyte), ReportID (lowbyte) */
				/* we only have one report type, so don't look at wValue */
				usbMsgPtr = (uchar *)&reportBuffer;
				return sizeof(report_t);
			} else if (rq->bRequest == USBRQ_HID_GET_IDLE) {
				usbMsgPtr = &idle_rate;
				return 1;
			} else if (rq->bRequest == USBRQ_HID_SET_IDLE) {
				idle_rate = rq->wValue.bytes[1];
			}
		} else {
			/* no vendor specific requests implemented */
		}
		return 0;
	}
	
	uchar	usbFunctionDescriptor(struct usbRequest *rq) {
		if ((rq->bmRequestType & USBRQ_TYPE_MASK) != USBRQ_TYPE_STANDARD) {
			return 0;
		}

		if (rq->bRequest == USBRQ_GET_DESCRIPTOR) {
			// USB spec 9.4.3, high byte is descriptor type
			switch (rq->wValue.bytes[1]) {
				case USBDESCR_DEVICE:
					usbMsgPtr = usbDescrDevice;
					return sizeof(usbDescrDevice);
				case USBDESCR_HID_REPORT:
					usbMsgPtr = touch_usbHidReportDescriptor;
					return sizeof(touch_usbHidReportDescriptor);
			}
		}
		return 0;
	}

#ifdef __cplusplus
} // extern "C"
#endif

#endif // __DigiTouch_h__