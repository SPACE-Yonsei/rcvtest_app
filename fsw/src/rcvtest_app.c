/*******************************************************************************
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
** File: rcvtest_app.c
**
** Purpose:
**   This file contains the source code for the Rcvtest app.
**
*******************************************************************************/

/*
** Include Files:
*/
#include "rcvtest_app_events.h"
#include "rcvtest_app_version.h"
#include "rcvtest_app.h"
#include "rcvtest_app_table.h"

#include "hk_msgids.h"

/* The sample_lib module provides the SAMPLE_LIB_Function() prototype */
#include <string.h>
#include "sample_lib.h"
#include "hyun_app_msgids.h"
/* This spacey.h provides essential data for CANSAT AAS 2024 mission */
#include "libs/spacey.h"

/*
** global data
*/
RCVTEST_APP_Data_t RCVTEST_APP_Data;

/*
User Made Function
*/
void RCVTEST_APP_RcvDatafromHYUN(const SPACEY_LIB_MSG_CHAR20_t *data)
{
    /*
    const SPACEY_LIB_MSG_CHAR20_Payload_t *PData = &data->Payload;
    char rcvdata[20];
    (void)CFE_SB_MessageStringGet(rcvdata, PData->TextData, "", sizeof(rcvdata),sizeof(PData->TextData));
    printf("%s\n", rcvdata);*/
    return;
}

void RCVTEST_APP_CHECK_COMBINED_HK_DATA(const HK_COMBINED_PCK_1_STRUCTURE_t *data)
{
    printf("%ld\n", sizeof(data->TlmHeader));
    printf("%u %u %u\n", data->ES_CFECoreChecksum, data->EVS_MessageFormatMode, data->TIME_ClockStateFlags);
    
    return;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * *  * * * * **/
/* RCVTEST_APP_Main() -- Application entry point and main process loop         */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * *  * * * * **/
void RCVTEST_APP_Main(void)
{

    int32            status;
    CFE_SB_Buffer_t *SBBufPtr;

    /*
    ** Create the first Performance Log entry
    */
    CFE_ES_PerfLogEntry(RCVTEST_APP_PERF_ID);

    /*
    ** Perform application specific initialization
    ** If the Initialization fails, set the RunStatus to
    ** CFE_ES_RunStatus_APP_ERROR and the App will not enter the RunLoop
    */
    status = RCVTEST_APP_Init();
    if (status != CFE_SUCCESS)
    {
        RCVTEST_APP_Data.RunStatus = CFE_ES_RunStatus_APP_ERROR;
    }

    /*
    ** SAMPLE Runloop
    */
    while (CFE_ES_RunLoop(&RCVTEST_APP_Data.RunStatus) == true)
    {
        /*
        ** Performance Log Exit Stamp
        */
        CFE_ES_PerfLogExit(RCVTEST_APP_PERF_ID);

        /* Pend on receipt of command packet */
        status = CFE_SB_ReceiveBuffer(&SBBufPtr, RCVTEST_APP_Data.CommandPipe, CFE_SB_PEND_FOREVER);

        /*
        ** Performance Log Entry Stamp
        */
        CFE_ES_PerfLogEntry(RCVTEST_APP_PERF_ID);

        if (status == CFE_SUCCESS)
        {
            RCVTEST_APP_ProcessCommandPacket(SBBufPtr);
        }
        else
        {
            CFE_EVS_SendEvent(RCVTEST_APP_PIPE_ERR_EID, CFE_EVS_EventType_ERROR,
                              "RCVTEST APP: SB Pipe Read Error, App Will Exit");

            RCVTEST_APP_Data.RunStatus = CFE_ES_RunStatus_APP_ERROR;
        }
    }  

    /*
    ** Performance Log Exit Stamp
    */
    CFE_ES_PerfLogExit(RCVTEST_APP_PERF_ID);

    CFE_ES_ExitApp(RCVTEST_APP_Data.RunStatus);

} /* End of RCVTEST_APP_Main() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  */
/*                                                                            */
/* RCVTEST_APP_Init() --  initialization                                       */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
int32 RCVTEST_APP_Init(void)
{
    int32 status;

    RCVTEST_APP_Data.RunStatus = CFE_ES_RunStatus_APP_RUN;

    /*
    ** Initialize app command execution counters
    */
    RCVTEST_APP_Data.CmdCounter = 0;
    RCVTEST_APP_Data.ErrCounter = 0;

    /*
    ** Initialize app configuration data
    */
    RCVTEST_APP_Data.PipeDepth = RCVTEST_APP_PIPE_DEPTH;

    strncpy(RCVTEST_APP_Data.PipeName, "RCVTEST_APP_CMD_PIPE", sizeof(RCVTEST_APP_Data.PipeName));
    RCVTEST_APP_Data.PipeName[sizeof(RCVTEST_APP_Data.PipeName) - 1] = 0;

    /*
    ** Initialize event filter table...
    */
    RCVTEST_APP_Data.EventFilters[0].EventID = RCVTEST_APP_STARTUP_INF_EID;
    RCVTEST_APP_Data.EventFilters[0].Mask    = 0x0000;
    RCVTEST_APP_Data.EventFilters[1].EventID = RCVTEST_APP_COMMAND_ERR_EID;
    RCVTEST_APP_Data.EventFilters[1].Mask    = 0x0000;
    RCVTEST_APP_Data.EventFilters[2].EventID = RCVTEST_APP_COMMANDNOP_INF_EID;
    RCVTEST_APP_Data.EventFilters[2].Mask    = 0x0000;
    RCVTEST_APP_Data.EventFilters[3].EventID = RCVTEST_APP_COMMANDRST_INF_EID;
    RCVTEST_APP_Data.EventFilters[3].Mask    = 0x0000;
    RCVTEST_APP_Data.EventFilters[4].EventID = RCVTEST_APP_INVALID_MSGID_ERR_EID;
    RCVTEST_APP_Data.EventFilters[4].Mask    = 0x0000;
    RCVTEST_APP_Data.EventFilters[5].EventID = RCVTEST_APP_LEN_ERR_EID;
    RCVTEST_APP_Data.EventFilters[5].Mask    = 0x0000;
    RCVTEST_APP_Data.EventFilters[6].EventID = RCVTEST_APP_PIPE_ERR_EID;
    RCVTEST_APP_Data.EventFilters[6].Mask    = 0x0000;

    /*
    ** Register the events
    */
    status = CFE_EVS_Register(RCVTEST_APP_Data.EventFilters, RCVTEST_APP_EVENT_COUNTS, CFE_EVS_EventFilter_BINARY);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("Rcvtest app: Error Registering Events, RC = 0x%08lX\n", (unsigned long)status);
        return (status);
    }

    /*
    ** Initialize housekeeping packet (clear user data area).
    */
    CFE_MSG_Init(&RCVTEST_APP_Data.HkTlm.TlmHeader.Msg, RCVTEST_APP_MID_HOUSEKEEPING_RES, sizeof(RCVTEST_APP_Data.HkTlm));

    /*
    ** Create Software Bus message pipe.
    */
    status = CFE_SB_CreatePipe(&RCVTEST_APP_Data.CommandPipe, RCVTEST_APP_Data.PipeDepth, RCVTEST_APP_Data.PipeName);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("Rcvtest app: Error creating pipe, RC = 0x%08lX\n", (unsigned long)status);
        return (status);
    }

    /*
    Subscribe to SB test request data sent by hyun_app
    */
    //status = CFE_SB_Subscribe(HYUN_APP_MID_SBTEST_REQ, RCVTEST_APP_Data.CommandPipe);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("Rcvtest app: Error Subscribing to HK request, RC = 0x%08lX\n", (unsigned long)status);
        return (status);
    }

    /*
    Subscribe to HouseKeeper Combined Packet MID
    */
   status = CFE_SB_Subscribe(HK_MID_COMBINED_PKT1_RES, RCVTEST_APP_Data.CommandPipe);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("Rcvtest app: Error Subscribing to HK request, RC = 0x%08lX\n", (unsigned long)status);
        return (status);
    }
    /*
    ** Subscribe to Housekeeping request commands
    */
    status = CFE_SB_Subscribe(RCVTEST_APP_MID_HOUSEKEEPING_REQ, RCVTEST_APP_Data.CommandPipe);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("Rcvtest app: Error Subscribing to HK request, RC = 0x%08lX\n", (unsigned long)status);
        return (status);
    }

    /*
    ** Subscribe to ground command packets
    */
    status = CFE_SB_Subscribe(RCVTEST_APP_MID_GROUNDCMD_REQ, RCVTEST_APP_Data.CommandPipe);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("Rcvtest app: Error Subscribing to Command, RC = 0x%08lX\n", (unsigned long)status);

        return (status);
    }
	status = CFE_SB_Subscribe(HYUN_APP_MID_SENDTORCVTEST_RES, RCVTEST_APP_Data.CommandPipe);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("Rcvtest app: Error Subscribing to Command, RC = 0x%08lX\n", (unsigned long)status);

        return (status);
    }
    /*
    ** Register Table(s)
    */
    status = CFE_TBL_Register(&RCVTEST_APP_Data.TblHandles[0], "RcvtestappTable", sizeof(RCVTEST_APP_Table_t),
                              CFE_TBL_OPT_DEFAULT, RCVTEST_APP_TblValidationFunc);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("Rcvtest app: Error Registering Table, RC = 0x%08lX\n", (unsigned long)status);

        return (status);
    }
    else
    {
        status = CFE_TBL_Load(RCVTEST_APP_Data.TblHandles[0], CFE_TBL_SRC_FILE, RCVTEST_APP_TABLE_FILE);
    }

    CFE_EVS_SendEvent(RCVTEST_APP_STARTUP_INF_EID, CFE_EVS_EventType_INFORMATION, "Rcvtest app Initialized.%s",
                      RCVTEST_APP_VERSION_STRING);

    return (CFE_SUCCESS);

} /* End of RCVTEST_APP_Init() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*  Name:  RCVTEST_APP_ProcessCommandPacket                                    */
/*                                                                            */
/*  Purpose:                                                                  */
/*     This routine will process any packet that is received on the SAMPLE    */
/*     command pipe.                                                          */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * *  * *  * * * * */
void RCVTEST_APP_ProcessCommandPacket(CFE_SB_Buffer_t *SBBufPtr)
{
    CFE_SB_MsgId_t MsgId = CFE_SB_INVALID_MSG_ID;

    CFE_MSG_GetMsgId(&SBBufPtr->Msg, &MsgId);
    //printf("rcvtest app process MID = 0x%x\n", (unsigned int)CFE_SB_MsgIdToValue(MsgId));

    switch (MsgId)
    {
        case RCVTEST_APP_MID_GROUNDCMD_REQ:
            RCVTEST_APP_ProcessGroundCommand(SBBufPtr);
            break;
        case RCVTEST_APP_MID_HOUSEKEEPING_REQ:
            RCVTEST_APP_ReportHousekeeping((CFE_MSG_CommandHeader_t *)SBBufPtr);
            break;
        case HYUN_APP_MID_SBTEST_REQ:
            RCVTEST_APP_ProcessTestInputData(SBBufPtr);
            break;
        case HYUN_APP_MID_SENDTORCVTEST_RES:
            RCVTEST_APP_RcvDatafromHYUN((const SPACEY_LIB_MSG_CHAR20_t *)SBBufPtr);
            break;
        case HK_MID_COMBINED_PKT1_RES:
            printf("HK combined PCK RCV\n");
            RCVTEST_APP_CHECK_COMBINED_HK_DATA((const HK_COMBINED_PCK_1_STRUCTURE_t *)SBBufPtr);
            
            break;
        default:
            CFE_EVS_SendEvent(RCVTEST_APP_INVALID_MSGID_ERR_EID, CFE_EVS_EventType_ERROR,
                              "SAMPLE: invalid command packet,MID = 0x%x", (unsigned int)CFE_SB_MsgIdToValue(MsgId));
            break;
    }

    return;

} /* End RCVTEST_APP_ProcessCommandPacket */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/* RCVTEST_APP_ProcessTestInputData() -- TEST DATA RCV FUNCTION               */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
void RCVTEST_APP_ProcessTestInputData(CFE_SB_Buffer_t *SBBufPtr)
{
    printf("RCVTESTAPP : RCV SUCCESS\n");
    
    //HYUN_APP_TUTORIAL_t (HYUN_APP_TUTORIAL_t *)SBBufPtr;
    RCVTEST_APP_MSGPRINT_TEST((const HYUN_APP_TUTORIAL_t *)SBBufPtr);
    return;
}

void RCVTEST_APP_MSGPRINT_TEST(const HYUN_APP_TUTORIAL_t *data)
{
    const HYUN_APP_TUTORIAL_Payload_t *DataPayload = &data->Payload;
    printf("%s\n", DataPayload->TextData);
    return;
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/* RCVTEST_APP_ProcessGroundCommand() -- SAMPLE ground commands                */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
void RCVTEST_APP_ProcessGroundCommand(CFE_SB_Buffer_t *SBBufPtr)
{
    CFE_MSG_FcnCode_t CommandCode = 0;

    CFE_MSG_GetFcnCode(&SBBufPtr->Msg, &CommandCode);

    /*
    ** Process "known" Rcvtest app ground commands
    */
    switch (CommandCode)
    {
        case RCVTEST_APP_NOOP_CC:
            if (RCVTEST_APP_VerifyCmdLength(&SBBufPtr->Msg, sizeof(RCVTEST_APP_NoopCmd_t)))
            {
                RCVTEST_APP_Noop((RCVTEST_APP_NoopCmd_t *)SBBufPtr);
            }

            break;

        case RCVTEST_APP_RESET_COUNTERS_CC:
            if (RCVTEST_APP_VerifyCmdLength(&SBBufPtr->Msg, sizeof(RCVTEST_APP_ResetCountersCmd_t)))
            {
                RCVTEST_APP_ResetCounters((RCVTEST_APP_ResetCountersCmd_t *)SBBufPtr);
            }

            break;

        case RCVTEST_APP_PROCESS_CC:
            if (RCVTEST_APP_VerifyCmdLength(&SBBufPtr->Msg, sizeof(RCVTEST_APP_ProcessCmd_t)))
            {
                RCVTEST_APP_Process((RCVTEST_APP_ProcessCmd_t *)SBBufPtr);
            }

            break;

        /* default case already found during FC vs length test */
        default:
            CFE_EVS_SendEvent(RCVTEST_APP_COMMAND_ERR_EID, CFE_EVS_EventType_ERROR,
                              "Invalid ground command code: CC = %d", CommandCode);
            break;
    }

    return;

} /* End of RCVTEST_APP_ProcessGroundCommand() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*  Name:  RCVTEST_APP_ReportHousekeeping                                     */
/*                                                                            */
/*  Purpose:                                                                  */
/*         This function is triggered in response to a task telemetry request */
/*         from the housekeeping task. This function will gather the Apps     */
/*         telemetry, packetize it and send it to the housekeeping task via   */
/*         the software bus                                                   */
/* * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * *  * *  * * * * */
int32 RCVTEST_APP_ReportHousekeeping(const CFE_MSG_CommandHeader_t *Msg)
{
    int i;

    /*
    ** Get command execution counters...
    */
    RCVTEST_APP_Data.HkTlm.Payload.CommandErrorCounter = RCVTEST_APP_Data.ErrCounter;
    RCVTEST_APP_Data.HkTlm.Payload.CommandCounter      = RCVTEST_APP_Data.CmdCounter;

    /*
    ** Send housekeeping telemetry packet...
    */
    CFE_SB_TimeStampMsg(&RCVTEST_APP_Data.HkTlm.TlmHeader.Msg);
    CFE_SB_TransmitMsg(&RCVTEST_APP_Data.HkTlm.TlmHeader.Msg, true);

    /*
    ** Manage any pending table loads, validations, etc.
    */
    for (i = 0; i < RCVTEST_APP_NUMBER_OF_TABLES; i++)
    {
        CFE_TBL_Manage(RCVTEST_APP_Data.TblHandles[i]);
    }

    return CFE_SUCCESS;

} /* End of RCVTEST_APP_ReportHousekeeping() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/* RCVTEST_APP_Noop -- SAMPLE NOOP commands                                        */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
int32 RCVTEST_APP_Noop(const RCVTEST_APP_NoopCmd_t *Msg)
{

    RCVTEST_APP_Data.CmdCounter++;

    CFE_EVS_SendEvent(RCVTEST_APP_COMMANDNOP_INF_EID, CFE_EVS_EventType_INFORMATION, "SAMPLE: NOOP command %s",
                      RCVTEST_APP_VERSION);

    return CFE_SUCCESS;

} /* End of RCVTEST_APP_Noop */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*  Name:  RCVTEST_APP_ResetCounters                                               */
/*                                                                            */
/*  Purpose:                                                                  */
/*         This function resets all the global counter variables that are     */
/*         part of the task telemetry.                                        */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * *  * *  * * * * */
int32 RCVTEST_APP_ResetCounters(const RCVTEST_APP_ResetCountersCmd_t *Msg)
{

    RCVTEST_APP_Data.CmdCounter = 0;
    RCVTEST_APP_Data.ErrCounter = 0;

    CFE_EVS_SendEvent(RCVTEST_APP_COMMANDRST_INF_EID, CFE_EVS_EventType_INFORMATION, "SAMPLE: RESET command");

    return CFE_SUCCESS;

} /* End of RCVTEST_APP_ResetCounters() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*  Name:  RCVTEST_APP_Process                                                     */
/*                                                                            */
/*  Purpose:                                                                  */
/*         This function Process Ground Station Command                       */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * *  * *  * * * * */
int32 RCVTEST_APP_Process(const RCVTEST_APP_ProcessCmd_t *Msg)
{
    int32               status;
    RCVTEST_APP_Table_t *TblPtr;
    const char *        TableName = "RCVTEST_APP.RcvtestappTable";

    /* Sample Use of Table */

    status = CFE_TBL_GetAddress((void *)&TblPtr, RCVTEST_APP_Data.TblHandles[0]);

    if (status < CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("Rcvtest app: Fail to get table address: 0x%08lx", (unsigned long)status);
        return status;
    }

    CFE_ES_WriteToSysLog("Rcvtest app: Table Value 1: %d  Value 2: %d", TblPtr->Int1, TblPtr->Int2);

    RCVTEST_APP_GetCrc(TableName);

    status = CFE_TBL_ReleaseAddress(RCVTEST_APP_Data.TblHandles[0]);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("Rcvtest app: Fail to release table address: 0x%08lx", (unsigned long)status);
        return status;
    }

    /* Invoke a function provided by RCVTEST_APP_LIB */
    SAMPLE_LIB_Function();

    return CFE_SUCCESS;

} /* End of RCVTEST_APP_ProcessCC */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/* RCVTEST_APP_VerifyCmdLength() -- Verify command packet length                   */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
bool RCVTEST_APP_VerifyCmdLength(CFE_MSG_Message_t *MsgPtr, size_t ExpectedLength)
{
    bool              result       = true;
    size_t            ActualLength = 0;
    CFE_SB_MsgId_t    MsgId        = CFE_SB_INVALID_MSG_ID;
    CFE_MSG_FcnCode_t FcnCode      = 0;

    CFE_MSG_GetSize(MsgPtr, &ActualLength);

    /*
    ** Verify the command packet length.
    */
    if (ExpectedLength != ActualLength)
    {
        CFE_MSG_GetMsgId(MsgPtr, &MsgId);
        CFE_MSG_GetFcnCode(MsgPtr, &FcnCode);

        CFE_EVS_SendEvent(RCVTEST_APP_LEN_ERR_EID, CFE_EVS_EventType_ERROR,
                          "Invalid Msg length: ID = 0x%X,  CC = %u, Len = %u, Expected = %u",
                          (unsigned int)CFE_SB_MsgIdToValue(MsgId), (unsigned int)FcnCode, (unsigned int)ActualLength,
                          (unsigned int)ExpectedLength);

        result = false;

        RCVTEST_APP_Data.ErrCounter++;
    }

    return (result);

} /* End of RCVTEST_APP_VerifyCmdLength() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* RCVTEST_APP_TblValidationFunc -- Verify contents of First Table      */
/* buffer contents                                                 */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 RCVTEST_APP_TblValidationFunc(void *TblData)
{
    int32               ReturnCode = CFE_SUCCESS;
    RCVTEST_APP_Table_t *TblDataPtr = (RCVTEST_APP_Table_t *)TblData;

    /*
    ** Sample Table Validation
    */
    
    if (TblDataPtr->Int1 > RCVTEST_APP_TBL_ELEMENT_1_MAX)
    {
        /* First element is out of range, return an appropriate error code */
        ReturnCode = RCVTEST_APP_TABLE_OUT_OF_RANGE_ERR_CODE;
    }

    return ReturnCode;

} /* End of RCVTEST_APP_TBLValidationFunc() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* RCVTEST_APP_GetCrc -- Output CRC                                     */
/*                                                                 */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void RCVTEST_APP_GetCrc(const char *TableName)
{
    int32          status;
    uint32         Crc;
    CFE_TBL_Info_t TblInfoPtr;

    status = CFE_TBL_GetInfo(&TblInfoPtr, TableName);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("Rcvtest app: Error Getting Table Info");
    }
    else
    {
        Crc = TblInfoPtr.Crc;
        CFE_ES_WriteToSysLog("Rcvtest app: CRC: 0x%08lX\n\n", (unsigned long)Crc);
    }

    return;

} /* End of RCVTEST_APP_GetCrc */
