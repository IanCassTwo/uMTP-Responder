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


uint32_t mtp_op_NikonGetVendorPropCodes(mtp_ctx * ctx,MTP_PACKET_HEADER * mtp_packet_hdr, int * size,uint32_t * ret_params, int * ret_params_size)
{
	if(!ctx->fs_db)
		return MTP_RESPONSE_SESSION_NOT_OPEN;

	//FIXME return an array of all Nikon vendor prop codes
	// e.g.
	//00000000  92 00 00 00 17 D0 18 D0  19 D0 1A D0 1B D0 1C D0  |................|
	//00000010  1D D0 1E D0 1F D0 20 D0  21 D0 22 D0 23 D0 24 D0  |...... .!.".#.$.|
	//00000020  25 D0 26 D0 27 D0 28 D0  29 D0 32 D0 45 D0 4F D0  |%.&.'.(.).2.E.O.|
	//00000030  50 D0 52 D0 54 D0 56 D0  58 D0 59 D0 5A D0 5B D0  |P.R.T.V.X.Y.Z.[.|

	return MTP_RESPONSE_OK;

}

