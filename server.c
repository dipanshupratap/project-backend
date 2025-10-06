#include "mongoose.h"
#include <stdio.h>
#include <string.h>
#include "backend.h"

#define PORT "8000"

Map g_map = { .count = 0 };

static void send_json(struct mg_connection *nc, const char *json) {
    mg_printf(nc, "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n"
                  "Access-Control-Allow-Origin: *\r\n"
                  "Content-Length: %d\r\n\r\n%s",
                  (int)strlen(json), json);
}

// Simple JSON parse, expects ["a","b","c","d","e","f","g"]
int parse_json_answers(const char *json, char answers[7][MAX_LEN]) {
    int idx = 0;
    const char *p = json;
    while (*p && idx < 7) {
        while (*p && *p != '"') p++;
        if (!*p) break;
        p++;
        int l = 0;
        while (p[l] && p[l] != '"') l++;
        if (l >= MAX_LEN) l = MAX_LEN - 1;
        strncpy(answers[idx], p, l); answers[idx][l] = 0;
        idx++; p += l + 1;
    }
    return idx == 7;
}

static void handle_request(struct mg_connection *nc, int ev, void *ev_data) {
    if (ev != MG_EV_HTTP_REQUEST) return;
    struct http_message *hm = (struct http_message *)ev_data;

    if (mg_vcmp(&hm->uri, "/api/strength") == 0) {
        char pw[101] = {0};
        mg_get_http_var(&hm->body, "password", pw, sizeof(pw));
        const char *result = checkStrength(pw);
        char resp[128];
        snprintf(resp, sizeof(resp), "{\"strength\":\"%s\"}", result);
        send_json(nc, resp);
    }
    else if (mg_vcmp(&hm->uri, "/api/generate") == 0) {
        char answers[7][MAX_LEN];
        char buf[1024] = {0};
        memcpy(buf, hm->body.p, hm->body.len < sizeof(buf) ? hm->body.len : sizeof(buf)-1);
        if (!parse_json_answers(buf, answers)) {
            mg_printf(nc,"HTTP/1.1 400 Bad Request\r\n\r\n{\"error\":\"7 answers required\"}");
            return;
        }
        Queue q; initQueue(&q);
        for (int i = 0; i < 7; i++) enqueue(&q, answers[i]);
        char pwds[3][MAX_LEN];
        generatePasswords(&q, pwds);
        for (int i = 0; i < 3; i++) {
            addToMap(&g_map, pwds[i], &q);
            savePasswordToFile(pwds[i], &q);
        }
        char resp[512];
        snprintf(resp, sizeof(resp), "{\"passwords\":[\"%s\",\"%s\",\"%s\"]}", pwds[0], pwds[1], pwds[2]);
        send_json(nc, resp);
    }
    else if (mg_vcmp(&hm->uri, "/api/decode") == 0) {
        char pw[MAX_LEN] = {0};
        mg_get_http_var(&hm->body, "password", pw, sizeof(pw));
        char ans[7][MAX_LEN];
        if (!decodePassword(&g_map, pw, ans)) {
            mg_printf(nc, "HTTP/1.1 404 Not Found\r\n\r\n{\"error\":\"Not found\"}");
        } else {
            char resp[512];
            snprintf(resp, sizeof(resp),
                "{\"Name\":\"%s\",\"Hobby\":\"%s\",\"FavFood\":\"%s\","
                "\"DOB\":\"%s\",\"Color\":\"%s\",\"Movie\":\"%s\",\"PetName\":\"%s\"}",
                ans[0], ans[1], ans[2], ans[3], ans[4], ans[5], ans[6]);
            send_json(nc, resp);
        }
    }
    else {
        mg_printf(nc, "HTTP/1.1 404 Not Found\r\n\r\n");
    }
}

int main() {
    struct mg_mgr mgr;
    mg_mgr_init(&mgr, NULL);
    mg_bind(&mgr, PORT, handle_request);
    printf("C Password API running at http://localhost:%s/\n", PORT);
    for (;;) mg_mgr_poll(&mgr, 1000);
    mg_mgr_free(&mgr);
    return 0;
}
