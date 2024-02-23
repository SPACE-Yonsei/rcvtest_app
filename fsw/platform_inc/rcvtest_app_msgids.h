/************************************************************************
**
**      GSC-18128-1, "Core Flight Executive Version 6.7"
**
**      Copyright (c) 2006-2019 United States Government as represented by
**      the Administrator of the National Aeronautics and Space Administration.
**      All Rights Reserved.
**
**      Licensed under the Apache License, Version 2.0 (the "License");
**      you may not use this file except in compliance with the License.
**      You may obtain a copy of the License at
**
**        http://www.apache.org/licenses/LICENSE-2.0
**
**      Unless required by applicable law or agreed to in writing, software
**      distributed under the License is distributed on an "AS IS" BASIS,
**      WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
**      See the License for the specific language governing permissions and
**      limitations under the License.
**
*************************************************************************/

/**
 * @file
 *
 * Define Rcvtest app Message IDs
 *
 * \note The Rcvtest app assumes default configuration which uses V1 of message id implementation
 */

#ifndef RCVTEST_APP_MSGIDS_H
#define RCVTEST_APP_MSGIDS_H

/* V1 Command Message IDs must be 0x18xx */
#define RCVTEST_APP_CMD_MID     0x1882
#define RCVTEST_APP_SEND_HK_MID 0x1883
#define RCVTEST_APP_RCVTEST_MID 0x1830 // 앱간 통신 연습용. 실제 Mission에 쓰이지 않을 것!

/* V1 Telemetry Message IDs must be 0x08xx */
#define RCVTEST_APP_HK_TLM_MID 0x0883

#endif /* RCVTEST_APP_MSGIDS_H */