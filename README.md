# lwip_freeRTOS
## Synopsis
Implementation of a TCP/IP stack for FreeRTOS on the heivs' ARMEBS4 board.

The stack used is lwip: http://savannah.nongnu.org/projects/lwip/.

A port of mbedTLS is being used for encryption: https://tls.mbed.org.

The project run on FreeRTOS: www.freertos.org.

## Informations
This project only works with the heivs libraries and on the ARMEBS4 board.
(http://wiki.hevs.ch/uit/index.php5/Hardware/ARMEBS/4)

## TODO

- Move src/lwip and src/mbedtls to libheivs_stm32/src
- Move include/lwip and include/mbedtls to libheivs_stm32/include
- Move the project to the demo branch of the svn
