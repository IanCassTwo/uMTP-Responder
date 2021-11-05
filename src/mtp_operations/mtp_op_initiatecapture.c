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
#include "mtp_ops_helpers.h"
#include "mtp_constant.h"
#include "mtp_properties.h"
#include "mtp_operations.h"

uint32_t mtp_op_InitiateCapture(mtp_ctx * ctx,MTP_PACKET_HEADER * mtp_packet_hdr, int * size,uint32_t * ret_params, int * ret_params_size)
{
        uint32_t storageid = 0;
        char* storage_path = NULL;
	pthread_t  thread_id;

	if(!ctx->fs_db)
		return MTP_RESPONSE_SESSION_NOT_OPEN;


	// TODO: save correct object format
	// TODO: unique name with numbers rather than junk

	// Have params?
	if (sizeof(MTP_PACKET_HEADER) == *size) {
		storage_path = ctx->storages[0].root_path;
	} else {
        	storageid = peek(mtp_packet_hdr, sizeof(MTP_PACKET_HEADER) + 0, 4); // Get param 1 - Storage ID
		PRINT_DEBUG("MTP_OPERATION_INITIATE_CAPTURE : storage id 0x%x", storageid);
		storage_path = mtp_get_storage_root(ctx, storageid);
	}

        if (storage_path == NULL) 
        {
		PRINT_DEBUG("MTP_OPERATION_INITIATE_CAPTURE : Invalid storage id 0x%x", storageid);
                return MTP_RESPONSE_INVALID_STORAGE_ID;
        }

	PRINT_DEBUG("MTP_OPERATION_INITIATE_CAPTURE : storage path %s", storage_path);

	// Store transaction id & take photo
	ctx->InitiateCaptureTxId = mtp_packet_hdr->tx_id;
	pthread_create(&thread_id, NULL, capture, storage_path);

	PRINT_DEBUG("MTP_OPERATION_INITIATE_CAPTURE : Success!");

	return MTP_RESPONSE_OK;
}

