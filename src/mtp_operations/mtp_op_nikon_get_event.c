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
 * @file   mtp_op_initiatecapture.c
 * @brief  trigger a photo to be saved to the requested container
 * @author Jean-François DEL NERO <Jean-Francois.DELNERO@viveris.fr>
 * @author Ian Cass <ian@wheep.co.uk>
 */

#define _GNU_SOURCE
#include "buildconf.h"

#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <pthread.h>


#include "logs_out.h"

#include "mtp.h"
#include "mtp_helpers.h"
#include "mtp_constant.h"
#include "mtp_properties.h"
#include "mtp_operations.h"
#include "usb_gadget_fct.h"

uint32_t mtp_op_NikonGetEvent(mtp_ctx * ctx,MTP_PACKET_HEADER * mtp_packet_hdr, int * size,uint32_t * ret_params, int * ret_params_size)
{
        int sz;
	uint32_t params[5];
	int psize = 0;


	if(!ctx->fs_db)
		return MTP_RESPONSE_SESSION_NOT_OPEN;


        pthread_mutex_lock( &ctx->inotify_mutex );

	PRINT_DEBUG("MTP_OPERATION_NIKON_GET_EVENT 0x%x", mtp_packet_hdr->tx_id);

	if (ctx->EventTxId) {

		PRINT_DEBUG("MTP_OPERATION_NIKON_GET_EVENT Found outstanding event 0x%x = 0x%x", ctx->EventType, ctx->EventTxId);
		params[0] = (ctx->EventType << 16) | (0x0001 & 0xffff);
		params[1] = ctx->EventTxId;
		psize = sizeof(uint32_t) * 2;
		ctx->EventTxId = 0;
		ctx->EventType = 0;
	} 

	sz = build_response(ctx, mtp_packet_hdr->tx_id, MTP_CONTAINER_TYPE_DATA, mtp_packet_hdr->code, ctx->wrbuffer, ctx->usb_wr_buffer_max_size, &params, psize);
	if(sz<0)
		goto error;
	// Update packet size
	poke32(ctx->wrbuffer, 0, ctx->usb_wr_buffer_max_size, sz);

	PRINT_DEBUG("MTP_OPERATION_NIKON_GET_EVENT response (%d Bytes):",sz);
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

