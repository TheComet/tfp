#include "gmock/gmock.h"
#include "cstructures/init.h"

using namespace testing;

class LibraryInitEnvironment : public testing::Environment
{
public:
    virtual ~LibraryInitEnvironment() {}

    virtual void SetUp()
    {
        testing::FLAGS_gtest_death_test_style = "threadsafe";
        ASSERT_THAT(cstructures_init(), Eq(0));
    }

    virtual void TearDown()
    {
        cstructures_deinit();
    }
};

const testing::Environment* const libraryInitEnvironment =
        testing::AddGlobalTestEnvironment(new LibraryInitEnvironment);
