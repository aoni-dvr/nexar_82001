/**
 *  @file AmbaTransfer.c
 *
 *  @copyright Copyright (c) 2017 Ambarella, Inc.
 *
 *  This file and its contents ("Software") are protected by intellectual property rights including, without limitation,
 *  U.S. and/or foreign copyrights.  This Software is also the confidential and proprietary information of Ambarella, Inc.
 *  and its licensors.  You may not use, reproduce, disclose, distribute, modify, or otherwise prepare derivative works
 *  of this Software or any portion thereof except pursuant to a signed license agreement or nondisclosure agreement with
 *  Ambarella, Inc. or its authorized affiliates.  In the absence of such an agreement, you agree to promptly notify and
 *  return this Software to Ambarella, Inc.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 *  AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details Implementation of AmbaTransfer interface
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include "AmbaTransfer.h"

#define AMBATRANFER_DEF_CHANNELAMOUNT (8)

#define EOS ('\0')

#define ErrMsg(fmt,...) printf("%s: "fmt"\n",__FUNCTION__,##__VA_ARGS__)
#define Debug ErrMsg

typedef struct {
    char Name[AMBATRANFER_NAME_MAXLENGTH];
    AMBA_TRANSFER_IMPL_s *Impl;
} AMBA_TRANSFER_CHANNEL_NODE_s;

typedef struct {
    INT32 TotalAmount;
    AMBA_TRANSFER_CHANNEL_NODE_s *Nodes;
} AMBA_TRANSFER_CHANNEL_LIST_s;

static AMBA_TRANSFER_CHANNEL_LIST_s G_ChannelList = {0};
static unsigned char IsInited = 0;

/******* Framework internal function *******/
//return >=0: found, <0: Not found
static INT32 CheckChByName(char *ChName, INT32 *FreeIdx)
{
    INT32 i, First = -1;

    for (i = 0; i < G_ChannelList.TotalAmount; i++) {
        if(!strcmp(G_ChannelList.Nodes[i].Name, ChName)) { //found
            break;
        } else {
            if (First < 0) {
                if (strlen(G_ChannelList.Nodes[i].Name) == 0) {
                    First = i;
                }
            }
        }
    }

    if (FreeIdx != NULL) {
        *FreeIdx = First;
    }

    if (i == G_ChannelList.TotalAmount) { //Not found
        i = -1;
    }

    return i;
}

//return <0: Fail. Others: Idx of the node.
static INT32 InsertCh(char *Name, AMBA_TRANSFER_IMPL_s *Impl)
{
    INT32 Idx, Rval;

    Rval = CheckChByName(Name, &Idx);
    if (Rval < 0) { //New channel
        if (Idx < 0) { //List FULL!!
            ErrMsg("Cannot find free Index! Name:[%s]", Name);
            Rval = -1;
        } else {
            G_ChannelList.Nodes[Idx].Impl = Impl;
            strcpy(G_ChannelList.Nodes[Idx].Name, Name);
            Rval = Idx;
        }
    } else { //Channel exists
        Debug("Channel [%s] exists. Idx:%d", Name, Rval);
    }

    return Rval;
}

#if 0
//return 0: Success. Others: Fail
static INT32 RemoveChByName(char *Name)
{
    INT32 Rval = 0;

    Rval = CheckChByName(Name, NULL);
    if (Rval < 0) { //New channel
        Debug("Channel [%s] not exists.", Name);
        Rval = 0;
    } else { //Channel exists
        Debug("Channel [%s] exists. Idx:%d", Name, Rval);
        G_ChannelList.Nodes[Rval].Impl = NULL;
        G_ChannelList.Nodes[Rval].Name[0] = EOS;
    }

    return Rval;
}
#endif

/******* Framework interface *******/
UINT32 AmbaTransfer_GetDefConfig(AMBA_TRANSFER_DEFCONFIG_s *DefConf)
{
    UINT32 Rval;

    if (DefConf != NULL) {
        DefConf->ChAmount = AMBATRANFER_DEF_CHANNELAMOUNT;
        Rval = TRANSFER_OK;
    } else {
        ErrMsg("DefConf is NULL!!");
        Rval = TRANSFER_ERR_INVALID_CONF;
    }

    return Rval;
}

UINT32 AmbaTransfer_Init(AMBA_TRANSFER_DEFCONFIG_s *Conf)
{
    UINT32 Rval;

    Rval = TRANSFER_OK;
    if (Conf != NULL) {
        if (G_ChannelList.TotalAmount != 0) {
            ErrMsg("Already done init. req:%d, curr:%d", Conf->ChAmount, G_ChannelList.TotalAmount);
            Rval = TRANSFER_OK;
        } else {
            UINT32 MemSize = (UINT32)Conf->ChAmount * sizeof(AMBA_TRANSFER_CHANNEL_NODE_s);

            G_ChannelList.Nodes = (AMBA_TRANSFER_CHANNEL_NODE_s *)malloc(MemSize);

            if (G_ChannelList.Nodes != NULL) {
                memset(G_ChannelList.Nodes, 0, MemSize);
                G_ChannelList.TotalAmount = Conf->ChAmount;

                Debug("Create List @%p, size:%u", G_ChannelList.Nodes, MemSize);
                Rval = TRANSFER_OK;
            } else {
                ErrMsg("MemAllocate returns fail! %d", Rval);
                Rval = TRANSFER_ERR_OUT_OF_MEMORY;
            }
        }
    } else {
        ErrMsg("Conf is NULL!!");
        Rval = TRANSFER_ERR_INVALID_CONF;
    }

    return Rval;
}

UINT32 AmbaTransfer_GetConfig(AMBA_TRANSFER_CHANNEL_CONFIG_s *Conf)
{
    UINT32 Rval;

    if (Conf != NULL) {
        Conf->Name[0] = EOS;
        Conf->Impl = NULL;
        Conf->ImplSetting = NULL;
        Rval = TRANSFER_OK;
    } else {
        ErrMsg("Conf is NULL!!");
        Rval = TRANSFER_ERR_INVALID_CONF;
    }

    return Rval;
}

//Create the channel
UINT32 AmbaTransfer_Create(INT32 *Handler, AMBA_TRANSFER_CHANNEL_CONFIG_s *Conf)
{
    INT32 Idx = -1;
    UINT32 Rval;

    if (Handler != NULL){
        if(Conf != NULL) {
            if(Conf->Impl == NULL) {
                ErrMsg("Conf->Impl is NULL");
                Rval = TRANSFER_ERR_INVALID_CONF;
            } else if (Conf->Name[0] == EOS) {
                ErrMsg("Conf->Name is invalid.");
                Rval = TRANSFER_ERR_INVALID_CONF;
            } else {
                Rval = Conf->Impl->Init(Conf->ImplSetting);
                if (Rval != TRANSFER_OK) {
                    ErrMsg("Impl->Init() returns Fail. %d", Rval);
                } else {
                    Idx = InsertCh(Conf->Name, Conf->Impl);
                    if (Idx < 0) {
                        ErrMsg("InsertCh returns Fail! %d", Idx);
                        Rval = TRANSFER_ERR_CHANNEL_ERROR;
                    } else {
                        Debug("%s @ %d", Conf->Name, Idx);
                        Rval = TRANSFER_OK;
                    }
                }
            }
        } else {
            ErrMsg("Conf is NULL!!");
            Rval = TRANSFER_ERR_INVALID_CONF;
        }
        *Handler = Idx;
    } else {
        ErrMsg("Handler is NULL!!");
        Rval = TRANSFER_ERR_INVALID_HANDLER;
    }

    return Rval;
}

UINT32 AmbaTransfer_Delete(INT32 Handler, void *Info)
{
    UINT32 Rval;
    const AMBA_TRANSFER_IMPL_s *Impl;

    if ((Handler >= 0) && (Handler < G_ChannelList.TotalAmount)) {
        Impl = G_ChannelList.Nodes[Handler].Impl;
        Rval = Impl->Release(Info);
        if (Rval == TRANSFER_OK) {
             memset(&G_ChannelList.Nodes[Handler], 0, sizeof(AMBA_TRANSFER_CHANNEL_NODE_s));
        } else {
            ErrMsg("Fail to delete. Rval = %d", Rval);
        }
    } else {
        ErrMsg("Invalid Handler!! (%d)", Handler);
        Rval = TRANSFER_ERR_INVALID_HANDLER;
    }

    return Rval;
}

//Sender interface
//[Client] try to connect with specified channel
UINT32 AmbaTransfer_Connect(INT32 *Handler, AMBA_TRANSFER_CONNECT_s *Info)
{
    INT32 Idx = -1;
    UINT32 Rval;

    if (Handler != NULL){
        if(Info != NULL) {
            if (Info->Name[0] == EOS) {
                ErrMsg("Info->Name is invalid.");
                Rval = TRANSFER_ERR_INVALID_INFO;
            } else {
                if (!IsInited) {
                    AMBA_TRANSFER_DEFCONFIG_s Conf = {.ChAmount = AMBATRANFER_DEF_CHANNELAMOUNT};
                    Rval = AmbaTransfer_Init(&Conf);
                    if (Rval == TRANSFER_OK) {
                        IsInited = 1;
                    } else {
                        ErrMsg("Fail to Init");
                        return Rval;
                    }
                }

                Idx = CheckChByName(Info->Name, NULL);
                if (Idx < 0) {
                    AMBA_TRANSFER_CHANNEL_CONFIG_s Conf;
                    ErrMsg("Channel not found! %d", Idx);
                    Rval = TRANSFER_ERR_CHANNEL_ERROR;

                    if (!strcmp(Info->Name, "TRANSCAMCTRL")) {
                        extern AMBA_TRANSFER_IMPL_s Transfer_Impl_Camctrl;

                        strcpy(Conf.Name, Info->Name);
                        Conf.Impl = &Transfer_Impl_Camctrl;
                        Conf.ImplSetting = NULL;

                        Rval = AmbaTransfer_Create(Handler, &Conf);
                        if (Rval != TRANSFER_OK) {
                            ErrMsg("Fail to AmbaTransfer_Create");
                        } else {
                            Idx = *Handler;
                        }
                    }
                }

                if (Idx >= 0) {
                    const AMBA_TRANSFER_IMPL_s *Impl;

                    Debug("Found %s @ %d", Info->Name, Idx);
                    Impl = G_ChannelList.Nodes[Idx].Impl;
                    Rval = Impl->Connect(Info);
                }
            }
        } else {
            ErrMsg("Info is NULL!!");
            Rval = TRANSFER_ERR_INVALID_INFO;
        }
        *Handler = Idx;
    } else {
        ErrMsg("Handler is NULL!!");
        Rval = TRANSFER_ERR_INVALID_HANDLER;
    }

    return Rval;
}

//[Client] try to disconnect with specified channel
UINT32 AmbaTransfer_Disconnect(INT32 Handler, AMBA_TRANSFER_DISCONNECT_s *Info)
{
    UINT32 Rval;

    if ((Handler >= 0) && (Handler < G_ChannelList.TotalAmount)) {
        if(Info != NULL) {
            const AMBA_TRANSFER_IMPL_s *Impl;

            Impl = G_ChannelList.Nodes[Handler].Impl;
            Rval = Impl->Disconnect(Info);
        } else {
            ErrMsg("Info is NULL!!");
            Rval = TRANSFER_ERR_INVALID_INFO;
        }
    } else {
        ErrMsg("Invalid Handler!! (%d)", Handler);
        Rval = TRANSFER_ERR_INVALID_HANDLER;
    }

    return Rval;
}

//[Client] try to send data
UINT32 AmbaTransfer_Send(INT32 Handler, AMBA_TRANSFER_SEND_s *Info)
{
    UINT32 Rval;

    if ((Handler >= 0) && (Handler < G_ChannelList.TotalAmount)) {
        if(Info != NULL) {
            const AMBA_TRANSFER_IMPL_s *Impl;

            Impl = G_ChannelList.Nodes[Handler].Impl;
            Rval = Impl->Send(Info);
        } else {
            ErrMsg("Info is NULL!!");
            Rval = TRANSFER_ERR_INVALID_INFO;
        }
    } else {
        ErrMsg("Invalid Handler!! (%d)", Handler);
        Rval = TRANSFER_ERR_INVALID_HANDLER;
    }

    return Rval;
}

//Receiver interface
//[Server] try to register service function for specified channel
UINT32 AmbaTransfer_Register(INT32 *Handler, AMBA_TRANSFER_REGISTER_s *Info)
{
    INT32 Idx = -1;
    UINT32 Rval;

    if (Handler != NULL){
        if(Info != NULL) {
            if (Info->Name[0] == EOS) {
                ErrMsg("Info->Name is invalid.");
                Rval = TRANSFER_ERR_INVALID_INFO;
            } else {
                Idx = CheckChByName(Info->Name, NULL);
                if (Idx < 0) {
                    ErrMsg("Channel not found! %d", Idx);
                    Rval = TRANSFER_ERR_CHANNEL_ERROR;
                } else {
                    const AMBA_TRANSFER_IMPL_s *Impl;
                    Debug("Found %s @ %d", Info->Name, Idx);
                    Impl = G_ChannelList.Nodes[Idx].Impl;
                    Rval = Impl->Register(Info);
                }
            }
        } else {
            ErrMsg("Info is NULL!!");
            Rval = TRANSFER_ERR_INVALID_INFO;
        }
        *Handler = Idx;
    } else {
        ErrMsg("Handler is NULL!!");
        Rval = TRANSFER_ERR_INVALID_HANDLER;
    }

    return Rval;
}

//[Server] try to unregister service function for specified channel
UINT32 AmbaTransfer_Unregister(INT32 Handler, AMBA_TRANSFER_UNREGISTER_s *Info)
{
    UINT32 Rval;

    if ((Handler >= 0) && (Handler < G_ChannelList.TotalAmount)) {
        if(Info != NULL) {
            const AMBA_TRANSFER_IMPL_s *Impl;

            Impl = G_ChannelList.Nodes[Handler].Impl;
            Rval = Impl->Unregister(Info);
        } else {
            ErrMsg("Info is NULL!!");
            Rval = TRANSFER_ERR_INVALID_INFO;
        }
    } else {
        ErrMsg("Invalid Handler!! (%d)", Handler);
        Rval = TRANSFER_ERR_INVALID_HANDLER;
    }

    return Rval;
}

//[Server] try to receive data. Blocking call.
UINT32 AmbaTransfer_Receive(INT32 Handler, AMBA_TRANSFER_RECEIVE_s *Info)
{
    UINT32 Rval;

    if ((Handler >= 0) && (Handler < G_ChannelList.TotalAmount)) {
        if(Info != NULL) {
            const AMBA_TRANSFER_IMPL_s *Impl;

            Impl = G_ChannelList.Nodes[Handler].Impl;
            Rval = Impl->Recv(Info);
        } else {
            ErrMsg("Info is NULL!!");
            Rval = TRANSFER_ERR_INVALID_INFO;
        }
    } else {
        ErrMsg("Invalid Handler!! (%d)", Handler);
        Rval = TRANSFER_ERR_INVALID_HANDLER;
    }

    return Rval;
}

UINT32 AmbaTransfer_GetStatus(INT32 Handler, AMBA_TRANSFER_GETSTATUS_s *Info)
{
    UINT32 Rval;

    if ((Handler >= 0) && (Handler < G_ChannelList.TotalAmount)) {
        if(Info != NULL) {
            const AMBA_TRANSFER_IMPL_s *Impl;

            Impl = G_ChannelList.Nodes[Handler].Impl;
            Rval = Impl->GetStatus(Info);
        } else {
            ErrMsg("Info is NULL!!");
            Rval = TRANSFER_ERR_INVALID_INFO;
        }
    } else {
        ErrMsg("Invalid Handler!! (%d)", Handler);
        Rval = TRANSFER_ERR_INVALID_HANDLER;
    }

    return Rval;
}

