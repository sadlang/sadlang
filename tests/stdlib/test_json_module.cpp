// بسم الله الرحمن الرحيم
/**
 * @file test_json_module.cpp
 * @brief JSON Module Tests - اختبارات وحدة JSON
 * 
 * الحمد لله رب العالمين
 */

#include "stdlib/json/json_module.h"
#include <gtest/gtest.h>
#include <sstream>

using namespace sad::stdlib::json;

// ============================================================================
// JsonValue Tests - اختبارات قيمة JSON
// ============================================================================

TEST(JsonValueTest, NullConstruction) {
    JsonValue value;
    EXPECT_TRUE(value.is_null());
    EXPECT_EQ(value.type(), JsonType::NULL_TYPE);
}

TEST(JsonValueTest, BooleanConstruction) {
    JsonValue value_true(true);
    EXPECT_TRUE(value_true.is_boolean());
    EXPECT_TRUE(value_true.as_boolean());
    
    JsonValue value_false(false);
    EXPECT_FALSE(value_false.as_boolean());
}

TEST(JsonValueTest, NumberConstruction) {
    JsonValue value_double(3.14);
    EXPECT_TRUE(value_double.is_number());
    EXPECT_DOUBLE_EQ(value_double.as_number(), 3.14);
    
    JsonValue value_int(42);
    EXPECT_EQ(value_int.as_int(), 42);
}

TEST(JsonValueTest, StringConstruction) {
    JsonValue value("Hello World");
    EXPECT_TRUE(value.is_string());
    EXPECT_EQ(value.as_string(), "Hello World");
    
    JsonValue value_arabic("مرحبا");
    EXPECT_EQ(value_arabic.as_string(), "مرحبا");
}

TEST(JsonValueTest, ArrayConstruction) {
    JsonArray arr;
    arr.push(JsonValue(1));
    arr.push(JsonValue(2));
    arr.push(JsonValue(3));
    
    JsonValue value(arr);
    EXPECT_TRUE(value.is_array());
    EXPECT_EQ(value.as_array().size(), 3);
}

TEST(JsonValueTest, ObjectConstruction) {
    JsonObject obj;
    obj.set("name", JsonValue("Ahmad"));
    obj.set("age", JsonValue(25));
    
    JsonValue value(obj);
    EXPECT_TRUE(value.is_object());
    EXPECT_EQ(value.as_object().size(), 2);
}

TEST(JsonValueTest, TypeChecking) {
    JsonValue null_val;
    JsonValue bool_val(true);
    JsonValue num_val(42);
    JsonValue str_val("test");
    
    JsonArray temp_arr;
    JsonValue arr_val(temp_arr);
    
    JsonObject temp_obj;
    JsonValue obj_val(temp_obj);
    
    EXPECT_TRUE(null_val.is_null());
    EXPECT_TRUE(bool_val.is_boolean());
    EXPECT_TRUE(num_val.is_number());
    EXPECT_TRUE(str_val.is_string());
    EXPECT_TRUE(arr_val.is_array());
    EXPECT_TRUE(obj_val.is_object());
}

TEST(JsonValueTest, GetWithDefault) {
    JsonValue value("test");
    
    EXPECT_EQ(value.get_string("default"), "test");
    EXPECT_EQ(value.get_number(0.0), 0.0);
    EXPECT_EQ(value.get_boolean(true), true);
}

TEST(JsonValueTest, Equality) {
    JsonValue v1(42);
    JsonValue v2(42);
    JsonValue v3(43);
    
    EXPECT_EQ(v1, v2);
    EXPECT_NE(v1, v3);
}

// ============================================================================
// JsonArray Tests - اختبارات مصفوفة JSON
// ============================================================================

TEST(JsonArrayTest, PushPop) {
    JsonArray arr;
    
    arr.push(JsonValue(1));
    arr.push(JsonValue(2));
    arr.push(JsonValue(3));
    
    EXPECT_EQ(arr.size(), 3);
    EXPECT_FALSE(arr.empty());
    
    arr.pop();
    EXPECT_EQ(arr.size(), 2);
}

TEST(JsonArrayTest, Indexing) {
    JsonArray arr;
    arr.push(JsonValue(10));
    arr.push(JsonValue(20));
    arr.push(JsonValue(30));
    
    EXPECT_EQ(arr[0].as_int(), 10);
    EXPECT_EQ(arr[1].as_int(), 20);
    EXPECT_EQ(arr[2].as_int(), 30);
    
    arr[1] = JsonValue(25);
    EXPECT_EQ(arr[1].as_int(), 25);
}

TEST(JsonArrayTest, Clear) {
    JsonArray arr;
    arr.push(JsonValue(1));
    arr.push(JsonValue(2));
    
    EXPECT_FALSE(arr.empty());
    arr.clear();
    EXPECT_TRUE(arr.empty());
}

TEST(JsonArrayTest, OutOfRange) {
    JsonArray arr;
    arr.push(JsonValue(1));
    
    EXPECT_THROW(arr.at(10), JsonException);
}

// ============================================================================
// JsonObject Tests - اختبارات كائن JSON
// ============================================================================

TEST(JsonObjectTest, SetGet) {
    JsonObject obj;
    
    obj.set("name", JsonValue("Ahmad"));
    obj.set("age", JsonValue(25));
    
    EXPECT_EQ(obj.get("name").as_string(), "Ahmad");
    EXPECT_EQ(obj.get("age").as_int(), 25);
}

TEST(JsonObjectTest, Has) {
    JsonObject obj;
    obj.set("key", JsonValue("value"));
    
    EXPECT_TRUE(obj.has("key"));
    EXPECT_FALSE(obj.has("missing"));
}

TEST(JsonObjectTest, Remove) {
    JsonObject obj;
    obj.set("key1", JsonValue(1));
    obj.set("key2", JsonValue(2));
    
    EXPECT_EQ(obj.size(), 2);
    obj.remove("key1");
    EXPECT_EQ(obj.size(), 1);
    EXPECT_FALSE(obj.has("key1"));
}

TEST(JsonObjectTest, Keys) {
    JsonObject obj;
    obj.set("a", JsonValue(1));
    obj.set("b", JsonValue(2));
    obj.set("c", JsonValue(3));
    
    auto keys = obj.keys();
    EXPECT_EQ(keys.size(), 3);
}

TEST(JsonObjectTest, Clear) {
    JsonObject obj;
    obj.set("key", JsonValue("value"));
    
    EXPECT_FALSE(obj.empty());
    obj.clear();
    EXPECT_TRUE(obj.empty());
}

// ============================================================================
// JsonParser Tests - اختبارات محلل JSON
// ============================================================================

TEST(JsonParserTest, ParseNull) {
    JsonValue value = JsonParser::parse("null");
    EXPECT_TRUE(value.is_null());
}

TEST(JsonParserTest, ParseBoolean) {
    JsonValue value_true = JsonParser::parse("true");
    EXPECT_TRUE(value_true.as_boolean());
    
    JsonValue value_false = JsonParser::parse("false");
    EXPECT_FALSE(value_false.as_boolean());
}

TEST(JsonParserTest, ParseNumber) {
    JsonValue int_val = JsonParser::parse("42");
    EXPECT_EQ(int_val.as_int(), 42);
    
    JsonValue double_val = JsonParser::parse("3.14");
    EXPECT_DOUBLE_EQ(double_val.as_number(), 3.14);
    
    JsonValue negative = JsonParser::parse("-10");
    EXPECT_EQ(negative.as_int(), -10);
    
    JsonValue scientific = JsonParser::parse("1.5e2");
    EXPECT_DOUBLE_EQ(scientific.as_number(), 150.0);
}

TEST(JsonParserTest, ParseString) {
    JsonValue value = JsonParser::parse("\"Hello World\"");
    EXPECT_EQ(value.as_string(), "Hello World");
    
    JsonValue empty = JsonParser::parse("\"\"");
    EXPECT_EQ(empty.as_string(), "");
}

TEST(JsonParserTest, ParseStringEscapes) {
    JsonValue value = JsonParser::parse(R"("Line 1\nLine 2\tTabbed")");
    EXPECT_EQ(value.as_string(), "Line 1\nLine 2\tTabbed");
    
    JsonValue quotes = JsonParser::parse(R"("He said \"Hello\"")");
    EXPECT_EQ(quotes.as_string(), "He said \"Hello\"");
}

TEST(JsonParserTest, ParseArray) {
    JsonValue value = JsonParser::parse("[1, 2, 3, 4, 5]");
    EXPECT_TRUE(value.is_array());
    EXPECT_EQ(value.as_array().size(), 5);
    EXPECT_EQ(value[2].as_int(), 3);
}

TEST(JsonParserTest, ParseEmptyArray) {
    JsonValue value = JsonParser::parse("[]");
    EXPECT_TRUE(value.is_array());
    EXPECT_TRUE(value.as_array().empty());
}

TEST(JsonParserTest, ParseNestedArray) {
    JsonValue value = JsonParser::parse("[[1, 2], [3, 4]]");
    EXPECT_TRUE(value.is_array());
    EXPECT_EQ(value.as_array().size(), 2);
    EXPECT_TRUE(value[0].is_array());
    EXPECT_EQ(value[0][1].as_int(), 2);
}

TEST(JsonParserTest, ParseObject) {
    JsonValue value = JsonParser::parse(R"({"name": "Ahmad", "age": 25})");
    EXPECT_TRUE(value.is_object());
    EXPECT_EQ(value["name"].as_string(), "Ahmad");
    EXPECT_EQ(value["age"].as_int(), 25);
}

TEST(JsonParserTest, ParseEmptyObject) {
    JsonValue value = JsonParser::parse("{}");
    EXPECT_TRUE(value.is_object());
    EXPECT_TRUE(value.as_object().empty());
}

TEST(JsonParserTest, ParseNestedObject) {
    JsonValue value = JsonParser::parse(R"({
        "person": {
            "name": "Ahmad",
            "address": {
                "city": "Cairo"
            }
        }
    })");
    
    EXPECT_TRUE(value.is_object());
    EXPECT_EQ(value["person"]["name"].as_string(), "Ahmad");
    EXPECT_EQ(value["person"]["address"]["city"].as_string(), "Cairo");
}

TEST(JsonParserTest, ParseComplex) {
    std::string json = R"({
        "name": "Ahmad",
        "age": 25,
        "skills": ["C++", "Python", "JavaScript"],
        "address": {
            "city": "Cairo",
            "country": "Egypt"
        },
        "active": true,
        "score": null
    })";
    
    JsonValue value = JsonParser::parse(json);
    EXPECT_TRUE(value.is_object());
    EXPECT_EQ(value["name"].as_string(), "Ahmad");
    EXPECT_EQ(value["age"].as_int(), 25);
    EXPECT_EQ(value["skills"].as_array().size(), 3);
    EXPECT_EQ(value["address"]["city"].as_string(), "Cairo");
    EXPECT_TRUE(value["active"].as_boolean());
    EXPECT_TRUE(value["score"].is_null());
}

TEST(JsonParserTest, ParseInvalid) {
    EXPECT_THROW(JsonParser::parse("invalid"), JsonException);
    EXPECT_THROW(JsonParser::parse("{invalid}"), JsonException);
    EXPECT_THROW(JsonParser::parse("[1, 2,]"), JsonException);
}

TEST(JsonParserTest, ParseWithComments) {
    JsonParseOptions options;
    options.allow_comments = true;
    
    std::string json = R"({
        // This is a comment
        "name": "Ahmad" /* inline comment */
    })";
    
    JsonValue value = JsonParser::parse(json, options);
    EXPECT_EQ(value["name"].as_string(), "Ahmad");
}

TEST(JsonParserTest, ParseWithTrailingCommas) {
    JsonParseOptions options;
    options.allow_trailing_commas = true;
    
    JsonValue arr = JsonParser::parse("[1, 2, 3,]", options);
    EXPECT_EQ(arr.as_array().size(), 3);
    
    JsonValue obj = JsonParser::parse(R"({"a": 1, "b": 2,})", options);
    EXPECT_EQ(obj.as_object().size(), 2);
}

TEST(JsonParserTest, Validate) {
    EXPECT_TRUE(JsonParser::validate(R"({"valid": true})"));
    EXPECT_FALSE(JsonParser::validate("invalid"));
}

// ============================================================================
// JsonGenerator Tests - اختبارات مولد JSON
// ============================================================================

TEST(JsonGeneratorTest, StringifyNull) {
    JsonValue value;
    EXPECT_EQ(JsonGenerator::stringify(value), "null");
}

TEST(JsonGeneratorTest, StringifyBoolean) {
    JsonValue value_true(true);
    EXPECT_EQ(JsonGenerator::stringify(value_true), "true");
    
    JsonValue value_false(false);
    EXPECT_EQ(JsonGenerator::stringify(value_false), "false");
}

TEST(JsonGeneratorTest, StringifyNumber) {
    JsonValue value(42);
    EXPECT_EQ(JsonGenerator::stringify(value), "42");
    
    JsonValue decimal(3.14);
    std::string result = JsonGenerator::stringify(decimal);
    EXPECT_TRUE(result.find("3.14") != std::string::npos);
}

TEST(JsonGeneratorTest, StringifyString) {
    JsonValue value("Hello");
    EXPECT_EQ(JsonGenerator::stringify(value), "\"Hello\"");
}

TEST(JsonGeneratorTest, StringifyArray) {
    JsonArray arr;
    arr.push(JsonValue(1));
    arr.push(JsonValue(2));
    arr.push(JsonValue(3));
    
    JsonValue value(arr);
    std::string result = JsonGenerator::stringify(value);
    EXPECT_TRUE(result.find("[") != std::string::npos);
    EXPECT_TRUE(result.find("1") != std::string::npos);
}

TEST(JsonGeneratorTest, StringifyObject) {
    JsonObject obj;
    obj.set("name", JsonValue("Ahmad"));
    obj.set("age", JsonValue(25));
    
    JsonValue value(obj);
    std::string result = JsonGenerator::stringify(value);
    EXPECT_TRUE(result.find("name") != std::string::npos);
    EXPECT_TRUE(result.find("Ahmad") != std::string::npos);
}

TEST(JsonGeneratorTest, Pretty) {
    JsonObject obj;
    obj.set("name", JsonValue("Ahmad"));
    obj.set("age", JsonValue(25));
    
    JsonValue value(obj);
    std::string result = JsonGenerator::pretty(value);
    
    EXPECT_TRUE(result.find("\n") != std::string::npos);
    EXPECT_TRUE(result.find("  ") != std::string::npos);
}

TEST(JsonGeneratorTest, Minify) {
    JsonObject obj;
    obj.set("name", JsonValue("Ahmad"));
    
    JsonValue value(obj);
    std::string result = JsonGenerator::minify(value);
    
    EXPECT_EQ(result.find("\n"), std::string::npos);
}

// ============================================================================
// JSONPath Tests - اختبارات مسار JSON
// ============================================================================

TEST(JsonPathTest, QueryRoot) {
    JsonValue value = JsonParser::parse(R"({"name": "Ahmad"})");
    auto results = JsonPath::query(value, "$");
    
    EXPECT_EQ(results.size(), 1);
}

TEST(JsonPathTest, QueryProperty) {
    JsonValue value = JsonParser::parse(R"({"name": "Ahmad", "age": 25})");
    auto results = JsonPath::query(value, "name");
    
    EXPECT_EQ(results.size(), 1);
    EXPECT_EQ(results[0].as_string(), "Ahmad");
}

TEST(JsonPathTest, QueryNested) {
    JsonValue value = JsonParser::parse(R"({
        "person": {
            "name": "Ahmad"
        }
    })");
    
    auto results = JsonPath::query(value, "person.name");
    EXPECT_EQ(results.size(), 1);
    EXPECT_EQ(results[0].as_string(), "Ahmad");
}

TEST(JsonPathTest, QueryArray) {
    JsonValue value = JsonParser::parse(R"([1, 2, 3, 4, 5])");
    auto results = JsonPath::query(value, "2");
    
    EXPECT_EQ(results.size(), 1);
    EXPECT_EQ(results[0].as_int(), 3);
}

TEST(JsonPathTest, Exists) {
    JsonValue value = JsonParser::parse(R"({"name": "Ahmad"})");
    
    EXPECT_TRUE(JsonPath::exists(value, "name"));
    EXPECT_FALSE(JsonPath::exists(value, "missing"));
}

TEST(JsonPathTest, Get) {
    JsonValue value = JsonParser::parse(R"({"name": "Ahmad"})");
    JsonValue result = JsonPath::get(value, "name");
    
    EXPECT_EQ(result.as_string(), "Ahmad");
}

// ============================================================================
// Utility Tests - اختبارات الدوال المساعدة
// ============================================================================

TEST(UtilityTest, Escape) {
    EXPECT_EQ(json_escape("Hello\nWorld"), "Hello\\nWorld");
    EXPECT_EQ(json_escape("Quote: \""), "Quote: \\\"");
    EXPECT_EQ(json_escape("Tab:\t"), "Tab:\\t");
}

TEST(UtilityTest, Unescape) {
    EXPECT_EQ(json_unescape("Hello\\nWorld"), "Hello\nWorld");
    EXPECT_EQ(json_unescape("Quote: \\\""), "Quote: \"");
}

TEST(UtilityTest, TypeToString) {
    EXPECT_EQ(json_type_to_string(JsonType::NULL_TYPE), "null");
    EXPECT_EQ(json_type_to_string(JsonType::BOOLEAN), "boolean");
    EXPECT_EQ(json_type_to_string(JsonType::NUMBER), "number");
    EXPECT_EQ(json_type_to_string(JsonType::STRING), "string");
    EXPECT_EQ(json_type_to_string(JsonType::ARRAY), "array");
    EXPECT_EQ(json_type_to_string(JsonType::OBJECT), "object");
}

TEST(UtilityTest, Merge) {
    JsonObject obj1;
    obj1.set("a", JsonValue(1));
    obj1.set("b", JsonValue(2));
    
    JsonObject obj2;
    obj2.set("b", JsonValue(3));
    obj2.set("c", JsonValue(4));
    
    JsonValue merged = json_merge(JsonValue(obj1), JsonValue(obj2));
    
    EXPECT_EQ(merged["a"].as_int(), 1);
    EXPECT_EQ(merged["b"].as_int(), 3);
    EXPECT_EQ(merged["c"].as_int(), 4);
}

TEST(UtilityTest, Clone) {
    JsonObject obj;
    obj.set("name", JsonValue("Ahmad"));
    JsonValue original(obj);
    
    JsonValue cloned = json_clone(original);
    
    cloned.as_object().set("name", JsonValue("Ali"));
    
    EXPECT_EQ(original["name"].as_string(), "Ahmad");
    EXPECT_EQ(cloned["name"].as_string(), "Ali");
}

TEST(UtilityTest, Equals) {
    JsonValue v1 = JsonParser::parse(R"({"a": 1, "b": 2})");
    JsonValue v2 = JsonParser::parse(R"({"a": 1, "b": 2})");
    JsonValue v3 = JsonParser::parse(R"({"a": 1, "b": 3})");
    
    EXPECT_TRUE(json_equals(v1, v2));
    EXPECT_FALSE(json_equals(v1, v3));
}

// ============================================================================
// Edge Cases - حالات حدية
// ============================================================================

TEST(EdgeCaseTest, DeepNesting) {
    std::string json = R"({
        "level1": {
            "level2": {
                "level3": {
                    "level4": {
                        "value": 42
                    }
                }
            }
        }
    })";
    
    JsonValue value = JsonParser::parse(json);
    EXPECT_EQ(value["level1"]["level2"]["level3"]["level4"]["value"].as_int(), 42);
}

TEST(EdgeCaseTest, LargeArray) {
    JsonArray arr;
    for (int i = 0; i < 1000; i++) {
        arr.push(JsonValue(i));
    }
    
    JsonValue value(arr);
    EXPECT_EQ(value.as_array().size(), 1000);
    EXPECT_EQ(value[999].as_int(), 999);
}

TEST(EdgeCaseTest, UnicodeStrings) {
    JsonValue value = JsonParser::parse(R"({"text": "مرحبا بالعالم 🌍"})");
    EXPECT_EQ(value["text"].as_string(), "مرحبا بالعالم 🌍");
}

TEST(EdgeCaseTest, RoundTrip) {
    std::string original = R"({"name":"Ahmad","age":25,"skills":["C++","Python"]})";
    
    JsonValue parsed = JsonParser::parse(original);
    std::string generated = JsonGenerator::minify(parsed);
    JsonValue reparsed = JsonParser::parse(generated);
    
    EXPECT_TRUE(json_equals(parsed, reparsed));
}

// ============================================================================
// Main - الدالة الرئيسية
// ============================================================================

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

// الحمد لله رب العالمين
