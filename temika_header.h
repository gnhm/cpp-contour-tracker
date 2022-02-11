// Reads movies and converts them to tiffs
// 13.11.2012
// Jurij Kotar
// Compile with gcc -o movie2tiff movie2tiff.c -O3 -Wall -ltiff

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <stdbool.h>
#include <libgen.h>

//
// Common camera defines
//
#define CAMERA_MOVIE_MAGIC 0x496d6554 // TemI
#define CAMERA_MOVIE_VERSION 1
#define CAMERA_TYPE_IIDC	1
#define CAMERA_TYPE_ANDOR	2
#define CAMERA_TYPE_XIMEA	3
#define CAMERA_PIXELMODE_MONO_8		1
#define CAMERA_PIXELMODE_MONO_16BE	2 // Big endian
#define CAMERA_PIXELMODE_MONO_16LE	3 // Little endian

//
// IIDC defines
//
#define IIDC_MOVIE_HEADER_LENGTH 172
// Feature modes
#define IIDC_FEATURE_MODE_OFF ( 1<<0 )
#define IIDC_FEATURE_MODE_RELATIVE ( 1<<1 )
#define IIDC_FEATURE_MODE_ABSOLUTE ( 1<<2 )
#define IIDC_FEATURE_MODE_AUTO ( 1<<3 )
#define IIDC_FEATURE_MODE_ONEPUSH ( 1<<4 )
#define IIDC_FEATURE_MODE_ADVANCED ( 1<<5 )
// Trigger
#define IIDC_TRIGGER_INTERNAL  -1
#define IIDC_TRIGGER_EXTERNAL0 0
#define IIDC_TRIGGER_EXTERNAL1 1
#define IIDC_TRIGGER_EXTERNAL15 7

//
// Andor defines
//
#define ANDOR_MOVIE_HEADER_LENGTH 128
// VS Speeds
#define ANDOR_VALUE_VS_SPEED_MIN 4
#define ANDOR_VALUE_VS_SPEED_MAX 0
#define ANDOR_VALUE_VS_SPEED_0_3 0
#define ANDOR_VALUE_VS_SPEED_0_5 1
#define ANDOR_VALUE_VS_SPEED_0_9 2
#define ANDOR_VALUE_VS_SPEED_1_7 3
#define ANDOR_VALUE_VS_SPEED_3_3 4
// VS Amplitudes
#define ANDOR_VALUE_VS_AMPLITUDE_MIN 0
#define ANDOR_VALUE_VS_AMPLITUDE_MAX 4
#define ANDOR_VALUE_VS_AMPLITUDE_0 0
#define ANDOR_VALUE_VS_AMPLITUDE_1 1
#define ANDOR_VALUE_VS_AMPLITUDE_2 2
#define ANDOR_VALUE_VS_AMPLITUDE_3 3
#define ANDOR_VALUE_VS_AMPLITUDE_4 4
// Shutter
#define ANDOR_VALUE_SHUTTER_AUTO 0
#define ANDOR_VALUE_SHUTTER_OPEN 1
#define ANDOR_VALUE_SHUTTER_CLOSE 2
// Cooler
#define ANDOR_VALUE_COOLER_OFF 0
#define ANDOR_VALUE_COOLER_ON 1
// Cooler mode
#define ANDOR_VALUE_COOLER_MODE_RETURN 0
#define ANDOR_VALUE_COOLER_MODE_MAINTAIN 1
// Fan
#define ANDOR_VALUE_FAN_FULL 0
#define ANDOR_VALUE_FAN_LOW 1
#define ANDOR_VALUE_FAN_OFF 2
// ADC
#define ANDOR_VALUE_ADC_14BIT 0
#define ANDOR_VALUE_ADC_16BIT 1
// Amplifier
#define ANDOR_VALUE_AMPLIFIER_EM 0
#define ANDOR_VALUE_AMPLIFIER_CON 1
// Preamp gain
#define ANDOR_VALUE_PREAMP_GAIN_1_0 0
#define ANDOR_VALUE_PREAMP_GAIN_2_4 1
#define ANDOR_VALUE_PREAMP_GAIN_5_1 2
// Trigger
#define ANDOR_VALUE_TRIGGER_INTERNAL  0
#define ANDOR_VALUE_TRIGGER_EXTERNAL 1
#define ANDOR_VALUE_TRIGGER_FAST_EXTERNAL -1 // Combination of external and SetFastExtTrigger
#define ANDOR_VALUE_TRIGGER_EXTERNAL_START 6
#define ANDOR_VALUE_TRIGGER_EXTERNAL_EXPOSURE  7
#define ANDOR_VALUE_TRIGGER_SOFTWARE  10

//
// Ximea defines
//
#define XIMEA_MOVIE_HEADER_LENGTH 240
#define XIMEA_TRIGGER_INTERNAL 0
#define XIMEA_TRIGGER_EXTERNAL 1
#define XIMEA_TRIGGER_SOFTWARE 3

//
// Common camera frame struct
//
struct camera_save_struct {
	//
	// Common stuff
	//
	uint32_t magic; // 'AndO'
	uint32_t version;
	uint32_t type; // Camera type
	uint32_t pixelmode; // Pixel mode
	uint32_t length_header; // Header data in bytes ( Everything except image data )
	uint32_t length_data; // Total data length in bytes;
};

//
// IIDC movie frame struct
//
union iidc_save_feature_value {
	uint32_t value;
	float absvalue;
};

struct iidc_save_struct {
	//
	// Common stuff
	//
	uint32_t magic; // 'TemI'
	uint32_t version;
	uint32_t type; // Camera type
	uint32_t pixelmode; // Pixel mode
	uint32_t length_header; // Header data in bytes ( Everything except image data )
	uint32_t length_data; // Total data length in bytes;

	//
	// Camera specific stuff
	//
	// Camera properties
	uint64_t i_guid;
	uint32_t i_vendor_id;
	uint32_t i_model_id;

	// Frame properties
	uint32_t i_video_mode;
	uint32_t i_color_coding;

	uint64_t i_timestamp; // microseconds

	uint32_t i_size_x_max; // Sensor size
	uint32_t i_size_y_max;
	uint32_t i_size_x; // Selected region
	uint32_t i_size_y;
	uint32_t i_pos_x;
	uint32_t i_pos_y;

	uint32_t i_pixnum; // Number of pixels
	uint32_t i_stride; // Number of bytes per image line
	uint32_t i_data_depth;  // Number of bits per pixel.

	uint32_t i_image_bytes; // Number of bytes used for the image (image data only, no padding)
	uint64_t i_total_bytes; // Total size of the frame buffer in bytes. May include packet multiple padding and intentional padding (vendor specific)

	// Features
	uint32_t i_brightness_mode; // Current mode
	union iidc_save_feature_value i_brightness; // Can be also float if mode is IIDC_FEATURE_MODE_ABSOLUTE (1<<2)

	uint32_t i_exposure_mode;
	union iidc_save_feature_value i_exposure;

	uint32_t i_gamma_mode;
	union iidc_save_feature_value i_gamma;

	uint32_t i_shutter_mode;
	union iidc_save_feature_value i_shutter;

	uint32_t i_gain_mode;
	union iidc_save_feature_value i_gain;

	uint32_t i_temperature_mode;
	union iidc_save_feature_value i_temperature;

	uint32_t i_trigger_delay_mode;
	union iidc_save_feature_value i_trigger_delay;

	int32_t i_trigger_mode;

	// Advanced features
	uint32_t i_avt_channel_balance_mode;
	int32_t i_avt_channel_balance;

	// Image data
	uint8_t *data;
} __attribute__((__packed__));

//
// Andor movie frame struct
//
struct andor_save_struct {
	//
	// Common stuff
	//
	uint32_t magic; // 'TemI'
	uint32_t version;
	uint32_t type; // Camera type
	uint32_t pixelmode; // Pixel mode
	uint32_t length_header; // Header data in bytes ( Everything except image data )
	uint32_t length_data; // Total data length in bytes;

	//
	// Camera specific stuff
	//
	// Timestamp
	uint64_t a_timestamp_sec;
	uint64_t a_timestamp_nsec;

	// Frame properties
	int32_t a_x_size_max; // Sensor size
	int32_t a_y_size_max;
	int32_t a_x_start; // Selected size and positions
	int32_t a_x_end;
	int32_t a_y_start;
	int32_t a_y_end;
	int32_t a_x_bin;
	int32_t a_y_bin;

	// Camera settings
	int32_t a_ad_channel; // ADC
	int32_t a_amplifier; // EM or classical preamplifier
	int32_t a_preamp_gain; // Preamplifier gain
	int32_t a_em_gain; // EM gain
	int32_t a_hs_speed; // HS speed
	int32_t a_vs_speed; // VS speed
	int32_t a_vs_amplitude; // VS amplitude
	float a_exposure; // Exposure time in seconds
	int32_t a_shutter; // Shutter
	int32_t a_trigger; // Trigger
	int32_t a_temperature; // Temperature
	int32_t a_cooler; // Cooler
	int32_t a_cooler_mode; // Cooler mode
	int32_t a_fan; // Fan

	//
	// Image data
	//
	uint8_t *data;
} __attribute__((__packed__));

//
// Ximea movie frame struct
//
struct ximea_save_struct {
	//
	// Common stuff
	//
	uint32_t magic; // 'TemI'
	uint32_t version;
	uint32_t type; // Camera type
	uint32_t pixelmode; // Pixel mode
	uint32_t length_header; // Header data in bytes ( Everything except image data )
	uint32_t length_data; // Total data length in bytes;

	//
	// Camera specific stuff
	//
	char x_name[100]; // Camera name
	uint32_t x_serial_number; // Serial number

	// Timestamp
	uint64_t x_timestamp_sec;
	uint64_t x_timestamp_nsec;

	// Sensor
	uint32_t x_size_x_max; // Sensor size
	uint32_t x_size_y_max;
	uint32_t x_size_x; // Selected region
	uint32_t x_size_y;
	uint32_t x_pos_x;
	uint32_t x_pos_y;

	//
	// Features
	//
	uint32_t x_exposure; // Exposure [us]
	float x_gain; // Gain [dB]
	uint32_t x_downsampling; // Downsampling, 1 1x1, 2 2x2
	uint32_t x_downsampling_type; // 0 binning, 1 skipping
	uint32_t x_bpc; // Bad Pixels Correction, 0 disabled, 1 enabled
	uint32_t x_lut; // Look up table, 0 disabled, 1 enabled
	uint32_t x_trigger; // Trigger

	// Automatic exposure/gain
	uint32_t x_aeag; // 0 disabled, 1 enabled
	float x_aeag_exposure_priority; // Priority of exposure versus gain 0.0 1.0
	uint32_t x_aeag_exposure_max_limit; // Maximum exposure time [us]
	float x_aeag_gain_max_limit; // Maximum gain [dB]
	uint32_t x_aeag_average_intensity; // Average intensity level [%]

	// High dynamic range
	uint32_t x_hdr; // 0 disabled, 1 enabled
	uint32_t x_hdr_t1; // Exposure time of the first slope [us]
	uint32_t x_hdr_t2; // Exposure time of the second slope [us]
	uint32_t x_hdr_t3; // Exposure time of the third slope [us]
	uint32_t x_hdr_kneepoint1; // Kneepoint 1 [%]
	uint32_t x_hdr_kneepoint2; // Kneepoint 2 [%]

	//
	// Image data
	//
	uint8_t *data;
} __attribute__((__packed__));

struct camera_frame_struct {
        uint64_t timestamp_sec; // Timestamp
        uint64_t timestamp_nsec;

        uint64_t pixelmode;

        uint32_t pixel_max; // maximum pixel value, 2^8-1 -> 2^16-1

        uint32_t size_x;
        uint32_t size_y;
        uint32_t pos_x;
        uint32_t pos_y;
        uint32_t max_size_x; // Sensor size
        uint32_t max_size_y;

        uint8_t image; //Image pointer // JAMIE - curious if this is causing my segmentation fault
	// original line, in case I change it: uint8_t *image;
};

void iidc_to_frame(struct iidc_save_struct *iidc, struct camera_frame_struct *frame) {

	//uint64_t i_timestamp; // microseconds
	frame->timestamp_sec = iidc->i_timestamp/1000000;
	frame->timestamp_nsec = (iidc->i_timestamp - (frame->timestamp_sec*1000000))*1000;

	frame->pixelmode = iidc->pixelmode; // Pixel mode
	frame->pixel_max = iidc->i_data_depth;

	frame->max_size_x = iidc->i_size_x_max; // Sensor size
	frame->max_size_y = iidc->i_size_y_max; // Sensor size
	frame->size_x = iidc->i_size_x; // Sensor size
	frame->size_y = iidc->i_size_y; // Sensor size
	frame->pos_x = iidc->i_pos_x;
	frame->pos_y = iidc->i_pos_y;

}

#define NAMELENGTH 100
