/* ============================================================================
 * Lab 7 — NUCLEO-F401RE host
 *   USART1 (PA9 TX / PA10 RX)  <-> board A, the node
 *   USART6 (PC6 TX / PC7 RX)   <-> board B, the gateway
 *   USART2 (ST-LINK VCP)        console, 115200 8N1
 *   TIM2 free-running, 1 µs per count (PSC 83 at 84 MHz, ARR 0xFFFFFFFF)
 *
 * Two modules that share NO variables:
 *   lab7_node.c    — makes readings, keeps the ring buffer, resends on ack
 *   lab7_gateway.c — receives readings from B, measures latency, sends the ack
 * The only shared thing is the send-time table in lab7_io.c, used to
 * compute dt for printing. It is never used for the acknowledgement.
 * ==========================================================================*/
#ifndef LAB7_H
#define LAB7_H
#include "main.h"
#include <stdint.h>

/* ---- frame: 0xAA type len payload[len] chk  (chk = XOR of type,len,payload) */
#define MSG_READING         0x01  /* STM32->A: seq,temp,hum,soil,batt (9 B)   */
#define MSG_RESEND          0x02  /* STM32->A: same, an older reading         */
#define MSG_INTERVAL_REPORT 0x03  /* STM32->A: u16 seconds now in use         */
#define MSG_A_ACK           0x11  /* A->STM32: u16 acknowledged up to         */
#define MSG_A_SET_INTERVAL  0x12  /* A->STM32: u16 seconds written over air   */
#define MSG_A_LINK          0x13  /* A->STM32: u8 1 up / 0 down               */
#define MSG_SEND_ACK        0x21  /* STM32->B: u16, B writes it to the node   */
#define MSG_WRITE_INTERVAL  0x22  /* STM32->B: u16 seconds                    */
#define MSG_CONN_PARAMS     0x23  /* STM32->B: u16 int(1.25ms) lat to(10ms)   */
#define MSG_B_RECORD        0x31  /* B->STM32: record: 9 B reading + flags    */
#define MSG_B_INTERVAL      0x32  /* B->STM32: u16 interval reported back     */
#define MSG_B_LINK          0x33  /* B->STM32: u8 1 streaming / 0 dropped     */
#define MSG_B_PARAMS        0x34  /* B->STM32: granted int, lat, timeout      */

typedef struct { int16_t temp; uint16_t hum; uint16_t soil; uint8_t batt; } reading_t;

extern UART_HandleTypeDef huart1, huart2, huart6;
extern TIM_HandleTypeDef htim2;

/* called from main.c */
void lab7_init(void);
void lab7_loop(void);

/* lab7_io.c */
uint32_t now_us(void);
void     frame_send(UART_HandleTypeDef *h, uint8_t type, const uint8_t *p, uint8_t len, uint32_t *t_start);
void     con_printf(const char *fmt, ...);
void     print_time(uint32_t t_us);        /* prints "[  12.345 s] " */
void     stamp_sent(uint16_t seq, uint32_t t_us);
int      stamp_lookup(uint16_t seq, uint32_t *t_us);

/* lab7_node.c */
void node_init(void);
void node_poll(void);
void node_on_frame(uint8_t type, const uint8_t *p, uint8_t len, uint32_t t_rx);
void node_set_catchup(int on);

/* lab7_gateway.c */
void gw_init(void);
void gw_on_frame(uint8_t type, const uint8_t *p, uint8_t len, uint32_t t_rx);
void gw_cmd_params(uint32_t interval_ms, uint32_t latency, uint32_t timeout_ms);
void gw_cmd_interval(uint16_t seconds);
void gw_mark(void);
void gw_stats(void);
void gw_reset(void);

#endif
