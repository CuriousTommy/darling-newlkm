/*
 * Copyright (c) 2015-2016 Apple Inc. All rights reserved.
 *
 * @APPLE_LICENSE_HEADER_START@
 *
 * This file contains Original Code and/or Modifications of Original Code
 * as defined in and that are subject to the Apple Public Source License
 * Version 2.0 (the 'License'). You may not use this file except in
 * compliance with the License. Please obtain a copy of the License at
 * http://www.opensource.apple.com/apsl/ and read it before using this
 * file.
 *
 * The Original Code and all software distributed under the License are
 * distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT.
 * Please see the License for the specific language governing rights and
 * limitations under the License.
 *
 * @APPLE_LICENSE_HEADER_END@
 */

#ifndef __os_log_private_h
#define __os_log_private_h

#include <os/log.h>
#include <firehose/tracepoint_private.h>
#include <sys/queue.h>
#include <os/availability.h>

__BEGIN_DECLS

/*!
 * @function os_log_with_args
 *
 * @abstract
 * os_log variant that supports va_list args.
 *
 * @discussion
 * os_log variant that supports va_list args.  This SPI should only be used
 * to shim legacy logging systems through os_log.
 *
 * @param oslog
 * Pass OS_LOG_DEFAULT or a log object previously created with os_log_create.
 *
 * @param type
 * Pass one of the following message types.
 *   OS_LOG_TYPE_DEFAULT
 *   OS_LOG_TYPE_DEBUG
 *   OS_LOG_TYPE_INFO
 *   OS_LOG_TYPE_ERROR
 *   OS_LOG_TYPE_FAULT
 *
 * @param format
 * A format string to generate a human-readable log message when the log
 * line is decoded.  Supports all standard printf types in addition to %@
 * and %m (objects and errno respectively).
 *
 * @param args
 * A va_list containing the values for the format string.
 *
 * @param ret_addr
 * Pass the __builtin_return_address(0) of the function that created the
 * va_list from variadic arguments.  The caller must be the same binary
 * that generated the message and provided the format string.
 */
    __OSX_AVAILABLE_STARTING(__MAC_10_12, __IPHONE_10_0)
OS_EXPORT OS_NOTHROW OS_LOG_NOTAILCALL
void
os_log_with_args(os_log_t oslog, os_log_type_t type, const char *format, va_list args, void *ret_addr);

/*!
 * @enum oslog_stream_link_type_t
 */
OS_ENUM(oslog_stream_link_type, uint8_t,
    oslog_stream_link_type_log       = 0x0,
    oslog_stream_link_type_metadata  = 0x1,
    );

/*!
 * @typedef oslog_stream_buf_entry_t
 */
typedef struct oslog_stream_buf_entry_s {
	STAILQ_ENTRY(oslog_stream_buf_entry_s) buf_entries;
	uint64_t timestamp;
	int offset;
	uint16_t size;
	oslog_stream_link_type_t type;
	struct firehose_tracepoint_s metadata[];
} *oslog_stream_buf_entry_t;

typedef struct os_log_pack_s {
    uint64_t        olp_continuous_time;
    struct timespec olp_wall_time;
    const void     *olp_mh;
    const void     *olp_pc;
    const char     *olp_format;
    uint8_t         olp_data[];
} os_log_pack_s, *os_log_pack_t;

// from https://github.com/apple/swift/blob/master/stdlib/public/Darwin/os/os.m
API_AVAILABLE(macosx(10.12.4), ios(10.3), tvos(10.2), watchos(3.2))
size_t
_os_log_pack_size(size_t os_log_format_buffer_size);

API_AVAILABLE(macosx(10.12.4), ios(10.3), tvos(10.2), watchos(3.2))
uint8_t *
_os_log_pack_fill(os_log_pack_t pack, size_t size, int saved_errno, const void *dso, const char *fmt);

API_AVAILABLE(macosx(10.12.4), ios(10.3), tvos(10.2), watchos(3.2))
void
os_log_pack_send(os_log_pack_t pack, os_log_t log, os_log_type_t type);

__END_DECLS

#endif // __os_log_private_h
