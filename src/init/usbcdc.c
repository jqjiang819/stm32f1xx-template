#include <stdatomic.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "init.h"

#include "usb.h"
#include "usb_cdc.h"

#define CDC_EP0_SIZE      0x08
#define CDC_RXD_EP        0x01
#define CDC_TXD_EP        0x81
#define CDC_DATA_SZ       0x40
#define CDC_NTF_EP        0x82
#define CDC_NTF_SZ        0x08
#define CDC_RX_BUFFER_LEN 0x200
#define CDC_TX_BUFFER_LEN 0x200

#define USBD_LANGID_STRING       USB_LANGID_ENG_US
#define USBD_VID                 0x0483
#define USBD_PID                 0x5740
#define USBD_MANUFACTURER_STRING "STMicroelectronics"
#define USBD_PRODUCT_STRING      "STM32 Virtual ComPort"

enum {
    STRDESC_LANG_IDX,
    STRDESC_MANUFACTURER_IDX,
    STRDESC_PRODUCT_IDX,

    STRDESC_IDX_MAX,
};

struct usb_cdc_descriptor {
    struct usb_config_descriptor config;
    struct usb_iad_descriptor comm_iad;
    struct usb_interface_descriptor comm;
    struct usb_cdc_header_desc cdc_hdr;
    struct usb_cdc_call_mgmt_desc cdc_mgmt;
    struct usb_cdc_acm_desc cdc_acm;
    struct usb_cdc_union_desc cdc_union;
    struct usb_endpoint_descriptor comm_ep;
    struct usb_interface_descriptor data;
    struct usb_endpoint_descriptor data_eprx;
    struct usb_endpoint_descriptor data_eptx;
} __attribute__((packed));

usbd_device usbd;
uint32_t usbd_reqbuf[0x20];

uint8_t rxbuf[CDC_DATA_SZ];
volatile uint16_t rxbuf_size = 0;
atomic_flag rxbuf_lock = ATOMIC_FLAG_INIT;

uint8_t txbuf[CDC_DATA_SZ];
volatile uint16_t txbuf_size = 0;
atomic_flag txbuf_lock = ATOMIC_FLAG_INIT;

cdc_msg_callback_t on_receive_msg_clbk;

static const struct usb_device_descriptor usbd_desc = {
    .bLength = sizeof(struct usb_device_descriptor),
    .bDescriptorType = USB_DTYPE_DEVICE,
    .bcdUSB = VERSION_BCD(2, 0, 0),
    .bDeviceClass = USB_CLASS_IAD,
    .bDeviceSubClass = USB_SUBCLASS_IAD,
    .bDeviceProtocol = USB_PROTO_IAD,
    .bMaxPacketSize0 = CDC_EP0_SIZE,
    .idVendor = USBD_VID,
    .idProduct = USBD_PID,
    .bcdDevice = VERSION_BCD(1, 0, 0),
    .iManufacturer = STRDESC_MANUFACTURER_IDX,
    .iProduct = STRDESC_PRODUCT_IDX,
    .iSerialNumber = INTSERIALNO_DESCRIPTOR,
    .bNumConfigurations = 1,
};

static const struct usb_cdc_descriptor usbcdc_desc = {
    .config =
        {
            .bLength = sizeof(struct usb_config_descriptor),
            .bDescriptorType = USB_DTYPE_CONFIGURATION,
            .wTotalLength = sizeof(struct usb_cdc_descriptor),
            .bNumInterfaces = 2,
            .bConfigurationValue = 1,
            .iConfiguration = NO_DESCRIPTOR,
            .bmAttributes = USB_CFG_ATTR_RESERVED | USB_CFG_ATTR_SELFPOWERED,
            .bMaxPower = USB_CFG_POWER_MA(100),
        },
    .comm_iad =
        {
            .bLength = sizeof(struct usb_iad_descriptor),
            .bDescriptorType = USB_DTYPE_INTERFASEASSOC,
            .bFirstInterface = 0,
            .bInterfaceCount = 2,
            .bFunctionClass = USB_CLASS_CDC,
            .bFunctionSubClass = USB_CDC_SUBCLASS_ACM,
            .bFunctionProtocol = USB_PROTO_NONE,
            .iFunction = NO_DESCRIPTOR,
        },
    .comm =
        {
            .bLength = sizeof(struct usb_interface_descriptor),
            .bDescriptorType = USB_DTYPE_INTERFACE,
            .bInterfaceNumber = 0,
            .bAlternateSetting = 0,
            .bNumEndpoints = 1,
            .bInterfaceClass = USB_CLASS_CDC,
            .bInterfaceSubClass = USB_CDC_SUBCLASS_ACM,
            .bInterfaceProtocol = USB_PROTO_NONE,
            .iInterface = NO_DESCRIPTOR,
        },
    .cdc_hdr =
        {
            .bFunctionLength = sizeof(struct usb_cdc_header_desc),
            .bDescriptorType = USB_DTYPE_CS_INTERFACE,
            .bDescriptorSubType = USB_DTYPE_CDC_HEADER,
            .bcdCDC = VERSION_BCD(1, 1, 0),
        },
    .cdc_mgmt =
        {
            .bFunctionLength = sizeof(struct usb_cdc_call_mgmt_desc),
            .bDescriptorType = USB_DTYPE_CS_INTERFACE,
            .bDescriptorSubType = USB_DTYPE_CDC_CALL_MANAGEMENT,
            .bmCapabilities = 0,
            .bDataInterface = 1,

        },
    .cdc_acm =
        {
            .bFunctionLength = sizeof(struct usb_cdc_acm_desc),
            .bDescriptorType = USB_DTYPE_CS_INTERFACE,
            .bDescriptorSubType = USB_DTYPE_CDC_ACM,
            .bmCapabilities = 0,
        },
    .cdc_union =
        {
            .bFunctionLength = sizeof(struct usb_cdc_union_desc),
            .bDescriptorType = USB_DTYPE_CS_INTERFACE,
            .bDescriptorSubType = USB_DTYPE_CDC_UNION,
            .bMasterInterface0 = 0,
            .bSlaveInterface0 = 1,
        },
    .comm_ep =
        {
            .bLength = sizeof(struct usb_endpoint_descriptor),
            .bDescriptorType = USB_DTYPE_ENDPOINT,
            .bEndpointAddress = CDC_NTF_EP,
            .bmAttributes = USB_EPTYPE_INTERRUPT,
            .wMaxPacketSize = CDC_NTF_SZ,
            .bInterval = 0xFF,
        },
    .data =
        {
            .bLength = sizeof(struct usb_interface_descriptor),
            .bDescriptorType = USB_DTYPE_INTERFACE,
            .bInterfaceNumber = 1,
            .bAlternateSetting = 0,
            .bNumEndpoints = 2,
            .bInterfaceClass = USB_CLASS_CDC_DATA,
            .bInterfaceSubClass = USB_SUBCLASS_NONE,
            .bInterfaceProtocol = USB_PROTO_NONE,
            .iInterface = NO_DESCRIPTOR,
        },
    .data_eprx =
        {
            .bLength = sizeof(struct usb_endpoint_descriptor),
            .bDescriptorType = USB_DTYPE_ENDPOINT,
            .bEndpointAddress = CDC_RXD_EP,
            .bmAttributes = USB_EPTYPE_BULK,
            .wMaxPacketSize = CDC_DATA_SZ,
            .bInterval = 0x01,
        },
    .data_eptx =
        {
            .bLength = sizeof(struct usb_endpoint_descriptor),
            .bDescriptorType = USB_DTYPE_ENDPOINT,
            .bEndpointAddress = CDC_TXD_EP,
            .bmAttributes = USB_EPTYPE_BULK,
            .wMaxPacketSize = CDC_DATA_SZ,
            .bInterval = 0x01,
        },
};

static const struct usb_string_descriptor strdesc_lang = USB_ARRAY_DESC(USB_LANGID_ENG_US);
static const struct usb_string_descriptor strdesc_manuf = USB_STRING_DESC(USBD_MANUFACTURER_STRING);
static const struct usb_string_descriptor strdesc_prod = USB_STRING_DESC(USBD_PRODUCT_STRING);
static const struct usb_string_descriptor* const strdesc_ptrs[] = {
    &strdesc_lang,
    &strdesc_manuf,
    &strdesc_prod,
};

void USBCDC_OnReceiveEventClbk(usbd_device* dev, uint8_t ep) {
    while (atomic_flag_test_and_set(&rxbuf_lock)) {}
    int32_t rxlen = usbd_ep_read(dev, ep, rxbuf, CDC_DATA_SZ);
    if (rxlen > 0 && on_receive_msg_clbk != NULL) {
        on_receive_msg_clbk(rxbuf, rxlen);
    }
    atomic_flag_clear(&rxbuf_lock);
}

void USBCDC_OnTransmitEventClbk(usbd_device* dev, uint8_t ep) {
    while (atomic_flag_test_and_set(&txbuf_lock)) {}
    if (txbuf_size > 0) {
        int32_t txlen = usbd_ep_write(dev, ep, txbuf, txbuf_size < CDC_DATA_SZ ? txbuf_size : CDC_DATA_SZ);
        if (txlen > 0) {
            txbuf_size -= txlen;
            memmove(txbuf, txbuf + txlen, txbuf_size);
        }
    }
    atomic_flag_clear(&txbuf_lock);
}

static void USBCDC_OnEpEventClbk(usbd_device* dev, uint8_t event, uint8_t ep) {
    if (event == usbd_evt_eptx) {
        USBCDC_OnTransmitEventClbk(dev, ep);
    } else if (event == usbd_evt_eprx) {
        USBCDC_OnReceiveEventClbk(dev, ep);
    }
}

static usbd_respond USBCDC_ConfigClbk(usbd_device* dev, uint8_t cfg) {
    switch (cfg) {
    case 0:
        /* deconfiguring device */
        usbd_ep_deconfig(dev, CDC_NTF_EP);
        usbd_ep_deconfig(dev, CDC_TXD_EP);
        usbd_ep_deconfig(dev, CDC_RXD_EP);
        usbd_reg_endpoint(dev, CDC_RXD_EP, 0);
        usbd_reg_endpoint(dev, CDC_TXD_EP, 0);
        return usbd_ack;
    case 1:
        /* configuring device */
        usbd_ep_config(dev, CDC_RXD_EP, USB_EPTYPE_BULK /*| USB_EPTYPE_DBLBUF*/, CDC_DATA_SZ);
        usbd_ep_config(dev, CDC_TXD_EP, USB_EPTYPE_BULK /*| USB_EPTYPE_DBLBUF*/, CDC_DATA_SZ);
        usbd_ep_config(dev, CDC_NTF_EP, USB_EPTYPE_INTERRUPT, CDC_NTF_SZ);
        usbd_reg_endpoint(dev, CDC_RXD_EP, USBCDC_OnEpEventClbk);
        usbd_reg_endpoint(dev, CDC_TXD_EP, USBCDC_OnEpEventClbk);
        usbd_ep_write(dev, CDC_TXD_EP, 0, 0);
        return usbd_ack;
    default:
        return usbd_fail;
    }
}

static usbd_respond USBCDC_ControlClbk(usbd_device* dev, usbd_ctlreq* req, usbd_rqc_callback* callback) {
    if (((USB_REQ_RECIPIENT | USB_REQ_TYPE) & req->bmRequestType) == (USB_REQ_INTERFACE | USB_REQ_CLASS) &&
        req->wIndex == 0) {
        switch (req->bRequest) {
        case USB_CDC_SET_CONTROL_LINE_STATE:
            return usbd_ack;
        case USB_CDC_SET_LINE_CODING:
            return usbd_ack;
        case USB_CDC_GET_LINE_CODING:
            return usbd_ack;
        default:
            return usbd_fail;
        }
    }
    return usbd_fail;
}

static usbd_respond USBCDC_DescriptionClbk(usbd_ctlreq* req, void** address, uint16_t* length) {
    const uint8_t dtype = req->wValue >> 8;
    const uint8_t dnumber = req->wValue & 0xFF;
    const void* desc;
    uint16_t len = 0;
    switch (dtype) {
    case USB_DTYPE_DEVICE:
        desc = &usbd_desc;
        break;
    case USB_DTYPE_CONFIGURATION:
        desc = &usbcdc_desc;
        len = sizeof(usbcdc_desc);
        break;
    case USB_DTYPE_STRING:
        if (dnumber >= STRDESC_IDX_MAX) {
            return usbd_fail;
        }
        desc = strdesc_ptrs[dnumber];
        break;
    default:
        return usbd_fail;
    }
    if (len == 0) {
        len = ((struct usb_header_descriptor*)desc)->bLength;
    }
    *address = (void*)desc;
    *length = len;
    return usbd_ack;
};

void USBCDC_Init() {
    usbd_init(&usbd, &usbd_hw, CDC_EP0_SIZE, usbd_reqbuf, sizeof(usbd_reqbuf));
    usbd_reg_config(&usbd, USBCDC_ConfigClbk);
    usbd_reg_control(&usbd, USBCDC_ControlClbk);
    usbd_reg_descr(&usbd, USBCDC_DescriptionClbk);

    usbd_enable(&usbd, true);
    usbd_connect(&usbd, true);

    HAL_NVIC_SetPriority(USB_LP_CAN1_RX0_IRQn, 9, 0);
    HAL_NVIC_EnableIRQ(USB_LP_CAN1_RX0_IRQn);

    HAL_NVIC_SetPriority(USBWakeUp_IRQn, 8, 0);
    HAL_NVIC_EnableIRQ(USBWakeUp_IRQn);
}

void USBCDC_SetReceiveCallback(cdc_msg_callback_t callback) {
    on_receive_msg_clbk = callback;
}

void USBCDC_Transmit(uint8_t* data, uint16_t len) {
    usbd_ep_write(&usbd, CDC_TXD_EP, data, len < CDC_DATA_SZ ? len : CDC_DATA_SZ);
    usbd_poll(&usbd);
}
