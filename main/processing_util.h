#ifndef PROCESSING_UTIL_H
# define PROCESSING_UTIL_H

# define SCO_CVSD_PA_PREBUFFER_MS 50
# define SCO_MSBC_PA_PREBUFFER_MS 50
# define SCO_REPORT_PERIOD 100

# define NUM_CHANNELS 1
# define CVSD_SAMPLE_RATE 8000
# define MSBC_SAMPLE_RATE 16000
# define BYTES_PER_FRAME 2

# define CVSD_PA_PREBUFFER_BYTES (SCO_CVSD_PA_PREBUFFER_MS * CVSD_SAMPLE_RATE / 1000 * BYTES_PER_FRAME)
# define MSBC_PA_PREBUFFER_BYTES (SCO_MSBC_PA_PREBUFFER_MS * MSBC_SAMPLE_RATE / 1000 * BYTES_PER_FRAME)

# define MAX_NUM_MSBC_SAMPLES (16 * 8)

# include <stdio.h>
# include "btstack_audio.h"
# include "btstack_debug.h"
# include "btstack_ring_buffer.h"
# include "classic/btstack_cvsd_plc.h"
# include "classic/btstack_sbc.h"
# include "classic/hfp.h"
# include "classic/hfp_msbc.h"
# include "hci.h"

# include "pcm_util.h"

void	processing_close(void);
void	processing_init(void);
void	processing_receive(uint8_t *packet, uint16_t size);
void	processing_send(hci_con_handle_t con_handle);
void	processing_set_codec(uint8_t codec);

#endif
