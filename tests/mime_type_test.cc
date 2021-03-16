#include "gtest/gtest.h"
#include "mime_types.h"

class mime_type_test : public ::testing::Test {
protected:
   std::string test_string;
   std::string test_result;

   std::string test_string1;
   std::string test_result1;
};

TEST_F(mime_type_test,mime_types_test_1){

	// when the type of the file is jpg, it exists in the list.
	// and it returns the proper string.
	test_string = "jpg";
	test_result = get_ext_type(test_string);
	EXPECT_TRUE(test_result == "image/jpeg");

	// when the type of the file does not exist in the list.
	// and it returns the default string.
	test_string1 = "aa";
	test_result1 = get_ext_type(test_string1);
	EXPECT_TRUE(test_result1 == "application/text");

}