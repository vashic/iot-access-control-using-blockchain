
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "contiki.h"
#include "contiki-net.h"
#include "erbium.h"
#include "dev/button-sensor.h"

#if WITH_COAP == 3
#include "er-coap-03.h"
#elif WITH_COAP == 7
#include "er-coap-07.h"
#elif WITH_COAP == 12
#include "er-coap-12.h"
#elif WITH_COAP == 13
#include "er-coap-13.h"
#else
#warning "Erbium example without CoAP-specifc functionality"
#endif /* CoAP-specific example */

#define DEBUG 0
#if DEBUG
#define PRINTF(...) printf(__VA_ARGS__)
#define PRINT6ADDR(addr) PRINTF("[%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x]", ((uint8_t *)addr)[0], ((uint8_t *)addr)[1], ((uint8_t *)addr)[2], ((uint8_t *)addr)[3], ((uint8_t *)addr)[4], ((uint8_t *)addr)[5], ((uint8_t *)addr)[6], ((uint8_t *)addr)[7], ((uint8_t *)addr)[8], ((uint8_t *)addr)[9], ((uint8_t *)addr)[10], ((uint8_t *)addr)[11], ((uint8_t *)addr)[12], ((uint8_t *)addr)[13], ((uint8_t *)addr)[14], ((uint8_t *)addr)[15])
#define PRINTLLADDR(lladdr) PRINTF("[%02x:%02x:%02x:%02x:%02x:%02x]",(lladdr)->addr[0], (lladdr)->addr[1], (lladdr)->addr[2], (lladdr)->addr[3],(lladdr)->addr[4], (lladdr)->addr[5])
#else
#define PRINTF(...)
#define PRINT6ADDR(addr)
#define PRINTLLADDR(addr)
#endif

#include "dev/radio-sensor.h"
static uint16_t camera_value = 1;

EVENT_RESOURCE(event, METHOD_GET, "sensors/camera", "title=\"Camera\";obs");

void event_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
  REST.set_header_content_type(response, REST.type.TEXT_PLAIN);
  /* Usually, a CoAP server would response with the current resource representation. */
  const char *msg = "CAMERA ON(1) ";
  REST.set_response_payload(response, (uint8_t *)msg, strlen(msg));

  /* A post_handler that handles subscriptions/observing will be called for periodic resources by the framework. */
}

/* Additionally, a handler function named [resource name]_event_handler must be implemented for each PERIODIC_RESOURCE defined.
 * It will be called by the REST manager process with the defined period. */
void event_event_handler(resource_t *r)
{
  static uint16_t event_counter = 0;
  static char content[50];
  
  ++event_counter;

  PRINTF("TICK %u for /%s\n", event_counter, r->url);
  if(event_counter%2==0)
  {
    camera_value=1;
  }
  else{ camera_value=0;}
  /* Build notification. */
  coap_packet_t notification[1]; /* This way the packet can be treated as pointer as usual. */
  coap_init_message(notification, COAP_TYPE_CON, REST.status.OK, 0 );
  coap_set_payload(notification, content, snprintf(content, sizeof(content), "CAMERA STATE %u",camera_value));

  /* Notify the registered observers with the given message type, observe option, and payload. */
  REST.notify_subscribers(r, event_counter, notification);
}



PERIODIC_RESOURCE(radio, METHOD_GET, "sensors/cameravalue", "title=\"Radio\";obs", 60*CLOCK_SECOND);
void radio_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
  //uint16_t light_photosynthetic = light_sensor.value(LIGHT_SENSOR_PHOTOSYNTHETIC);
  //uint16_t light_solar = light_sensor.value(LIGHT_SENSOR_TOTAL_SOLAR);
  REST.set_header_content_type(response, REST.type.TEXT_PLAIN);
  const char *msg = "camera value, periodic";
  REST.set_response_payload(response, msg, strlen(msg));  
}
void radio_periodic_handler(resource_t *r)
{
  static char content[50];
  //uint16_t light_photosynthetic = light_sensor.value(LIGHT_SENSOR_PHOTOSYNTHETIC);
  uint16_t radio_v = radio_sensor.value(RADIO_SENSOR_LAST_VALUE);
  if(camera_value == 1){
  /* Build notification. */
  coap_packet_t notification[1]; /* This way the packet can be treated as pointer as usual. */
  coap_init_message(notification, COAP_TYPE_NON, REST.status.OK, 0 );
  coap_set_payload(notification, content, snprintf(content, sizeof(content), "%u",radio_v));
  /* Notify the registered observers with the given message type, observe option, and payload. */
  REST.notify_subscribers(r, radio_v, notification);}
}


PROCESS(rest_server_example, "Camera Sensor");
AUTOSTART_PROCESSES(&rest_server_example);

PROCESS_THREAD(rest_server_example, ev, data)
{
  PROCESS_BEGIN();

  PRINTF("Starting Erbium Example Server\n");

  PRINTF("uIP buffer: %u\n", UIP_BUFSIZE);
  PRINTF("LL header: %u\n", UIP_LLH_LEN);
  PRINTF("IP+UDP header: %u\n", UIP_IPUDPH_LEN);
  PRINTF("REST max chunk: %u\n", REST_MAX_CHUNK_SIZE);
  SENSORS_ACTIVATE(button_sensor);
  /* Initialize the REST engine. */
  rest_init_engine();
  SENSORS_ACTIVATE(radio_sensor);
  rest_activate_periodic_resource(&periodic_resource_radio);
  rest_activate_event_resource(&resource_event);



  /* Define application-specific events here. */
  while(1) {
    PROCESS_WAIT_EVENT();
 if (ev == sensors_event && data == &button_sensor) {
      PRINTF("BUTTON\n");
      /* Call the event_handler for this application-specific event. */
      event_event_handler(&resource_event);
    }
  } /* while (1) */

  PROCESS_END();
}
