/**
 * Copyright (c) 2021 Ambarella International LP
 *
 * This file and its contents ("Software") are protected by intellectual
 * property rights including, without limitation, U.S. and/or foreign
 * copyrights. This Software is also the confidential and proprietary
 * information of Ambarella International LP and its licensors. You may not use, reproduce,
 * disclose, distribute, modify, or otherwise prepare derivative works of this
 * Software or any portion thereof except pursuant to a signed license agreement
 * or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
 * In the absence of such an agreement, you agree to promptly notify and return
 * this Software to Ambarella International LP
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 * MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */
#include <config.h>
#include <memory.h>
#include <uart.h>
#include <io.h>
#include <arch.h>
#include <common.h>

#define USBD_REG(x)		(USBD_BASE + (x))

#define USBD_CFG		USBD_REG(0x0400)
#define USBD_CTRL		USBD_REG(0x0404)
#define USBD_STA		USBD_REG(0x0408)
#define USBD_INT		USBD_REG(0x040C)
#define USBD_INT_MASK		USBD_REG(0x0410)
#define USB_EP_INT		USBD_REG(0x0414)
#define USB_EP_INT_MASK		USBD_REG(0x0418)

#define USB_EP_CTRL(ep)		USBD_REG((ep) * 0x20 + 0x0000)
#define USB_EP_STA(ep)		USBD_REG((ep) * 0x20 + 0x0004)
#define USB_EP_IN_BUFSZ(ep)	USBD_REG((ep) * 0x20 + 0x0008)
#define USB_EP_OUT_PKFM(ep)	USBD_REG((ep) * 0x20 + 0x0008)
#define USB_EP_MAX_PKSZ(ep)	USBD_REG((ep) * 0x20 + 0x000c)
#define USB_EP_SETUP_PTR(ep)	USBD_REG((ep) * 0x20 + 0x0010)
#define USB_EP_DESC_PTR(ep)	USBD_REG((ep) * 0x20 + 0x0014)

#define UDC2O_REG(ep)		USBD_REG((ep) * 0x4 + 0x0504)

#define EP_TYPE_CTRL		0
#define EP_TYPE_ISO		1
#define EP_TYPE_BULK		2
#define EP_TYPE_INT		3

#define USB_DEVICE_DESCRIPTOR		1
#define USB_CONFIGURATION_DESCRIPTOR	2
#define USB_STRING_DESCRIPTOR		3
#define USB_INTERFACE_DESCRIPTOR	4
#define USB_ENDPOINT_DESCRIPTOR		5

#define EP_CTRL_IN		0
#define EP_BULK_IN		1
#define EP_CTRL_OUT		16
#define EP_BULK_OUT		17

#define RFSM_STATE_CMD		0x5a
#define RFSM_STATE_DOWNLOAD	0x5b

#define SETUP(typ, req)		(((typ) << 8) | (req))

struct setup_desc {
	unsigned int status;
	unsigned int reserved;
	unsigned int data0;
	unsigned int data1;
};

struct ep_data_req {
	unsigned int status;
	unsigned int reserved;
	unsigned int data_ptr;
	unsigned int next_desc_ptr;
};

struct ep_data_list
{
	unsigned char data[512];
	unsigned char used;
	unsigned int size;
	struct ep_data_list *next;
};

struct udc_ep {
	unsigned char in;
	unsigned char num;
	unsigned char type;
	volatile unsigned char tx_busy;
	unsigned int maxpkt;
	struct ep_data_req *req;
	void (*complete)(struct ep_data_req *req);
};

struct setup_packet
{
	unsigned char type;
	unsigned char request;
	unsigned short value;
	unsigned short index;
	unsigned short length;
} __attribute__ ((packed));


struct udc_descriptor {
	struct udc_descriptor *next;
	unsigned short value;
	unsigned short len;
	unsigned char data[0];
};

static struct udc_descriptor *desc_list = NULL;
static struct ep_data_list *rx_list = NULL;
static unsigned char *desc_pool;
static unsigned int desc_pool_offset = 0;
static const char manufacturer[] = "Ambarella";
static const char product[] = "Ambarella USB generic class";
static const char serialno[] = "123456789ABC";
static const char interface[] = "fastboot";
static const char *logo = "\r\n*** Ambarella Recovery ***\r\n";
static struct setup_desc *setup;

static struct ep_data_list *ep_data_pool;
static int rfsm_state = RFSM_STATE_CMD;
static volatile unsigned short vendor_id;
static volatile unsigned short product_id;

void (*udc_data_handle)(void *data, unsigned int size);
void (*udc_cmd_handle)(void *data, unsigned int size);

static void epin_ctrl_complete(struct ep_data_req *req);
static void epin_bulk_complete(struct ep_data_req *req);
static void epout_ctrl_complete(struct ep_data_req *req);
static void epout_bulk_complete(struct ep_data_req *req);

static struct udc_ep ept[] = {
	[0] = {
		.in = 1,
		.num = EP_CTRL_IN,
		.type = EP_TYPE_CTRL,
		.maxpkt = 64,
		.complete = epin_ctrl_complete,
	},

	[1] = {
		.in = 1,
		.num = EP_BULK_IN,
		.type = EP_TYPE_BULK,
		.maxpkt = 512,
		.tx_busy = 0,
		.complete = epin_bulk_complete,
	},
	[2] = {
		.in = 0,
		.num = EP_CTRL_OUT,
		.type = EP_TYPE_CTRL,
		.maxpkt = 64,
		.complete = epout_ctrl_complete,
	},

	[3] = {
		.in = 0,
		.num = EP_BULK_OUT,
		.type = EP_TYPE_BULK,
		.maxpkt = 512,
		.complete = epout_bulk_complete,
	},
};


static void wait_sof_complete(int timeout)
{
	int ssof, nsof;

	ssof = (readl(USBD_STA) >> 18);

	do {
		nsof = (readl(USBD_STA) >> 18);
		if (ssof != nsof) {
			ssof = nsof;
			timeout --;
		}
	} while(timeout > 0);
}

static struct ep_data_list *get_free_ep_data_list(void)
{
	int i;
	for (i = 0; i < 32; i++)
		if (!ep_data_pool[i].used)
			break;

	ep_data_pool[i].used = 1;

	return &ep_data_pool[i];
}
static void enable_device_rx_dma(void)
{
	setbitsl(USBD_CTRL, 1 << 2);
}

static void set_endp_nak(int ep_num)
{
	setbitsl(USB_EP_CTRL(ep_num), 1 << 7);
}

static void clear_endp_nak(int ep_num)
{
	setbitsl(USB_EP_CTRL(ep_num), 1 << 8);
}

static void poll_endp_tx(int ep_num)
{
	setbitsl(USB_EP_CTRL(ep_num), 1 << 3);
}

static void epin_ctrl_complete(struct ep_data_req *req)
{
	pr_debug("epin_ctrl_complete.\n");
}
static void epin_bulk_complete(struct ep_data_req *req)
{
	pr_debug("epout_bulk_complete.\n");
}
static void epout_ctrl_complete(struct ep_data_req *req)
{
	pr_debug("epout_ctrl_complete.\n");
	enable_device_rx_dma();
}
static void epout_bulk_complete(struct ep_data_req *req)
{
	int size = req->status & 0xffff;
	struct ep_data_list *edata;

	pr_debug("epout_bulk_complete.\n");
	pr_debug("request status = %08x\n", req->status);

	edata = get_free_ep_data_list();
	memcpy(edata->data, (void *)(unsigned long)req->data_ptr, size);
	edata->data[size] = '\0';
	edata->size = size;
	edata->next = rx_list;
	rx_list = edata;

	set_endp_nak(EP_BULK_IN);
}

static void *alloc_desc(int size)
{
	void *rval;

	rval = (void *)&desc_pool[desc_pool_offset];
	desc_pool_offset += ((size + 63) / 64) * 64;
	return rval;
}

static void ep_setup_desc_prepare(int ep_num)
{
	setup->status = 0;
	setup->reserved = 0xffffffff;
	setup->data0 = 0xffffffff;
	setup->data1 = 0xffffffff;

	writel(USB_EP_SETUP_PTR(ep_num), (unsigned long)setup);

	enable_device_rx_dma();
}

static struct udc_ep *get_udc_ep_by_num(int ep_num)
{
	int ep_idx;
	for (ep_idx = 0; ep_idx < (sizeof(ept) / sizeof(struct udc_ep)); ep_idx++)
		if(ept[ep_idx].num == ep_num)
			break;

	return &ept[ep_idx];
}
static void start_rx(int ep_num)
{
	struct udc_ep *ep = get_udc_ep_by_num(ep_num);
	struct ep_data_req *req = ep->req;

	req->status = (0x0 << 30) | (0x1 << 27);

	enable_device_rx_dma();
	clear_endp_nak(ep_num);
}

static void start_tx(int ep_num, void *buffer, int len)
{
	struct udc_ep *ep = get_udc_ep_by_num(ep_num);
	struct ep_data_req *req = ep->req;

	ep->tx_busy = 1;
	memcpy((void *)(unsigned long)req->data_ptr, buffer, len);
	req->status = (0x0 << 30) | (0x1 << 27) | len;

	clear_endp_nak(ep_num);
	poll_endp_tx(ep_num);
	wait_sof_complete(1000);
}

static void setup_tx(void *buffer, int len)
{
	struct udc_ep *ep = get_udc_ep_by_num(EP_CTRL_IN);
	struct ep_data_req *req = ep->req;

	memcpy((void *)(unsigned long)req->data_ptr, buffer, len);
	req->status = (0x0 << 30) | (0x1 << 27) | len;

	clear_endp_nak(EP_CTRL_IN);
	poll_endp_tx(EP_CTRL_IN);
	wait_sof_complete(1000);

	start_rx(EP_CTRL_OUT);
}

static void udc_device_handle(unsigned int value)
{
	unsigned int status, enum_speed, endpval;
	int i;

	switch (value) {
	case 1 << 6:		/* speed enumeration complete */
		status = readl(USBD_STA);
		enum_speed = (status >> 13) & 0x3;
		if (enum_speed) {
			uart_putstr("Enumerated FULL/LOW speed [Unsupported]\n");
			break;
		}

		uart_putstr("Enumerated HIGH speed\n");

		for (i = 0; i < (sizeof(ept) / sizeof(struct udc_ep)); i++) {

			endpval = 1 << 7;
			endpval |= ((ept[i].type == EP_TYPE_CTRL) ? (0 << 0) : (1 << 0));
			endpval |= (ept[i].in ? (1 << 4) : (0 << 4));
			endpval |= (ept[i].type << 5);
			endpval |= (ept[i].maxpkt << 19);
			writel(UDC2O_REG(i), endpval);
		}

		setbitsl(USBD_CFG, 1 << 17);		/* enable dynamic UDC programing .*/
		break;
	case 1 << 1:		/* set interface */
	case 1 << 0:		/* set configuration */
		setbitsl(USBD_CTRL, 1 << 13);		/* dynamic UDC programing complete */
		start_rx(EP_BULK_OUT);
		break;
	default:
		;
#if 0
		pr_log("device int->%x has no handle!\n", value);
#endif
	}
}
void epout_handle_setup(unsigned int ep)
{
	struct setup_packet *s;
	struct udc_descriptor *desc;
	int len;

	s = (struct setup_packet *)&setup->data0;

	pr_debug("type: %02x, request: %02x, value: %04x, index: %04x, length: %04x\n",
			s->type,
			s->request,
			s->value,
			s->index,
			s->length);
	switch (SETUP(s->type, s->request))
	{
		case SETUP(0x80, 0x06):
			for (desc = desc_list; desc; desc = desc->next)
				if(desc->value == s->value) {
					len = (desc->len > s->length ? s->length : desc->len);
					setup_tx(desc->data, len);
					break;
				}
			break;
		default:
#if 0
			pr_log("type: %02x, request: %02x, value: %04x, index: %04x, length: %04x\n",
					s->type,
					s->request,
					s->value,
					s->index,
					s->length);
#endif
			break;
	}
	ep_setup_desc_prepare(ep);
}

static void udc_epin_handle(unsigned int ep)
{
	unsigned int status;
	struct udc_ep *endp = get_udc_ep_by_num(ep);

	status = readl(USB_EP_STA(ep));
	writel(USB_EP_STA(ep), status);

	pr_debug("%s, ep = %d, ep_status = %08x\n",
			__func__, ep, status);

	if (status & (1 << 28))
		pr_debug("epin: cdc_clear.\n");
	if (status & (1 << 27))
		pr_debug("epin: xfer done.\n");
	if (status & (1 << 26))
		pr_debug("epin: receive set stall.\n");
	if (status & (1 << 25))
		pr_debug("epin: receive clear stall.\n");
	if (status & (1 << 23))
		pr_debug("epin: ISO in done.\n");
	if (status & (1 << 10)) {
		endp->tx_busy = 0;
		pr_debug("epin: TX DMA done.\n");
	} if (status & (1 << 9))
		pr_debug("epin: Error response.\n");
	if (status & (1 << 7))
		pr_debug("epin: buffer not availiable.\n");
	if (status & (1 << 6)) {
		pr_debug("epin: IN token.\n");
		set_endp_nak(ep);
	}

	if ((status & (1 << 4)) == (1 << 4))
		pr_debug("epin: receive data.\n");

	if ((status & (2 << 4)) == (2 << 4)) {
		pr_debug("epin: receive SETUP data.\n");
	}
}

static void udc_epout_handle(unsigned int ep)
{
	unsigned int status;
	struct udc_ep *endp = get_udc_ep_by_num(ep);
	struct ep_data_req *req = endp->req;

	status = readl(USB_EP_STA(ep));
	writel(USB_EP_STA(ep), status);

	pr_debug("[%d] %s %08x\n",ep, __func__, status);

	if (status & (1 << 28))
		pr_debug("cdc_clear.\n");
	if (status & (1 << 27))
		pr_debug("xfer done.\n");
	if (status & (1 << 26))
		pr_debug("receive set stall.\n");
	if (status & (1 << 25))
		pr_debug("receive clear stall.\n");
	if (status & (1 << 23))
		pr_debug("ISO in done.\n");
	if (status & (1 << 10))
		pr_debug("TX DMA done.\n");
	if (status & (1 << 9))
		pr_debug("Error response.\n");
	if (status & (1 << 7))
		pr_debug("buffer not availiable.\n");
	if (status & (1 << 6))
		pr_debug("IN token.\n");

	if ((status & (1 << 4)) == (1 << 4)) {
		pr_debug("receive data.\n");
		if (endp->complete)
			endp->complete(req);
	}

	if ((status & (2 << 4)) == (2 << 4)) {
		pr_debug("receive SETUP data.\n");
		epout_handle_setup(ep);
	}

}

void udc_irq_handle(void *unused)
{
	unsigned int value;
	int ep;

	value = readl(USBD_INT);
	if (value) {
		writel(USBD_INT, value);

		udc_device_handle(value);
	}

	value = readl(USB_EP_INT);
	if (value) {
		writel(USB_EP_INT, value);

		for (ep = 0; ep < (sizeof(ept) / sizeof(struct udc_ep)); ep++)
			if((1 << ept[ep].num) & value) {
				if (!ept[ep].in)
					udc_epout_handle(ept[ep].num);
				else
					udc_epin_handle(ept[ep].num);
			}
	}
}

static void udc_endpoint_config(int ep_index)
{
	struct udc_ep *ep = &ept[ep_index];
	struct ep_data_req *req;

	/* set endpoint type */
	writel(USB_EP_CTRL(ep->num), ep->type << 4);

	/* set 'epin' buffer size */
	if (ep->in)
		writel(USB_EP_IN_BUFSZ(ep->num), ep->maxpkt / 4);

	if (!ep->in && ep->type == EP_TYPE_CTRL)
		ep_setup_desc_prepare(ep->num);

	/* set 'epout' max packet size */
	writel(USB_EP_MAX_PKSZ(ep->num), ep->maxpkt);

	req = kmalloc(sizeof(struct ep_data_req));
	if(!req) {
#if 0
		pr_log("malloc data request error.\n");
#endif
		return ;
	}

	ep->req = req;

	req->data_ptr = (unsigned long)kmalloc(ep->maxpkt);
	if (!req->data_ptr) {
#if 0
		pr_log("malloc data buffer error.\n");
#endif
		return ;
	}

	pr_debug("req: %08x, data_ptr: %08x\n", req, req->data_ptr);

	/* DMA done | LAST frame */
	req->status = (0x2 << 30) | (0x1 << 27);
	req->next_desc_ptr = (unsigned long)req;

	writel(USB_EP_DESC_PTR(ep->num), (unsigned long)req);
}

static struct udc_descriptor *udc_alloc_desc(char type, char num, char len)
{
	struct udc_descriptor *desc =
		(struct udc_descriptor *)alloc_desc(sizeof(struct udc_descriptor) + len);

	desc->next = NULL;
	desc->value = (type << 8) | num;
	desc->len = len;
	desc->data[0] = len;
	desc->data[1] = type;

	return desc;
}
static void udc_descriptor_register(struct udc_descriptor *desc)
{
	desc->next = desc_list;
	desc_list = desc;
}

static void udc_descriptor_language(void)
{
	struct udc_descriptor *desc;

	desc = udc_alloc_desc(USB_STRING_DESCRIPTOR, 0, 4);
	desc->data[2] = 0x09;
	desc->data[3] = 0x04;

	udc_descriptor_register(desc);
}
static void __udc_descriptor_string(int idx, const char *string)
{

	struct udc_descriptor *desc;
	int len;
	unsigned char *data, *str;

	str = (unsigned char *)string;
	len = strlen(string);
	desc = udc_alloc_desc(USB_STRING_DESCRIPTOR, idx, 2 + len * 2);
	data = &desc->data[2];
	while(len --){
		*data++ = *str++;
		*data++ = 0x00;
	}

	udc_descriptor_register(desc);
}

static void udc_descriptor_string(void)
{

	/* 1. manufacturer string */
	__udc_descriptor_string(1, manufacturer);

	/* 2. product string */
	__udc_descriptor_string(2, product);

	/* 3. serial number string */
	__udc_descriptor_string(3, serialno);

	/* 4. interface func string */
	__udc_descriptor_string(4, interface);
}
static void udc_descriptor_device(void)
{
	struct udc_descriptor *desc;

	desc = udc_alloc_desc(USB_DEVICE_DESCRIPTOR, 0, 18);
	desc->data[2] = 0x00;
	desc->data[3] = 0x02;		/* bcdUSB 2.00 */
	desc->data[4] = 0x00;		/* bDeviceClass*/
	desc->data[5] = 0x00;		/* bDeviceSubClass */
	desc->data[6] = 0x00;		/* bDeviceProtocol */
	desc->data[7] = 0x40;		/* MaxPktSize ep0 */

#if 0
	/* vendor id */
	desc->data[8] = 0xd1;		/* Google */
	desc->data[9] = 0x18;

	/* product id */
	desc->data[10] = 0xe0;		/* Google Nexus (generic) */
	desc->data[11] = 0x4e;
#else
	/* vendor id */
	desc->data[8] = vendor_id;
	desc->data[9] = vendor_id >> 8;

	/* product id */
	desc->data[10] = product_id;
	desc->data[11] = product_id >> 8;
#endif

	/* device release num */
	desc->data[12] = 0x00;
	desc->data[13] = 0x00;

	desc->data[14] = 1;	/* manufacturer string */
	desc->data[15] = 2;	/* product string */
	desc->data[16] = 3;	/* serial number string */
	desc->data[17] = 1;

	udc_descriptor_register(desc);
}

static void udc_descriptor_configuration(void)
{
	struct udc_descriptor *desc;
	unsigned char *data;
	int size;

	size = 9 + 9 + 2 * 7;	/* CONFIGURATION + INTERFACE + 2 * ENDPINT */
	desc = udc_alloc_desc(USB_CONFIGURATION_DESCRIPTOR, 0, size);
	data = &desc->data[0];

	data[0] = 0x9;
	data[2] = size;
	data[3] = size >> 8;
	data[4] = 0x01;		/* number of interface  */
	data[5] = 0x01;		/* configuration value */
	data[6] = 0x00;		/* configuration string */
	data[7] = 0xc0;		/* attributes */
	data[8] = 100;		/* max power */

	data += 9;
	data[0] = 0x09;
	data[1] = 0x04;		/* TYPE_INTERFACE */
	data[2] = 0x00;		/* ifc number */
	data[3] = 0x00;		/* alt number */
	data[4] = 0x02;		/* ifc endpoint number */
	data[5] = 0xff;		/* ifc Class */

	/*
	 * data[6]	ifc SubClass
	 * data[7]	ifc Protocol
	 * data[8]	INTERFACE string
	 */

	if (vendor_id == 0x18d1) {	/* fastboot */
		data[6] = 0x42;		/* ifc SubClass */
		data[7] = 0x03;		/* ifc Protocol */
		data[8] = 0x04;		/* INTERFACE string */
	} else {			/* AmbDL*/
		data[6] = 0xff;		/* ifc SubClass */
		data[7] = 0x00;		/* ifc Protocol */
		data[8] = 0x00;		/* INTERFACE string */
	}
	data += 9;
	data[0] = 0x07;
	data[1] = 0x05;		/* TYPE_ENDPOINT */
	data[2] = 0x01 | 0x00;	/* endpoint OUT */
	data[3] = 0x02;		/* BULK */
	data[4] = 512 & 0xff;
	data[5] = (512 >> 8) & 0xff;
	data[6] = 0x00;

	data += 7;
	data[0] = 0x07;
	data[1] = 0x05;		/* TYPE_ENDPOINT */
	data[2] = 0x01 | 0x80;	/* endpoint IN */
	data[3] = 0x02;		/* BULK */
	data[4] = 512 & 0xff;
	data[5] = (512 >> 8) & 0xff;
	data[6] = 0x01;

	udc_descriptor_register(desc);
}
static void udc_desc_configuration(void)
{
	udc_descriptor_language();
	udc_descriptor_device();
	udc_descriptor_configuration();
	udc_descriptor_string();
}


void udc_start(void)
{
	struct ep_data_list *rxdata;

	for (;;) {

		if (readl(USBD_INT) || readl(USB_EP_INT))
			udc_irq_handle(NULL);

		rxdata = rx_list;

		if (!rxdata) {
			__asm__ volatile("nop");
			__asm__ volatile("isb");
			__asm__ volatile("dsb sy");
			continue;
		}

		while (rxdata) {
			pr_debug("rxdata: %d %08x %08x\n",
					rxdata->size,
					*(unsigned int *)rxdata->data,
					rxdata->next);

			if (rfsm_state == RFSM_STATE_CMD) {
				if (udc_cmd_handle)
					udc_cmd_handle(rxdata->data, rxdata->size);
			} else if (rfsm_state == RFSM_STATE_DOWNLOAD) {
				if (udc_data_handle)
					udc_data_handle(rxdata->data, rxdata->size);
			}

			rxdata->used = 0;
			rx_list = rxdata->next;
			rxdata = rx_list;
		}
		start_rx(EP_BULK_OUT);
	}
}

void udc_exit(void)
{
	writel(USBD_CTRL, 0x400);
	writel(USBD_CFG, 0x4);
}

void udc_state_download(void)
{
	rfsm_state = RFSM_STATE_DOWNLOAD;
}

void udc_state_cmd(void)
{
	rfsm_state = RFSM_STATE_CMD;
}

void udc_bulk_transfer(void *data, unsigned len)
{
	start_tx(EP_BULK_IN, data, len);
}

void udc_register_helper(unsigned short vid,
		unsigned short pid,
		void (*cmd)(void *, unsigned int),
		void (*data)(void *, unsigned int))
{
	vendor_id = vid;
	product_id = pid;

	udc_cmd_handle = cmd;
	udc_data_handle = data;
}

void usb_download(void)
{
	int ep, epmask = 0;

	uart_putstr(logo);

#ifdef __BUILD_DATE__
	pr_log(__BUILD_DATE__);
	pr_log("\n");
#endif

	setbitsl(USBD_CTRL, 1 << 10);
	setbitsl(USBD_CFG, 1 << 2);
	mdelay(10);

	plat_enable_usb();
	mdelay(10);			/* 'delay' is more stable ? */

	plat_usb_reset();
	mdelay(10);

	desc_pool = kmalloc(4096);
	setup = kmalloc(sizeof(struct setup_desc));
	ep_data_pool = kmalloc(sizeof(struct ep_data_list) * 32);

	memset(ep_data_pool, 0, sizeof(struct ep_data_list) * 32);

	for (ep = 0; ep < (sizeof(ept) / sizeof(struct udc_ep)); ep++)
		udc_endpoint_config(ep);

	/* 2, 3, 5, 6, 19 */
	writel(USBD_CFG, 0x8006c);

	/* 2, 3, 9 */
	writel(USBD_CTRL, 0x20c);

	writel(USBD_INT_MASK, 0xffffffff);
	writel(USBD_INT, 0xffffffff);
	writel(USB_EP_INT, 0xffffffff);
	writel(USB_EP_INT_MASK, 0xffffffff);

	/* 0, 1, 3, 6 */
	writel(USBD_INT_MASK, ~(0x43));

	for (ep = 0; ep < (sizeof(ept) / sizeof(struct udc_ep)); ep++)
		epmask |= (1 << ept[ep].num);
	writel(USB_EP_INT_MASK, ~epmask);

	udc_desc_configuration();

	setbitsl(USBD_CTRL, 1 << 0);
	udc_start();
}
