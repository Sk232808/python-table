/*
 * Five-subject marks checker.
 * Build with MinGW: gcc shivam.c -o marks_webapp.exe -lws2_32
 * Run marks_webapp.exe, then open http://127.0.0.1:8080
 */
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#pragma comment(lib, "Ws2_32.lib")

#define PORT 8080
#define REQUEST_SIZE 16384
#define RESPONSE_SIZE 32768

static const char *page_start =
    "<!doctype html><html lang=\"en\"><head>"
    "<meta charset=\"UTF-8\"><meta name=\"viewport\" content=\"width=device-width,initial-scale=1\">"
    "<title>Marks Result Checker</title><style>"
    ":root{font-family:Georgia,serif;color:#17212b;background:#e7f0ec}"
    "*{box-sizing:border-box}body{margin:0;min-height:100vh;display:grid;place-items:center;padding:24px}"
    ".box{width:min(100%,620px);background:#fffdf8;border:1px solid #ded8ca;padding:clamp(24px,6vw,52px);box-shadow:12px 12px 0 #bfd1c9}"
    "h1{font-size:clamp(2.2rem,7vw,4rem);line-height:.95;margin:0 0 12px;max-width:460px}"
    ".intro,.field,button,.result,.error{font-family:Arial,sans-serif}.intro{color:#697386;line-height:1.5;margin:0 0 28px}"
    ".grid{display:grid;grid-template-columns:repeat(2,1fr);gap:16px}.field{font-size:.78rem;font-weight:bold;text-transform:uppercase;letter-spacing:.04em}"
    "input{display:block;width:100%;margin-top:7px;padding:13px 12px;border:1px solid #ded8ca;font-size:1rem}"
    "input:focus{outline:2px solid #df7047;outline-offset:2px}button{width:100%;margin-top:24px;padding:15px;border:0;background:#df7047;color:white;font-size:1rem;font-weight:bold;cursor:pointer}"
    "button:hover{background:#b74e2d}.result,.error{margin-top:22px;padding:18px;line-height:1.5;border-left:5px solid #287d56;background:#e8f4ed;color:#287d56}"
    ".result.fail,.error{border-color:#bd3e45;background:#fbecee;color:#bd3e45}.result strong{display:block;font-size:1.5rem}"
    "@media(max-width:420px){.grid{grid-template-columns:1fr}}</style></head><body><main class=\"box\">"
    "<h1>Marks result checker</h1><p class=\"intro\">Enter marks out of 100 for five subjects. Your average percentage decides the result.</p>"
    "<form method=\"post\" action=\"/check\"><div class=\"grid\">";

static void send_all(SOCKET client, const char *text)
{
    int remaining = (int)strlen(text);
    const char *cursor = text;
    while (remaining > 0) {
        int sent = send(client, cursor, remaining, 0);
        if (sent == SOCKET_ERROR) {
            return;
        }
        cursor += sent;
        remaining -= sent;
    }
}

static int read_mark(const char *body, int number, double *mark)
{
    char key[16];
    char *start;
    char *end;

    sprintf_s(key, sizeof(key), "mark%d=", number);
    start = strstr(body, key);
    if (start == NULL) {
        return 0;
    }
    start += strlen(key);
    *mark = strtod(start, &end);
    return end != start && (*mark >= 0.0 && *mark <= 100.0);
}

static void make_page(char *response, size_t response_size, const char *body)
{
    const char *subjects[] = {"Subject 1", "Subject 2", "Subject 3", "Subject 4", "Subject 5"};
    double marks[5];
    double percentage = 0.0;
    int valid = 1;
    int has_submission = body[0] != '\0';
    int written;
    int index;

    for (index = 0; index < 5; index++) {
        marks[index] = 0.0;
        if (!read_mark(body, index + 1, &marks[index])) {
            valid = 0;
        }
        percentage += marks[index];
    }

    written = sprintf_s(response, response_size,
        "HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=UTF-8\r\nConnection: close\r\n\r\n%s",
        page_start);
    for (index = 0; index < 5 && written > 0; index++) {
        written += sprintf_s(response + written, response_size - (size_t)written,
            "<label class=field>%s<input type=number name=mark%d min=0 max=100 step=any required></label>",
            subjects[index], index + 1);
    }
    if (written <= 0) {
        return;
    }
    written += sprintf_s(response + written, response_size - (size_t)written,
        "</div><button type=submit>Check result</button></form>");
    if (valid) {
        percentage /= 5.0;
        written += sprintf_s(response + written, response_size - (size_t)written,
            "<div class=\"result%s\" role=status><strong>%s</strong>Average percentage: %.2f%%</div>",
            percentage >= 33.0 ? "" : " fail",
            percentage >= 33.0 ? "Congratulations! You passed." : "You failed.",
            percentage);
    } else if (has_submission) {
        written += sprintf_s(response + written, response_size - (size_t)written,
            "<div class=error role=alert>Please enter marks between 0 and 100 for all five subjects.</div>");
    }
    sprintf_s(response + written, response_size - (size_t)written, "</main></body></html>");
}

int main(void)
{
    WSADATA wsa_data;
    SOCKET server;
    struct sockaddr_in address;

    if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0) {
        printf("Could not start Winsock.\n");
        return 1;
    }
    server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (server == INVALID_SOCKET) {
        printf("Could not create server socket.\n");
        WSACleanup();
        return 1;
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    address.sin_port = htons(PORT);
    if (bind(server, (struct sockaddr *)&address, sizeof(address)) == SOCKET_ERROR || listen(server, 5) == SOCKET_ERROR) {
        printf("Could not listen on port %d.\n", PORT);
        closesocket(server);
        WSACleanup();
        return 1;
    }

    printf("Marks checker running at http://127.0.0.1:%d\n", PORT);
    printf("Press Ctrl+C to stop.\n");
    while (1) {
        SOCKET client = accept(server, NULL, NULL);
        char request[REQUEST_SIZE] = {0};
        char response[RESPONSE_SIZE] = {0};
        int received;
        const char *body;

        if (client == INVALID_SOCKET) {
            continue;
        }
        received = recv(client, request, sizeof(request) - 1, 0);
        body = strstr(request, "\r\n\r\n");
        if (received > 0 && body != NULL && strncmp(request, "POST /check", 11) == 0) {
            make_page(response, sizeof(response), body + 4);
        } else {
            make_page(response, sizeof(response), "");
        }
        send_all(client, response);
        closesocket(client);
    }
}
