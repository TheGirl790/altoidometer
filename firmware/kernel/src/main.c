#include "esp_partition.h"

void app_main(void)
{
    esp_partition_iterator_t it =
        esp_partition_find(
            ESP_PARTITION_TYPE_ANY,
            ESP_PARTITION_SUBTYPE_ANY,
            NULL
        );

    while (it != NULL) {
        const esp_partition_t *p = esp_partition_get(it);

        printf(
            "label: %-16s "
            "type: 0x%02x "
            "subtype: 0x%02x "
            "offset: 0x%08lx "
            "size: 0x%08lx "
            "encrypted: %s\n",

            p->label,
            p->type,
            p->subtype,
            (unsigned long)p->address,
            (unsigned long)p->size,
            p->encrypted ? "yes" : "no"
        );

        it = esp_partition_next(it);
    }

    esp_partition_iterator_release(it);
}