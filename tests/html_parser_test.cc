#include "html_parser.h"
#include "gtest/gtest.h"

class html_parser_test_fixture : public testing::Test {
    protected:
    std::string prefix = "/test";
    std::string out = "";
    std::string href = "href";
};

TEST_F(html_parser_test_fixture, single_tag) {
    std::string doc = "<a href=\"/foo\"></a>";
    out = html_parsing::update_tag_attribute(doc, prefix, href);
    std::string expected_out = "<a href=\"" + prefix + "/foo\"></a>";

    EXPECT_EQ(out, expected_out);
}

TEST_F(html_parser_test_fixture, many_attributes_test) {
    std::string doc = "<html><a hello=\"\" world=\"\" foo=\"\" href=\"/f\" bar=\"\" bazz=\"\"></html>";
    out = html_parsing::update_tag_attribute(doc, prefix, href);
    std::string expected_out = "<html><a hello=\"\" world=\"\" foo=\"\" href=\"" + prefix + "/f\" bar=\"\" bazz=\"\"></html>";
    
    EXPECT_EQ(out, expected_out);
}

TEST_F(html_parser_test_fixture, href_in_self_terminating_tag) {
    std::string doc = "<html><a href=\"/hi\"/></html>";
    out = html_parsing::update_tag_attribute(doc, prefix, href);
    std::string expected_out = "<html><a href=\"" + prefix + "/hi\"/></html>";
    
    EXPECT_EQ(out, expected_out);
}

TEST_F(html_parser_test_fixture, http_address_not_changed) {
    std::string doc = "<a href=\"https://www.google.com\"></a>";
    out = html_parsing::update_tag_attribute(doc, prefix, href);
    
    EXPECT_EQ(doc, out);
}

TEST_F(html_parser_test_fixture, href_instance_in_body_not_changed) {
    std::string doc = "<html>href=\"/hello\"</html>";
    out = html_parsing::update_tag_attribute(doc, prefix, href);

    EXPECT_EQ(doc, out);
}

TEST_F(html_parser_test_fixture, href_in_another_attribute_s_value_not_changed) {
    std::string doc = "<a id=\"href=\"/foo\"\"></a>";
    out = html_parsing::update_tag_attribute(doc, prefix, href);

    EXPECT_EQ(doc, out);
}

TEST_F(html_parser_test_fixture, fake_tag_with_href_not_changed) {
    std::string doc = "<p><2 href=\"/foo\"></p>";
    out = html_parsing::update_tag_attribute(doc, prefix, href);

    EXPECT_EQ(doc, out);
}

TEST_F(html_parser_test_fixture, spaced_tag_with_href_not_changed) {
    std::string doc = "<p> < href="" ></p>";
    out = html_parsing::update_tag_attribute(doc, prefix, href);

    EXPECT_EQ(doc, out);
}

TEST_F(html_parser_test_fixture, href_no_leading_slash_not_changed) {
    std::string doc = "<a href=\"foo\"></a>";
    out = html_parsing::update_tag_attribute(doc, prefix, href);

    EXPECT_EQ(doc, out);
}

TEST_F(html_parser_test_fixture, href_in_comment_not_changed) {
    std::string doc = "<!-- href=\"/foo\" -->";
    out = html_parsing::update_tag_attribute(doc, prefix, href);

    EXPECT_EQ(doc, out);
}

TEST_F(html_parser_test_fixture, href_in_tag_in_comment_changed) {
    // Logically if you had code commented out and want to uncomment it
    // then you would want the href to be updated too. lol
    std::string doc = "<!-- <a href=\"/foo\"></a>";
    out = html_parsing::update_tag_attribute(doc, prefix, href);
    std::string expected_out = "<!-- <a href=\"" + prefix + "/foo\"></a>";

    EXPECT_EQ(out, expected_out);
}