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
 * @file   mtp_op_setobjectpropvalue.c
 * @brief  set object prop value operation.
 * @author Jean-François DEL NERO <Jean-Francois.DELNERO@viveris.fr>
 */

#include "buildconf.h"

#include <pthread.h>
#include <inttypes.h>

#include "logs_out.h"

#include "mtp.h"
#include "mtp_helpers.h"
#include "mtp_constant.h"
#include "mtp_properties.h"
#include "mtp_operations.h"

uint32_t mtp_op_SetDevicePropValue(mtp_ctx * ctx,MTP_PACKET_HEADER * mtp_packet_hdr, int * size,uint32_t * ret_params, int * ret_params_size)
{
	uint32_t response_code;

	if(!ctx->fs_db)
		return MTP_RESPONSE_SESSION_NOT_OPEN;

	switch(mtp_packet_hdr->operation)
	{
		case MTP_CONTAINER_TYPE_COMMAND:

			ctx->SetDevicePropValue_PropCode = peek(mtp_packet_hdr, sizeof(MTP_PACKET_HEADER), 4);  // Get param 1 - prop code

			// no response to send, wait for the data...
			response_code = MTP_RESPONSE_NO_RESPONSE;
		break;

		case MTP_CONTAINER_TYPE_DATA:
			response_code = setDevicePropValue(ctx, mtp_packet_hdr, ctx->SetDevicePropValue_PropCode);
		break;

		default:
			response_code = MTP_RESPONSE_OPERATION_NOT_SUPPORTED;
		break;
	}

	return response_code;
}
