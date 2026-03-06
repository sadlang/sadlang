package sad.android.communication.graphql

import android.os.Handler
import android.os.Looper
import java.io.*
import java.net.HttpURLConnection
import java.net.URL
import java.util.concurrent.ConcurrentHashMap
import java.util.concurrent.ExecutorService
import java.util.concurrent.Executors

/**
 * جسر GraphQL - استعلامات وطفرات GraphQL
 * GraphQL Bridge - GraphQL queries and mutations
 * 
 * @author فريق لغة ص
 */
object SadGraphQLBridge {
    
    private val executor: ExecutorService = Executors.newFixedThreadPool(4)
    private val mainHandler = Handler(Looper.getMainLooper())
    private val clients = ConcurrentHashMap<String, GraphQLClient>()
    
    /**
     * عميل GraphQL / GraphQL client
     */
    data class GraphQLClient(
        val id: String,
        val endpoint: String,
        val headers: MutableMap<String, String> = mutableMapOf(),
        var connectTimeout: Int = 15000,
        var readTimeout: Int = 30000
    )
    
    /**
     * طلب GraphQL / GraphQL request
     */
    data class GraphQLRequest(
        val query: String,
        val variables: Map<String, Any?> = emptyMap(),
        val operationName: String? = null
    )
    
    /**
     * استجابة GraphQL / GraphQL response
     */
    data class GraphQLResponse(
        val data: String?,
        val errors: List<GraphQLError>?,
        val extensions: String?,
        val success: Boolean,
        val rawResponse: String?
    )
    
    /**
     * خطأ GraphQL / GraphQL error
     */
    data class GraphQLError(
        val message: String,
        val locations: List<Location>?,
        val path: List<Any>?,
        val extensions: Map<String, Any>?
    )
    
    /**
     * موقع الخطأ / Error location
     */
    data class Location(
        val line: Int,
        val column: Int
    )
    
    /**
     * مستمع الاستجابة / Response callback
     */
    interface GraphQLCallback {
        fun onSuccess(response: GraphQLResponse)
        fun onError(error: String)
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         إدارة العملاء
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * إنشاء عميل / Create client
     */
    fun createClient(endpoint: String, headers: Map<String, String> = emptyMap()): String {
        val clientId = "graphql:${System.currentTimeMillis()}"
        
        val client = GraphQLClient(
            id = clientId,
            endpoint = endpoint,
            headers = headers.toMutableMap()
        )
        
        clients[clientId] = client
        return clientId
    }
    
    /**
     * تعيين ترويسة / Set header
     */
    fun setHeader(clientId: String, name: String, value: String): Boolean {
        val client = clients[clientId] ?: return false
        client.headers[name] = value
        return true
    }
    
    /**
     * تعيين Bearer Token / Set auth token
     */
    fun setAuthToken(clientId: String, token: String): Boolean {
        return setHeader(clientId, "Authorization", "Bearer $token")
    }
    
    /**
     * إغلاق العميل / Close client
     */
    fun closeClient(clientId: String): Boolean {
        return clients.remove(clientId) != null
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         الاستعلامات
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * تنفيذ استعلام / Execute query
     */
    fun query(
        clientId: String,
        query: String,
        variables: Map<String, Any?> = emptyMap(),
        callback: GraphQLCallback
    ) {
        execute(clientId, GraphQLRequest(query, variables), callback)
    }
    
    /**
     * تنفيذ استعلام متزامن / Execute synchronous query
     */
    fun querySync(
        clientId: String,
        query: String,
        variables: Map<String, Any?> = emptyMap()
    ): GraphQLResponse {
        return executeSync(clientId, GraphQLRequest(query, variables))
    }
    
    /**
     * تنفيذ طفرة / Execute mutation
     */
    fun mutate(
        clientId: String,
        mutation: String,
        variables: Map<String, Any?> = emptyMap(),
        callback: GraphQLCallback
    ) {
        execute(clientId, GraphQLRequest(mutation, variables), callback)
    }
    
    /**
     * تنفيذ طفرة متزامنة / Execute synchronous mutation
     */
    fun mutateSync(
        clientId: String,
        mutation: String,
        variables: Map<String, Any?> = emptyMap()
    ): GraphQLResponse {
        return executeSync(clientId, GraphQLRequest(mutation, variables))
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         التنفيذ
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * تنفيذ طلب / Execute request
     */
    fun execute(clientId: String, request: GraphQLRequest, callback: GraphQLCallback) {
        val client = clients[clientId]
        if (client == null) {
            callback.onError("العميل غير موجود")
            return
        }
        
        executor.execute {
            val response = performRequest(client, request)
            mainHandler.post {
                if (response.success) {
                    callback.onSuccess(response)
                } else {
                    callback.onError(response.errors?.firstOrNull()?.message ?: "خطأ غير معروف")
                }
            }
        }
    }
    
    /**
     * تنفيذ متزامن / Synchronous execution
     */
    fun executeSync(clientId: String, request: GraphQLRequest): GraphQLResponse {
        val client = clients[clientId]
            ?: return GraphQLResponse(
                data = null,
                errors = listOf(GraphQLError("العميل غير موجود", null, null, null)),
                extensions = null,
                success = false,
                rawResponse = null
            )
        
        return performRequest(client, request)
    }
    
    private fun performRequest(client: GraphQLClient, request: GraphQLRequest): GraphQLResponse {
        var connection: HttpURLConnection? = null
        
        try {
            connection = URL(client.endpoint).openConnection() as HttpURLConnection
            
            connection.apply {
                requestMethod = "POST"
                doOutput = true
                doInput = true
                connectTimeout = client.connectTimeout
                readTimeout = client.readTimeout
                setRequestProperty("Content-Type", "application/json")
                setRequestProperty("Accept", "application/json")
                
                client.headers.forEach { (key, value) ->
                    setRequestProperty(key, value)
                }
            }
            
            // بناء body
            val body = buildRequestBody(request)
            connection.outputStream.bufferedWriter().use { it.write(body) }
            
            val responseCode = connection.responseCode
            val responseBody = if (responseCode in 200..299) {
                connection.inputStream.bufferedReader().readText()
            } else {
                connection.errorStream?.bufferedReader()?.readText() ?: ""
            }
            
            return parseResponse(responseBody)
            
        } catch (e: Exception) {
            return GraphQLResponse(
                data = null,
                errors = listOf(GraphQLError(e.message ?: "خطأ في الاتصال", null, null, null)),
                extensions = null,
                success = false,
                rawResponse = null
            )
        } finally {
            connection?.disconnect()
        }
    }
    
    private fun buildRequestBody(request: GraphQLRequest): String {
        val sb = StringBuilder()
        sb.append("{")
        sb.append("\"query\":\"${escapeJson(request.query)}\"")
        
        if (request.variables.isNotEmpty()) {
            sb.append(",\"variables\":")
            sb.append(mapToJson(request.variables))
        }
        
        request.operationName?.let {
            sb.append(",\"operationName\":\"$it\"")
        }
        
        sb.append("}")
        return sb.toString()
    }
    
    private fun parseResponse(json: String): GraphQLResponse {
        // تحليل بسيط - للاستخدام الكامل يُفضل مكتبة JSON
        val hasErrors = json.contains("\"errors\"")
        val hasData = json.contains("\"data\"")
        
        val dataMatch = Regex("\"data\"\\s*:\\s*(\\{[^}]*\\}|null)").find(json)
        val data = dataMatch?.groupValues?.get(1)
        
        val errors = if (hasErrors) {
            parseErrors(json)
        } else null
        
        return GraphQLResponse(
            data = data,
            errors = errors,
            extensions = null,
            success = hasData && !hasErrors,
            rawResponse = json
        )
    }
    
    private fun parseErrors(json: String): List<GraphQLError> {
        // تحليل بسيط للأخطاء
        val errorPattern = Regex("\"message\"\\s*:\\s*\"([^\"]+)\"")
        val messages = errorPattern.findAll(json).map { it.groupValues[1] }.toList()
        
        return messages.map { GraphQLError(it, null, null, null) }
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         بناء الاستعلامات
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * بناء استعلام / Query builder
     */
    class QueryBuilder {
        private val sb = StringBuilder()
        private var indentLevel = 0
        
        fun query(name: String? = null, block: QueryBuilder.() -> Unit): String {
            sb.append("query")
            name?.let { sb.append(" $it") }
            sb.append(" {\n")
            indentLevel++
            block()
            indentLevel--
            sb.append("}")
            return sb.toString()
        }
        
        fun mutation(name: String? = null, block: QueryBuilder.() -> Unit): String {
            sb.append("mutation")
            name?.let { sb.append(" $it") }
            sb.append(" {\n")
            indentLevel++
            block()
            indentLevel--
            sb.append("}")
            return sb.toString()
        }
        
        fun field(name: String, args: Map<String, Any>? = null, block: (QueryBuilder.() -> Unit)? = null) {
            indent()
            sb.append(name)
            
            args?.let {
                sb.append("(")
                val argsStr = it.entries.joinToString(", ") { (k, v) ->
                    "$k: ${formatValue(v)}"
                }
                sb.append(argsStr)
                sb.append(")")
            }
            
            if (block != null) {
                sb.append(" {\n")
                indentLevel++
                block()
                indentLevel--
                indent()
                sb.append("}\n")
            } else {
                sb.append("\n")
            }
        }
        
        fun fields(vararg names: String) {
            names.forEach { field(it) }
        }
        
        fun fragment(name: String, on: String, block: QueryBuilder.() -> Unit) {
            sb.append("\nfragment $name on $on {\n")
            indentLevel++
            block()
            indentLevel--
            sb.append("}\n")
        }
        
        fun useFragment(name: String) {
            indent()
            sb.append("...$name\n")
        }
        
        private fun indent() {
            repeat(indentLevel) { sb.append("  ") }
        }
        
        private fun formatValue(value: Any): String {
            return when (value) {
                is String -> "\"$value\""
                is Number -> value.toString()
                is Boolean -> value.toString()
                is List<*> -> "[${value.joinToString(", ") { formatValue(it!!) }}]"
                is Map<*, *> -> {
                    val entries = value.entries.joinToString(", ") { (k, v) ->
                        "$k: ${formatValue(v!!)}"
                    }
                    "{$entries}"
                }
                else -> "\"$value\""
            }
        }
    }
    
    /**
     * إنشاء استعلام / Create query
     */
    fun buildQuery(block: QueryBuilder.() -> Unit): String {
        val builder = QueryBuilder()
        return builder.query(block = block)
    }
    
    /**
     * إنشاء طفرة / Create mutation
     */
    fun buildMutation(block: QueryBuilder.() -> Unit): String {
        val builder = QueryBuilder()
        return builder.mutation(block = block)
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         مساعدات JSON
    // ═══════════════════════════════════════════════════════════════
    
    private fun mapToJson(map: Map<String, Any?>): String {
        val entries = map.entries.joinToString(",") { (key, value) ->
            "\"$key\":${valueToJson(value)}"
        }
        return "{$entries}"
    }
    
    private fun valueToJson(value: Any?): String {
        return when (value) {
            null -> "null"
            is String -> "\"${escapeJson(value)}\""
            is Number -> value.toString()
            is Boolean -> value.toString()
            is Map<*, *> -> mapToJson(value as Map<String, Any?>)
            is List<*> -> "[${value.joinToString(",") { valueToJson(it) }}]"
            else -> "\"${escapeJson(value.toString())}\""
        }
    }
    
    private fun escapeJson(str: String): String {
        return str.replace("\\", "\\\\")
            .replace("\"", "\\\"")
            .replace("\n", "\\n")
            .replace("\r", "\\r")
            .replace("\t", "\\t")
    }
    
    /**
     * تحرير الموارد / Release resources
     */
    fun release() {
        clients.clear()
        executor.shutdown()
    }
}
