#define USAGE_PAGE(x) 0x05, x
#define USAGE_PAGE_DESKTOP 1
#define USAGE_PAGE_DIGITIZERS 0x0d

#define USAGE(x) 0x09, x
#define USAGE_PEN 0x02
#define USAGE_TOUCH_SCREEN 0x04
#define USAGE_STYLUS 0x20
#define USAGE_IN_RANGE 0x32
#define USAGE_CONFIDENCE 0x47
#define USAGE_TIP 0x42
#define USAGE_X 0x30
#define USAGE_Y 0x31

#define COLLECTION(x) 0xA1, x
#define COLLECTION_APPLICATION 0x01
#define COLLECTION_LOGICAL 0x02
#define END_COLLECTION() 0xc0

#define REPORT_SIZE(x) 0x75, x
#define REPORT_COUNT(x) 0x95, x

#define LOGICAL_MINIMUM_8(x) 0x15, x
#define LOGICAL_MAXIMUM_8(x) 0x25, x
#define LOGICAL_MINIMUM_16(x) 0x16, x&0xFF, x>>8
#define LOGICAL_MAXIMUM_16(x) 0x26, x&0xFF, x>>8

#define PHYSICAL_MINIMUM_8(x) 0x35, x
#define PHYSICAL_MAXIMUM_8(x) 0x45, x
#define PHYSICAL_MINIMUM_16(x) 0x36, x&0xFF, x>>8
#define PHYSICAL_MAXIMUM_16(x) 0x46, x&0xFF, x>>8

#define INPUT_DATA_VAR_ABS() 0x81, 0x2
#define PADDING() 0x81, 0x3
#define UNIT_EXPOENT() 0x55, 0x0e
#define UNIT() 0x65, 0x33
