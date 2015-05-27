/****************************************************************************/
/**
 * \file		config.h
 * \brief		Project specific defines
 *
 * \author		marc dot pignat at hevs dot ch
 * \copyright	Copyright HES-SO Valais/Wallis 2013. All rights reserved.
 * \license		CC0 - public domain, this is a configuration file
 *
 * This file MUST be provided by every project, in the "config/" directory.
 *
 * \warning This file MUST not be included, it will be included by "heivs/config.h"
 */
/****************************************************************************/

/****************************************************************************/
#ifndef _HEIVS_CHECK_CONFIG_H
#error This file MUST not be included, you SHOULD include "heivs/config.h" instead.
#endif
/****************************************************************************/

/****************************************************************************
  ____            _                         __ _
 | __ )  __ _ ___(_) ___    ___ ___  _ __  / _(_) __ _
 |  _ \ / _` / __| |/ __|  / __/ _ \| '_ \| |_| |/ _` |
 | |_) | (_| \__ \ | (__  | (_| (_) | | | |  _| | (_| |
 |____/ \__,_|___/_|\___|  \___\___/|_| |_|_| |_|\__, |
                                                 |___/
*****************************************************************************/

/****************************************************************************/
/**
 * \brief Use FreeRTOS
 *
 * Possible values :
 *  1 : Use FreeRTOS
 *  0 : run standalone
 *
 * Example projects not using FreeRTOS:
 * \see $LIBHEIVS_STM32/demos/leds
 * \see $LIBHEIVS_STM32/demos/stdio
 * \see $LIBHEIVS_STM32/demos/test_cmsis_dsp
 * \see $LIBHEIVS_STM32/demos/leds
 *
 * Example projects using FreeRTOS:
 * \see $LIBHEIVS_STM32/demos/hello_freertos
 */
#define USE_FREERTOS 1
/****************************************************************************/

/****************************************************************************/
/**
 * \brief Use uGfx
 *
 * Possible values :
 *  0 : don't use ugfx
 *  1 : use ugfx
 *
 * \see $LIBHEIVS_STM32/demos/hello_freertos
 */
#define USE_UGFX 1
/****************************************************************************/

/****************************************************************************/
/**
 * \brief Use lwip (light-weight TCP/IP stack)
 *
 * Possible values :
 *  0 : don't use lwip
 *  1 : use lwip
 *
 * \see $LIBHEIVS_STM32/demos/tcp_ip_freertos
 */
#define USE_LWIP 1
/****************************************************************************/

/****************************************************************************
  _   _ ____  ____                     __ _
 | | | / ___|| __ )    ___ ___  _ __  / _(_) __ _
 | | | \___ \|  _ \   / __/ _ \| '_ \| |_| |/ _` |
 | |_| |___) | |_) | | (_| (_) | | | |  _| | (_| |
  \___/|____/|____/   \___\___/|_| |_|_| |_|\__, |
                                            |___/
*****************************************************************************/

/****************************************************************************/
/**
 * \brief Enable code for USB device support (0 or 1)
 *
 * Your project will need config/usb.h, config/usbd_conf.h and usbd_usr.c.
 * Samples files can be found in heivs/samples/
 *
 * A complete USB CDC device project is available at :
 * $LIBHEIVS_STM32/demos/leds_usb_serial_cdc
 */

#define USE_STM32_USB_USE_DEVICE_MODE 0
/****************************************************************************/

/****************************************************************************/
/**
 * \brief Enable code for USB host support (0 or 1)
 *
 * Your project will need config/usb.h, config/usbh_conf.h and usbh_usr.c.
 * Samples files can be found in heivs/samples/
 *
 * FIXME : there is currently no sample project for that
 */

#define USE_STM32_USB_HOST_MODE 0
/****************************************************************************/

/****************************************************************************/
/**
 * \brief USB OTG Completely untested support (0 or 1)
 *
 * \warning, please re-read the brief
 */

#define USE_STM32_USB_USE_OTG_MODE 0
/****************************************************************************
  _____                      _
 | ____|_  ___ __   ___ _ __| |_
 |  _| \ \/ / '_ \ / _ \ '__| __|
 | |___ >  <| |_) |  __/ |  | |_
 |_____/_/\_\ .__/ \___|_|   \__|
            |_|
*****************************************************************************/

/****************************************************************************/
/**
 * \brief use assert in the library functions provided by ST
 *
 * FIXME : does not compile at this time
 */
#define USE_STM32_LIB_ASSERT 0
/****************************************************************************/

/****************************************************************************/
/**
 * \brief Boot time is more critical than debug?  (0 or 1)
 *
 * When programming over JTAG, the whole board is reset multiple times and the
 * CPU can't be stopped at the reset vector, causing the program to be run.
 * The run is long enough to go past main and do some printf (the boot time has
 * been measured below 3 microseconds). Certain external devices, especially
 * those without reset pin, can be crashed...
 *
 * To prevent this behavior, there is a workaround in time_init(), which wait
 * some time (a few hundred of milliseconds).
 */

#define USE_BOOT_TIME_CRITICAL 0
/****************************************************************************/
