package sad.android.widgets.weather

import android.content.Context
import android.os.Handler
import android.os.Looper
import java.io.BufferedReader
import java.io.InputStreamReader
import java.net.HttpURLConnection
import java.net.URL
import java.util.concurrent.ConcurrentHashMap
import java.util.concurrent.ExecutorService
import java.util.concurrent.Executors

/**
 * جسر ويدجت الطقس - عرض حالة الطقس
 * Weather Widget Bridge - Weather condition display
 * 
 * @author فريق لغة ص
 */
object SadWidgetWeatherBridge {
    
    private var context: Context? = null
    private val mainHandler = Handler(Looper.getMainLooper())
    private val executor: ExecutorService = Executors.newSingleThreadExecutor()
    private val weatherCache = ConcurrentHashMap<String, CachedWeather>()
    
    /**
     * طقس مخزن مؤقتاً / Cached weather
     */
    data class CachedWeather(
        val data: WeatherData,
        val timestamp: Long,
        val expiresAt: Long
    )
    
    /**
     * بيانات الطقس / Weather data
     */
    data class WeatherData(
        val location: String,
        val temperature: Double,
        val temperatureUnit: TemperatureUnit,
        val condition: WeatherCondition,
        val conditionText: String,
        val humidity: Int,
        val windSpeed: Double,
        val windDirection: String,
        val pressure: Double,
        val visibility: Double,
        val uvIndex: Int,
        val feelsLike: Double,
        val icon: String,
        val sunrise: String?,
        val sunset: String?,
        val lastUpdated: Long
    )
    
    /**
     * وحدة الحرارة / Temperature unit
     */
    enum class TemperatureUnit {
        CELSIUS,
        FAHRENHEIT,
        KELVIN
    }
    
    /**
     * حالة الطقس / Weather condition
     */
    enum class WeatherCondition {
        SUNNY,
        PARTLY_CLOUDY,
        CLOUDY,
        OVERCAST,
        MIST,
        FOG,
        LIGHT_RAIN,
        RAIN,
        HEAVY_RAIN,
        THUNDERSTORM,
        LIGHT_SNOW,
        SNOW,
        HEAVY_SNOW,
        SLEET,
        HAIL,
        DUST,
        SAND,
        SMOKE,
        TORNADO,
        UNKNOWN
    }
    
    /**
     * توقع يومي / Daily forecast
     */
    data class DailyForecast(
        val date: String,
        val dayName: String,
        val highTemp: Double,
        val lowTemp: Double,
        val condition: WeatherCondition,
        val conditionText: String,
        val icon: String,
        val chanceOfRain: Int
    )
    
    /**
     * رد نداء الطقس / Weather callback
     */
    interface WeatherCallback {
        fun onWeatherLoaded(weather: WeatherData)
        fun onForecastLoaded(forecast: List<DailyForecast>)
        fun onError(error: String)
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         التهيئة
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * تهيئة / Initialize
     */
    fun initialize(ctx: Context) {
        context = ctx.applicationContext
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         جلب الطقس
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * جلب الطقس بالموقع / Fetch weather by location
     */
    fun fetchWeather(
        latitude: Double,
        longitude: Double,
        apiKey: String,
        apiProvider: ApiProvider = ApiProvider.OPEN_WEATHER_MAP,
        unit: TemperatureUnit = TemperatureUnit.CELSIUS,
        callback: WeatherCallback
    ) {
        val cacheKey = "$latitude,$longitude"
        
        // التحقق من التخزين المؤقت
        weatherCache[cacheKey]?.let { cached ->
            if (System.currentTimeMillis() < cached.expiresAt) {
                callback.onWeatherLoaded(cached.data)
                return
            }
        }
        
        executor.execute {
            try {
                val url = buildApiUrl(apiProvider, latitude, longitude, apiKey, unit)
                val response = makeHttpRequest(url)
                val weather = parseWeatherResponse(apiProvider, response, unit)
                
                // تخزين مؤقت لمدة 30 دقيقة
                val cached = CachedWeather(
                    data = weather,
                    timestamp = System.currentTimeMillis(),
                    expiresAt = System.currentTimeMillis() + 30 * 60 * 1000
                )
                weatherCache[cacheKey] = cached
                
                mainHandler.post { callback.onWeatherLoaded(weather) }
                
            } catch (e: Exception) {
                mainHandler.post { callback.onError(e.message ?: "خطأ في جلب الطقس") }
            }
        }
    }
    
    /**
     * جلب الطقس بالمدينة / Fetch weather by city
     */
    fun fetchWeatherByCity(
        city: String,
        apiKey: String,
        apiProvider: ApiProvider = ApiProvider.OPEN_WEATHER_MAP,
        unit: TemperatureUnit = TemperatureUnit.CELSIUS,
        callback: WeatherCallback
    ) {
        val cacheKey = "city:$city"
        
        weatherCache[cacheKey]?.let { cached ->
            if (System.currentTimeMillis() < cached.expiresAt) {
                callback.onWeatherLoaded(cached.data)
                return
            }
        }
        
        executor.execute {
            try {
                val url = buildCityApiUrl(apiProvider, city, apiKey, unit)
                val response = makeHttpRequest(url)
                val weather = parseWeatherResponse(apiProvider, response, unit)
                
                val cached = CachedWeather(
                    data = weather,
                    timestamp = System.currentTimeMillis(),
                    expiresAt = System.currentTimeMillis() + 30 * 60 * 1000
                )
                weatherCache[cacheKey] = cached
                
                mainHandler.post { callback.onWeatherLoaded(weather) }
                
            } catch (e: Exception) {
                mainHandler.post { callback.onError(e.message ?: "خطأ في جلب الطقس") }
            }
        }
    }
    
    /**
     * مزودو API / API providers
     */
    enum class ApiProvider {
        OPEN_WEATHER_MAP,
        WEATHER_API,
        CUSTOM
    }
    
    private fun buildApiUrl(
        provider: ApiProvider,
        lat: Double,
        lon: Double,
        apiKey: String,
        unit: TemperatureUnit
    ): String {
        val unitParam = when (unit) {
            TemperatureUnit.CELSIUS -> "metric"
            TemperatureUnit.FAHRENHEIT -> "imperial"
            TemperatureUnit.KELVIN -> "standard"
        }
        
        return when (provider) {
            ApiProvider.OPEN_WEATHER_MAP -> 
                "https://api.openweathermap.org/data/2.5/weather?lat=$lat&lon=$lon&appid=$apiKey&units=$unitParam&lang=ar"
            ApiProvider.WEATHER_API ->
                "https://api.weatherapi.com/v1/current.json?key=$apiKey&q=$lat,$lon&lang=ar"
            ApiProvider.CUSTOM -> ""
        }
    }
    
    private fun buildCityApiUrl(
        provider: ApiProvider,
        city: String,
        apiKey: String,
        unit: TemperatureUnit
    ): String {
        val unitParam = when (unit) {
            TemperatureUnit.CELSIUS -> "metric"
            TemperatureUnit.FAHRENHEIT -> "imperial"
            TemperatureUnit.KELVIN -> "standard"
        }
        
        val encodedCity = java.net.URLEncoder.encode(city, "UTF-8")
        
        return when (provider) {
            ApiProvider.OPEN_WEATHER_MAP ->
                "https://api.openweathermap.org/data/2.5/weather?q=$encodedCity&appid=$apiKey&units=$unitParam&lang=ar"
            ApiProvider.WEATHER_API ->
                "https://api.weatherapi.com/v1/current.json?key=$apiKey&q=$encodedCity&lang=ar"
            ApiProvider.CUSTOM -> ""
        }
    }
    
    private fun makeHttpRequest(url: String): String {
        val connection = URL(url).openConnection() as HttpURLConnection
        connection.requestMethod = "GET"
        connection.connectTimeout = 10000
        connection.readTimeout = 10000
        
        val reader = BufferedReader(InputStreamReader(connection.inputStream))
        val response = reader.readText()
        reader.close()
        connection.disconnect()
        
        return response
    }
    
    private fun parseWeatherResponse(
        provider: ApiProvider,
        json: String,
        unit: TemperatureUnit
    ): WeatherData {
        // تحليل بسيط - للاستخدام الكامل يُفضل مكتبة JSON
        return when (provider) {
            ApiProvider.OPEN_WEATHER_MAP -> parseOpenWeatherMap(json, unit)
            ApiProvider.WEATHER_API -> parseWeatherApi(json, unit)
            ApiProvider.CUSTOM -> throw Exception("Custom provider requires custom parser")
        }
    }
    
    private fun parseOpenWeatherMap(json: String, unit: TemperatureUnit): WeatherData {
        // تحليل بسيط باستخدام regex
        val temp = Regex("\"temp\"\\s*:\\s*([\\d.]+)").find(json)?.groupValues?.get(1)?.toDoubleOrNull() ?: 0.0
        val feelsLike = Regex("\"feels_like\"\\s*:\\s*([\\d.]+)").find(json)?.groupValues?.get(1)?.toDoubleOrNull() ?: 0.0
        val humidity = Regex("\"humidity\"\\s*:\\s*(\\d+)").find(json)?.groupValues?.get(1)?.toIntOrNull() ?: 0
        val description = Regex("\"description\"\\s*:\\s*\"([^\"]+)\"").find(json)?.groupValues?.get(1) ?: ""
        val name = Regex("\"name\"\\s*:\\s*\"([^\"]+)\"").find(json)?.groupValues?.get(1) ?: ""
        val windSpeed = Regex("\"speed\"\\s*:\\s*([\\d.]+)").find(json)?.groupValues?.get(1)?.toDoubleOrNull() ?: 0.0
        val icon = Regex("\"icon\"\\s*:\\s*\"([^\"]+)\"").find(json)?.groupValues?.get(1) ?: ""
        
        return WeatherData(
            location = name,
            temperature = temp,
            temperatureUnit = unit,
            condition = mapConditionFromIcon(icon),
            conditionText = description,
            humidity = humidity,
            windSpeed = windSpeed,
            windDirection = "",
            pressure = 0.0,
            visibility = 0.0,
            uvIndex = 0,
            feelsLike = feelsLike,
            icon = icon,
            sunrise = null,
            sunset = null,
            lastUpdated = System.currentTimeMillis()
        )
    }
    
    private fun parseWeatherApi(json: String, unit: TemperatureUnit): WeatherData {
        val temp = if (unit == TemperatureUnit.CELSIUS) {
            Regex("\"temp_c\"\\s*:\\s*([\\d.]+)").find(json)?.groupValues?.get(1)?.toDoubleOrNull() ?: 0.0
        } else {
            Regex("\"temp_f\"\\s*:\\s*([\\d.]+)").find(json)?.groupValues?.get(1)?.toDoubleOrNull() ?: 0.0
        }
        
        val humidity = Regex("\"humidity\"\\s*:\\s*(\\d+)").find(json)?.groupValues?.get(1)?.toIntOrNull() ?: 0
        val text = Regex("\"text\"\\s*:\\s*\"([^\"]+)\"").find(json)?.groupValues?.get(1) ?: ""
        val name = Regex("\"name\"\\s*:\\s*\"([^\"]+)\"").find(json)?.groupValues?.get(1) ?: ""
        
        return WeatherData(
            location = name,
            temperature = temp,
            temperatureUnit = unit,
            condition = WeatherCondition.UNKNOWN,
            conditionText = text,
            humidity = humidity,
            windSpeed = 0.0,
            windDirection = "",
            pressure = 0.0,
            visibility = 0.0,
            uvIndex = 0,
            feelsLike = temp,
            icon = "",
            sunrise = null,
            sunset = null,
            lastUpdated = System.currentTimeMillis()
        )
    }
    
    private fun mapConditionFromIcon(icon: String): WeatherCondition {
        return when {
            icon.startsWith("01") -> WeatherCondition.SUNNY
            icon.startsWith("02") -> WeatherCondition.PARTLY_CLOUDY
            icon.startsWith("03") -> WeatherCondition.CLOUDY
            icon.startsWith("04") -> WeatherCondition.OVERCAST
            icon.startsWith("09") -> WeatherCondition.LIGHT_RAIN
            icon.startsWith("10") -> WeatherCondition.RAIN
            icon.startsWith("11") -> WeatherCondition.THUNDERSTORM
            icon.startsWith("13") -> WeatherCondition.SNOW
            icon.startsWith("50") -> WeatherCondition.MIST
            else -> WeatherCondition.UNKNOWN
        }
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         التحويل
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * تحويل الحرارة / Convert temperature
     */
    fun convertTemperature(temp: Double, from: TemperatureUnit, to: TemperatureUnit): Double {
        if (from == to) return temp
        
        // التحويل إلى سيلسيوس أولاً
        val celsius = when (from) {
            TemperatureUnit.CELSIUS -> temp
            TemperatureUnit.FAHRENHEIT -> (temp - 32) * 5 / 9
            TemperatureUnit.KELVIN -> temp - 273.15
        }
        
        // التحويل من سيلسيوس إلى الوحدة المطلوبة
        return when (to) {
            TemperatureUnit.CELSIUS -> celsius
            TemperatureUnit.FAHRENHEIT -> celsius * 9 / 5 + 32
            TemperatureUnit.KELVIN -> celsius + 273.15
        }
    }
    
    /**
     * الحصول على أيقونة الطقس / Get weather icon
     */
    fun getConditionArabicText(condition: WeatherCondition): String {
        return when (condition) {
            WeatherCondition.SUNNY -> "مشمس"
            WeatherCondition.PARTLY_CLOUDY -> "غائم جزئياً"
            WeatherCondition.CLOUDY -> "غائم"
            WeatherCondition.OVERCAST -> "ملبد بالغيوم"
            WeatherCondition.MIST -> "ضبابي"
            WeatherCondition.FOG -> "ضباب كثيف"
            WeatherCondition.LIGHT_RAIN -> "مطر خفيف"
            WeatherCondition.RAIN -> "ممطر"
            WeatherCondition.HEAVY_RAIN -> "مطر غزير"
            WeatherCondition.THUNDERSTORM -> "عاصفة رعدية"
            WeatherCondition.LIGHT_SNOW -> "ثلج خفيف"
            WeatherCondition.SNOW -> "ثلوج"
            WeatherCondition.HEAVY_SNOW -> "ثلوج كثيفة"
            WeatherCondition.SLEET -> "مطر ثلجي"
            WeatherCondition.HAIL -> "بَرَد"
            WeatherCondition.DUST -> "غبار"
            WeatherCondition.SAND -> "عاصفة رملية"
            WeatherCondition.SMOKE -> "دخان"
            WeatherCondition.TORNADO -> "إعصار"
            WeatherCondition.UNKNOWN -> "غير معروف"
        }
    }
    
    /**
     * مسح التخزين المؤقت / Clear cache
     */
    fun clearCache() {
        weatherCache.clear()
    }
    
    /**
     * تحرير الموارد / Release resources
     */
    fun release() {
        weatherCache.clear()
        executor.shutdown()
        context = null
    }
}
