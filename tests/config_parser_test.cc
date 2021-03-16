#include "gtest/gtest.h"
#include "config_parser.h"

TEST(Parser_test_constructor, SimpleConfig) {
  NginxConfigParser parser;
  NginxConfig out_config;

  bool success = parser.Parse("test_configs/test_config0", &out_config);

  EXPECT_TRUE(success);
}

class ParserTest : public ::testing::Test {
protected:
    void SetUp() override {
        SetTest(test1_,  config1_, "test_configs/test_config1");
        SetTest(test2_,  config2_, "test_configs/test_config2");
        SetTest(test3_,  config3_, "test_configs/test_config3");
        SetTest(test4_,  config4_, "test_configs/test_config4");
        SetTest(test5_,  config5_, "test_configs/test_config5");
        SetTest(test6_,  config6_, "WRONG_FILE_NAME");
        SetTest(test7_,  config7_, "test_configs/test_config7");
        SetTest(test8_,  config8_, "test_configs/test_config8");
        SetTest(test9_,  config9_, "test_configs/test_config9");
        SetTest(test10_,  config10_, "test_configs/test_config10");
        SetTest(test11_,  config11_, "test_configs/test_config11");
        SetTest(test12_,  config12_, "test_configs/test_config12");
    }
    void SetTest(bool &test, std::string &config, const char *filename) {
        NginxConfigParser parser;
        NginxConfig out_config;
        test = parser.Parse(filename, &out_config);
        config = out_config.ToString();
    }

    bool test1_;
    bool test2_;
    bool test3_;
    bool test4_;
    bool test5_;
    bool test6_;
    bool test7_;
    bool test8_;
    bool test9_;
    bool test10_;
    bool test11_;
    bool test12_;

    std::string config1_;
    std::string config2_;
    std::string config3_;
    std::string config4_;
    std::string config5_;
    std::string config6_;
    std::string config7_;
    std::string config8_;
    std::string config9_;
    std::string config10_;
    std::string config11_;
    std::string config12_;

};

TEST_F(ParserTest, parser_test_syntax) {
    EXPECT_TRUE(test1_);
    EXPECT_TRUE(test2_);
    EXPECT_TRUE(test3_);
    EXPECT_FALSE(test4_);
    EXPECT_FALSE(test5_);
    EXPECT_FALSE(test7_);
    EXPECT_FALSE(test8_);
    EXPECT_FALSE(test9_);
    EXPECT_TRUE(test10_);
    EXPECT_FALSE(test11_);
    EXPECT_FALSE(test12_);
}

TEST_F(ParserTest, parser_test_io) {
    EXPECT_FALSE(test6_);

    std::string test1_config = "server {\n  listen 80;\n}\n";
    EXPECT_EQ(config1_, test1_config);

    std::string test3_config = "foo 'c' http {\n  server {\n    location / {\n      root /data;\n    }\n  }\n}\n";
    EXPECT_EQ(config3_, test3_config);
}

