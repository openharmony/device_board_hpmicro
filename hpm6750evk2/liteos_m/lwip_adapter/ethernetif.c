/*
 * Copyright (c) 2022 HPMicro
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "hpm_lwip.h"
#include "ethernetif.h"

#include "lwip/def.h"
#include "lwip/mem.h"
#include "lwip/pbuf.h"
#include "netif/etharp.h"
#include "lwip/err.h"
#include "lwip/timeouts.h"
#include "ethernetif.h"
#include "hpm_enet_drv.h"
#include <string.h>
#include <los_task.h>
#include <los_sem.h>

/**
* In this function, the hardware should be initialized.
* Called from ethernetif_init().
*
* @param netif the already initialized lwip network interface structure
*        for this ethernetif
*/
static void low_level_init(struct netif *netif)
{
    struct HpmEnetDevice *dev = (struct HpmEnetDevice *)netif->state;
    /* set netif MAC hardware address length */
    netif->hwaddr_len = ETHARP_HWADDR_LEN;

    /* set netif MAC hardware address */
    netif->hwaddr[0] =  dev->macAddr[0];
    netif->hwaddr[1] =  dev->macAddr[1];
    netif->hwaddr[2] =  dev->macAddr[2];
    netif->hwaddr[3] =  dev->macAddr[3];
    netif->hwaddr[4] =  dev->macAddr[4];
    netif->hwaddr[5] =  dev->macAddr[5];

    /* set netif maximum transfer unit */
    netif->mtu = 1500;

    /* need to judge from phy status */
    netif->flags |= NETIF_FLAG_LINK_UP;

    /* Accept broadcast address and ARP traffic */
    netif->flags |= NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_IGMP;
}


/**
* This function should do the actual transmission of the packet. The packet is
* contained in the pbuf that is passed to the function. This pbuf
* might be chained.
*
* @param netif the lwip network interface structure for this ethernetif
* @param p the MAC packet to send (e.g. IP packet including MAC addresses and type)
* @return ERR_OK if the packet could be sent
*         an err_t value if the packet couldn't be sent
*
* @note Returning ERR_MEM here if a DMA queue of your MAC is full can lead to
*       strange results. You might consider waiting for space in the DMA queue
*       to become availale since the stack doesn't retry to send a packet
*       dropped because of memory failure (except for the TCP timers).
*/

static err_t low_level_output(struct netif *netif, struct pbuf *p)
{
    struct HpmEnetDevice *dev = (struct HpmEnetDevice *)netif->state;
    enet_desc_t *desc = &dev->desc;
    uint32_t tx_buff_size = desc->tx_buff_cfg.size;
    struct pbuf *q;
    uint8_t *buffer;
    __IO enet_tx_desc_t *dma_tx_desc;
    uint32_t frame_length = 0;
    uint32_t buffer_offset = 0;
    uint32_t bytes_left_to_copy = 0;
    uint32_t payload_offset = 0;
    enet_tx_desc_t  *tx_desc_list_cur = desc->tx_desc_list_cur;

    dma_tx_desc = tx_desc_list_cur;
    buffer = (uint8_t *)(dma_tx_desc->tdes2_bm.buffer1);
    buffer_offset = 0;

    /* copy frame from pbufs to driver buffers */
    for (q = p; q != NULL; q = q->next)
    {
        /* Get bytes in current lwIP buffer */
        bytes_left_to_copy = q->len;
        payload_offset = 0;

        /* Check if the length of data to copy is bigger than Tx buffer size*/
        while ((bytes_left_to_copy + buffer_offset) > tx_buff_size)
        {
            /* Copy data to Tx buffer*/
            memcpy((uint8_t *)((uint8_t *)buffer + buffer_offset),
                    (uint8_t *)((uint8_t *)q->payload + payload_offset),
                    tx_buff_size - buffer_offset);

            /* Point to next descriptor */
            dma_tx_desc = (enet_tx_desc_t *)(dma_tx_desc->tdes3_bm.next_desc);

            /* Check if the buffer is available */
            if (dma_tx_desc->tdes0_bm.own != 0)
            {
                printf("DMA tx desc buffer is not valid\n");
                return ERR_USE;
            }

            buffer = (uint8_t *)(dma_tx_desc->tdes2_bm.buffer1);

            bytes_left_to_copy = bytes_left_to_copy - (tx_buff_size - buffer_offset);
            payload_offset = payload_offset + (tx_buff_size - buffer_offset);
            frame_length = frame_length + (tx_buff_size - buffer_offset);
            buffer_offset = 0;
        }

        /* Copy the remaining bytes */
        memcpy((uint8_t *)((uint8_t *)buffer + buffer_offset),
                (uint8_t *)((uint8_t *)q->payload + payload_offset),
                bytes_left_to_copy);

        buffer_offset = buffer_offset + bytes_left_to_copy;
        frame_length = frame_length + bytes_left_to_copy;
    }

    /* Prepare transmit descriptors to give to DMA */


    frame_length += 4;
    enet_prepare_transmission_descriptors(dev->base, &desc->tx_desc_list_cur, frame_length, desc->tx_buff_cfg.size);

    return ERR_OK;
}

/**
* Should allocate a pbuf and transfer the bytes of the incoming
* packet from the interface into the pbuf.
*
* @param netif the lwip network interface structure for this ethernetif
* @return a pbuf filled with the received packet (including MAC header)
*         NULL on memory error
*/
static struct pbuf *low_level_input(struct netif *netif)
{
    struct HpmEnetDevice *dev = (struct HpmEnetDevice *)netif->state;
    enet_desc_t *desc = &dev->desc;
    struct pbuf *p = NULL, *q = NULL;
    uint32_t rx_buff_size = desc->rx_buff_cfg.size;
    uint16_t len = 0;
    uint8_t *buffer;
    enet_frame_t frame = {0, 0, 0};
    enet_rx_desc_t *dma_rx_desc;
    uint32_t buffer_offset = 0;
    uint32_t payload_offset = 0;
    uint32_t bytes_left_to_copy = 0;
    uint32_t i = 0;

    /* Get a received frame */
    frame = enet_get_received_frame_interrupt(&desc->rx_desc_list_cur,
                                              &desc->rx_frame_info,
                                              desc->rx_buff_cfg.count);

    /* Obtain the size of the packet and put it into the "len" variable. */
    len = frame.length;
    buffer = (uint8_t *)frame.buffer;

    if (len > 0)
    {
        /* allocate a pbuf chain of pbufs from the Lwip buffer pool */
        p = pbuf_alloc(PBUF_RAW, len, PBUF_POOL);
    }

    if (p != NULL)
    {
        dma_rx_desc = frame.rx_desc;
        buffer_offset = 0;
        for (q = p; q != NULL; q = q->next)
        {
            bytes_left_to_copy = q->len;
            payload_offset = 0;

            /* Check if the length of bytes to copy in current pbuf is bigger than Rx buffer size*/
            while ((bytes_left_to_copy + buffer_offset) > rx_buff_size)
            {
                /* Copy data to pbuf */
                memcpy((uint8_t *)((uint8_t *)q->payload + payload_offset), (uint8_t *)((uint8_t *)buffer + buffer_offset), (rx_buff_size - buffer_offset));

                /* Point to next descriptor */
                dma_rx_desc = (enet_rx_desc_t *)(dma_rx_desc->rdes3_bm.next_desc);
                buffer = (uint8_t *)(dma_rx_desc->rdes2_bm.buffer1);

                bytes_left_to_copy = bytes_left_to_copy - (rx_buff_size - buffer_offset);
                payload_offset = payload_offset + (rx_buff_size - buffer_offset);
                buffer_offset = 0;
            }
            /* Copy remaining data in pbuf */
            memcpy((uint8_t *)((uint8_t *)q->payload + payload_offset), (uint8_t *)((uint8_t *)buffer + buffer_offset), bytes_left_to_copy);
            buffer_offset = buffer_offset + bytes_left_to_copy;
        }
    }
    else
    {
        return NULL;
    }

    /* Release descriptors to DMA */
    /* Point to first descriptor */
    dma_rx_desc = frame.rx_desc;

    /* Set Own bit in Rx descriptors: gives the buffers back to DMA */
    for (i = 0; i < desc->rx_frame_info.seg_count; i++)
    {
        dma_rx_desc->rdes0_bm.own = 1;
        dma_rx_desc = (enet_rx_desc_t*)(dma_rx_desc->rdes3_bm.next_desc);
    }

    /* Clear Segment_Count */
    desc->rx_frame_info.seg_count = 0;

    return p;
}


/**
* This function is the ethernetif_input task, it is processed when a packet
* is ready to be read from the interface. It uses the function low_level_input()
* that should handle the actual reception of bytes from the network
* interface. Then the type of the received packet is determined and
* the appropriate input function is called.
*
* @param netif the lwip network interface structure for this ethernetif
*/

 /*
  invoked after receiving data packet
 */
err_t ethernetif_input(struct netif *netif)
{
    err_t err;
    struct pbuf *p = NULL;
    /* move received packet into a new pbuf */
    while ((p = low_level_input(netif)) != NULL) {
        /* no packet could be read, silently ignore this */
        if (p == NULL) {
            return ERR_MEM;
        }

        /* entry point to the LwIP stack */
        err = netif->input(p, netif);

        if (err != ERR_OK) {
            LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_input: IP input error\n"));
            pbuf_free(p);
        }
    }
    return err;
}

static VOID *ethernetif_recv_thread(UINT32 arg)
{
    struct netif *netif = (struct netif *)arg;
    struct HpmEnetDevice *dev = (struct HpmEnetDevice *)netif->state;

    while (1) {
        LOS_SemPend(dev->rxSemHandle, 0xffffffff);
        ethernetif_input(netif);
    }
}

static __attribute__((section(".interrupt.text"))) VOID hpm_enet_isr(VOID *parm)
{
    struct netif *netif = (struct netif *)parm;
    struct HpmEnetDevice *dev = (struct HpmEnetDevice *)netif->state;

    uint32_t status = dev->base->DMA_STATUS;

    if (ENET_DMA_STATUS_GLPII_GET(status)) {
        dev->base->DMA_STATUS |= ENET_DMA_STATUS_GLPII_SET(ENET_DMA_STATUS_GLPII_GET(status));
    }

    if (ENET_DMA_STATUS_RI_GET(status)) {
        dev->base->DMA_STATUS |= ENET_DMA_STATUS_RI_SET(ENET_DMA_STATUS_RI_GET(status));
        LOS_SemPost(dev->rxSemHandle);
    } else {
        printf("error ---status = 0x%X\n", status);
    }
}

void ethernetif_recv_start(struct netif *netif)
{
    struct HpmEnetDevice *dev = (struct HpmEnetDevice *)netif->state;
    UINT32 taskID = LOS_ERRNO_TSK_ID_INVALID;
    UINT32 ret;
    TSK_INIT_PARAM_S task = {0};

    LOS_SemCreate(0, &dev->rxSemHandle);

    HwiIrqParam irqParam;
    irqParam.pDevId = netif;
    LOS_HwiCreate(HPM2LITEOS_IRQ(dev->irqNum), 1, 0, (HWI_PROC_FUNC)hpm_enet_isr, &irqParam);
    LOS_HwiEnable(HPM2LITEOS_IRQ(dev->irqNum));

    /* Create host Task */
    task.pfnTaskEntry = (TSK_ENTRY_FUNC)ethernetif_recv_thread;
    task.uwStackSize = 4096;
    task.pcName = (char *)(dev->name);
    task.usTaskPrio = 3;
    task.uwArg = (UINTPTR)netif;
    task.uwResved = LOS_TASK_STATUS_DETACHED;
    ret = LOS_TaskCreate(&taskID, &task);
    if (ret != LOS_OK) {
        LWIP_DEBUGF(SYS_DEBUG, ("sys_thread_new: LOS_TaskCreate error %u\n", ret));
        return -1;
    }
}

/**
* Should be called at the beginning of the program to set up the
* network interface. It calls the function low_level_init() to do the
* actual setup of the hardware.
*
* This function should be passed as a parameter to netif_add().
*
* @param netif the lwip network interface structure for this ethernetif
* @return ERR_OK if the loopif is initialized
*         ERR_MEM if private data couldn't be allocated
*         any other err_t on error
*/
err_t ethernetif_init(struct netif *netif)
{
    LWIP_ASSERT("netif != NULL", (netif != NULL));
    struct HpmEnetDevice *dev = (struct HpmEnetDevice *)netif->state;

#if LWIP_NETIF_HOSTNAME
    /* Initialize interface hostname */
    netif->hostname = "lwip";
#endif /* LWIP_NETIF_HOSTNAME */

    netif->name[0] = dev->name[0];
    netif->name[1] = dev->name[1];

    netif->output = etharp_output;
    netif->linkoutput = low_level_output;

    /* initialize the hardware */
    low_level_init(netif);

    ethernetif_recv_start(netif);

    return ERR_OK;
}
