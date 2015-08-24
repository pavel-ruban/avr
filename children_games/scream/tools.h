extern void uart_str_send_P(PGM_P);
extern void uart_str_send(void *);

#define TX_NEWLINE	uart_str_send_P(PSTR("\n"))

