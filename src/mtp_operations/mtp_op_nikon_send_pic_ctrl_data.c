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
 * @file   mtp_op_getdevicepropvalue.c
 * @brief  get device prop value operation.
 * @author Jean-François DEL NERO <Jean-Francois.DELNERO@viveris.fr>
 */

#include "buildconf.h"

#include <inttypes.h>
#include <pthread.h>

#include "mtp.h"
#include "mtp_helpers.h"
#include "mtp_constant.h"
#include "mtp_operations.h"
#include "mtp_properties.h"
#include "usb_gadget_fct.h"

#include "logs_out.h"

uint32_t mtp_op_NikonGetPicCtrlData(mtp_ctx * ctx,MTP_PACKET_HEADER * mtp_packet_hdr, int * size,uint32_t * ret_params, int * ret_params_size)
{
	uint32_t prop_code;
	int sz,tmp_sz;
	uint8_t response_data[31];

	if(!ctx->fs_db)
		return MTP_RESPONSE_SESSION_NOT_OPEN;

	pthread_mutex_lock( &ctx->inotify_mutex );

	prop_code = peek(mtp_packet_hdr, sizeof(MTP_PACKET_HEADER), 4);     // Get param 1 - PropCode

	PRINT_DEBUG("MTP_OPERATION_NIKON_GET_PIC_CTRL_DATA : (Prop code : 0x%.4X )", prop_code);

	response_data[0] = 1; // Normal
	response_data[1] = 0; // Colour
	response_data[2] = 0; // Normal
	response_data[3] = 0x30;
	response_data[4] = 0x30;
	response_data[5] = 0x30;
	response_data[6] = 0x30;
	response_data[7] = 0x30;
	response_data[8] = 0x30;
	response_data[9] = 0x30;
	response_data[10] = 0x30;
	response_data[11] = 0x30;
	response_data[12] = 0x30;
	response_data[13] = 0x30;
	response_data[14] = 0x30;
	response_data[15] = 0x30;
	response_data[16] = 0x30;
	response_data[17] = 0x30;
	response_data[18] = 0x30;
	response_data[19] = 0x30;
	response_data[20] = 0x30;
	response_data[21] = 0x30;
	response_data[22] = 0x00;
	response_data[23] = 0; // Invalid
	response_data[24] = 0; // value
	response_data[25] = 0; // saturation
	response_data[26] = 0; // hue
	response_data[27] = 0; // sharpening
	response_data[28] = 0; // contrast
	response_data[29] = 0; // brightness
	response_data[30] = 0; // custom
	sz = sizeof(uint8_t) * 31;

	tmp_sz = build_response(ctx, mtp_packet_hdr->tx_id, MTP_CONTAINER_TYPE_DATA, mtp_packet_hdr->code, ctx->wrbuffer, ctx->usb_wr_buffer_max_size, &response_data, sz);
	if(sz < 0)
		goto error;

	sz += tmp_sz;

	// Update packet size
	poke32(ctx->wrbuffer, 0, ctx->usb_wr_buffer_max_size, sz);

	PRINT_DEBUG("MTP_OPERATION_NIKON_GET_PIC_CTRL_DATA response (%d Bytes):",sz);
	PRINT_DEBUG_BUF(ctx->wrbuffer, sz);

	write_usb(ctx->usb_ctx,EP_DESCRIPTOR_IN,ctx->wrbuffer,sz);

	check_and_send_USB_ZLP(ctx , sz );

	*size = sz;

	pthread_mutex_unlock( &ctx->inotify_mutex );

	return MTP_RESPONSE_OK;

error:
	pthread_mutex_unlock( &ctx->inotify_mutex );

	return MTP_RESPONSE_GENERAL_ERROR;
}
