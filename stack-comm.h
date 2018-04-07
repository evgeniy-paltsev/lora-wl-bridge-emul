#ifndef STACK_COMM_H
#define STACK_COMM_H

#include <stdint.h>
#include <stdbool.h>
#include "list.h"

#define CONFIG_STK_PACK_CONST_LEN		19
#define CONFIG_STK_PAYLOAD_LEN			CONFIG_STK_PACK_CONST_LEN - 3

/* packet structure */
#define STK_P_HEADER_L1		0
#define STK_P_PAYLOAD_L1	1
#define STK_P_CRC_LO		17
#define STK_P_CRC_HI		18

enum l1_pack_type {
	TYPE_INTERNAL = 0,
	TYPE_SINGLE,
	TYPE_MULT,
	TYPE_MULT_LAST
};

union stk_pack_header_l1 {
	struct {
		uint8_t type: 2, pad: 6;
	} generic_pack;
	struct {
		uint8_t type: 2, payload_len: 4, id: 2;
	} single_pack;
	struct {
		uint8_t type: 2, pack_num: 4, id: 2;
	} modbus_pack;
	uint8_t byte;
};

union stk_pack_checksum {
	uint16_t sum_16;
	struct {
		/* LE expected */
		uint8_t hi;
		uint8_t lo;
	} sum_8;
};

enum master_state {
	M_IDLE = 0,
	M_SEND_SINGLE,
	M_WAIT_ACK_SINGLE,

	/* slave states */
//	S_IDLE,
	S_WAIT_FOR_PACK,	/* wait for any packet from master */
	S_RECEIVED,		/* received smth with walid CRC */
	S_PREPARE_RESP,		/* recieved valid pack, prepare response/ACK */
	S_SEND_ACK,

	S_IDLE,
	S_SEND,
	S_RECEIVING_CONT,

};

enum mac_state {
	MAC_IDLE = 0,
	MAC_SENDING,
	MAC_RECEIVING_CONT
};

enum llc_state {
	LLC_IDLE = 0,
/*	LLC_RECEIVED, */	/* used only in multi-pack link */
	LLC_RECEIVED_ALL,
/*	LLC_SEND, */		/* used only in multi-pack link */
	LLC_SEND_ALL
};

struct wls_pack {
	uint8_t data[CONFIG_STK_PACK_CONST_LEN];
	struct list_head mac_node;

//	bool owned;
};

struct stl_transmiter {
	uint8_t id;
	uint8_t ts_pack[CONFIG_STK_PACK_CONST_LEN];	// depricated
	/* local buffer */
	uint8_t ts_buff[CONFIG_STK_PACK_CONST_LEN];	// depricated
	enum master_state state;			// depricated

	enum mac_state mac_state;
	enum llc_state llc_state;

	uint8_t received_in_buff;			// depricated
	uint8_t recv_buff[CONFIG_STK_PACK_CONST_LEN];	// depricated
	uint8_t to_send_in_buff;			// depricated
	uint8_t send_buff[CONFIG_STK_PACK_CONST_LEN];	// depricated

	uint8_t retries_num;

	struct list_head send_list;
	struct wls_pack send_list_pool[10];

	struct list_head recv_list;
	struct wls_pack recv_list_pool[10];
};

uint16_t stk_checksumm(const uint8_t *data_p, uint16_t length);
bool stk_test_pack_checksumm(const uint8_t *data_p);
void stk_set_pack_checksumm(uint8_t *data_p);

void stk_m_send_single(struct stl_transmiter *tsm, const uint8_t *payload, uint16_t len);
void stk_m_wait_for_pack(struct stl_transmiter *tsm);

void stk_s_wait_for_pack(struct stl_transmiter *tsm);
void stk_s_process_pack_generic(struct stl_transmiter *tsm, uint8_t *pack);
void stk_s_process_pack_single(struct stl_transmiter *tsm, uint8_t *pack);

void stk_s_comm_loop(struct stl_transmiter *tsm);
void stk_m_comm_loop(struct stl_transmiter *tsm);

#endif /* STACK_COMM_H */