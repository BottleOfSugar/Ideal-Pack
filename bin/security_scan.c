#include <stdio.h>
#include <stdlib.h>
#include <curl/curl.h>
#include <string.h>
// Funkcja callback do przetwarzania nagłówków HTTP
size_t header_callback(char *buffer, size_t size, size_t nitems, void *userdata) {
    // Nagłówek ma format: "Key: Value"
    printf("%.*s", (int)(size * nitems), buffer);

    // Sprawdź czy dany nagłówek zawiera zabezpieczenia
    if (strstr(buffer, "Strict-Transport-Security") != NULL) {
        printf("Znaleziono nagłówek HSTS!\n");
    }
    if (strstr(buffer, "X-Frame-Options") != NULL) {
        printf("Znaleziono nagłówek X-Frame-Options!\n");
    }
    if (strstr(buffer, "X-XSS-Protection") != NULL) {
        printf("Znaleziono nagłówek X-XSS-Protection!\n");
    }

    return nitems * size;
}

int main(void) {
    CURL *curl;
    CURLcode res;

    // Inicjalizacja cURL
    curl = curl_easy_init();
    if (curl) {
        // Ustawiamy URL
        curl_easy_setopt(curl, CURLOPT_URL, "https://www.youtube.com/watch?v=kt2vIUEpeEs&t=1064s");

        // Ustawiamy, żeby cURL przetwarzał tylko nagłówki HTTP
        curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, header_callback);

        // Wysyłamy żądanie HTTP
        res = curl_easy_perform(curl);

        // Sprawdzamy, czy operacja się powiodła
        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        }

        // Zamykanie cURL
        curl_easy_cleanup(curl);
    }

    return 0;
}
