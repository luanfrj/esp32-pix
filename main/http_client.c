#include "http_client.h"

static const char *TAG = "Http client";

esp_err_t _http_event_handler(esp_http_client_event_t *evt)
{
    static char *output_buffer;  // Buffer to store response of http request from event handler
    static int output_len;       // Stores number of bytes read
    switch(evt->event_id) {
        case HTTP_EVENT_ERROR:
            ESP_LOGD(TAG, "HTTP_EVENT_ERROR");
            break;
        case HTTP_EVENT_ON_CONNECTED:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_CONNECTED");
            break;
        case HTTP_EVENT_HEADER_SENT:
            ESP_LOGD(TAG, "HTTP_EVENT_HEADER_SENT");
            break;
        case HTTP_EVENT_ON_HEADER:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
            break;
        case HTTP_EVENT_ON_DATA:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
            /*
             *  Check for chunked encoding is added as the URL for chunked encoding used in this example returns binary data.
             *  However, event handler can also be used in case chunked encoding is used.
             */
            if (!esp_http_client_is_chunked_response(evt->client)) {
                // If user_data buffer is configured, copy the response into the buffer
                if (evt->user_data) {
                    memcpy(evt->user_data + output_len, evt->data, evt->data_len);
                } else {
                    if (output_buffer == NULL) {
                        output_buffer = (char *) malloc(esp_http_client_get_content_length(evt->client));
                        output_len = 0;
                        if (output_buffer == NULL) {
                            ESP_LOGE(TAG, "Failed to allocate memory for output buffer");
                            return ESP_FAIL;
                        }
                    }
                    memcpy(output_buffer + output_len, evt->data, evt->data_len);
                }
                output_len += evt->data_len;
            }

            break;
        case HTTP_EVENT_ON_FINISH:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_FINISH");
            if (output_buffer != NULL) {
                free(output_buffer);
                output_buffer = NULL;
            }
            output_len = 0;
            break;
        case HTTP_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");
            int mbedtls_err = 0;
            esp_err_t err = esp_tls_get_and_clear_last_error(evt->data, &mbedtls_err, NULL);
            if (err != 0) {
                ESP_LOGI(TAG, "Last esp error code: 0x%x", err);
                ESP_LOGI(TAG, "Last mbedtls failure: 0x%x", mbedtls_err);
            }
            if (output_buffer != NULL) {
                free(output_buffer);
                output_buffer = NULL;
            }
            output_len = 0;
            break;
    }
    return ESP_OK;
}

void http_get_qrcode(char *buffer, uint32_t order_id)
{
    char query[30];
    sprintf(query, "id=%d", order_id);

    esp_http_client_config_t config = {
        .host = CONFIG_PIX_GATEWAY_HOST,
        .path = "/pix/qrcode/",
        .query = query,
        .event_handler = _http_event_handler,
        .timeout_ms = 9000
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);

    esp_http_client_set_method(client, HTTP_METHOD_GET);
    esp_err_t err = esp_http_client_open(client, 0);

    if (err == ESP_OK) {
        int content_len = esp_http_client_fetch_headers(client);
        ESP_LOGI(TAG, "content_length = %d", content_len);
        int data_len = esp_http_client_read_response(client, buffer, content_len);
        ESP_LOGI(TAG, "Read length = %d", data_len);
        buffer[content_len] = 0;
        ESP_LOGI(TAG, "HTTP GET Status = %d, content_length = %d",
                esp_http_client_get_status_code(client),
                esp_http_client_get_content_length(client));
    } else {
        ESP_LOGE(TAG, "HTTP GET request failed: %s", esp_err_to_name(err));
    }

    esp_http_client_cleanup(client);
}

void http_get_qrcode_test(char *buffer)
{
    esp_http_client_config_t config = {
        .host = CONFIG_PIX_GATEWAY_HOST,
        .path = "/pix/teste",
        .event_handler = _http_event_handler,
        .timeout_ms = 5000
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);

    esp_http_client_set_method(client, HTTP_METHOD_GET);
    esp_err_t err = esp_http_client_open(client, 0);

    if (err == ESP_OK) {
        int content_len = esp_http_client_fetch_headers(client);
        ESP_LOGI(TAG, "content_length = %d", content_len);
        int data_len = esp_http_client_read_response(client, buffer, content_len);
        ESP_LOGI(TAG, "Read length = %d", data_len);
        buffer[content_len] = 0;
        ESP_LOGI(TAG, "HTTP GET Status = %d, content_length = %d",
                esp_http_client_get_status_code(client),
                esp_http_client_get_content_length(client));
    } else {
        ESP_LOGE(TAG, "HTTP GET request failed: %s", esp_err_to_name(err));
    }

    esp_http_client_cleanup(client);
}

uint8_t http_get_order_status(uint32_t order_id)
{
    char path[30];
    sprintf(path, "/pix/orders/%d/status", order_id);

    esp_http_client_config_t config = {
        .host = CONFIG_PIX_GATEWAY_HOST,
        .path = path,
        .event_handler = _http_event_handler,
        .timeout_ms = 9000
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);

    esp_http_client_set_method(client, HTTP_METHOD_GET);
    esp_err_t err = esp_http_client_open(client, 0);

    char buffer[2];

    if (err == ESP_OK) {
        int content_len = esp_http_client_fetch_headers(client);
        ESP_LOGI(TAG, "content_length = %d", content_len);
        int data_len = esp_http_client_read_response(client, buffer, content_len);
        ESP_LOGI(TAG, "Read length = %d", data_len);
        buffer[content_len] = 0;
        ESP_LOGI(TAG, "HTTP GET Status = %d, content_length = %d",
                esp_http_client_get_status_code(client),
                esp_http_client_get_content_length(client));
    } else {
        ESP_LOGE(TAG, "HTTP GET request failed: %s", esp_err_to_name(err));
    }

    esp_http_client_cleanup(client);

    return (uint8_t) atoi(buffer);
}
