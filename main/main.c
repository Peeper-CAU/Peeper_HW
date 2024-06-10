#include "btstack_audio.h"
#include "btstack_port_esp32.h"
#include "btstack_run_loop.h"
#include "nvs_flash.h"

#include "ble_util.h"
#include "hfp_util.h"
#include "pcm_util.h"

extern int btstack_main(int argc, const char *argv[]);

int app_main(void)
{
	nvs_flash_erase();
	nvs_flash_init();

    btstack_init();

    btstack_audio_source_set_instance(btstack_audio_esp32_source_get_instance());
    btstack_audio_sink_set_instance(btstack_audio_esp32_sink_get_instance());

	init_ble();
	init_hfp();

    btstack_run_loop_execute();

    return 0;
}
