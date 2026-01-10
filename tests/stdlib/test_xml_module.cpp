// بسم الله الرحمن الرحيم
/**
 * @file test_xml_module.cpp
 * @brief XML Module Tests - اختبارات وحدة XML
 * 
 * الحمد لله رب العالمين
 */

#include "xml/xml_module.h"
#include <gtest/gtest.h>

using namespace sad::stdlib::xml;

// ============================================================================
// XmlAttribute Tests - اختبارات سمة XML
// ============================================================================

TEST(XmlAttributeTest, Construction) {
    XmlAttribute attr("id", "123");
    EXPECT_EQ(attr.name(), "id");
    EXPECT_EQ(attr.value(), "123");
}

TEST(XmlAttributeTest, SetName) {
    XmlAttribute attr;
    attr.set_name("class");
    EXPECT_EQ(attr.name(), "class");
}

TEST(XmlAttributeTest, SetValue) {
    XmlAttribute attr("key", "old");
    attr.set_value("new");
    EXPECT_EQ(attr.value(), "new");
}

TEST(XmlAttributeTest, Namespace) {
    XmlAttribute attr("attr", "value");
    attr.set_namespace_uri("http://example.com");
    EXPECT_EQ(attr.namespace_uri(), "http://example.com");
}

// ============================================================================
// XmlElement Tests - اختبارات عنصر XML
// ============================================================================

TEST(XmlElementTest, Construction) {
    XmlElement element("div");
    EXPECT_EQ(element.name(), "div");
    EXPECT_EQ(element.type(), XmlNodeType::ELEMENT);
}

TEST(XmlElementTest, SetName) {
    XmlElement element("div");
    element.set_name("span");
    EXPECT_EQ(element.name(), "span");
}

TEST(XmlElementTest, Attributes) {
    XmlElement element("div");
    
    element.set_attribute("id", "main");
    element.set_attribute("class", "container");
    
    EXPECT_TRUE(element.has_attribute("id"));
    EXPECT_EQ(element.get_attribute("id"), "main");
    EXPECT_EQ(element.attribute_count(), 2);
}

TEST(XmlElementTest, RemoveAttribute) {
    XmlElement element("div");
    element.set_attribute("id", "test");
    
    EXPECT_TRUE(element.has_attribute("id"));
    element.remove_attribute("id");
    EXPECT_FALSE(element.has_attribute("id"));
}

TEST(XmlElementTest, TextContent) {
    XmlElement element("p");
    element.set_text("Hello World");
    
    EXPECT_EQ(element.text(), "Hello World");
}

TEST(XmlElementTest, AppendText) {
    XmlElement element("p");
    element.set_text("Hello");
    element.append_text(" World");
    
    EXPECT_EQ(element.text(), "Hello World");
}

TEST(XmlElementTest, Children) {
    XmlElement parent("div");
    XmlElement child1("span");
    XmlElement child2("p");
    
    parent.append_child(child1);
    parent.append_child(child2);
    
    EXPECT_EQ(parent.child_count(), 2);
    EXPECT_FALSE(parent.children().empty());
}

TEST(XmlElementTest, ChildrenByName) {
    XmlElement parent("div");
    
    XmlElement span1("span");
    XmlElement span2("span");
    XmlElement p("p");
    
    parent.append_child(span1);
    parent.append_child(span2);
    parent.append_child(p);
    
    auto spans = parent.children("span");
    EXPECT_EQ(spans.size(), 2);
}

TEST(XmlElementTest, FirstChild) {
    XmlElement parent("div");
    XmlElement child("span");
    child.set_text("First");
    
    parent.append_child(child);
    
    auto first = parent.first_child();
    ASSERT_TRUE(first.has_value());
    EXPECT_EQ(first->text(), "First");
}

TEST(XmlElementTest, ClearChildren) {
    XmlElement parent("div");
    parent.append_child(XmlElement("span"));
    
    EXPECT_EQ(parent.child_count(), 1);
    parent.clear_children();
    EXPECT_EQ(parent.child_count(), 0);
}

TEST(XmlElementTest, Namespace) {
    XmlElement element("div");
    element.set_namespace_uri("http://www.w3.org/1999/xhtml");
    element.set_namespace_prefix("html");
    
    EXPECT_EQ(element.namespace_uri(), "http://www.w3.org/1999/xhtml");
    EXPECT_EQ(element.namespace_prefix(), "html");
}

// ============================================================================
// XmlDocument Tests - اختبارات مستند XML
// ============================================================================

TEST(XmlDocumentTest, Construction) {
    XmlDocument doc;
    EXPECT_EQ(doc.version(), "1.0");
    EXPECT_EQ(doc.encoding(), "UTF-8");
}

TEST(XmlDocumentTest, SetRoot) {
    XmlDocument doc;
    XmlElement root("root");
    
    doc.set_root(root);
    EXPECT_EQ(doc.root().name(), "root");
}

TEST(XmlDocumentTest, SetVersion) {
    XmlDocument doc;
    doc.set_version("1.1");
    EXPECT_EQ(doc.version(), "1.1");
}

TEST(XmlDocumentTest, SetEncoding) {
    XmlDocument doc;
    doc.set_encoding("UTF-16");
    EXPECT_EQ(doc.encoding(), "UTF-16");
}

TEST(XmlDocumentTest, Validate) {
    XmlDocument doc;
    XmlElement root("root");
    doc.set_root(root);
    
    EXPECT_TRUE(doc.validate());
}

// ============================================================================
// XmlParser Tests - اختبارات محلل XML
// ============================================================================

TEST(XmlParserTest, ParseSimpleElement) {
    std::string xml = "<root></root>";
    XmlDocument doc = XmlParser::parse(xml);
    
    EXPECT_EQ(doc.root().name(), "root");
}

TEST(XmlParserTest, ParseSelfClosing) {
    std::string xml = "<root/>";
    XmlDocument doc = XmlParser::parse(xml);
    
    EXPECT_EQ(doc.root().name(), "root");
}

TEST(XmlParserTest, ParseWithAttributes) {
    std::string xml = R"(<root id="123" class="main"></root>)";
    XmlDocument doc = XmlParser::parse(xml);
    
    EXPECT_EQ(doc.root().get_attribute("id"), "123");
    EXPECT_EQ(doc.root().get_attribute("class"), "main");
}

TEST(XmlParserTest, ParseWithText) {
    std::string xml = "<root>Hello World</root>";
    XmlDocument doc = XmlParser::parse(xml);
    
    EXPECT_EQ(doc.root().text(), "Hello World");
}

TEST(XmlParserTest, ParseWithChildren) {
    std::string xml = R"(
        <root>
            <child1>Text 1</child1>
            <child2>Text 2</child2>
        </root>
    )";
    
    XmlDocument doc = XmlParser::parse(xml);
    EXPECT_EQ(doc.root().child_count(), 2);
    
    auto children = doc.root().children();
    EXPECT_EQ(children[0].name(), "child1");
    EXPECT_EQ(children[1].name(), "child2");
}

TEST(XmlParserTest, ParseNested) {
    std::string xml = R"(
        <root>
            <parent>
                <child>
                    <grandchild>Deep Text</grandchild>
                </child>
            </parent>
        </root>
    )";
    
    XmlDocument doc = XmlParser::parse(xml);
    auto parent = doc.root().first_child();
    ASSERT_TRUE(parent.has_value());
    
    auto child = parent->first_child();
    ASSERT_TRUE(child.has_value());
    
    auto grandchild = child->first_child();
    ASSERT_TRUE(grandchild.has_value());
    EXPECT_EQ(grandchild->text(), "Deep Text");
}

TEST(XmlParserTest, ParseWithDeclaration) {
    std::string xml = R"(<?xml version="1.0" encoding="UTF-8"?><root></root>)";
    XmlDocument doc = XmlParser::parse(xml);
    
    EXPECT_EQ(doc.version(), "1.0");
    EXPECT_EQ(doc.encoding(), "UTF-8");
}

TEST(XmlParserTest, ParseWithEntities) {
    std::string xml = "<root>Text with &lt;tags&gt; &amp; &quot;quotes&quot;</root>";
    XmlDocument doc = XmlParser::parse(xml);
    
    std::string text = doc.root().text();
    EXPECT_TRUE(text.find("<") != std::string::npos);
    EXPECT_TRUE(text.find(">") != std::string::npos);
    EXPECT_TRUE(text.find("&") != std::string::npos);
}

TEST(XmlParserTest, ParseComplex) {
    std::string xml = R"(<?xml version="1.0"?>
        <library>
            <book id="1" available="true">
                <title>C++ Programming</title>
                <author>Bjarne Stroustrup</author>
                <year>2013</year>
            </book>
            <book id="2" available="false">
                <title>Clean Code</title>
                <author>Robert Martin</author>
                <year>2008</year>
            </book>
        </library>
    )";
    
    XmlDocument doc = XmlParser::parse(xml);
    EXPECT_EQ(doc.root().name(), "library");
    
    auto books = doc.root().children("book");
    EXPECT_EQ(books.size(), 2);
    
    EXPECT_EQ(books[0].get_attribute("id"), "1");
    auto title = books[0].first_child("title");
    ASSERT_TRUE(title.has_value());
    EXPECT_EQ(title->text(), "C++ Programming");
}

TEST(XmlParserTest, Validate) {
    EXPECT_TRUE(XmlParser::validate("<root></root>"));
    EXPECT_FALSE(XmlParser::validate("<invalid>"));
}

// ============================================================================
// XmlGenerator Tests - اختبارات مولد XML
// ============================================================================

TEST(XmlGeneratorTest, StringifySimple) {
    XmlElement element("root");
    XmlDocument doc;
    doc.set_root(element);
    
    std::string xml = XmlGenerator::stringify(doc);
    EXPECT_TRUE(xml.find("<root") != std::string::npos);
}

TEST(XmlGeneratorTest, StringifyWithAttributes) {
    XmlElement element("div");
    element.set_attribute("id", "main");
    element.set_attribute("class", "container");
    
    XmlDocument doc;
    doc.set_root(element);
    
    std::string xml = XmlGenerator::stringify(doc);
    EXPECT_TRUE(xml.find("id=\"main\"") != std::string::npos);
    EXPECT_TRUE(xml.find("class=\"container\"") != std::string::npos);
}

TEST(XmlGeneratorTest, StringifyWithText) {
    XmlElement element("p");
    element.set_text("Hello World");
    
    XmlDocument doc;
    doc.set_root(element);
    
    std::string xml = XmlGenerator::stringify(doc);
    EXPECT_TRUE(xml.find("Hello World") != std::string::npos);
}

TEST(XmlGeneratorTest, StringifyWithChildren) {
    XmlElement root("root");
    
    XmlElement child1("child1");
    child1.set_text("Text 1");
    
    XmlElement child2("child2");
    child2.set_text("Text 2");
    
    root.append_child(child1);
    root.append_child(child2);
    
    XmlDocument doc;
    doc.set_root(root);
    
    std::string xml = XmlGenerator::stringify(doc);
    EXPECT_TRUE(xml.find("<child1>") != std::string::npos);
    EXPECT_TRUE(xml.find("<child2>") != std::string::npos);
}

TEST(XmlGeneratorTest, Pretty) {
    XmlElement root("root");
    XmlElement child("child");
    root.append_child(child);
    
    XmlDocument doc;
    doc.set_root(root);
    
    std::string xml = XmlGenerator::pretty(doc);
    EXPECT_TRUE(xml.find("\n") != std::string::npos);
    EXPECT_TRUE(xml.find("  ") != std::string::npos);
}

TEST(XmlGeneratorTest, Minify) {
    XmlElement root("root");
    XmlElement child("child");
    root.append_child(child);
    
    XmlDocument doc;
    doc.set_root(root);
    
    std::string xml = XmlGenerator::minify(doc);
    EXPECT_EQ(xml.find("\n  "), std::string::npos);
}

// ============================================================================
// XPath Tests - اختبارات مسار XPath
// ============================================================================

TEST(XPathTest, QueryRoot) {
    std::string xml = "<root><child>Text</child></root>";
    XmlDocument doc = XmlParser::parse(xml);
    
    auto results = XPath::query(doc, "/");
    EXPECT_EQ(results.size(), 1);
}

TEST(XPathTest, QueryChild) {
    std::string xml = "<root><child>Text</child></root>";
    XmlDocument doc = XmlParser::parse(xml);
    
    auto results = XPath::query(doc, "child");
    EXPECT_EQ(results.size(), 1);
    EXPECT_EQ(results[0].name(), "child");
}

TEST(XPathTest, QueryNested) {
    std::string xml = R"(
        <root>
            <parent>
                <child>Deep</child>
            </parent>
        </root>
    )";
    
    XmlDocument doc = XmlParser::parse(xml);
    auto results = XPath::query(doc, "parent/child");
    
    EXPECT_EQ(results.size(), 1);
    EXPECT_EQ(results[0].text(), "Deep");
}

TEST(XPathTest, Exists) {
    std::string xml = "<root><child>Text</child></root>";
    XmlDocument doc = XmlParser::parse(xml);
    
    EXPECT_TRUE(XPath::exists(doc, "child"));
    EXPECT_FALSE(XPath::exists(doc, "missing"));
}

TEST(XPathTest, First) {
    std::string xml = R"(
        <root>
            <item>First</item>
            <item>Second</item>
        </root>
    )";
    
    XmlDocument doc = XmlParser::parse(xml);
    auto result = XPath::first(doc, "item");
    
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->text(), "First");
}

// ============================================================================
// Utility Tests - اختبارات الدوال المساعدة
// ============================================================================

TEST(XmlUtilityTest, Escape) {
    EXPECT_EQ(xml_escape("<tag>"), "&lt;tag&gt;");
    EXPECT_EQ(xml_escape("A & B"), "A &amp; B");
    EXPECT_EQ(xml_escape("\"quoted\""), "&quot;quoted&quot;");
    EXPECT_EQ(xml_escape("'apostrophe'"), "&apos;apostrophe&apos;");
}

TEST(XmlUtilityTest, Unescape) {
    EXPECT_EQ(xml_unescape("&lt;tag&gt;"), "<tag>");
    EXPECT_EQ(xml_unescape("A &amp; B"), "A & B");
    EXPECT_EQ(xml_unescape("&quot;quoted&quot;"), "\"quoted\"");
}

TEST(XmlUtilityTest, NodeTypeToString) {
    EXPECT_EQ(xml_node_type_to_string(XmlNodeType::ELEMENT), "element");
    EXPECT_EQ(xml_node_type_to_string(XmlNodeType::TEXT), "text");
    EXPECT_EQ(xml_node_type_to_string(XmlNodeType::CDATA), "cdata");
}

TEST(XmlUtilityTest, Clone) {
    XmlElement original("div");
    original.set_text("Original");
    
    XmlElement cloned = xml_clone(original);
    cloned.set_text("Cloned");
    
    EXPECT_EQ(original.text(), "Original");
    EXPECT_EQ(cloned.text(), "Cloned");
}

TEST(XmlUtilityTest, Equals) {
    XmlElement elem1("div");
    elem1.set_attribute("id", "test");
    elem1.set_text("Content");
    
    XmlElement elem2("div");
    elem2.set_attribute("id", "test");
    elem2.set_text("Content");
    
    XmlElement elem3("span");
    
    EXPECT_TRUE(xml_equals(elem1, elem2));
    EXPECT_FALSE(xml_equals(elem1, elem3));
}

// ============================================================================
// Edge Cases - حالات حدية
// ============================================================================

TEST(XmlEdgeCaseTest, EmptyElement) {
    XmlElement element("empty");
    XmlDocument doc;
    doc.set_root(element);
    
    std::string xml = XmlGenerator::stringify(doc);
    EXPECT_TRUE(xml.find("/>") != std::string::npos || 
                xml.find("></empty>") != std::string::npos);
}

TEST(XmlEdgeCaseTest, ManyChildren) {
    XmlElement root("root");
    
    for (int i = 0; i < 100; i++) {
        XmlElement child("child");
        child.set_text("Child " + std::to_string(i));
        root.append_child(std::move(child));
    }
    
    EXPECT_EQ(root.child_count(), 100);
}

TEST(XmlEdgeCaseTest, DeepNesting) {
    std::string xml = R"(
        <level1>
            <level2>
                <level3>
                    <level4>
                        <level5>Deep Content</level5>
                    </level4>
                </level3>
            </level2>
        </level1>
    )";
    
    XmlDocument doc = XmlParser::parse(xml);
    EXPECT_EQ(doc.root().name(), "level1");
}

TEST(XmlEdgeCaseTest, UnicodeContent) {
    XmlElement element("p");
    element.set_text("مرحبا بالعالم 🌍");
    
    XmlDocument doc;
    doc.set_root(element);
    
    std::string xml = doc.to_string();
    XmlDocument reparsed = XmlParser::parse(xml);
    
    EXPECT_EQ(reparsed.root().text(), "مرحبا بالعالم 🌍");
}

TEST(XmlEdgeCaseTest, RoundTrip) {
    std::string original = R"(<root id="test"><child>Content</child></root>)";
    
    XmlDocument doc1 = XmlParser::parse(original);
    std::string generated = XmlGenerator::minify(doc1);
    XmlDocument doc2 = XmlParser::parse(generated);
    
    EXPECT_TRUE(xml_equals(doc1.root(), doc2.root()));
}

// ============================================================================
// Main - الدالة الرئيسية
// ============================================================================
// Note: main() is defined in test_json_module.cpp for the combined test executable

// الحمد لله رب العالمين
