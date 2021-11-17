/*
 * uMTP Responder
 * Copyright (c) 2018 - 2020 Viveris Technologies
 *
 * uMTP Responder is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 3.0 of the License, or (at your option) any later version.
 *
 * uMTP Responder is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License version 3 for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with uMTP Responder; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

/**
 * @file   mtp_properties.c
 * @brief  MTP properties datasets helpers
 * @author Jean-François DEL NERO <Jean-Francois.DELNERO@viveris.fr>
 */

#include "buildconf.h"

#include <inttypes.h>
#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>

#include "mtp.h"
#include "mtp_helpers.h"
#include "mtp_constant.h"
#include "mtp_constant_strings.h"
#include "mtp_properties.h"

#include "fs_handles_db.h"
#include "usb_gadget_fct.h"

#include "logs_out.h"

formats_property capture_properties[]=
{   // prop_code                       data_type         getset    default value          group code
	{ MTP_FORMAT_EXIF_JPEG    , (uint16_t[]){   MTP_PROPERTY_STORAGE_ID, MTP_PROPERTY_OBJECT_FORMAT, MTP_PROPERTY_PROTECTION_STATUS, MTP_PROPERTY_OBJECT_SIZE,
												MTP_PROPERTY_OBJECT_FILE_NAME, MTP_PROPERTY_DATE_MODIFIED, MTP_PROPERTY_PARENT_OBJECT, MTP_PROPERTY_PERSISTENT_UID,
												MTP_PROPERTY_NAME, MTP_PROPERTY_DISPLAY_NAME, MTP_PROPERTY_DATE_CREATED, MTP_PROPERTY_DESCRIPTION, MTP_PROPERTY_WIDTH,
												MTP_PROPERTY_HEIGHT, MTP_PROPERTY_DATE_AUTHORED,
												0xFFFF}
	},
	{ MTP_FORMAT_DEFINED    , (uint16_t[]){   MTP_PROPERTY_STORAGE_ID, MTP_PROPERTY_OBJECT_FORMAT, MTP_PROPERTY_PROTECTION_STATUS, MTP_PROPERTY_OBJECT_SIZE,
												MTP_PROPERTY_OBJECT_FILE_NAME, MTP_PROPERTY_DATE_MODIFIED, MTP_PROPERTY_PARENT_OBJECT, MTP_PROPERTY_PERSISTENT_UID,
												MTP_PROPERTY_NAME, MTP_PROPERTY_DISPLAY_NAME, MTP_PROPERTY_DATE_CREATED, MTP_PROPERTY_DESCRIPTION, MTP_PROPERTY_WIDTH,
												MTP_PROPERTY_HEIGHT, MTP_PROPERTY_DATE_AUTHORED,
												0xFFFF}
	},
	{ 0xFFFF  , (uint16_t[]){ 0xFFFF } }

};

formats_property fmt_properties[]=
{   // prop_code                       data_type         getset    default value          group code
	{ MTP_FORMAT_UNDEFINED    , (uint16_t[]){   MTP_PROPERTY_STORAGE_ID, MTP_PROPERTY_OBJECT_FORMAT, MTP_PROPERTY_PROTECTION_STATUS, MTP_PROPERTY_OBJECT_SIZE,
												MTP_PROPERTY_OBJECT_FILE_NAME, MTP_PROPERTY_DATE_MODIFIED, MTP_PROPERTY_PARENT_OBJECT, MTP_PROPERTY_PERSISTENT_UID,
												MTP_PROPERTY_NAME, MTP_PROPERTY_DISPLAY_NAME, MTP_PROPERTY_DATE_CREATED,
												0xFFFF}
	},
	{ MTP_FORMAT_ASSOCIATION  , (uint16_t[]){   MTP_PROPERTY_STORAGE_ID, MTP_PROPERTY_OBJECT_FORMAT, MTP_PROPERTY_PROTECTION_STATUS, MTP_PROPERTY_OBJECT_SIZE,
												MTP_PROPERTY_OBJECT_FILE_NAME, MTP_PROPERTY_DATE_MODIFIED, MTP_PROPERTY_PARENT_OBJECT, MTP_PROPERTY_PERSISTENT_UID,
												MTP_PROPERTY_NAME, MTP_PROPERTY_DISPLAY_NAME, MTP_PROPERTY_DATE_CREATED,
												0xFFFF}
	},
	{ MTP_FORMAT_AVI          , (uint16_t[]){   MTP_PROPERTY_STORAGE_ID, MTP_PROPERTY_OBJECT_FORMAT, MTP_PROPERTY_PROTECTION_STATUS, MTP_PROPERTY_OBJECT_SIZE,
												MTP_PROPERTY_OBJECT_FILE_NAME, MTP_PROPERTY_DATE_MODIFIED, MTP_PROPERTY_PARENT_OBJECT, MTP_PROPERTY_PERSISTENT_UID,
												MTP_PROPERTY_NAME, MTP_PROPERTY_DISPLAY_NAME, MTP_PROPERTY_DATE_CREATED, MTP_PROPERTY_ARTIST, MTP_PROPERTY_ALBUM_NAME,
												MTP_PROPERTY_DURATION, MTP_PROPERTY_DESCRIPTION, MTP_PROPERTY_WIDTH, MTP_PROPERTY_HEIGHT, MTP_PROPERTY_DATE_AUTHORED,
												0xFFFF}
	},
	{ MTP_FORMAT_EXIF_JPEG    , (uint16_t[]){   MTP_PROPERTY_STORAGE_ID, MTP_PROPERTY_OBJECT_FORMAT, MTP_PROPERTY_PROTECTION_STATUS, MTP_PROPERTY_OBJECT_SIZE,
												MTP_PROPERTY_OBJECT_FILE_NAME, MTP_PROPERTY_DATE_MODIFIED, MTP_PROPERTY_PARENT_OBJECT, MTP_PROPERTY_PERSISTENT_UID,
												MTP_PROPERTY_NAME, MTP_PROPERTY_DISPLAY_NAME, MTP_PROPERTY_DATE_CREATED, MTP_PROPERTY_DESCRIPTION, MTP_PROPERTY_WIDTH,
												MTP_PROPERTY_HEIGHT, MTP_PROPERTY_DATE_AUTHORED,
												0xFFFF}
	},
	{ MTP_FORMAT_DEFINED    , (uint16_t[]){   MTP_PROPERTY_STORAGE_ID, MTP_PROPERTY_OBJECT_FORMAT, MTP_PROPERTY_PROTECTION_STATUS, MTP_PROPERTY_OBJECT_SIZE,
												MTP_PROPERTY_OBJECT_FILE_NAME, MTP_PROPERTY_DATE_MODIFIED, MTP_PROPERTY_PARENT_OBJECT, MTP_PROPERTY_PERSISTENT_UID,
												MTP_PROPERTY_NAME, MTP_PROPERTY_DISPLAY_NAME, MTP_PROPERTY_DATE_CREATED, MTP_PROPERTY_DESCRIPTION, MTP_PROPERTY_WIDTH,
												MTP_PROPERTY_HEIGHT, MTP_PROPERTY_DATE_AUTHORED,
												0xFFFF}
	},

	{ 0xFFFF  , (uint16_t[]){ 0xFFFF } }

};

profile_property properties[]=
{   // prop_code                       data_type         getset    default value          group code           format id
	{MTP_PROPERTY_STORAGE_ID,          MTP_TYPE_UINT32,    0x00,   0x00000000           , 0x000000001 , 0x00 , 0xFFFF },
	{MTP_PROPERTY_OBJECT_FORMAT,       MTP_TYPE_UINT16,    0x00,   MTP_FORMAT_UNDEFINED  , 0x000000000 , 0x00 , MTP_FORMAT_UNDEFINED },
	{MTP_PROPERTY_OBJECT_FORMAT,       MTP_TYPE_UINT16,    0x00,   MTP_FORMAT_ASSOCIATION, 0x000000000 , 0x00 , MTP_FORMAT_ASSOCIATION },
	{MTP_PROPERTY_OBJECT_FORMAT,       MTP_TYPE_UINT16,    0x00,   MTP_FORMAT_TEXT       , 0x000000000 , 0x00 , MTP_FORMAT_TEXT },
	{MTP_PROPERTY_OBJECT_FORMAT,       MTP_TYPE_UINT16,    0x00,   MTP_FORMAT_HTML       , 0x000000000 , 0x00 , MTP_FORMAT_HTML },
	{MTP_PROPERTY_OBJECT_FORMAT,       MTP_TYPE_UINT16,    0x00,   MTP_FORMAT_WAV        , 0x000000000 , 0x00 , MTP_FORMAT_WAV },
	{MTP_PROPERTY_OBJECT_FORMAT,       MTP_TYPE_UINT16,    0x00,   MTP_FORMAT_MP3        , 0x000000000 , 0x00 , MTP_FORMAT_MP3 },
	{MTP_PROPERTY_OBJECT_FORMAT,       MTP_TYPE_UINT16,    0x00,   MTP_FORMAT_MPEG       , 0x000000000 , 0x00 , MTP_FORMAT_MPEG },
	{MTP_PROPERTY_OBJECT_FORMAT,       MTP_TYPE_UINT16,    0x00,   MTP_FORMAT_EXIF_JPEG  , 0x000000000 , 0x00 , MTP_FORMAT_EXIF_JPEG },
	{MTP_PROPERTY_OBJECT_FORMAT,       MTP_TYPE_UINT16,    0x00,   MTP_FORMAT_BMP        , 0x000000000 , 0x00 , MTP_FORMAT_BMP },
	{MTP_PROPERTY_OBJECT_FORMAT,       MTP_TYPE_UINT16,    0x00,   MTP_FORMAT_AIFF       , 0x000000000 , 0x00 , MTP_FORMAT_AIFF },
	{MTP_PROPERTY_OBJECT_FORMAT,       MTP_TYPE_UINT16,    0x00,   MTP_FORMAT_MPEG       , 0x000000000 , 0x00 , MTP_FORMAT_MPEG },
	{MTP_PROPERTY_OBJECT_FORMAT,       MTP_TYPE_UINT16,    0x00,   MTP_FORMAT_WMA        , 0x000000000 , 0x00 , MTP_FORMAT_WMA },
	{MTP_PROPERTY_OBJECT_FORMAT,       MTP_TYPE_UINT16,    0x00,   MTP_FORMAT_OGG        , 0x000000000 , 0x00 , MTP_FORMAT_OGG },
	{MTP_PROPERTY_OBJECT_FORMAT,       MTP_TYPE_UINT16,    0x00,   MTP_FORMAT_AAC        , 0x000000000 , 0x00 , MTP_FORMAT_AAC },
	{MTP_PROPERTY_OBJECT_FORMAT,       MTP_TYPE_UINT16,    0x00,   MTP_FORMAT_MP4_CONTAINER                 , 0x000000000 , 0x00 , MTP_FORMAT_MP4_CONTAINER },
	{MTP_PROPERTY_OBJECT_FORMAT,       MTP_TYPE_UINT16,    0x00,   MTP_FORMAT_3GP_CONTAINER                 , 0x000000000 , 0x00 , MTP_FORMAT_3GP_CONTAINER },
	{MTP_PROPERTY_OBJECT_FORMAT,       MTP_TYPE_UINT16,    0x00,   MTP_FORMAT_ABSTRACT_AV_PLAYLIST          , 0x000000000 , 0x00 , MTP_FORMAT_ABSTRACT_AV_PLAYLIST },
	{MTP_PROPERTY_OBJECT_FORMAT,       MTP_TYPE_UINT16,    0x00,   MTP_FORMAT_WPL_PLAYLIST                  , 0x000000000 , 0x00 , MTP_FORMAT_WPL_PLAYLIST },
	{MTP_PROPERTY_OBJECT_FORMAT,       MTP_TYPE_UINT16,    0x00,   MTP_FORMAT_M3U_PLAYLIST                  , 0x000000000 , 0x00 , MTP_FORMAT_M3U_PLAYLIST },
	{MTP_PROPERTY_OBJECT_FORMAT,       MTP_TYPE_UINT16,    0x00,   MTP_FORMAT_PLS_PLAYLIST                  , 0x000000000 , 0x00 , MTP_FORMAT_PLS_PLAYLIST },
	{MTP_PROPERTY_OBJECT_FORMAT,       MTP_TYPE_UINT16,    0x00,   MTP_FORMAT_XML_DOCUMENT                  , 0x000000000 , 0x00 , MTP_FORMAT_XML_DOCUMENT },
	{MTP_PROPERTY_OBJECT_FORMAT,       MTP_TYPE_UINT16,    0x00,   MTP_FORMAT_FLAC                          , 0x000000000 , 0x00 , MTP_FORMAT_FLAC },
	{MTP_PROPERTY_OBJECT_FORMAT,       MTP_TYPE_UINT16,    0x00,   MTP_FORMAT_AVI                           , 0x000000000 , 0x00 , MTP_FORMAT_AVI },
	{MTP_PROPERTY_OBJECT_FORMAT,       MTP_TYPE_UINT16,    0x00,   MTP_FORMAT_ASF                           , 0x000000000 , 0x00 , MTP_FORMAT_ASF },
	{MTP_PROPERTY_OBJECT_FORMAT,       MTP_TYPE_UINT16,    0x00,   MTP_FORMAT_MS_WORD_DOCUMENT              , 0x000000000 , 0x00 , MTP_FORMAT_MS_WORD_DOCUMENT },
	{MTP_PROPERTY_OBJECT_FORMAT,       MTP_TYPE_UINT16,    0x00,   MTP_FORMAT_MS_EXCEL_SPREADSHEET          , 0x000000000 , 0x00 , MTP_FORMAT_MS_EXCEL_SPREADSHEET },
	{MTP_PROPERTY_OBJECT_FORMAT,       MTP_TYPE_UINT16,    0x00,   MTP_FORMAT_MS_POWERPOINT_PRESENTATION    , 0x000000000 , 0x00 , MTP_FORMAT_MS_POWERPOINT_PRESENTATION },

	{MTP_PROPERTY_OBJECT_SIZE,         MTP_TYPE_UINT64,    0x00,   0x0000000000000000 , 0x000000000 , 0x00 , MTP_FORMAT_ASSOCIATION },
	{MTP_PROPERTY_STORAGE_ID,          MTP_TYPE_UINT32,    0x00,   0x00000000         , 0x000000000 , 0x00 , MTP_FORMAT_ASSOCIATION },
	{MTP_PROPERTY_PROTECTION_STATUS,   MTP_TYPE_UINT16,    0x00,   0x0000             , 0x000000000 , 0x00 , MTP_FORMAT_ASSOCIATION },
	{MTP_PROPERTY_DISPLAY_NAME,        MTP_TYPE_STR,       0x00,   0x0000             , 0x000000000 , 0x00 , MTP_FORMAT_ASSOCIATION },
	{MTP_PROPERTY_OBJECT_FILE_NAME,    MTP_TYPE_STR,       0x01,   0x0000             , 0x000000000 , 0x00 , MTP_FORMAT_ASSOCIATION },
	{MTP_PROPERTY_DATE_CREATED,        MTP_TYPE_STR,       0x00,   0x00               , 0x000000000 , 0x00 , MTP_FORMAT_ASSOCIATION },
	{MTP_PROPERTY_DATE_MODIFIED,       MTP_TYPE_STR,       0x00,   0x00               , 0x000000000 , 0x00 , MTP_FORMAT_ASSOCIATION },
	{MTP_PROPERTY_PARENT_OBJECT,       MTP_TYPE_UINT32,    0x00,   0x00000000         , 0x000000000 , 0x00 , MTP_FORMAT_ASSOCIATION },
	{MTP_PROPERTY_PERSISTENT_UID,      MTP_TYPE_UINT128,   0x00,   0x00               , 0x000000000 , 0x00 , MTP_FORMAT_ASSOCIATION },
	{MTP_PROPERTY_NAME,                MTP_TYPE_STR,       0x00,   0x00               , 0x000000000 , 0x00 , MTP_FORMAT_ASSOCIATION },

	{MTP_PROPERTY_OBJECT_SIZE,         MTP_TYPE_UINT64,    0x00,   0x0000000000000000 , 0x000000000 , 0x00 , MTP_FORMAT_UNDEFINED },
	{MTP_PROPERTY_STORAGE_ID,          MTP_TYPE_UINT32,    0x00,   0x00000000         , 0x000000000 , 0x00 , MTP_FORMAT_UNDEFINED },
	{MTP_PROPERTY_PROTECTION_STATUS,   MTP_TYPE_UINT16,    0x00,   0x0000             , 0x000000000 , 0x00 , MTP_FORMAT_UNDEFINED },
	{MTP_PROPERTY_DISPLAY_NAME,        MTP_TYPE_STR,       0x00,   0x0000             , 0x000000000 , 0x00 , MTP_FORMAT_UNDEFINED },
	{MTP_PROPERTY_OBJECT_FILE_NAME,    MTP_TYPE_STR,       0x01,   0x0000             , 0x000000000 , 0x00 , MTP_FORMAT_UNDEFINED },
	{MTP_PROPERTY_DATE_CREATED,        MTP_TYPE_STR,       0x00,   0x00               , 0x000000000 , 0x00 , MTP_FORMAT_UNDEFINED },
	{MTP_PROPERTY_DATE_MODIFIED,       MTP_TYPE_STR,       0x00,   0x00               , 0x000000000 , 0x00 , MTP_FORMAT_UNDEFINED },
	{MTP_PROPERTY_PARENT_OBJECT,       MTP_TYPE_UINT32,    0x00,   0x00000000         , 0x000000000 , 0x00 , MTP_FORMAT_UNDEFINED },
	{MTP_PROPERTY_PERSISTENT_UID,      MTP_TYPE_UINT128,   0x00,   0x00               , 0x000000000 , 0x00 , MTP_FORMAT_UNDEFINED },
	{MTP_PROPERTY_NAME,                MTP_TYPE_STR,       0x00,   0x00               , 0x000000000 , 0x00 , MTP_FORMAT_UNDEFINED },

	//{MTP_PROPERTY_ASSOCIATION_TYPE,    MTP_TYPE_UINT16,    0x00,   0x0001             , 0x000000000 , 0x00 , 0xFFFF },
	{MTP_PROPERTY_ASSOCIATION_DESC,    MTP_TYPE_UINT32,    0x00,   0x00000000         , 0x000000000 , 0x00 , 0xFFFF },
	{MTP_PROPERTY_PROTECTION_STATUS,   MTP_TYPE_UINT16,    0x00,   0x0000             , 0x000000000 , 0x00 , 0xFFFF },
	{MTP_PROPERTY_HIDDEN,              MTP_TYPE_UINT16,    0x00,   0x0000             , 0x000000000 , 0x00 , 0xFFFF },

	{0xFFFF,                           MTP_TYPE_UINT32,    0x00,   0x00000000         , 0x000000000 , 0x00 }
};

profile_property dev_properties[]=
{
	{
		prop_code: MTP_DEVICE_PROPERTY_BATTERY_LEVEL,
		data_type: MTP_TYPE_UINT8,
		getset: 0x00,
		default_value: 0x64,
		current_value: 0x3C,
		form_flag: 0x01,
		format_id: MTP_TYPE_UINT8,
		nbparam: 3,
		extra: {0x00, 0x64, 0x01}
	},
	/*
	{
		prop_code: MTP_DEVICE_PROPERTY_FOCAL_LENGTH,
		data_type: MTP_TYPE_UINT16,
		getset: 0x00,
		default_value: 0x00,
		current_value: 0x00,
		form_flag: 0x00,
	},
	{
		prop_code: MTP_DEVICE_PROPERTY_F_NUMBER,
		data_type: MTP_TYPE_UINT16,
		getset: 0x00,
		default_value: 0x00,
		current_value: 0x00,
		form_flag: 0x00,
	},
	*/
	{
		prop_code: MTP_DEVICE_PROPERTY_DEVICE_ICON,
		data_type: MTP_TYPE_UINT8,
		getset: 0x00,
		default_value: 0x01,
		current_value: 0x01,
		form_flag: 0x00,
	},
	{
		prop_code: MTP_DEVICE_PROPERTY_EXPOSURE_METERING_MODE,
		data_type: MTP_TYPE_UINT16,
		getset: 0x00,
		default_value: 0x01,
		current_value: 0x01,
		form_flag: 0x02,
		format_id: MTP_TYPE_UINT16,
		nbparam: 1,
		extra: {1}
	},
	{
		prop_code: MTP_DEVICE_PROPERTY_WHITE_BALANCE,
		data_type: MTP_TYPE_UINT16,
		getset: 0x01,
		default_value: 0x02,
		current_value: 0x02,
		form_flag: 0x02,
		format_id: MTP_TYPE_UINT16,
		nbparam: 5,
		extra: {1, 2, 4, 5, 6}
	},
	/*
	{
		prop_code: MTP_DEVICE_PROPERTY_STILL_CAPTURE_MODE,
		data_type: MTP_TYPE_UINT16,
		getset: 0x00,
		default_value: 0x01,
		current_value: 0x01,
		form_flag: 0x02,
		format_id: MTP_TYPE_UINT16,
		nbparam: 1,
		extra: {1}
	},
	*/
	{
		prop_code: MTP_DEVICE_PROPERTY_EXPOSURE_PROGRAM_MODE,
		data_type: MTP_TYPE_UINT16,
		getset: 0x01,
		default_value: 0x02,
		current_value: 0x01,
		form_flag: 0x02,
		format_id: MTP_TYPE_UINT16,
		nbparam: 2,
		extra: {1, 2}
	},
	{
		prop_code: MTP_DEVICE_PROPERTY_COMPRESSION_SETTING,
		data_type: MTP_TYPE_UINT8,
		getset: 0x01,
		default_value: 0x01,
		current_value: 0x01,
		form_flag: 0x02,
		format_id: MTP_TYPE_UINT8,
		nbparam: 4,
		extra: {0x00, 0x01, 0x02, 0x04}
	},
	{
		prop_code: MTP_DEVICE_PROPERTY_EXPOSURE_INDEX,
		data_type: MTP_TYPE_UINT16,
		getset: 0x01,
		default_value: 200,
		current_value: 200,
		form_flag: 0x02,
		format_id: MTP_TYPE_UINT16,
		nbparam: 19,
		extra: {100, 200, 300, 400, 500, 600, 700, 800, 900, 1000, 1100, 1200, 1300, 1400, 1500, 1600, 3200, 4800, 6400}
	},
	{
		prop_code: MTP_DEVICE_PROPERTY_EXPOSURE_TIME,
		data_type: MTP_TYPE_UINT32,
		getset: 0x01,
		default_value: 0x00000028,
		current_value: 0x00000028,
		form_flag: 0x02,
		format_id: MTP_TYPE_UINT32,
		nbparam: 60,
		extra: {1, 2, 3, 4, 5, 6, 8, 10, 12, 13, 15, 20, 25, 28, 31, 40, 50, 55, 62, 80, 100, 111, 125, 166, 200, 222, 250, 333, 400, 500, 666, 769, 1000, 1250, 1666, 2000, 2500, 3333, 4000, 5000, 6250, 6666, 7692, 10000, 13000, 15000, 16000, 20000, 25000, 30000, 40000, 50000, 60000, 80000, 100000, 130000, 150000, 200000, 250000, 300000}
	},

	// String types are VERY painful. Work out how to make them easier
	{
		prop_code: MTP_DEVICE_PROPERTY_IMAGE_SIZE,
		data_type: MTP_TYPE_STR,
		getset: 0x01,
		default_value_str: {0xA, 0x33, 0x00, 0x32, 0x00, 0x38, 0x00, 0x30, 0x00, 0x78, 0x00, 0x32, 0x00, 0x34, 0x00, 0x36, 0x00, 0x34, 0x00, 0x00, 0x00},
		current_value_str: {0xA, 0x33, 0x00, 0x32, 0x00, 0x38, 0x00, 0x30, 0x00, 0x78, 0x00, 0x32, 0x00, 0x34, 0x00, 0x36, 0x00, 0x34, 0x00, 0x00, 0x00},
		form_flag: 0x02,
		format_id: MTP_TYPE_STR,
		nbparam: 44, 
		extra: {0x02, 0x00, 0xA, 0x32, 0x00, 0x30, 0x00, 0x32, 0x00, 0x38, 0x00, 0x78, 0x00, 0x31, 0x00, 0x35, 0x00, 0x32, 0x00, 0x30, 0x00, 0x00, 0x00, 0xA, 0x33, 0x00, 0x32, 0x00, 0x38, 0x00, 0x30, 0x00, 0x78, 0x00, 0x32, 0x00, 0x34, 0x00, 0x36, 0x00, 0x34, 0x00, 0x00, 0x00},
	},

	/*
	{
		prop_code: MTP_DEVICE_PROPERTY_NIKON_RECORDINGMEDIA,
		data_type: MTP_TYPE_UINT8,
		getset: 0x01,
		default_value: 0x0,
		current_value: 0x0,
		form_flag: 0x01,
		format_id: MTP_TYPE_UINT8,
		nbparam: 3,
		extra: {0x00, 0x1, 0x01} 
	},
	{
		prop_code: MTP_DEVICE_PROPERTY_NIKON_AFMODESELECT,
		data_type: MTP_TYPE_UINT8,
		getset: 0x01,
		default_value: 0x02,
		current_value: 0x03,
		form_flag: 0x01,
		format_id: MTP_TYPE_UINT8,
		nbparam: 3,
		extra: {0x00, 0x04, 0x01}
	},
	{
		prop_code: MTP_DEVICE_PROPERTY_NIKON_SHUTTERSPEED,
		data_type: MTP_TYPE_UINT32,
		getset: 0x01,
		default_value: 69536,
		current_value: 65696,
		form_flag: 0x02,
		format_id: MTP_TYPE_UINT32,
		nbparam: 13,
		extra: {69536, 68736, 68036, 67536, 67136, 66786, 66536, 66336, 66176, 66036, 65936, 65856, 65786, 65736, 65696, 65661, 65636, 65616, 65596, 65586, 65576, 65566, 65561, 65556, 65551, 65549, 65546, 65544, 65542, 65541, 65540, 65539, 655385, 65538, 655376, 655373, 65537, 851978, 1048586, 131073, 1638410, 196609, 262145, 327681, 393217, 524289, 655361, 851969, 983041, 1310721, 1638401, 1966081, 4294967295}
	},
	{
		prop_code: MTP_DEVICE_PROPERTY_NIKON_LIVEVIEWSTATUS,
		data_type: MTP_TYPE_UINT8,
		getset: 0x00,
		default_value: 0x0,
		current_value: 0x0,
		form_flag: 0x01,
		format_id: MTP_TYPE_UINT8,
		nbparam: 3,
		extra: {0x00, 0x01, 0x01} 
	},
	{
		prop_code: MTP_DEVICE_PROPERTY_NIKON_AFMODESELECT,
		data_type: MTP_TYPE_UINT8,
		getset: 0x01,
		default_value: 0x2,
		current_value: 0x3,
		form_flag: 0x01,
		format_id: MTP_TYPE_UINT8,
		nbparam: 3,
		extra: {0x00, 0x4, 0x01} 
	},
	{
		prop_code: MTP_DEVICE_PROPERTY_NIKON_RECORDINGMEDIA,
		data_type: MTP_TYPE_UINT8,
		getset: 0x01,
		default_value: 0x0,
		current_value: 0x1,
		form_flag: 0x01,
		format_id: MTP_TYPE_UINT8,
		nbparam: 3,
		extra: {0x00, 0x1, 0x01} 
	},
	{
		prop_code: MTP_DEVICE_PROPERTY_NIKON_WBCOLORTEMP,
		data_type: MTP_TYPE_UINT8,
		getset: 0x01,
		default_value: 0x20,
		current_value: 0x21,
		form_flag: 0x01,
		format_id: MTP_TYPE_UINT8,
		nbparam: 3,
		extra: {0x01, 0x1e, 0x01} 
	},
	{
		prop_code: MTP_DEVICE_PROPERTY_NIKON_USBSPEED,
		data_type: MTP_TYPE_UINT8,
		getset: 0x00,
		default_value: 0x01,
		current_value: 0x01,
		form_flag: 0x00,
	},
	{
		prop_code: MTP_DEVICE_PROPERTY_NIKON_EXTERNALDCIN,
		data_type: MTP_TYPE_UINT8,
		getset: 0x01,
		default_value: 0x00,
		current_value: 0x00,
		form_flag: 0x01,
		format_id: MTP_TYPE_UINT8,
		nbparam: 3,
		extra: {0x00, 0x01, 0x01}
	},
	{
		prop_code: 0xd1a8,
		data_type: MTP_TYPE_UINT16,
		getset: 0x01,
		default_value: 73536,
		current_value: 73536,
		form_flag: 0x02,
		format_id: MTP_TYPE_UINT16,
		nbparam: 22,
		extra: {73536,71936,70536,69536,68736,68036,67536,67136,66786,66536,66336,66176,66036,65936,65856,65786,65736,65696,65661,65636,65616,65596}
	},
	*/

	{
		prop_code: 0xFFFF
	}
};

int build_properties_dataset(mtp_ctx * ctx,void * buffer, int maxsize,uint32_t property_id,uint32_t format_id)
{
	int ofs,i,j;

	ofs = 0;

	PRINT_DEBUG("build_properties_dataset : 0x%.4X (%s) (Format : 0x%.4X - %s )", property_id, mtp_get_property_string(property_id), format_id, mtp_get_format_string(format_id));

	i = 0;
	while(properties[i].prop_code != 0xFFFF )
	{
		if( ( properties[i].prop_code == property_id ) && ( properties[i].format_id == format_id ) )
		{
			break;
		}
		i++;
	}

	if( properties[i].prop_code == 0xFFFF )
	{
		// Looking for default value
		i = 0;
		while(properties[i].prop_code != 0xFFFF )
		{
			if( ( properties[i].prop_code == property_id ) && ( properties[i].format_id == 0xFFFF ) )
			{
				break;
			}
			i++;
		}
	}

	if( properties[i].prop_code == property_id )
	{
		ofs = poke16(buffer, ofs, maxsize, properties[i].prop_code);            // PropertyCode
		ofs = poke16(buffer, ofs, maxsize, properties[i].data_type);            // DataType
		ofs = poke08(buffer, ofs, maxsize, properties[i].getset);               // Get / Set

		switch(properties[i].data_type)
		{

			case MTP_TYPE_STR:
			case MTP_TYPE_UINT8:
				ofs = poke08(buffer, ofs, maxsize, properties[i].default_value);                         // DefaultValue
			break;

			case MTP_TYPE_UINT16:
				ofs = poke16(buffer, ofs, maxsize, properties[i].default_value);                         // DefaultValue
			break;

			case MTP_TYPE_UINT32:
				ofs = poke32(buffer, ofs, maxsize, properties[i].default_value);                         // DefaultValue
			break;

			case MTP_TYPE_UINT64:
				ofs = poke32(buffer, ofs, maxsize, properties[i].default_value & 0xFFFFFFFF);            // DefaultValue
				ofs = poke32(buffer, ofs, maxsize, properties[i].default_value >> 32);
			break;

			case MTP_TYPE_UINT128:
				for(j=0;j<4;j++)
				{
					ofs = poke32(buffer, ofs, maxsize, properties[i].default_value);
				}
			break;

			default:
				PRINT_ERROR("build_properties_dataset : Unsupported data type : 0x%.4X", properties[i].data_type );
			break;

		}

		ofs = poke32(buffer, ofs, maxsize, properties[i].current_value);        // Current value
		ofs = poke08(buffer, ofs, maxsize, properties[i].form_flag);            // Form flag
	}

	return ofs;
}

int build_device_properties_dataset(mtp_ctx * ctx,void * buffer, int maxsize,uint32_t property_id)
{
	int ofs,i, j, a;
	uint8_t l;

	ofs = 0;

	PRINT_DEBUG("build_device_properties_dataset : 0x%.4X (%s)", property_id, mtp_get_property_string(property_id));

	i = 0;
	while(dev_properties[i].prop_code != 0xFFFF && dev_properties[i].prop_code != property_id)
	{
		i++;
	}

	if( dev_properties[i].prop_code == property_id )
	{
		ofs = poke16(buffer, ofs, maxsize, dev_properties[i].prop_code);            // PropertyCode
		ofs = poke16(buffer, ofs, maxsize, dev_properties[i].data_type);            // DataType
		ofs = poke08(buffer, ofs, maxsize, dev_properties[i].getset);               // Get / Set

		switch(dev_properties[i].data_type)
		{

			case MTP_TYPE_STR:
				// default value
				j = 0;
				l = dev_properties[i].default_value_str[0];
				ofs = poke08(buffer, ofs, maxsize, l);
				for (j = 1; j < (l * 2) + 1; j++) {
					ofs = poke08(buffer, ofs, maxsize, dev_properties[i].default_value_str[j]);
				}

				//current value
				j = 0;
				l = dev_properties[i].current_value_str[0];
				ofs = poke08(buffer, ofs, maxsize, l);
				for (j = 1; j < (l * 2) + 1; j++) {
					ofs = poke08(buffer, ofs, maxsize, dev_properties[i].current_value_str[j]);
				}

			case MTP_TYPE_UINT8:
				ofs = poke08(buffer, ofs, maxsize, dev_properties[i].default_value);
				ofs = poke08(buffer, ofs, maxsize, dev_properties[i].current_value);     
			break;

			case MTP_TYPE_UINT16:
				ofs = poke16(buffer, ofs, maxsize, dev_properties[i].default_value);
				ofs = poke16(buffer, ofs, maxsize, dev_properties[i].current_value);     
			break;

			case MTP_TYPE_UINT32:
				ofs = poke32(buffer, ofs, maxsize, dev_properties[i].default_value);
				ofs = poke32(buffer, ofs, maxsize, dev_properties[i].current_value);     
			break;

			case MTP_TYPE_UINT64:
				ofs = poke32(buffer, ofs, maxsize, dev_properties[i].default_value & 0xFFFFFFFF);
				ofs = poke32(buffer, ofs, maxsize, dev_properties[i].default_value >> 32);
				ofs = poke32(buffer, ofs, maxsize, dev_properties[i].current_value & 0xFFFFFFFF);     
				ofs = poke32(buffer, ofs, maxsize, dev_properties[i].current_value >> 32);     
			break;

			default:
				return 0;
			break;

		}

		ofs = poke08(buffer, ofs, maxsize, dev_properties[i].form_flag);            // Form flag

		if (dev_properties[i].form_flag != 0x00) {
			if (dev_properties[i].form_flag == 0x02 && dev_properties[i].data_type != MTP_TYPE_STR)
				ofs = poke16(buffer, ofs, maxsize, dev_properties[i].nbparam);


			for (a = 0; a < dev_properties[i].nbparam; a++) 
			{
				switch(dev_properties[i].format_id)
				{

					case MTP_TYPE_STR:
					case MTP_TYPE_UINT8:
					ofs = poke08(buffer, ofs, maxsize, dev_properties[i].extra[a]);
					break;

					case MTP_TYPE_UINT16:
					ofs = poke16(buffer, ofs, maxsize, dev_properties[i].extra[a]);
					break;

					case MTP_TYPE_UINT32:
					ofs = poke32(buffer, ofs, maxsize, dev_properties[i].extra[a]);
					break;

					default:
						return 0;
					break;
				}
			}
			
		}
	}

	return ofs;
}


int build_properties_supported_dataset(mtp_ctx * ctx,void * buffer, int maxsize,uint32_t format_id)
{
	int ofs,i,fmt_index;
	int nb_supported_prop;

	PRINT_DEBUG("build_properties_supported_dataset : (Format : 0x%.4X - %s )", format_id, mtp_get_format_string(format_id));

	fmt_index = 0;
	while(fmt_properties[fmt_index].format_code != 0xFFFF && fmt_properties[fmt_index].format_code != format_id )
	{
		fmt_index++;
	}

	if( fmt_properties[fmt_index].format_code == 0xFFFF )
		return 0;

	nb_supported_prop = 0;

	while( fmt_properties[fmt_index].properties[nb_supported_prop] != 0xFFFF )
		nb_supported_prop++;

	i = 0;

	ofs = poke32(buffer, 0, maxsize, nb_supported_prop);
	while( fmt_properties[fmt_index].properties[i] != 0xFFFF )
	{
		ofs = poke16(buffer, ofs, maxsize, fmt_properties[fmt_index].properties[i]);
		i++;
	}

	return ofs;
}

int setObjectPropValue(mtp_ctx * ctx,MTP_PACKET_HEADER * mtp_packet_hdr, uint32_t handle,uint32_t prop_code)
{
	fs_entry * entry;
	char * path;
	char * path2;
	char * old_filename;
	char tmpstr[256+1];
	unsigned int stringlen;
	uint32_t response_code;

	PRINT_DEBUG("setObjectPropValue : (Handle : 0x%.8X - Prop code : 0x%.4X )", handle, prop_code);

	response_code = 0x00000000;

	if( handle != 0xFFFFFFFF )
	{
		switch( prop_code )
		{
			case MTP_PROPERTY_OBJECT_FILE_NAME:
				entry = get_entry_by_handle(ctx->fs_db, handle);

				if( check_handle_access( ctx, entry, 0x00000000, 1, &response_code) )
					return response_code;

				path = build_full_path(ctx->fs_db, mtp_get_storage_root(ctx, entry->storage_id), entry);
				if(!path)
					return MTP_RESPONSE_GENERAL_ERROR;

				memset(tmpstr,0,sizeof(tmpstr));

				stringlen = peek(mtp_packet_hdr, sizeof(MTP_PACKET_HEADER), 1);

				if( stringlen > sizeof(tmpstr))
					stringlen = sizeof(tmpstr);

				unicode2charstring(tmpstr, (uint16_t *) ((char*)(mtp_packet_hdr) + sizeof(MTP_PACKET_HEADER) + 1), sizeof(tmpstr));
				tmpstr[ sizeof(tmpstr) - 1 ] = 0;

				old_filename = entry->name;

				entry->name = malloc(strlen(tmpstr)+1);
				if( entry->name )
				{
					strcpy(entry->name,tmpstr);
				}
				else
				{
					entry->name = old_filename;
					return MTP_RESPONSE_GENERAL_ERROR;
				}

				path2 = build_full_path(ctx->fs_db, mtp_get_storage_root(ctx, entry->storage_id), entry);
				if(!path2)
				{
					if( old_filename )
					{
						if(entry->name)
							free(entry->name);

						entry->name = old_filename;
					}

					free(path);
					return MTP_RESPONSE_GENERAL_ERROR;
				}

				if(rename(path, path2))
				{
					PRINT_ERROR("setObjectPropValue : Can't rename %s to %s", path, path2);

					if( old_filename )
					{
						if(entry->name)
							free(entry->name);

						entry->name = old_filename;
					}

					free(path);
					free(path2);
					return MTP_RESPONSE_GENERAL_ERROR;
				}

				free(path);
				free(path2);
				return MTP_RESPONSE_OK;
			break;

			default:
				return MTP_RESPONSE_INVALID_OBJECT_PROP_CODE;
			break;
		}
	}
	else
	{
		return MTP_RESPONSE_INVALID_OBJECT_HANDLE;
	}
}

int build_ObjectPropValue_dataset(mtp_ctx * ctx,void * buffer, int maxsize,uint32_t handle,uint32_t prop_code)
{
	int ofs;
	fs_entry * entry;
	char timestr[32];

	ofs = 0;

	PRINT_DEBUG("build_ObjectPropValue_dataset : Handle 0x%.8X - Property 0x%.4X (%s)", handle, prop_code, mtp_get_property_string(prop_code));

	entry = get_entry_by_handle(ctx->fs_db, handle);
	if( entry )
	{
		switch(prop_code)
		{
			case MTP_PROPERTY_OBJECT_FORMAT:
				if(entry->flags & ENTRY_IS_DIR)
					ofs = poke16(buffer, ofs, maxsize, MTP_FORMAT_ASSOCIATION);                          // ObjectFormat Code
				else
					ofs = poke16(buffer, ofs, maxsize, MTP_FORMAT_UNDEFINED);                            // ObjectFormat Code
			break;

			case MTP_PROPERTY_OBJECT_SIZE:
				ofs = poke32(buffer, ofs, maxsize, entry->size & 0xFFFFFFFF);
				ofs = poke32(buffer, ofs, maxsize, entry->size >> 32);
			break;

			case MTP_PROPERTY_DISPLAY_NAME:
				ofs = poke08(buffer, ofs, maxsize, 0);
			break;

			case MTP_PROPERTY_NAME:
			case MTP_PROPERTY_OBJECT_FILE_NAME:
				ofs = poke_string(buffer, ofs, maxsize, entry->name);                                      // Filename
			break;

			case MTP_PROPERTY_STORAGE_ID:
				ofs = poke32(buffer, ofs, maxsize, entry->storage_id);
			break;

			case MTP_PROPERTY_PARENT_OBJECT:
				ofs = poke32(buffer, ofs, maxsize, entry->parent);
			break;

			case MTP_PROPERTY_HIDDEN:
				ofs = poke16(buffer, ofs, maxsize, 0x0000);
			break;

			case MTP_PROPERTY_SYSTEM_OBJECT:
				ofs = poke16(buffer, ofs, maxsize, 0x0000);
			break;

			case MTP_PROPERTY_PROTECTION_STATUS:
				ofs = poke16(buffer, ofs, maxsize, 0x0000);
			break;

			case MTP_PROPERTY_ASSOCIATION_TYPE:
				if(entry->flags & ENTRY_IS_DIR)
						ofs = poke16(buffer, ofs, maxsize, 0x0001);                          // ObjectFormat Code
				else
						ofs = poke16(buffer, ofs, maxsize, 0x0000);                          // ObjectFormat Code
			break;

			case MTP_PROPERTY_ASSOCIATION_DESC:
				ofs = poke32(buffer, ofs, maxsize, 0x00000000);
			break;


			case MTP_PROPERTY_DATE_CREATED:
			case MTP_PROPERTY_DATE_MODIFIED:
				snprintf(timestr,sizeof(timestr),"%.4d%.2d%.2dT%.2d%.2d%.2d",1900 + 110, 1, 2, 10, 11,12);
				ofs = poke_string(buffer, ofs, maxsize, timestr);
			break;

			case MTP_PROPERTY_PERSISTENT_UID:
				ofs = poke32(buffer, ofs, maxsize, entry->handle);
				ofs = poke32(buffer, ofs, maxsize, entry->parent);
				ofs = poke32(buffer, ofs, maxsize, entry->storage_id);
				ofs = poke32(buffer, ofs, maxsize, 0x00000000);
			break;

			default:
				PRINT_ERROR("build_ObjectPropValue_dataset : Unsupported property : 0x%.4X (%s)", prop_code, mtp_get_property_string(prop_code));
				return 0;
			break;
		}
	}

	return ofs;
}

char* getpropvalueString(uint8_t* bytearray)
{

	int len = bytearray[0];
	PRINT_DEBUG("getpropvalueString: len = %d", len);
	char* str;

	// allocate memory for return value
	str = malloc(sizeof(char) * len);

	int i;
	int a = 0;
	for (i = 0; i < len*2; i+=2) {
		// Extract LSB char from input byte array
		char c = bytearray[i + 1] & 0xff;
		c |= bytearray[i + 1 + 1] << 8;

		// Load char into string array
		PRINT_DEBUG("getpropvalueString: char = %c", c);
		str[a] = c;
		a++;
	} 
	return str;
}

int setDevicePropValue(mtp_ctx * ctx,MTP_PACKET_HEADER * mtp_packet_hdr, uint32_t prop_code)
{

	int i, len;
	uint32_t prop_value;
	char* str;

	PRINT_DEBUG("setDevicePropValue: Finding prop_code 0x%x (%s)", prop_code, mtp_get_property_string(prop_code));
        i = 0;
        while(dev_properties[i].prop_code != 0xFFFF )
        {
                if( dev_properties[i].prop_code == prop_code )
                {
                        break;
                }
                i++;
        }

	if( dev_properties[i].prop_code == 0xFFFF )
	{
		PRINT_DEBUG("setDevicePropValue: returning MTP_RESPONSE_DEVICE_PROP_NOT_SUPPORTED");
		return MTP_RESPONSE_DEVICE_PROP_NOT_SUPPORTED;
	}

	if (dev_properties[i].getset != 0x01)
	{
		PRINT_DEBUG("setDevicePropValue: returning MTP_RESPONSE_ACCESS_DENIED");
		return MTP_RESPONSE_ACCESS_DENIED;
	}

        switch(dev_properties[i].format_id)
        {
                case MTP_TYPE_STR:

			len = ctx->rdbuffer[sizeof(MTP_PACKET_HEADER)];
			len = len * 2;
			memcpy(dev_properties[i].current_value_str, &ctx->rdbuffer[sizeof(MTP_PACKET_HEADER)], len);

			PRINT_DEBUG("setDevicePropValue: Setting prop_code 0x%x (%s) to MTP_TYPE_STR len %d", prop_code, mtp_get_property_string(prop_code), len);
			PRINT_DEBUG_BUF(dev_properties[i].current_value_str, len);

			str = getpropvalueString(dev_properties[i].current_value_str);
			PRINT_DEBUG("setDevicePropValue: string value is %s", str);

                break;

                case MTP_TYPE_UINT8:
			prop_value = ctx->rdbuffer[sizeof(MTP_PACKET_HEADER)];
			PRINT_DEBUG("setDevicePropValue: Setting prop_code 0x%x (%s) to MTP_TYPE_UINT8 0x%x", prop_code, mtp_get_property_string(prop_code), prop_value);
			dev_properties[i].current_value = prop_value;
                break;

                case MTP_TYPE_UINT16:
			PRINT_DEBUG("setDevicePropValue: MTP_TYPE_UINT16");
			prop_value = ctx->rdbuffer[sizeof(MTP_PACKET_HEADER)] & 0xff;
			prop_value |= ctx->rdbuffer[sizeof(MTP_PACKET_HEADER) + 1] << 8;
			PRINT_DEBUG("setDevicePropValue: Setting prop_code 0x%x (%s) to MTP_TYPE_UINT16 0x%x", prop_code, mtp_get_property_string(prop_code), prop_value);
			dev_properties[i].current_value = prop_value;
                break;

                case MTP_TYPE_UINT32:
			PRINT_DEBUG("setDevicePropValue: MTP_TYPE_UINT32");
			prop_value = ctx->rdbuffer[sizeof(MTP_PACKET_HEADER)] & 0xffffff;
			prop_value |= ctx->rdbuffer[sizeof(MTP_PACKET_HEADER) + 1] << 8;
			prop_value |= ctx->rdbuffer[sizeof(MTP_PACKET_HEADER) + 2] << 16;
			prop_value |= ctx->rdbuffer[sizeof(MTP_PACKET_HEADER) + 3] << 24;
			PRINT_DEBUG("setDevicePropValue: Setting prop_code 0x%x (%s) to MTP_TYPE_UINT32 0x%x", prop_code, mtp_get_property_string(prop_code), prop_value);
			dev_properties[i].current_value = prop_value;
                break;
	
		default:
			prop_value = 0;
			PRINT_DEBUG("setDevicePropValue: Setting prop_code 0x%x (%s) to UNKNOWN 0x%x", prop_code, mtp_get_property_string(prop_code), prop_value);
			dev_properties[i].current_value = prop_value;
		break;

        }



	return MTP_RESPONSE_OK;
}

int getDevicePropValue(mtp_ctx * ctx,void * buffer, int maxsize,uint32_t prop_code)
{
        int i, j;
	uint8_t l;
	int ofs;
	ofs = 0;

        PRINT_DEBUG("getDevicePropValue: Finding prop_code 0x%x", prop_code);
        i = 0;
        while(dev_properties[i].prop_code != 0xFFFF )
        {
                if( dev_properties[i].prop_code == prop_code )
                {
                        break;
                }
                i++;
        }

	if( dev_properties[i].prop_code == 0xFFFF )
	{
		PRINT_ERROR("getDevicePropValue : Unsupported property : 0x%.4X (%s)", prop_code, mtp_get_property_string(prop_code));
		return 0;
	}


	PRINT_DEBUG("getDevicePropValue : Property 0x%.4X (%s)", prop_code, mtp_get_property_string(prop_code));

	switch(dev_properties[i].format_id)
	{
		case MTP_TYPE_STR:
			//current value
			j = 0;
			l = dev_properties[i].current_value_str[0];
			ofs = poke08(buffer, ofs, maxsize, l);
			for (j = 1; j < (l * 2) + 1; j++) {
				ofs = poke08(buffer, ofs, maxsize, dev_properties[i].current_value_str[j]);
			}

		break;

		case MTP_TYPE_UINT8:
			ofs = poke08(buffer, ofs, maxsize, dev_properties[i].current_value);
		break;

		case MTP_TYPE_UINT16:
			ofs = poke16(buffer, ofs, maxsize, dev_properties[i].current_value);
		break;

		case MTP_TYPE_UINT32:
			ofs = poke32(buffer, ofs, maxsize, dev_properties[i].current_value);
		break;

		default:
			PRINT_ERROR("getDevicePropValue : Unsupported property : 0x%.4X (%s)", prop_code, mtp_get_property_string(prop_code));
			return 0;
		break;
	}

	return ofs;
}

int objectproplist_element(mtp_ctx * ctx, void * buffer, int * ofs, int maxsize, uint16_t prop_code, uint32_t handle, void * data,uint32_t prop_code_param)
{
	int i;

	if( (prop_code != prop_code_param) && (prop_code_param != 0xFFFFFFFF) )
	{
		return 0;
	}

	i = 0;
	while(properties[i].prop_code != 0xFFFF && properties[i].prop_code != prop_code)
	{
		i++;
	}

	if( properties[i].prop_code == prop_code )
	{
		*ofs = poke32(buffer, *ofs, maxsize, handle);
		*ofs = poke16(buffer, *ofs, maxsize, properties[i].prop_code);
		*ofs = poke16(buffer, *ofs, maxsize, properties[i].data_type);
		switch(properties[i].data_type)
		{
			case MTP_TYPE_STR:
				if(data)
					*ofs = poke_string(buffer, *ofs, maxsize, (char*)data);
				else
					*ofs = poke08(buffer, *ofs, maxsize, 0);
			break;
			case MTP_TYPE_UINT8:
				*ofs = poke08(buffer, *ofs, maxsize, *((uint8_t*)data));
			break;
			case MTP_TYPE_UINT16:
				*ofs = poke16(buffer, *ofs, maxsize, *((uint16_t*)data));
			break;
			case MTP_TYPE_UINT32:
				*ofs = poke32(buffer, *ofs, maxsize, *((uint32_t*)data));
			break;
			case MTP_TYPE_UINT64:
				*ofs = poke32(buffer, *ofs, maxsize, *((uint64_t*)data) & 0xFFFFFFFF);
				*ofs = poke32(buffer, *ofs, maxsize, *((uint64_t*)data) >> 32);
			break;
			case MTP_TYPE_UINT128:
				for(i=0;i<4;i++)
				{
					*ofs = poke32(buffer, *ofs, maxsize, *((uint32_t*)data)+i);
				}
			break;
			default:
				PRINT_ERROR("objectproplist_element : Unsupported data type : 0x%.4X", properties[i].data_type );
			break;
		}

		return 1;
	}

	return 0;
}

int build_objectproplist_dataset(mtp_ctx * ctx, void * buffer, int maxsize,fs_entry * entry, uint32_t handle,uint32_t format_id, uint32_t prop_code, uint32_t prop_current_value, uint32_t depth)
{
	struct stat64 entrystat;
	time_t t;
	struct tm lt;
	int ofs,ret,numberofelements;
	char * path;
	char timestr[32];
	uint32_t tmp_dword;
	uint32_t tmp_dword_array[4];

	ret = -1;
	path = build_full_path(ctx->fs_db, mtp_get_storage_root(ctx, entry->storage_id), entry);

	if(path)
	{
		ret = stat64(path, &entrystat);
	}

	if(ret)
	{
		if(path)
			free(path);
		return 0;
	}

	/* update the file size infomation */
	entry->size = entrystat.st_size;

	numberofelements = 0;

	ofs = poke32(buffer, 0, maxsize, numberofelements);   // Number of elements

	numberofelements += objectproplist_element(ctx, buffer, &ofs, maxsize, MTP_PROPERTY_STORAGE_ID, handle, &entry->storage_id,prop_code);

	if(entry->flags & ENTRY_IS_DIR)
		tmp_dword = MTP_FORMAT_ASSOCIATION;
	else
		tmp_dword = MTP_FORMAT_UNDEFINED;

	numberofelements += objectproplist_element(ctx, buffer, &ofs, maxsize, MTP_PROPERTY_OBJECT_FORMAT, handle, &tmp_dword,prop_code);

	if(entry->flags & ENTRY_IS_DIR)
		tmp_dword = MTP_ASSOCIATION_TYPE_GENERIC_FOLDER;
	else
		tmp_dword = 0x0000;

	numberofelements += objectproplist_element(ctx, buffer, &ofs, maxsize, MTP_PROPERTY_ASSOCIATION_TYPE, handle, &tmp_dword,prop_code);
	numberofelements += objectproplist_element(ctx, buffer, &ofs, maxsize, MTP_PROPERTY_PARENT_OBJECT, handle, &entry->parent,prop_code);
	numberofelements += objectproplist_element(ctx, buffer, &ofs, maxsize, MTP_PROPERTY_OBJECT_SIZE, handle, &entry->size,prop_code);

	tmp_dword = 0x0000;
	numberofelements += objectproplist_element(ctx, buffer, &ofs, maxsize, MTP_PROPERTY_PROTECTION_STATUS, handle, &tmp_dword,prop_code);

	numberofelements += objectproplist_element(ctx, buffer, &ofs, maxsize, MTP_PROPERTY_OBJECT_FILE_NAME, handle, entry->name,prop_code);
	numberofelements += objectproplist_element(ctx, buffer, &ofs, maxsize, MTP_PROPERTY_NAME, handle, entry->name,prop_code);
	numberofelements += objectproplist_element(ctx, buffer, &ofs, maxsize, MTP_PROPERTY_DISPLAY_NAME, handle, 0,prop_code);

	// Date Created (NR) "YYYYMMDDThhmmss.s"
	t = entrystat.st_mtime;
	localtime_r(&t, &lt);
	snprintf(timestr,sizeof(timestr),"%.4d%.2d%.2dT%.2d%.2d%.2d",1900 + lt.tm_year, lt.tm_mon + 1, lt.tm_mday, lt.tm_hour, lt.tm_min, lt.tm_sec);
	numberofelements += objectproplist_element(ctx, buffer, &ofs, maxsize, MTP_PROPERTY_DATE_CREATED, handle, &timestr,prop_code);

	// Date Modified (NR) "YYYYMMDDThhmmss.s"
	t = entrystat.st_mtime;
	localtime_r(&t, &lt);
	snprintf(timestr,sizeof(timestr),"%.4d%.2d%.2dT%.2d%.2d%.2d",1900 + lt.tm_year, lt.tm_mon + 1, lt.tm_mday, lt.tm_hour, lt.tm_min, lt.tm_sec);
	numberofelements += objectproplist_element(ctx, buffer, &ofs, maxsize, MTP_PROPERTY_DATE_MODIFIED, handle, &timestr,prop_code);

	tmp_dword_array[0] = entry->handle;
	tmp_dword_array[1] = entry->parent;
	tmp_dword_array[2] = entry->storage_id;
	tmp_dword_array[3] = 0x00000000;
	numberofelements += objectproplist_element(ctx, buffer, &ofs, maxsize, MTP_PROPERTY_PERSISTENT_UID, handle, &tmp_dword_array,prop_code);

	poke32(buffer, 0, maxsize, numberofelements);   // Number of elements

	return ofs;
}
