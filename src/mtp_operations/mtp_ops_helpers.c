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
 * @file   mtp_ops_helpers.c
 * @brief  mtp operations helpers
 * @author Jean-François DEL NERO <Jean-Francois.DELNERO@viveris.fr>
 */

#include "buildconf.h"

#include <inttypes.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <pthread.h>


#include "mtp.h"
#include "mtp_helpers.h"
#include "mtp_constant.h"
#include "mtp_operations.h"
#include "mtp_properties.h"
#include "usb_gadget_fct.h"
#include "inotify.h"

#include "logs_out.h"

mtp_size send_file_data( mtp_ctx * ctx, fs_entry * entry,mtp_offset offset, mtp_size maxsize )
{
	mtp_size actualsize;
	mtp_size j;
	int ofs;
	mtp_size blocksize;
	int file,bytes_read;
	mtp_offset buf_index;
	int io_buffer_index;
	int first_part_size;
	unsigned char * usb_buffer_ptr;

	if( !ctx->read_file_buffer )
	{
		ctx->read_file_buffer = malloc( ctx->read_file_buffer_size );
		if(!ctx->read_file_buffer)
			return 0;

		memset(ctx->read_file_buffer, 0, ctx->read_file_buffer_size);
	}

	usb_buffer_ptr = NULL;

	buf_index = -1;

	if( offset >= entry->size )
	{
		actualsize = 0;
	}
	else
	{
		if( offset + maxsize > entry->size )
			actualsize = entry->size - offset;
		else
			actualsize = maxsize;
	}

	poke32(ctx->wrbuffer, 0, ctx->usb_wr_buffer_max_size, sizeof(MTP_PACKET_HEADER) + actualsize);

	ofs = sizeof(MTP_PACKET_HEADER);

	PRINT_DEBUG("send_file_data : Offset 0x%"SIZEHEX" - Maxsize 0x%"SIZEHEX" - Size 0x%"SIZEHEX" - ActualSize 0x%"SIZEHEX, offset,maxsize,entry->size,actualsize);

	file = entry_open(ctx->fs_db, entry);
	if( file != -1 )
	{
		ctx->transferring_file_data = 1;

		j = 0;
		do
		{
			if((j + ((mtp_size)(ctx->usb_wr_buffer_max_size) - ofs)) < actualsize)
				blocksize = ((mtp_size)(ctx->usb_wr_buffer_max_size) - ofs);
			else
				blocksize = actualsize - j;

			// Is the target page loaded ?
			if( buf_index != ((offset + j) & ~((mtp_offset)(ctx->read_file_buffer_size-1))) )
			{
				bytes_read = entry_read(ctx->fs_db, file, ctx->read_file_buffer, ((offset + j) & ~((mtp_offset)(ctx->read_file_buffer_size-1))) , (mtp_size)ctx->read_file_buffer_size);
				if( bytes_read < 0 )
				{
					entry_close( file );
					return -1;
				}

				buf_index = ((offset + j) & ~((mtp_offset)(ctx->read_file_buffer_size-1)));
			}

			io_buffer_index = (offset + j) & (mtp_offset)(ctx->read_file_buffer_size-1);

			// Is a new page needed ?
			if( io_buffer_index + blocksize < ctx->read_file_buffer_size )
			{
				// No, just use the io buffer

				if( !ofs )
				{
					// Use the I/O buffer directly
					usb_buffer_ptr = (unsigned char *)&ctx->read_file_buffer[io_buffer_index];
				}
				else
				{
					memcpy(&ctx->wrbuffer[ofs], &ctx->read_file_buffer[io_buffer_index], blocksize );
					usb_buffer_ptr = (unsigned char *)&ctx->wrbuffer[0];
				}
			}
			else
			{
				// Yes, new page needed. Get the first part in the io buffer and the load a new page to get the remaining data.
				first_part_size = blocksize - ( ( io_buffer_index + blocksize ) - (mtp_size)ctx->read_file_buffer_size);

				memcpy(&ctx->wrbuffer[ofs], &ctx->read_file_buffer[io_buffer_index], first_part_size  );

				buf_index += (mtp_offset)ctx->read_file_buffer_size;
				bytes_read = entry_read(ctx->fs_db, file, ctx->read_file_buffer, buf_index , ctx->read_file_buffer_size);
				if( bytes_read < 0 )
				{
					entry_close( file );
					return -1;
				}

				memcpy(&ctx->wrbuffer[ofs + first_part_size], &ctx->read_file_buffer[0], blocksize - first_part_size );

				usb_buffer_ptr = (unsigned char *)&ctx->wrbuffer[0];
			}

			j   += blocksize;
			ofs += blocksize;

			PRINT_DEBUG("---> 0x%"SIZEHEX" (0x%X)",j,ofs);

			write_usb(ctx->usb_ctx, EP_DESCRIPTOR_IN, usb_buffer_ptr, ofs);

			ofs = 0;

		}while( j < actualsize && !ctx->cancel_req );

		ctx->transferring_file_data = 0;

		entry_close( file );

		if( ctx->cancel_req )
		{
			PRINT_DEBUG("send_file_data : Cancelled ! Aborted...");

			// Force a ZLP
			check_and_send_USB_ZLP(ctx , ctx->max_packet_size );

			actualsize = -2;
			ctx->cancel_req = 0;
		}
		else
		{
			PRINT_DEBUG("send_file_data : Full transfer done !");

			check_and_send_USB_ZLP(ctx , sizeof(MTP_PACKET_HEADER) + actualsize );
		}

	}
	else
		actualsize = -1;

	return actualsize;
}

int delete_tree(mtp_ctx * ctx,uint32_t handle)
{
	int ret;
	fs_entry * entry;
	char * path;
	ret = -1;

	entry = get_entry_by_handle(ctx->fs_db, handle);
	if(entry)
	{
		path = build_full_path(ctx->fs_db, mtp_get_storage_root(ctx, entry->storage_id), entry);

		if (path)
		{
			if(entry->flags & ENTRY_IS_DIR)
			{
				ret = fs_remove_tree( path );

				if(!ret)
				{
					entry->flags |= ENTRY_IS_DELETED;
					if( entry->watch_descriptor != -1 )
					{
						inotify_handler_rmwatch( ctx, entry->watch_descriptor );
						entry->watch_descriptor = -1;
					}
				}
				else
					scan_and_add_folder(ctx->fs_db, path, handle, entry->storage_id); // partially deleted ? update/sync the db.
			}
			else
			{
				ret = remove(path);
				if(!ret)
				{
					entry->flags |= ENTRY_IS_DELETED;
					if( entry->watch_descriptor != -1 )
					{
						inotify_handler_rmwatch( ctx, entry->watch_descriptor );
						entry->watch_descriptor = -1;
					}
				}
			}

			free(path);
		}
	}

	return ret;
}

uint32_t getPropValue(uint32_t prop_code)
{
        PRINT_DEBUG("getPropValue: Finding value for 0x%x", prop_code);
        int i = 0;

        while (dev_properties[i].prop_code != 0xFFFF )
        {
                if (dev_properties[i].prop_code == prop_code )
                {
                        break;
                }
                i++;
        }

        return dev_properties[i].current_value;
}

uint32_t getShutterSpeed()
{
        uint32_t ss = getPropValue(MTP_DEVICE_PROPERTY_EXPOSURE_TIME);
        if (ss > 0)
                return ss * 10;
	//FIXME = how to correlate different methods of setting shutter speed
        return 0;
}

void *capture (void *arg)
{

        char* storage_path;

        pid_t child_pid;
        char* tmpfilename;
        char* ffilename;
        char* filename;
        //uint32_t ss;
        //char* sspeed;

        storage_path = (char *)arg;

        pthread_detach(pthread_self());

        tmpfilename = tempnam("/tmp", "DSC_");

        child_pid = fork ();

        //FIXME error handling
        if (child_pid == -1)
	{
                PRINT_DEBUG("[%lu] capture parent : fork failed!", pthread_self());
                return;
	}

        if (child_pid != 0) {
                int status;
                PRINT_DEBUG("[%lu] capture parent : waiting for  raspistill to finish", pthread_self());
                waitpid(child_pid, &status, 0);

                //FIXME error handling
                PRINT_DEBUG("[%lu] capture parent : raspistill finished with %d", pthread_self(), status);
                if (!status) {
			// Let's copy the temporary file into the emulated SDCARD storage area
			filename = tempnam(storage_path, "DSC_");
			asprintf(&ffilename, "%s.jpg", filename);
			PRINT_DEBUG("[%lu] capture parent : Renaming %s to %s", pthread_self(),  tmpfilename, ffilename);
			status = rename(tmpfilename, ffilename);
			// ObjectAdded and CaptureComplete are sent by the inotify handler once the file appears on the storage root
                }
        } else {
                PRINT_DEBUG("[%lu] capture child : Saving to %s", pthread_self(), tmpfilename);

                //ss = getShutterSpeed();
                //asprintf(&sspeed, "%d", ss);

                char* arg_list[] = {
                                "raspistill",
                                "-mm",          // metering mode
                                "average",      // mode
                                "-th",          // thumbnail
                                "none",         //  - none
                                //"-raw",               // Include raw data at end of jpg
                                "-q",           // JPEG quality
                                "100",          //  - 100%
//                              "-ex",          // auto exposure mode
//                              "off",          //  - off
                                "-fli",         // flicker reduction
                                "off",          //  - off
                                //"-awb",         // auto white balance
                                //"off",          //  - off
                                //"--awbgains",   // auto white balance gains
                                //"2,1.53",       //  - what's this?
                                "-drc",         // Dynamic Range Compression
                                "off",          //  - off
                                "-md",          // Mode
                                "2",            //  - 3 = 4056x3040, 2 = 2028x1520 2x2 binned
                                "-n",           // No preview
                                "-bm",          // Burst capture mode
                                "-st",          // Force recomputation of statistics on stills capture pass
                                "-dg",          // Digital gain
                                "1",            //  - 1
                                "-ag",          // Analogue gain
                                "16",           //  - 16
                                "-o",
                                tmpfilename,
                                NULL
                        };
                int i = 0;
                while(arg_list[i] != NULL) {
                        PRINT_DEBUG("[%lu] capture child %d = %s", pthread_self(), i, arg_list[i]);
                        i++;
                }

                execvp ("raspistill", arg_list);
                abort ();
        }
}

uint32_t registerFiles(mtp_ctx * ctx, uint32_t storageid, uint32_t parent_handle)
{
        fs_entry * entry;
        char * full_path;
        char * tmp_str;
        int nb_of_handles;

        tmp_str = NULL;
        full_path = NULL;
        entry = NULL;

        if(parent_handle && parent_handle!=0xFFFFFFFF)
        {
                entry = get_entry_by_handle(ctx->fs_db, parent_handle);
                if(entry)
                {
                        tmp_str = build_full_path(ctx->fs_db, mtp_get_storage_root(ctx, entry->storage_id), entry);
                        full_path = tmp_str;
                }
        }
        else
        {
                // root folder
                parent_handle = 0x00000000;
                full_path = mtp_get_storage_root(ctx,storageid);
                entry = get_entry_by_handle_and_storageid(ctx->fs_db, parent_handle, storageid);
        }

        nb_of_handles = 0;

        if( full_path )
        {
                // Count the number of files...
                scan_and_add_folder(ctx->fs_db, full_path, parent_handle, storageid);
                init_search_handle(ctx->fs_db, parent_handle, storageid);

                while( get_next_child_handle(ctx->fs_db) )
                {
                        nb_of_handles++;
                }

                PRINT_DEBUG("registerFiles - %d objects found",nb_of_handles);

                // Restart
                init_search_handle(ctx->fs_db, parent_handle, storageid);

                // Register a watch point.
                if( entry )
                {
                        if ( entry->flags & ENTRY_IS_DIR )
                        {
                                PRINT_DEBUG("registerFiles : Adding watch for %s", full_path);
                                entry->watch_descriptor = inotify_handler_addwatch( ctx, full_path );
                        }
                }

                if (tmp_str)
                        free(tmp_str);
        }

        return nb_of_handles;

}
