#include "hfp_util.h"

static void	hfp_packet_handler(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size)
{
    // HFP 관련 패킷 처리 코드 추가
}

void	init_hfp(void)
{
    l2cap_init();
    rfcomm_init();
    sdp_init();
    hci_register_sco_packet_handler(hfp_packet_handler);
    hfp_hf_init(0);
    hfp_hf_register_packet_handler(hfp_packet_handler);
    sdp_register_service(NULL);
}
