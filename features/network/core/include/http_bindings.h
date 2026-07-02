// بسم الله الرحمن الرحيم
// HTTP Bindings - C++ to Sad Language Bridge
// روابط HTTP - جسر بين C++ ولغة ص

#ifndef SAD_NETWORK_HTTP_BINDINGS_H
#define SAD_NETWORK_HTTP_BINDINGS_H

#ifdef __cplusplus
extern "C"
{
#endif

    // ==========================================
    // HTTP Client Functions
    // ==========================================

    /**
     * @brief Create new HTTP client
     * @return Pointer to HttpClient instance
     */
    void *sad_http_client_new();

    /**
     * @brief Free HTTP client
     * @param client Pointer to HttpClient instance
     */
    void sad_http_client_free(void *client);

    /**
     * @brief Send GET request
     * @param client Pointer to HttpClient instance
     * @param url URL string
     * @return Pointer to HttpResponse instance
     */
    void *sad_http_client_get(void *client, const char *url);

    /**
     * @brief Send POST request
     * @param client Pointer to HttpClient instance
     * @param url URL string
     * @param body Request body
     * @return Pointer to HttpResponse instance
     */
    void *sad_http_client_post(void *client, const char *url, const char *body);

    /**
     * @brief Send PUT request
     */
    void *sad_http_client_put(void *client, const char *url, const char *body);

    /**
     * @brief Send DELETE request
     */
    void *sad_http_client_delete(void *client, const char *url);

    /**
     * @brief Send PATCH request
     */
    void *sad_http_client_patch(void *client, const char *url, const char *body);

    /**
     * @brief Set base URL
     */
    void sad_http_client_set_base_url(void *client, const char *base_url);

    /**
     * @brief Set default header
     */
    void sad_http_client_set_header(void *client, const char *key, const char *value);

    /**
     * @brief Set timeout in milliseconds
     */
    void sad_http_client_set_timeout(void *client, int timeout_ms);

    /**
     * @brief Set Bearer token for authentication
     */
    void sad_http_client_set_bearer_token(void *client, const char *token);

    /**
     * @brief Get last error message
     * @return Error message string (must be freed by caller)
     */
    char *sad_http_client_last_error(void *client);

    /**
     * @brief Check if last operation was successful
     */
    int sad_http_client_is_ok(void *client);

    // ==========================================
    // HTTP Response Functions
    // ==========================================

    /**
     * @brief Free HTTP response
     */
    void sad_http_response_free(void *response);

    /**
     * @brief Get response status code
     */
    int sad_http_response_status(void *response);

    /**
     * @brief Get response body
     * @return Body string (must be freed by caller)
     */
    char *sad_http_response_body(void *response);

    /**
     * @brief Get response header
     * @return Header value (must be freed by caller)
     */
    char *sad_http_response_header(void *response, const char *key);

    /**
     * @brief Check if response is successful (2xx)
     */
    int sad_http_response_is_success(void *response);

    // ==========================================
    // HTTP Server Functions
    // ==========================================

    /**
     * @brief (AR) نوع مؤشر دالة معالج الطلبات الأصلي
     * @brief (EN) Native handler callback type for HTTP route handlers
     *
     * @param request  (void*) مؤشر لكائن HttpRequest
     * @param response (void*) مؤشر لكائن HttpResponse
     * @param user_data (void*) بيانات سياق المستدعي (interpreter + function name)
     */
    typedef void (*sad_http_native_handler)(void *request, void *response, void *user_data);

    /**
     * @brief Create new HTTP server
     * @param port Port number
     * @return Pointer to HttpServer instance
     */
    void *sad_http_server_new(int port);

    /**
     * @brief Free HTTP server
     */
    void sad_http_server_free(void *server);

    /**
     * @brief Register GET route
     * @param server Pointer to HttpServer instance
     * @param path Route path
     * @param handler Handler function pointer
     */
    void sad_http_server_get(void *server, const char *path, void *handler);

    /**
     * @brief Register POST route
     */
    void sad_http_server_post(void *server, const char *path, void *handler);

    /**
     * @brief Register PUT route
     */
    void sad_http_server_put(void *server, const char *path, void *handler);

    /**
     * @brief Register DELETE route
     */
    void sad_http_server_delete(void *server, const char *path, void *handler);

    /**
     * @brief Start server and listen
     */
    void sad_http_server_listen(void *server);

    /**
     * @brief Stop server
     */
    void sad_http_server_stop(void *server);

    /**
     * @brief Enable CORS
     */
    void sad_http_server_enable_cors(void *server, const char *origin);

    // ==========================================
    // Callback-based Route Registration
    // (AR) تسجيل مسارات بمعالجات أصلية تدعم بيانات سياق
    // ==========================================

    /**
     * @brief (AR) تسجيل مسار GET مع معالج أصلي ودعم بيانات سياق
     * @brief (EN) Register GET route with native callback + user_data context
     */
    void sad_http_server_get_cb(void *server, const char *path,
                                sad_http_native_handler handler, void *user_data);

    /**
     * @brief (AR) تسجيل مسار POST مع معالج أصلي
     * @brief (EN) Register POST route with native callback + user_data context
     */
    void sad_http_server_post_cb(void *server, const char *path,
                                 sad_http_native_handler handler, void *user_data);

    /**
     * @brief (AR) تسجيل مسار PUT مع معالج أصلي
     * @brief (EN) Register PUT route with native callback + user_data context
     */
    void sad_http_server_put_cb(void *server, const char *path,
                                sad_http_native_handler handler, void *user_data);

    /**
     * @brief (AR) تسجيل مسار DELETE مع معالج أصلي
     * @brief (EN) Register DELETE route with native callback + user_data context
     */
    void sad_http_server_delete_cb(void *server, const char *path,
                                   sad_http_native_handler handler, void *user_data);

    // ==========================================
    // HTTP Request Functions (for handlers)
    // ==========================================

    /**
     * @brief Get request method
     * @return Method string (must be freed by caller)
     */
    char *sad_http_request_method(void *request);

    /**
     * @brief Get request path
     * @return Path string (must be freed by caller)
     */
    char *sad_http_request_path(void *request);

    /**
     * @brief Get request body
     * @return Body string (must be freed by caller)
     */
    char *sad_http_request_body(void *request);

    /**
     * @brief Get request header
     * @return Header value (must be freed by caller)
     */
    char *sad_http_request_header(void *request, const char *key);

    /**
     * @brief Get query parameter
     * @return Query param value (must be freed by caller)
     */
    char *sad_http_request_query_param(void *request, const char *key);

    // ==========================================
    // HTTP Response Functions (for handlers)
    // ==========================================

    /**
     * @brief Set response status
     */
    void sad_http_response_set_status(void *response, int status);

    /**
     * @brief Set response body
     */
    void sad_http_response_set_body(void *response, const char *body);

    /**
     * @brief Set response as JSON
     */
    void sad_http_response_set_json(void *response, const char *json);

    /**
     * @brief Set response as HTML
     */
    void sad_http_response_set_html(void *response, const char *html);

    /**
     * @brief Set response header
     */
    void sad_http_response_set_header(void *response, const char *key, const char *value);

    // ==========================================
    // Helper Functions
    // ==========================================

    /**
     * @brief Free string allocated by bindings
     */
    void sad_http_free_string(char *str);

#ifdef __cplusplus
}
#endif

#endif // SAD_NETWORK_HTTP_BINDINGS_H
